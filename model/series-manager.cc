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

#include "series-manager.h"

#include "color.h"
#include "netsimulyzer-3D-models.h"
#include "node-configuration.h"

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

NS_LOG_COMPONENT_DEFINE("SeriesManager");

namespace netsimulyzer
{

NS_OBJECT_ENSURE_REGISTERED(SeriesManager);

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

Ptr<SeriesManager>
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

} // namespace netsimulyzer

} // namespace ns3
