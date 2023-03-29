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

#include "value-axis.h"

#include <ns3/double.h>
#include <ns3/enum.h>
#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/string.h>

#include <algorithm>

namespace ns3::netsimulyzer
{

NS_OBJECT_ENSURE_REGISTERED(ValueAxis);

TypeId
ValueAxis::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::netsimulyzer::ValueAxis")
            .SetParent<ns3::Object>()
            .SetGroupName("netsimulyzer")
            .AddAttribute("Name",
                          "Unique name to represent this axis in visualizer elements",
                          StringValue(),
                          MakeStringAccessor(&ValueAxis::m_name),
                          MakeStringChecker())
            .AddAttribute("Minimum",
                          "The minimum value on the axis",
                          DoubleValue(),
                          MakeDoubleAccessor(&ValueAxis::m_min),
                          MakeDoubleChecker<double>())
            .AddAttribute("Maximum",
                          "The maximum value on the axis",
                          DoubleValue(),
                          MakeDoubleAccessor(&ValueAxis::m_max),
                          MakeDoubleChecker<double>())
            .AddAttribute("BoundMode",
                          "How the boundaries of the axis may move given a new value",
                          EnumValue(ValueAxis::BoundMode::HighestValue),
                          MakeEnumAccessor(&ValueAxis::m_boundMode),
                          // clang-format off
                          MakeEnumChecker(
                              BoundMode::Fixed, "Fixed",
                              BoundMode::HighestValue, "HighestValue"))
            .AddAttribute("Scale",
                          "The method to scale between tick marks on the axis",
                          EnumValue(ValueAxis::Scale::Linear),
                          MakeEnumAccessor(&ValueAxis::m_scale),
                          MakeEnumChecker(
                              ValueAxis::Linear, "Linear",
                              ValueAxis::Logarithmic, "Logarithmic"));
// clang-format on

    return tid;
}

void
ValueAxis::FixedRange(double min, double max)
{
    m_boundMode = BoundMode::Fixed;
    m_min = std::min(min, max);
    m_max = std::max(min, max);
}

void
ValueAxis::ScalingRange()
{
    m_boundMode = BoundMode::HighestValue;
    m_min = 0.0;
    m_max = 1.0;
}

void
ValueAxis::ScalingRange(double min, double max)
{
    m_boundMode = BoundMode::HighestValue;
    m_min = std::min(min, max);
    m_max = std::max(min, max);
}

} // namespace ns3::netsimulyzer