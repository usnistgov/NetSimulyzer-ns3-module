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

NS_OBJECT_ENSURE_REGISTERED(SeriesWrapper);
NS_OBJECT_ENSURE_REGISTERED(AccumulatorWrapper);
NS_OBJECT_ENSURE_REGISTERED(AverageValueWrapper);
NS_OBJECT_ENSURE_REGISTERED(SlidingValueWrapper);
NS_OBJECT_ENSURE_REGISTERED(SlidingLoadWrapper);
NS_OBJECT_ENSURE_REGISTERED(SeriesWrapperCollection);
NS_OBJECT_ENSURE_REGISTERED(SeriesWrapperMap);
NS_OBJECT_ENSURE_REGISTERED(SeriesManager);

/*          SeriesWrapper           */

TypeId
SeriesWrapper::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SeriesWrapper")
            .SetParent<ns3::Object> ()
            .SetGroupName ("netsimulyzer")
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

/*          SeriesWrapperCollection           */

TypeId
SeriesWrapperCollection::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SeriesWrapperCollection")
            .SetParent<ns3::Object> ()
            .SetGroupName ("netsimulyzer");
    return tid;
    // clang-format on
}

SeriesWrapperCollection*
SeriesWrapperCollection::AddWrapper(Ptr<SeriesWrapper> w)
{
    NS_LOG_FUNCTION(this << w);
    m_wrappers.emplace_back(w);
    m_collection->Add(w->GetSeries());
    return this;
};

Ptr<XYSeries>
SeriesWrapperCollection::GetSeries(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrappers.at(i)->GetSeries();
};

Ptr<SeriesWrapper>
SeriesWrapperCollection::GetWrapper(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrappers.at(i);
};

std::size_t
SeriesWrapperCollection::GetNSeries()
{
    return m_wrappers.size();
};

Ptr<SeriesCollection>
SeriesWrapperCollection::GetSeriesCollection()
{
    return m_collection;
};

SeriesWrapperCollection::SeriesWrapperCollection(Ptr<SeriesManager> manager)
{
    NS_LOG_FUNCTION(this << manager);
    m_collection = CreateObject<SeriesCollection>(manager->GetOrchestrator());
};

SeriesWrapperCollection::SeriesWrapperCollection(Ptr<SeriesManager> manager,
                                 std::string name,
                                 std::string x_axis,
                                 std::string y_axis)
    : SeriesWrapperCollection(manager)
{
    NS_LOG_FUNCTION(this << manager << name << x_axis << y_axis);

    m_collection->SetAttribute("Name", StringValue(name));
    m_collection->GetYAxis()->SetAttribute("Name", StringValue(y_axis));
    m_collection->GetXAxis()->SetAttribute("Name", StringValue(x_axis));
};

std::vector<Ptr<SeriesWrapper>>::iterator
SeriesWrapperCollection::begin()
{
    return m_wrappers.begin();
};

std::vector<Ptr<SeriesWrapper>>::iterator
SeriesWrapperCollection::end()
{
    return m_wrappers.end();
};

SeriesWrapper&
SeriesWrapperCollection::operator[](std::size_t i)
{
    return (*this->GetWrapper(i));
};

/*          SeriesManager           */

TypeId
SeriesManager::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SeriesManager")
            .SetParent<ns3::Object> ()
            .SetGroupName ("netsimulyzer")
            .AddAttribute("Orchestrator",
                          "The Orchestrator for the SeriesManager",
                          PointerValue(),
                          MakePointerAccessor(&SeriesManager::GetOrchestrator),
                          MakePointerChecker<Orchestrator>());
    return tid;
    // clang-format on
}

SeriesManager::SeriesManager(std::string outputFileName)
    : m_orchestrator(CreateObject<Orchestrator>(outputFileName)),
      m_configHelper(m_orchestrator)
{
    NS_LOG_FUNCTION(this << outputFileName);
};

SeriesManager::SeriesManager(std::string outputFileName, NodeContainer nodes)
    : SeriesManager(outputFileName)
{
    NS_LOG_FUNCTION(this << outputFileName << &nodes);
    SetNodes(nodes);
};

void
SeriesManager::SetNodes(NodeContainer nodes)
{
    NS_LOG_FUNCTION(this << &nodes);
    m_nodes = nodes;
    m_configContainer = m_configHelper.Install(m_nodes);
};

Ptr<SeriesWrapperCollection>
SeriesManager::GetCollection(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return m_collections.at(index);
};

