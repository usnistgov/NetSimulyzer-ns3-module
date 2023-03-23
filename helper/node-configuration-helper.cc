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

#include "node-configuration-helper.h"

#include <ns3/abort.h>
#include <ns3/log.h>
#include <ns3/pointer.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NodeConfigurationHelper");

namespace netsimulyzer
{

NodeConfigurationHelper::NodeConfigurationHelper(Ptr<Orchestrator> orchestrator)
    : m_orchestrator(orchestrator)
{
    Set("Orchestrator", PointerValue(orchestrator));
}

void
NodeConfigurationHelper::Set(const std::string& name, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_nodeConfiguration.Set(name, v);
}

NodeConfigurationContainer
NodeConfigurationHelper::Install(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this << node);
    auto config = m_nodeConfiguration.Create()->GetObject<NodeConfiguration>();
    node->AggregateObject(config);
    return {config};
}

NodeConfigurationContainer
NodeConfigurationHelper::Install(Ptr<Node> node, Ptr<NodeConfiguration> configuration) const
{
    NS_LOG_FUNCTION(this << node << &configuration);
    node->AggregateObject(configuration);
    return {configuration};
}

NodeConfigurationContainer
NodeConfigurationHelper::Install(NodeContainer& nodes) const
{
    NS_LOG_FUNCTION(this << &nodes);
    NodeConfigurationContainer container;

    for (auto node = nodes.Begin(); node != nodes.End(); node++)
    {
        auto config = m_nodeConfiguration.Create()->GetObject<NodeConfiguration>();

        (*node)->AggregateObject(config);

        container.Add(config);
    }

    return container;
}

NodeConfigurationContainer
NodeConfigurationHelper::Install(NodeContainer& nodes,
                                 NodeConfigurationContainer& configurations) const
{
    NS_LOG_FUNCTION(this << &nodes << &configurations);

    NS_ABORT_MSG_IF(nodes.GetN() > configurations.GetN(),
                    "Number of Nodes greater than number of NodeConfiguration objects");

    if (configurations.GetN() > nodes.GetN())
    {
        NS_LOG_WARN("Number of NodeConfiguration objects exceeds that of nodes "
                    "Only "
                    << nodes.GetN() << " Configurations will be used.");
    }

    NodeConfigurationContainer results;

    auto node = nodes.Begin();
    auto config = configurations.Begin();
    for (; node != nodes.End(); node++, config++)
    {
        (*node)->AggregateObject(*config);
        results.Add(*config);
    }

    return results;
}

} // namespace netsimulyzer
} // namespace ns3
