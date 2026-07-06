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
 * Author: Andrew Wagger <andrew.wagger@nist.gov>
 */

#include "net-visualizer.h"

#include "color.h"
#include "netsimulyzer-3D-models.h"
#include "node-configuration.h"
#include "orchestrator.h"
#include "series-collection.h"
#include "xy-series.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/pointer.h"
#include "ns3/ptr.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Visualizer");

namespace netsimulyzer
{

namespace visualizer
{
NS_OBJECT_ENSURE_REGISTERED(SeriesWrapper);
NS_OBJECT_ENSURE_REGISTERED(Accumulator);
NS_OBJECT_ENSURE_REGISTERED(SeriesContainer);
NS_OBJECT_ENSURE_REGISTERED(SeriesMap);
NS_OBJECT_ENSURE_REGISTERED(Visualizer);

/*          SeriesWrapper           */

TypeId
SeriesWrapper::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::SeriesWrapper")
            .SetParent<ns3::Object> ()
            .SetGroupName ("visualizer")
            .AddAttribute ("Series", "Series that is wrapped",
                        PointerValue (),
                        MakePointerAccessor (&SeriesWrapper::m_series),
                        MakePointerChecker<XYSeries> ());
    return tid;
    // clang-format on
}

Ptr<XYSeries>
SeriesWrapper::GetSeries()
{
    return m_series;
};

SeriesWrapper::SeriesWrapper(Ptr<XYSeries> series)
    : m_series(series)
{
    NS_LOG_FUNCTION(this << series);
};

Ptr<XYSeries>
SeriesWrapper::operator()()
{
    return this->GetSeries();
};

/*          SeriesContainer           */

TypeId
SeriesContainer::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::SeriesContainer")
            .SetParent<ns3::Object> ()
            .SetGroupName ("visualizer");
    return tid;
    // clang-format on
}

SeriesContainer*
SeriesContainer::AddWrapper(Ptr<SeriesWrapper> w)
{
    NS_LOG_FUNCTION(this << w);
    m_wrappers.emplace_back(w);
    m_collection->Add(w->GetSeries());
    return this;
};

Ptr<XYSeries>
SeriesContainer::GetSeries(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrappers.at(i)->GetSeries();
};

Ptr<SeriesWrapper>
SeriesContainer::GetWrapper(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrappers.at(i);
};

std::size_t
SeriesContainer::GetNSeries()
{
    return m_wrappers.size();
};

Ptr<SeriesCollection>
SeriesContainer::GetCollection()
{
    return m_collection;
};

SeriesContainer::SeriesContainer(Ptr<Visualizer> visualizer)
{
    NS_LOG_FUNCTION(this << visualizer);
    m_collection = CreateObject<SeriesCollection>(visualizer->GetOrchestrator());
};

SeriesContainer::SeriesContainer(Ptr<Visualizer> visualizer,
                                 std::string name,
                                 std::string x_axis,
                                 std::string y_axis)
    : SeriesContainer(visualizer)
{
    NS_LOG_FUNCTION(this << visualizer << name << x_axis << y_axis);

    m_collection->SetAttribute("Name", StringValue(name));
    m_collection->GetYAxis()->SetAttribute("Name", StringValue(y_axis));
    m_collection->GetXAxis()->SetAttribute("Name", StringValue(x_axis));
};

std::vector<Ptr<SeriesWrapper>>::iterator
SeriesContainer::begin()
{
    return m_wrappers.begin();
};

std::vector<Ptr<SeriesWrapper>>::iterator
SeriesContainer::end()
{
    return m_wrappers.end();
};

SeriesWrapper&
SeriesContainer::operator[](std::size_t i)
{
    return (*this->GetWrapper(i));
};

/*          Visualizer           */

TypeId
Visualizer::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::Visualizer")
            .SetParent<ns3::Object> ()
            .SetGroupName ("visualizer");
    return tid;
    // clang-format on
}

Visualizer::Visualizer(std::string outputFileName)
    : m_orchestrator(CreateObject<Orchestrator>(outputFileName)),
      m_configHelper(m_orchestrator)
{
    NS_LOG_FUNCTION(this << outputFileName);
};

Visualizer::Visualizer(std::string outputFileName, NodeContainer nodes)
    : Visualizer(outputFileName)
{
    NS_LOG_FUNCTION(this << outputFileName << &nodes);
    SetNodes(nodes);
};

void
Visualizer::SetNodes(NodeContainer nodes)
{
    NS_LOG_FUNCTION(this << &nodes);
    m_nodes = nodes;
    auto col_index = 0;
    for (auto node = m_nodes.Begin(); node != m_nodes.End(); node++)
    {
        auto nodeConfig = CreateObject<NodeConfiguration>(m_orchestrator);
        nodeConfig->SetModel(m_model);
        nodeConfig->SetBaseColor(m_colors.at(col_index % m_colors.size()));
        nodeConfig->SetScale(m_scale);
        m_configContainer.Add(nodeConfig);
        col_index++;
    }
    m_configHelper.Install(m_nodes, m_configContainer);
};