SeriesWrapperCollection&
SeriesManager::operator[](std::string index)
{
    return (*GetCollection(index));
};

SeriesManager*
SeriesManager::SetContainer(std::string index, Ptr<SeriesWrapperCollection> container)
{
    NS_LOG_FUNCTION(this << index << container);
    m_collections.insert({index, container});
    return this;
};

Ptr<NodeConfiguration>
SeriesManager::GetConfig(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_configContainer.Get(i);
};

NodeConfigurationHelper*
SeriesManager::ConfigHelper()
{
    return &m_configHelper;
}


Ptr<Node>
SeriesManager::GetNode(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_nodes.Get(i);
};

Ptr<Node>
SeriesManager::GetNodeById(uint32_t id)
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
SeriesManager::GetNNodes()
{
    return m_nodes.GetN();
};

Ptr<Orchestrator>
SeriesManager::GetOrchestrator() const
{
    return m_orchestrator;
};

std::unordered_map<std::string, Ptr<SeriesWrapperCollection>>::iterator
SeriesManager::begin()
{
    return m_collections.begin();
};

std::unordered_map<std::string, Ptr<SeriesWrapperCollection>>::iterator
SeriesManager::end()
{
    return m_collections.end();
};


/*          AccumulatorWrapper           */

TypeId
AccumulatorWrapper::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::AccumulatorWrapper")
            .SetParent<ns3::netsimulyzer::SeriesWrapper> ()
            .SetGroupName ("netsimulyzer")
            .AddAttribute ("Value", "Accumulated Value",
                        DoubleValue (),
                        MakeDoubleAccessor (&AccumulatorWrapper::m_value),
                        MakeDoubleChecker<double> ());
    return tid;
    // clang-format on
}

AccumulatorWrapper::AccumulatorWrapper(Ptr<XYSeries> series)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

void
AccumulatorWrapper::Update(Time time, double add)
{
    NS_LOG_FUNCTION(this << time << add);
    m_value += add;
    SeriesWrapper::GetSeries()->Append(time.GetSeconds(), m_value);
}

void
AccumulatorWrapper::Update(double add)
{
    Update(Simulator::Now(), add);
}

/*          AverageValueWrapper           */

TypeId
AverageValueWrapper::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::AverageValueWrapper")
            .SetParent<ns3::netsimulyzer::SeriesWrapper> ()
            .SetGroupName ("netsimulyzer")
            .AddAttribute ("Value", "Average value",
                        DoubleValue (),
                        MakeDoubleAccessor (&AverageValueWrapper::m_avg),
                        MakeDoubleChecker<double> ());
    return tid;
    // clang-format on
}

AverageValueWrapper::AverageValueWrapper(Ptr<XYSeries> series)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

void
AverageValueWrapper::Update(Time time, double value)
{
    NS_LOG_FUNCTION(this << time << value);
    m_avg = ((m_n * m_avg) + value) / (m_n + 1.0);
    m_n++;
    SeriesWrapper::GetSeries()->Append(time.GetSeconds(), m_avg);
}

void
AverageValueWrapper::Update(double value)
{
    Update(Simulator::Now(), value);
}

/*          SlidingValueWrapper         */

TypeId
SlidingValueWrapper::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SlidingValueWrapper")
            .SetParent<ns3::netsimulyzer::SeriesWrapper> ()
            .SetGroupName ("netsimulyzer")
            // .AddAttribute("Window",
            //               "The window for the sliding value",
            //               TimeValue(),
            //               MakeTimeAccessor(&SlidingValueWrapper::m_window),
            //               MakeTimeChecker(Seconds(0)))
            .AddAttribute("Value",
                          "Current sliding value",
                          DoubleValue(),
                          MakeDoubleAccessor(&SlidingValueWrapper::GetSlidingValue),
                          MakeDoubleChecker<double>());
    return tid;
    // clang-format on
}

SlidingValueWrapper::SlidingValueWrapper(Ptr<XYSeries> series)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

SlidingValueWrapper::SlidingValueWrapper(Ptr<XYSeries> series, Time interval)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series << interval);

    m_timer.SetDelay(interval);
    if (m_timer.GetDelay().IsPositive())
    {
        m_timed = true;
        m_timer.SetFunction(&SlidingValueWrapper::Flush, this);
        m_timer.Schedule();
    }
};

SlidingValueWrapper::SlidingValueWrapper(Ptr<XYSeries> series, Time window, double maxSampleFrequency)
    : SlidingValueWrapper(series)
{
    NS_LOG_FUNCTION(this << series << maxSampleFrequency);
    m_window = window;
    m_maxSampleFrequency = maxSampleFrequency;
};

