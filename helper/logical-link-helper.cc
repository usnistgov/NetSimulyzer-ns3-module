//
// Created by evan on 1/24/24.
//

#include "logical-link-helper.h"

#include "../model/logical-link.h"

#include <ns3/pointer.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("LogicalLinkHelper");

namespace netsimulyzer
{
LogicalLinkHelper::LogicalLinkHelper(Ptr<Orchestrator> orchestrator)
    : m_orchestrator{orchestrator}
{
    Set("Orchestrator", PointerValue(orchestrator));
}

void
LogicalLinkHelper::Set(const std::string& name, const AttributeValue& v)
{
    m_link.Set(name, v);
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(Ptr<Node> node1, Ptr<Node> node2)
{
    NS_LOG_FUNCTION(this << node1 << node2);
    auto link = m_link.Create()->GetObject<LogicalLink>();
    link->SetNodes(node1, node2);

    return link;
}

Ptr<LogicalLink>
LogicalLinkHelper::Link(const NodeContainer& twoNodes)
{
    NS_LOG_FUNCTION(this << &twoNodes);
    NS_ABORT_MSG_IF(twoNodes.GetN() < 2, "At least two Nodes required to make a LogicalLink");

    if (twoNodes.GetN() > 2)
    {
        NS_LOG_WARN("Number of Nodes passed to `Link` > 2, only linking the first 2");
    }

    return Link(twoNodes.Get(0), twoNodes.Get(1));
}

} // namespace netsimulyzer
} // namespace ns3
