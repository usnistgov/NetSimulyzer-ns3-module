//
// Created by evan on 1/24/24.
//

#include "logical-link.h"

#include <ns3/boolean.h>
#include <ns3/pointer.h>
#include <ns3/uinteger.h>

#include <cstdint>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("LogicalLink");

namespace netsimulyzer
{
NS_OBJECT_ENSURE_REGISTERED(LogicalLink);

LogicalLink::LogicalLink(Ptr<Orchestrator> orchestrator)
    : m_orchestrator{orchestrator}
{
    m_id = this->m_orchestrator->Register({this, true});
}

TypeId
LogicalLink::GetTypeId()
{
    static TypeId tid = TypeId("ns3::netsimulyzer::LogicalLink")
                            .SetParent<Object>()
                            .SetGroupName("netsimulyzer")
                            .AddConstructor<LogicalLink>()
                            .SetGroupName("netsimulyzer")
                            .AddAttribute("Id",
                                          "The unique ID of the Logical Link",
                                          TypeId::ATTR_GET,
                                          UintegerValue(0u),
                                          MakeUintegerAccessor(&LogicalLink::m_id),
                                          MakeUintegerChecker<unsigned int>())
                            .AddAttribute("Color",
                                          "Color to tint the rendered link "
                                          "If unset, uses the next color in the palette",
                                          OptionalValue<Color3>{},
                                          MakeOptionalAccessor<Color3>(&LogicalLink::m_color),
                                          MakeOptionalChecker<Color3>())
                            .AddAttribute("Orchestrator",
                                          "Orchestrator that manages this Logical Link",
                                          PointerValue(),
                                          MakePointerAccessor(&LogicalLink::GetOrchestrator,
                                                              &LogicalLink::SetOrchestrator),
                                          MakePointerChecker<Orchestrator>());
    return tid;
}

void
LogicalLink::SetOrchestrator(Ptr<Orchestrator> orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator);
    m_orchestrator = orchestrator;
    m_id = m_orchestrator->Register({this, true});
}

Ptr<Orchestrator>
LogicalLink::GetOrchestrator() const
{
    NS_LOG_FUNCTION(this);
    return m_orchestrator;
}

uint32_t
LogicalLink::GetId() const
{
    return m_id;
}

void
LogicalLink::SetNodes(const std::pair<Ptr<Node>, Ptr<Node>>& nodes)
{
    m_nodes = std::make_pair(nodes.first->GetId(), nodes.second->GetId());
}

void
LogicalLink::SetNodes(Ptr<Node> node1, Ptr<Node> node2)
{
    m_nodes = std::make_pair(node1->GetId(), node2->GetId());
}

void
LogicalLink::SetNodes(uint32_t node1, uint32_t node2)
{
    m_nodes = std::make_pair(node1, node2);
}

void
LogicalLink::SetNodes(const std::pair<uint32_t, uint32_t>& nodes)
{
    m_nodes = nodes;
}

const std::pair<uint32_t, uint32_t>&
LogicalLink::GetNodes() const
{
    return m_nodes;
}

} // namespace netsimulyzer
} // namespace ns3