SlidingValueWrapper::SlidingValueWrapper(Ptr<XYSeries> series, Time window, Time interval)
    : SlidingValueWrapper(series, interval)
{
    NS_LOG_FUNCTION(this << series << window);
    m_window = window;
};

void
SlidingValueWrapper::Update(Time time, double value)
{
    // prune back end
    for (auto pair = m_values.begin(); pair != m_values.end(); pair++)
    {
        Time t = pair->first;

        if (t < time - m_window)
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
SlidingValueWrapper::Update(double value)
{
    Update(Simulator::Now(), value);
}

void
SlidingValueWrapper::Append(Time time)
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
SlidingValueWrapper::Flush()
{
    Append(Simulator::Now());
    m_timer.Schedule();
}

double
SlidingValueWrapper::GetSlidingValue() const
{
    double acc = 0;
    for (auto pair = m_values.begin(); pair != m_values.end(); pair++)
    {
        acc += pair->second;
    }
    return acc;
}

/*          SlidingLoadWrapper         */

TypeId
SlidingLoadWrapper::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SlidingLoadWrapper")
            .SetParent<ns3::netsimulyzer::SlidingValueWrapper> ()
            .SetGroupName ("netsimulyzer");
    return tid;
    // clang-format on
}

SlidingLoadWrapper::SlidingLoadWrapper(Ptr<XYSeries> series)
    : SlidingValueWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

SlidingLoadWrapper::SlidingLoadWrapper(Ptr<XYSeries> series, Time interval)
    : SlidingValueWrapper(series, interval)
{
    NS_LOG_FUNCTION(this << series << interval);
};

SlidingLoadWrapper::SlidingLoadWrapper(Ptr<XYSeries> series,
                         Time window,
                         double bandwidth,
                         double maxSampleFrequency)
    : SlidingValueWrapper(series, window, maxSampleFrequency),
      m_bandwidth(bandwidth)
{
    NS_LOG_FUNCTION(this << series << bandwidth);
};

SlidingLoadWrapper::SlidingLoadWrapper(Ptr<XYSeries> series, Time window, double bandwidth, Time interval)
    : SlidingValueWrapper(series, window, interval),
      m_bandwidth(bandwidth)
{
    NS_LOG_FUNCTION(this << series << bandwidth);
};

void
SlidingLoadWrapper::Update(Time time, double value)
{
    SlidingValueWrapper::Update(time, 100.0 * value / m_bandwidth);
};

void
SlidingLoadWrapper::Update(double value)
{
    Update(Simulator::Now(), 100.0 * value / m_bandwidth);
}

double
SlidingLoadWrapper::GetSlidingValue() const
{
    return SlidingValueWrapper::GetSlidingValue();
}

/*          SeriesWrapperMap           */

TypeId
SeriesWrapperMap::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SeriesWrapperMap")
            .SetParent<ns3::netsimulyzer::SeriesWrapperCollection> ()
            .SetGroupName ("netsimulyzer");
    return tid;
    // clang-format on
}

SeriesWrapperMap::SeriesWrapperMap(Ptr<SeriesManager> manager)
    : SeriesWrapperCollection(manager)
{
    NS_LOG_FUNCTION(this << manager);
};

SeriesWrapperMap::SeriesWrapperMap(Ptr<SeriesManager> manager,
                     std::string name,
                     std::string x_axis,
                     std::string y_axis)
    : SeriesWrapperCollection(manager, name, x_axis, y_axis)
{
    NS_LOG_FUNCTION(this << manager << name << x_axis << y_axis);
};

SeriesWrapperMap*
SeriesWrapperMap::AddWrapper(std::string index, Ptr<SeriesWrapper> w)
{
    NS_LOG_FUNCTION(this << index << w);
    m_nameMap.insert({index, GetNSeries()});
    SeriesWrapperCollection::AddWrapper(w);
    return this;
};

Ptr<XYSeries>
SeriesWrapperMap::GetSeries(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesWrapperCollection::GetSeries(m_nameMap.at(index));
};

Ptr<SeriesWrapper>
SeriesWrapperMap::GetWrapper(const std::string& index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesWrapperCollection::GetWrapper(m_nameMap.at(index));
};

SeriesWrapper&
SeriesWrapperMap::operator[](const std::string& index)
{
    return (*this->GetWrapper(index));
};

} // namespace netsimulyzer

} // namespace ns3
