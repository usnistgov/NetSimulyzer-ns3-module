/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 *
 * Author: Evan Black <evan.black@nist.gov>
 */

#include "ecdf-sink.h"

#include <ns3/double.h>
#include <ns3/enum.h>
#include <ns3/nstime.h>

#include <algorithm>

namespace ns3::netsimulyzer
{

EcdfSink::EcdfSink(Ptr<Orchestrator> orchestrator, const std::string& name)
{
    m_series = CreateObject<XYSeries>(orchestrator);
    m_series->SetAttribute("Connection", EnumValue(XYSeries::ConnectionType::None));

    auto yAxis = m_series->GetYAxis();
    yAxis->SetAttribute("Name", StringValue("Percent"));
    yAxis->SetAttribute("BoundMode", EnumValue(ValueAxis::BoundMode::Fixed));
    yAxis->SetAttribute("Minimum", DoubleValue(0.0));
    yAxis->SetAttribute("Maximum", DoubleValue(1.0));

    m_series->SetAttribute("Name", StringValue(name));

    m_timer.SetFunction(&EcdfSink::Flush, this);
}

TypeId
EcdfSink::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::netsimulyzer::EcdfSink")
            .SetParent<ns3::Object>()
            .SetGroupName("netsimulyzer")
            .AddAttribute("Series",
                          "The internal series used for display",
                          TypeId::ATTR_GET,
                          PointerValue(),
                          MakePointerAccessor(&EcdfSink::m_series),
                          MakePointerChecker<XYSeries>())
            // clang-format off
            .AddAttribute("Connection",
                          "Type of connection to form between points in the series",
                          EnumValue(XYSeries::ConnectionType::Line),
                          MakeEnumAccessor(&EcdfSink::SetConnectionType,
                                           &EcdfSink::GetConnectionType),
                          MakeEnumChecker(
                              XYSeries::ConnectionType::None, "None",
                              XYSeries::ConnectionType::Line, "Line",
                              XYSeries::ConnectionType::Spline, "Spline"))
            // Keep clang-format off
            .AddAttribute("FlushMode",
                          "When to write the changes to the graph",
                          EnumValue(EcdfSink::FlushMode::OnWrite),
                          MakeEnumAccessor(&EcdfSink::SetFlushMode, &EcdfSink::GetFlushMode),
                          MakeEnumChecker(
                              EcdfSink::FlushMode::OnWrite, "OnWrite",
                              EcdfSink::FlushMode::Interval, "Interval",
                              EcdfSink::FlushMode::Manual,"Manual"))
            // clang-format on
            .AddAttribute("Interval",
                          "The interval to update the plot. "
                          "Only used when the `FlushMode` attribute is set to `Interval`",
                          TimeValue(Seconds(1.0)),
                          MakeTimeAccessor(&EcdfSink::SetInterval, &EcdfSink::GetInterval),
                          MakeTimeChecker())
            .AddAttribute("XAxis",
                          "The X axis of the internal series",
                          TypeId::ATTR_GET,
                          PointerValue(),
                          MakePointerAccessor(&EcdfSink::GetXAxis),
                          MakePointerChecker<ValueAxis>())
            .AddAttribute("YAxis",
                          "The Y axis of the internal series",
                          TypeId::ATTR_GET,
                          PointerValue(),
                          MakePointerAccessor(&EcdfSink::GetYAxis),
                          MakePointerChecker<ValueAxis>());

    return tid;
}

Ptr<XYSeries>
EcdfSink::GetSeries(void) const
{
    return m_series;
}

Ptr<ValueAxis>
EcdfSink::GetXAxis(void) const
{
    return m_series->GetXAxis();
}

Ptr<ValueAxis>
EcdfSink::GetYAxis(void) const
{
    return m_series->GetYAxis();
}

EcdfSink::FlushMode
EcdfSink::GetFlushMode(void) const
{
    return m_flushMode;
}

void
EcdfSink::SetFlushMode(EcdfSink::FlushMode mode)
{
    m_flushMode = mode;

    if (m_flushMode == FlushMode::Interval && m_timer.GetDelay().IsPositive())
        m_timer.Schedule();
    else
        m_timer.Cancel();
}

XYSeries::ConnectionType
EcdfSink::GetConnectionType(void) const
{
    EnumValue connectionType;
    m_series->GetAttribute("Connection", connectionType);

    return static_cast<XYSeries::ConnectionType>(connectionType.Get());
}

void
EcdfSink::SetConnectionType(XYSeries::ConnectionType value)
{
    m_series->SetAttribute("Connection", EnumValue(value));
}

void
EcdfSink::Append(double value)
{
    auto iter = std::find_if(m_data.begin(), m_data.end(), [value](const auto& item) {
        return item.point == value;
    });
    if (iter != m_data.end())
        iter->frequency++;
    else
        m_data.push_back(PointFrequency{value, 1u});

    std::sort(m_data.begin(), m_data.end());

    m_totalPoints++;

    if (m_flushMode == FlushMode::OnWrite)
        Flush();
}

void
EcdfSink::SetInterval(Time interval)
{
    NS_ASSERT_MSG(interval.IsPositive(), "`interval` must be greater than 0");
    if (m_timer.IsRunning())
        m_timer.Cancel();

    m_timer.SetDelay(interval);

    if (m_flushMode == FlushMode::Interval)
        m_timer.Schedule();
}

Time
EcdfSink::GetInterval(void) const
{
    return m_timer.GetDelay();
}

void
EcdfSink::SetRangeFixed(double min, double max)
{
    m_series->GetXAxis()->FixedRange(min, max);
}

void
EcdfSink::SetRangeScaling(double min, double max)
{
    m_series->GetXAxis()->ScalingRange(min, max);
}

void
EcdfSink::Flush(void)
{
    m_series->Clear();
    double total = 0.0;

    EnumValue connectionMode;
    m_series->GetAttribute("Connection", connectionMode);

    if (connectionMode.Get() == XYSeries::ConnectionType::None)
    {
        for (const auto& [point, count] : m_data)
        {
            auto percent = static_cast<double>(count) / m_totalPoints;
            m_series->Append(point, percent + total);
            total += percent;
        }

        return;
    }

    // Line/spline

    double lastY = 0.0;
    for (const auto& [point, count] : m_data)
    {
        auto percent = static_cast<double>(count) / m_totalPoints;
        m_series->Append(point, lastY);

        const auto y = percent + total;
        m_series->Append(point, y);
        lastY = y;

        total += percent;
    }

    if (m_flushMode == FlushMode::Interval)
        m_timer.Schedule();
}

void
EcdfSink::DoDispose(void)
{
    m_timer.Cancel();
    Object::DoDispose();
}

bool
EcdfSink::PointFrequency::operator<(const EcdfSink::PointFrequency& other) const
{
    return point < other.point;
}

} // namespace ns3::netsimulyzer
