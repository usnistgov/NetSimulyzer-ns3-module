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

#include "xy-series.h"
#include "series-collection.h"
#include "node-configuration.h"
#include "orchestrator.h"
#include "color.h"
#include "netsimulyzer-3D-models.h"

#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/string.h"
#include "ns3/pointer.h"



#include <string>
#include <unordered_map>
#include <vector>

#include "net-visualizer.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE("Visualizer");



namespace netsimulyzer{

namespace visualizer {
    NS_OBJECT_ENSURE_REGISTERED(SeriesWrapper);
        NS_OBJECT_ENSURE_REGISTERED(Accumulator);
    NS_OBJECT_ENSURE_REGISTERED(SeriesContainer);
        NS_OBJECT_ENSURE_REGISTERED(SeriesMap);
    NS_OBJECT_ENSURE_REGISTERED(Visualizer);


/*          SeriesWrapper           */


TypeId
SeriesWrapper::GetTypeId(void)
{
    // clang-format off
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::visualizer::SeriesCollection")
          .SetParent<ns3::Object> ()
          .SetGroupName ("visualizer");
  return tid;
    // clang-format on
}

Ptr<netsimulyzer::XYSeries>
SeriesWrapper::GetSeries() 
{
    NS_LOG_FUNCTION(this);
    return m_series;
};
SeriesWrapper::SeriesWrapper
(Ptr<netsimulyzer::XYSeries> series)
    : m_series(series)
{
    NS_LOG_FUNCTION(this << series);
};

/*          SeriesContainer           */


TypeId
SeriesContainer::GetTypeId(void)
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
    m_wrapper.emplace_back(w);
    m_collection->Add(w->GetSeries());
    return this;
};

Ptr<netsimulyzer::XYSeries>
SeriesContainer::GetSeries(uint32_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrapper.at(i)->GetSeries();
};

Ptr<SeriesWrapper>
SeriesContainer::GetWrapper(uint32_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrapper.at(i);
};

uint32_t
SeriesContainer::GetNSeries()
{
    NS_LOG_FUNCTION(this);
    return m_wrapper.size();
};

Ptr<netsimulyzer::SeriesCollection>
SeriesContainer::GetCollection()
{
    NS_LOG_FUNCTION(this);
    return m_collection;
};

SeriesContainer::SeriesContainer(Ptr<Visualizer> visualizer)
{
    NS_LOG_FUNCTION(this << visualizer);
    m_collection = CreateObject<netsimulyzer::SeriesCollection>(visualizer->GetOrchestrator());
};
SeriesContainer::SeriesContainer(Ptr<Visualizer> visualizer, std::string name, std::string x_axis, std::string y_axis)
    : SeriesContainer(visualizer) 
{
    NS_LOG_FUNCTION(this << visualizer << name << x_axis << y_axis);
    PointerValue axisValue;
    Ptr<netsimulyzer::ValueAxis> axis;
    m_collection->GetAttribute("YAxis", axisValue);
    axis = axisValue.Get<netsimulyzer::ValueAxis>();
    axis->SetAttribute("Name",StringValue(y_axis));
    m_collection->GetAttribute("XAxis", axisValue);
    axis = axisValue.Get<netsimulyzer::ValueAxis>();
    axis->SetAttribute("Name",StringValue(x_axis));
    m_collection->SetAttribute("Name",StringValue(name));
};

/*          Visualizer           */

TypeId
Visualizer::GetTypeId(void)
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
    : m_orchestrator(CreateObject<netsimulyzer::Orchestrator>(outputFileName))
    , m_configHelper(m_orchestrator)
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
    for(auto node = m_nodes.Begin(); node != m_nodes.End(); node++){                
        auto nodeConfig = CreateObject<netsimulyzer::NodeConfiguration>(m_orchestrator);
        nodeConfig->SetModel(netsimulyzer::models::SINGLE_BOARD_COMPUTER);
        nodeConfig->SetBaseColor(m_colors.at(col_index % m_colors.size()));
        nodeConfig->SetScale(3.0);
        m_configContainer.Add(nodeConfig);
        col_index++;
    }
    m_configHelper.Install(m_nodes,m_configContainer);
};

Ptr<SeriesContainer>
Visualizer::GetContainer(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return m_containers.at(index);
};

Visualizer*
Visualizer::SetContainer(std::string index,Ptr<SeriesContainer> container)
{
    NS_LOG_FUNCTION(this << index << container);
    m_containers.insert({index,container});
    return this;
};