std::string
Visualizer::GetDefaultModel()
{
    return m_model;
};

Visualizer*
Visualizer::SetDefaultModel(std::string model)
{
    NS_LOG_FUNCTION(this << model);
    m_model = model;
    return this;
};

double
Visualizer::GetDefaultScale()
{
    return m_scale;
};

Visualizer*
Visualizer::SetDefaultScale(double scale)
{
    NS_LOG_FUNCTION(this << scale);
    m_scale = scale;
    return this;
};

Ptr<SeriesContainer>
Visualizer::GetContainer(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return m_containers.at(index);
};

SeriesContainer&
Visualizer::operator[](std::string index)
{
    return (*this->GetContainer(index));
};

Visualizer*
Visualizer::SetContainer(std::string index, Ptr<SeriesContainer> container)
{
    NS_LOG_FUNCTION(this << index << container);
    m_containers.insert({index, container});
    return this;
};

Ptr<NodeConfiguration>
Visualizer::GetConfig(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_configContainer.Get(i);
};

NodeConfigurationHelper
Visualizer::GetConfigHelper()
{
    return m_configHelper;
}

const Color3
Visualizer::GetColor(std::size_t i) const
{
    NS_LOG_FUNCTION(this << i);
    return m_colors.at(i % m_colors.size());
};

Ptr<Node>
Visualizer::GetNode(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_nodes.Get(i);
};

Ptr<Node>
Visualizer::GetNodeById(uint32_t id)
{
    NS_LOG_FUNCTION(this << id);
    for (std::size_t i = 0; i < m_nodes.GetN(); ++i)
    {
        if (m_nodes.Get(i)->GetId() == id)
        {
            return m_nodes.Get(i);
        }
    }
    return nullptr;
}

std::size_t
Visualizer::GetNNodes()
{
    return m_nodes.GetN();
};

Ptr<Orchestrator>
Visualizer::GetOrchestrator()
{
    return m_orchestrator;
};

/*          Accumulator           */

TypeId
Accumulator::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::Accumulator")
            .SetParent<ns3::netsimulyzer::visualizer::SeriesWrapper> ()
            .SetGroupName ("visualizer")
            .AddAttribute ("Value", "Accumulated Value",
                        DoubleValue (),
                        MakeDoubleAccessor (&Accumulator::m_value),
                        MakeDoubleChecker<double> ());
    return tid;
    // clang-format on
}

Accumulator::Accumulator(Ptr<XYSeries> series)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

void
Accumulator::Update(Time time, double add)
{
    NS_LOG_FUNCTION(this << time << add);
    m_value += add;
    SeriesWrapper::GetSeries()->Append(time.GetSeconds(), m_value);
}

void
Accumulator::Update(double add)
{
    Update(Simulator::Now(), add);
}

/*          AverageValue           */

TypeId
AverageValue::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::AverageValue")
            .SetParent<ns3::netsimulyzer::visualizer::SeriesWrapper> ()
            .SetGroupName ("visualizer")
            .AddAttribute ("Value", "Average value",
                        DoubleValue (),
                        MakeDoubleAccessor (&AverageValue::m_avg),
                        MakeDoubleChecker<double> ());
    return tid;
    // clang-format on
}

AverageValue::AverageValue(Ptr<XYSeries> series)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

void
AverageValue::Update(Time time, double value)
{
    NS_LOG_FUNCTION(this << time << value);
    m_avg = ((m_n * m_avg) + value) / (m_n + 1.0);
    m_n++;
    SeriesWrapper::GetSeries()->Append(time.GetSeconds(), m_avg);
}

void
AverageValue::Update(double value)
{
    Update(Simulator::Now(), value);
}

/*          SlidingValue         */

TypeId
SlidingValue::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::SlidingValue")
            .SetParent<ns3::netsimulyzer::visualizer::SeriesWrapper> ()
            .SetGroupName ("visualizer");
    return tid;
    // clang-format on
}

SlidingValue::SlidingValue(Ptr<XYSeries> series)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

SlidingValue::SlidingValue(Ptr<XYSeries> series, Time interval)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series << interval);

    m_timer.SetDelay(interval);
    if (m_timer.GetDelay().IsPositive())
    {
        m_timed = true;
        m_timer.SetFunction(&SlidingValue::Flush, this);
        m_timer.Schedule();
    }
};

