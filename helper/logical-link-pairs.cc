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

#include "logical-link-pairs.h"

#include "ns3/color.h"
#include "ns3/double.h"
#include "ns3/logical-link.h"
#include "ns3/node.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("LinkPairs");

namespace netsimulyzer
{

LogicalLinkPairs::LogicalLinkPairs(Ptr<Orchestrator> orchestrator)
    : m_orchestrator(orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator);
};

Ptr<netsimulyzer::LogicalLink>
LogicalLinkPairs::GetLink(uint32_t nodeA, uint32_t nodeB)
{
    NS_LOG_FUNCTION(this << nodeA << nodeB);
    if (nodeA > nodeB)
    {
        std::swap(nodeA, nodeB);
    }
    if (this->m_pairMap.contains({nodeA, nodeB}))
    {
        return this->m_pairMap.at({nodeA, nodeB});
    }
    return nullptr;
};

Ptr<netsimulyzer::LogicalLink>
LogicalLinkPairs::GetLink(const Ptr<Node>& nodeA, const Ptr<Node>& nodeB)
{
    return GetLink(nodeA->GetId(), nodeB->GetId());
}

Ptr<netsimulyzer::LogicalLink>
LogicalLinkPairs::GetLink(const NodeContainer& twoNodes)
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to get a LogicalLink");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN("Number of Nodes passed to `GetLink` > 2, only getting the first 2");
    }

    return GetLink(twoNodes.Get(0)->GetId(), twoNodes.Get(1)->GetId());
}

void
LogicalLinkPairs::SetLink(uint32_t nodeA,
                          uint32_t nodeB,
                          Color3 color,
                          const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes)
{
    NS_LOG_FUNCTION(this << nodeA << nodeB << color);
    if (nodeA > nodeB)
    {
        std::swap(nodeA, nodeB);
    }
    if (!m_pairMap.contains({nodeA, nodeB}))
    {
        m_pairMap.insert(
            {{nodeA, nodeB},
             CreateObject<LogicalLink>(m_orchestrator, nodeA, nodeB, color, attributes)});
    }
    else
    {
        auto link = m_pairMap.at({nodeA, nodeB});
        link->Activate();
        link->SetColor(color);
        for (const auto& [name, value] : attributes)
        {
            // In the helper, the color attribute is always converted to
            // the constructor argument, so we don't want the attribute version
            if (name == "Color")
            {
                continue;
            }
            link->SetAttribute(name, *value);
        }
    }
}

void
LogicalLinkPairs::SetLink(const Ptr<Node>& nodeA,
                          const Ptr<Node>& nodeB,
                          Color3 color,
                          const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes)
{
    SetLink(nodeA->GetId(), nodeB->GetId(), color, attributes);
}

void
LogicalLinkPairs::SetLink(const NodeContainer& twoNodes,
                          Color3 color,
                          const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes)
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to make a LogicalLink");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN("Number of Nodes passed to `SetLink` > 2, only linking the first 2");
    }

    return SetLink(twoNodes.Get(0)->GetId(), twoNodes.Get(1)->GetId(), color, attributes);
}

void
LogicalLinkPairs::SetLink(uint32_t nodeA, uint32_t nodeB, Color3 color)
{
    SetLink(nodeA, nodeB, color, {});
}

void
LogicalLinkPairs::SetLink(const Ptr<Node>& nodeA, const Ptr<Node>& nodeB, Color3 color)
{
    SetLink(nodeA->GetId(), nodeB->GetId(), color, {});
}

void
LogicalLinkPairs::SetLink(const NodeContainer& twoNodes, Color3 color)
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to make a LogicalLink");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN("Number of Nodes passed to `SetLink` > 2, only linking the first 2");
    }

    return SetLink(twoNodes.Get(0)->GetId(), twoNodes.Get(1)->GetId(), color, {});
}

void
LogicalLinkPairs::SetLink(uint32_t nodeA, uint32_t nodeB)
{
    SetLink(nodeA, nodeB, WHITE);
}

void
LogicalLinkPairs::SetLink(const Ptr<Node>& nodeA, const Ptr<Node>& nodeB)
{
    SetLink(nodeA->GetId(), nodeB->GetId(), WHITE);
}

void
LogicalLinkPairs::SetLink(const NodeContainer& twoNodes)
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to make a LogicalLink");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN("Number of Nodes passed to `SetLink` > 2, only linking the first 2");
    }

    return SetLink(twoNodes.Get(0)->GetId(), twoNodes.Get(1)->GetId(), WHITE);
}

void
LogicalLinkPairs::RemoveLink(uint32_t nodeA, uint32_t nodeB)
{
    NS_LOG_FUNCTION(this << nodeA << nodeB);
    if (nodeA > nodeB)
    {
        auto k = nodeA;
        nodeA = nodeB;
        nodeB = k;
    }
    if (m_pairMap.contains({nodeA, nodeB}))
    {
        m_pairMap.at({nodeA, nodeB})->Deactivate();
    }
}

void
LogicalLinkPairs::RemoveLink(const Ptr<Node>& nodeA, const Ptr<Node>& nodeB)
{
    RemoveLink(nodeA->GetId(), nodeB->GetId());
}

void
LogicalLinkPairs::RemoveLink(const NodeContainer& twoNodes)
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to remove a link");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN(
            "Number of Nodes passed to `RemoveLink` > 2, only removing link for the first 2");
    }

    return RemoveLink(twoNodes.Get(0)->GetId(), twoNodes.Get(1)->GetId());
}

} // namespace netsimulyzer

} // namespace ns3