Ptr<netsimulyzer::NodeConfiguration>
Visualizer::GetConfig(uint32_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_configContainer.Get(i);
};

netsimulyzer::Color3
Visualizer::GetColor(uint32_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_colors.at(i % m_colors.size());
};

Ptr<Node>
Visualizer::GetNode(uint32_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_nodes.Get(i);
};

Ptr<Node>
Visualizer::GetNodeById(uint32_t id)
{
    NS_LOG_FUNCTION(this << id);
    for (uint32_t i = 0; i < m_nodes.GetN(); ++i)
    {
        if (m_nodes.Get(i)->GetId() == id)
        {
            return m_nodes.Get(i);
        }
    }
    return nullptr;
}

uint32_t
Visualizer::GetNNodes(void)
{
    NS_LOG_FUNCTION(this);
    return m_nodes.GetN();
};

Ptr<netsimulyzer::Orchestrator>
Visualizer::GetOrchestrator()
{
    NS_LOG_FUNCTION(this);
    return m_orchestrator;
};

Ptr<netsimulyzer::XYSeries>
Visualizer::MakeSeries()
{
    NS_LOG_FUNCTION(this);
    return CreateObject<netsimulyzer::XYSeries>(m_orchestrator);
};

Ptr<netsimulyzer::XYSeries>
Visualizer::MakeSeries(std::string name)
{
    NS_LOG_FUNCTION(this << name);
    auto series = MakeSeries();
    series->SetAttribute("Name", StringValue(name));
    return series;
};
Ptr<netsimulyzer::XYSeries>
Visualizer::MakeSeries(std::string name,netsimulyzer::Color3 color)
{
    NS_LOG_FUNCTION(this << name << color);
    auto series = MakeSeries(name);
    series->SetAttribute("Color", netsimulyzer::Color3Value(color));
    return series;
};
Ptr<netsimulyzer::XYSeries>
Visualizer::MakeSeries(std::string name,uint32_t col_index)
{
    NS_LOG_FUNCTION(this << name << col_index);
    return MakeSeries(name,GetColor(col_index));
};

void 
Visualizer::Dump()
{
    NS_LOG_FUNCTION(this);
    std::cout << "VISUALIZER:\n" << "CONTAINERS:\n";
    for(std::unordered_map<std::string, Ptr<SeriesContainer>>::iterator helper = m_containers.begin(); helper != m_containers.end(); helper++){          
        std::cout <<"\t" << helper->first << ": " << helper->second->GetNSeries() << "\n";
    }  
};

/*          Accumulator           */

TypeId
Accumulator::GetTypeId(void)
{
    // clang-format off
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::visualizer::Accumulator")
          .SetParent<ns3::netsimulyzer::visualizer::SeriesWrapper> ()
          .SetGroupName ("visualizer");
  return tid;
    // clang-format on
}

Accumulator::Accumulator(Ptr<netsimulyzer::XYSeries> series)
    : SeriesWrapper(series)
{
    NS_LOG_FUNCTION(this << series);
};

void 
Accumulator::Update(Time now, uint32_t add)
{
    NS_LOG_FUNCTION(this << now << add);
    m_value += add;
    SeriesWrapper::GetSeries()->Append(now.GetSeconds(),m_value); 
}

/*          SeriesMap           */

TypeId
SeriesMap::GetTypeId(void)
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

SeriesMap::SeriesMap(Ptr<Visualizer> visualizer, std::string name, std::string x_axis, std::string y_axis)
    : SeriesContainer(visualizer,name,x_axis,y_axis)
{
    NS_LOG_FUNCTION(this << visualizer << name << x_axis << y_axis);
};

SeriesMap*
SeriesMap::AddWrapper(std::string index, Ptr<SeriesWrapper> w)
{
    NS_LOG_FUNCTION(this << index << w);
    m_nameMap.insert({index,GetNSeries()});
    SeriesContainer::AddWrapper(w);
    return this;
};

Ptr<netsimulyzer::XYSeries>
SeriesMap::GetSeries(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesContainer::GetSeries(m_nameMap.at(index));
};
Ptr<SeriesWrapper>
SeriesMap::GetWrapper(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesContainer::GetWrapper(m_nameMap.at(index));
};

int main(){
    return 0;
}

} //namespace visualizer

} //namespace netsimulyzer

} //namespace ns3