SlidingValue::SlidingValue(Ptr<XYSeries> series, double window, double maxSampleFrequency)
    : SlidingValue(series)
{
    NS_LOG_FUNCTION(this << series << maxSampleFrequency);
    m_window = window;
    m_maxSampleFrequency = maxSampleFrequency;
};

SlidingValue::SlidingValue(Ptr<XYSeries> series, double window, Time interval)
    : SlidingValue(series, interval)
{
    NS_LOG_FUNCTION(this << series << window);
    m_window = window;
};

void
SlidingValue::Update(Time time, double value)
{
    // prune back end
    for (auto pair = m_values.begin(); pair != m_values.end(); pair++)
    {
        Time t = pair->first;
        if (t < time - Seconds(m_window))
        {
            m_values.erase(pair--);
        }
    }
    m_values.push_back({time, value});
    if (!m_timed)
    {
        Append(time);
    }
};

void
SlidingValue::Update(double value)
{
    Update(Simulator::Now(), value);
}

void
SlidingValue::Append(Time time)
{
    double acc = 0;
    for (auto pair = m_values.begin(); pair != m_values.end(); pair++)
    {
        acc += pair->second;
    }
    if (m_lastSample < time.GetSeconds() - m_maxSampleFrequency)
    {
        SeriesWrapper::GetSeries()->Append(time.GetSeconds(), acc);
        m_lastSample = time.GetSeconds();
    }
}

void
SlidingValue::Flush()
{
    Append(Simulator::Now());
    m_timer.Schedule();
}

double
SlidingValue::GetSlidingValue()
{
    double acc = 0;
    for (auto pair = m_values.begin(); pair != m_values.end(); pair++)
    {
        acc += pair->second;
    }
    return acc;
}

/*          SlidingLoad         */

TypeId
SlidingLoad::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::SlidingLoad")
            .SetParent<ns3::netsimulyzer::visualizer::SlidingValue> ()
            .SetGroupName ("visualizer");
    return tid;
    // clang-format on
}

SlidingLoad::SlidingLoad(Ptr<XYSeries> series)
    : SlidingValue(series)
{
    NS_LOG_FUNCTION(this << series);
};

SlidingLoad::SlidingLoad(Ptr<XYSeries> series, Time interval)
    : SlidingValue(series, interval)
{
    NS_LOG_FUNCTION(this << series << interval);
};

SlidingLoad::SlidingLoad(Ptr<XYSeries> series,
                         double window,
                         double bandwidth,
                         double maxSampleFrequency)
    : SlidingValue(series, window, maxSampleFrequency)
{
    NS_LOG_FUNCTION(this << series << bandwidth);
    m_bandwidth = bandwidth;
};

SlidingLoad::SlidingLoad(Ptr<XYSeries> series, double window, double bandwidth, Time interval)
    : SlidingValue(series, window, interval)
{
    NS_LOG_FUNCTION(this << series << bandwidth);
    m_bandwidth = bandwidth;
};

void
SlidingLoad::Update(Time time, double value)
{
    SlidingValue::Update(time, 100.0 * value / m_bandwidth);
};

void
SlidingLoad::Update(double value)
{
    Update(Simulator::Now(), value);
}

double
SlidingLoad::GetSlidingValue()
{
    return SlidingValue::GetSlidingValue();
}

/*          SeriesMap           */

TypeId
SeriesMap::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::visualizer::SeriesMap")
            .SetParent<ns3::netsimulyzer::visualizer::SeriesContainer> ()
            .SetGroupName ("visualizer");
    return tid;
    // clang-format on
}

SeriesMap::SeriesMap(Ptr<Visualizer> visualizer)
    : SeriesContainer(visualizer)
{
    NS_LOG_FUNCTION(this << visualizer);
};

SeriesMap::SeriesMap(Ptr<Visualizer> visualizer,
                     std::string name,
                     std::string x_axis,
                     std::string y_axis)
    : SeriesContainer(visualizer, name, x_axis, y_axis)
{
    NS_LOG_FUNCTION(this << visualizer << name << x_axis << y_axis);
};

SeriesMap*
SeriesMap::AddWrapper(std::string index, Ptr<SeriesWrapper> w)
{
    NS_LOG_FUNCTION(this << index << w);
    m_nameMap.insert({index, GetNSeries()});
    SeriesContainer::AddWrapper(w);
    return this;
};

Ptr<XYSeries>
SeriesMap::GetSeries(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesContainer::GetSeries(m_nameMap.at(index));
};

Ptr<SeriesWrapper>
SeriesMap::GetWrapper(const std::string& index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesContainer::GetWrapper(m_nameMap.at(index));
};

SeriesWrapper&
SeriesMap::operator[](const std::string& index)
{
    return (*this->GetWrapper(index));
};

} // namespace visualizer

} // namespace netsimulyzer

} // namespace ns3
