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

#include "logical-link-helper.h"

#include <ns3/color.h>
#include <ns3/logical-link.h>
#include <ns3/pointer.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("LogicalLinkHelper");

namespace netsimulyzer
{
LogicalLinkHelper::LogicalLinkHelper(Ptr<Orchestrator> orchestrator)
    : m_orchestrator{orchestrator}
{
}

void
LogicalLinkHelper::Set(const std::string& name, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this << name << &v);
    if (name.empty())
    {
        return;
    }

    TypeId::AttributeInformation info;
    if (!m_linkTid.LookupAttributeByName(name, &info))
    {
        NS_FATAL_ERROR("Invalid attribute set (" << name << ") on " << m_linkTid.GetName());
        return;
    }
    if (auto attrValue = info.checker->CreateValidValue(v); !attrValue)
    {
        NS_FATAL_ERROR("Invalid value for attribute set (" << name << ") on "
                                                           << m_linkTid.GetName());
        return;
    }

    m_attributes[name] = v.Copy();
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(const NodeContainer& twoNodes) const
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to make a LogicalLink");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN("Number of Nodes passed to `Link` > 2, only linking the first 2");
    }

    return Link(twoNodes.Get(0), twoNodes.Get(1));
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(const NodeContainer& twoNodes, const Color3 color) const
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to make a LogicalLink");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN("Number of Nodes passed to `Link` > 2, only linking the first 2");
    }

    return Link(twoNodes.Get(0), twoNodes.Get(1), color);
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(const Ptr<Node>& node1, const Ptr<Node>& node2) const
{
    NS_LOG_FUNCTION(this << node1 << node2);
    return Link(node1->GetId(), node2->GetId());
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(const Ptr<Node>& node1, const Ptr<Node>& node2, const Color3 color) const
{
    NS_LOG_FUNCTION(this << node1 << node2 << color);
    return Link(node1->GetId(), node2->GetId(), color);
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(const uint32_t node1, const uint32_t node2) const
{
    return Create(m_orchestrator, node1, node2);
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(const uint32_t node1, const uint32_t node2, const Color3 color) const
{
    return Create(m_orchestrator, node1, node2, color);
}

std::vector<Ptr<netsimulyzer::LogicalLink>> 
LogicalLinkHelper::LinkAllToNode(Ptr<Node> baseNode, NodeContainer group)
{
  std::vector<Ptr<netsimulyzer::LogicalLink>> links;
  
  if (baseNode != nullptr)
  {
    for (uint32_t i = 0; i < group.GetN(); i++)
    {
      links.push_back(Link(baseNode, group.Get(i)));
    }
  }
 
  return links;
}

std::vector<Ptr<netsimulyzer::LogicalLink>>
LogicalLinkHelper::LinkGroup(NodeContainer group)
{
  std::vector<Ptr<netsimulyzer::LogicalLink>> links;
  // link the i'th node with all the other nodes that come after it
  // thus bound is till GetN() - 1
  for (uint32_t i = 0; i < group.GetN() - 1; i++)
  {
    //the node to be connected to all the other nodes after it
    Ptr<Node> baseNode = group.Get(i);

    for (uint32_t j = i + 1; j < group.GetN(); j++)
    {
      links.push_back(Link(baseNode, group.Get(j)));
    }
  }

  return links;
}


Ptr<LogicalLink>
LogicalLinkHelper::Create(Ptr<Orchestrator> orchestrator,
                          const uint32_t nodeIdA,
                          const uint32_t nodeIdB) const
{
    // If we've got a color, don't advance the palette
    // by calling the no color constructor
    if (m_attributes.count("Color") == 1)
    {
        const auto color = DynamicCast<Color3Value>(m_attributes.at("Color"))->Get();
        return Create(orchestrator, nodeIdA, nodeIdB, color);
    }

    return CreateObject<LogicalLink>(orchestrator, nodeIdA, nodeIdB, m_attributes);
}

Ptr<LogicalLink>
LogicalLinkHelper::Create(Ptr<Orchestrator> orchestrator,
                          const uint32_t nodeIdA,
                          const uint32_t nodeIdB,
                          const Color3 color) const
{
    return CreateObject<LogicalLink>(orchestrator, nodeIdA, nodeIdB, color, m_attributes);
}

} // namespace netsimulyzer
} // namespace ns3
