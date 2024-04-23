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

LogicalLink::LogicalLink(Ptr<Orchestrator> orchestrator,
                         Ptr<const Node> a,
                         Ptr<const Node> b)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{a->GetId(), b->GetId()}
{
    // Handles if we're generated after the simualtion starts
    m_orchestrator->CreateLink(*this);
}

LogicalLink::LogicalLink(Ptr<Orchestrator> orchestrator,
                         uint32_t nodeIdA,
                         uint32_t nodeIdB)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{nodeIdA, nodeIdB}
{
    // Handles if we're generated after the simualtion starts
    m_orchestrator->CreateLink(*this);
}

TypeId
LogicalLink::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::netsimulyzer::LogicalLink")
            .SetParent<Object>()
            .SetGroupName("netsimulyzer")
            .AddAttribute("Id",
                          "The unique ID of the Logical Link",
                          TypeId::ATTR_GET,
                          UintegerValue(0u),
                          MakeUintegerAccessor(&LogicalLink::m_id),
                          MakeUintegerChecker<unsigned long>())
            .AddAttribute("Color",
                          "Color to tint the rendered link "
                          "If unset, uses the next color in the palette",
                          OptionalValue<Color3>{},
                          MakeOptionalAccessor<Color3>(&LogicalLink::GetColor, &LogicalLink::SetColor),
                          MakeOptionalChecker<Color3>())
            .AddAttribute("Active",
                          "Flag to display this link in the application and list "
                          "it as 'Active'",
                          BooleanValue(true),
                          MakeBooleanAccessor(&LogicalLink::IsActive, &LogicalLink::SetActive),
                          MakeBooleanChecker())
            .AddAttribute("Orchestrator",
                          "Orchestrator that manages this Logical Link",
                          TypeId::ATTR_GET,
                          PointerValue(),
                          MakePointerAccessor(&LogicalLink::m_orchestrator),
                          MakePointerChecker<Orchestrator>());

    return tid;
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
    NS_LOG_FUNCTION(this);
    return m_id;
}

void
LogicalLink::SetNodes(Ptr<Node> node1, Ptr<Node> node2)
{
    NS_LOG_FUNCTION(this);
    const auto newNodes = std::make_pair(node1->GetId(), node2->GetId());
    if (newNodes == m_nodes)
        return;

    m_nodes = newNodes;
    m_orchestrator->UpdateLink(*this);
}

void
LogicalLink::SetNodes(uint32_t node1, uint32_t node2)
{
    NS_LOG_FUNCTION(this);
    const auto newNodes = std::make_pair(node1, node2);

    if (newNodes == m_nodes)
        return;

    m_nodes = newNodes;
    m_orchestrator->UpdateLink(*this);
}

void
LogicalLink::SetNodes(const std::pair<Ptr<Node>, Ptr<Node>>& nodes)
{
    NS_LOG_FUNCTION(this << nodes.first << nodes.second);
    SetNodes(std::make_pair(nodes.first->GetId(), nodes.second->GetId()));

    const auto newNodes = std::make_pair(nodes.first->GetId(), nodes.second->GetId());
    if (newNodes == m_nodes)
        return;

    m_nodes = newNodes;
    m_orchestrator->UpdateLink(*this);
}

void
LogicalLink::SetNodes(const std::pair<uint32_t, uint32_t>& nodes)
{
    NS_LOG_FUNCTION(this);
    if (nodes == m_nodes)
        return;

    m_nodes = nodes;
    m_orchestrator->UpdateLink(*this);
}

const std::pair<uint32_t, uint32_t>&
LogicalLink::GetNodes() const
{
    NS_LOG_FUNCTION(this);
    return m_nodes;
}

void
LogicalLink::Activate()
{
    NS_LOG_FUNCTION(this);
    // Already active, no need to signal a change
    if (m_active)
        return;

    m_active = true;
    m_orchestrator->UpdateLink(*this);
}

void
LogicalLink::Deactivate()
{
    NS_LOG_FUNCTION(this);
    // Already not active, no need to signal a change
    if (!m_active)
        return;

    m_active = false;
    m_orchestrator->UpdateLink(*this);
}

void
LogicalLink::Toggle()
{
    NS_LOG_FUNCTION(this);
    m_active = !m_active;
    m_orchestrator->UpdateLink(*this);
}

bool
LogicalLink::IsActive() const
{
    NS_LOG_FUNCTION(this);
    return m_active;
}

void
LogicalLink::SetActive(bool value)
{
    if (m_ignoreSets)
        return;
    NS_LOG_FUNCTION(this);
    m_active = value;
    m_orchestrator->UpdateLink(*this);
}

const std::optional<Color3>&
LogicalLink::GetColor() const
{
    NS_LOG_FUNCTION(this);
    return m_color;
}

void LogicalLink::SetColor(std::optional<Color3> value) {
  if (m_ignoreSets)
    return;
  NS_LOG_FUNCTION(this);
  if (value == m_color)
    return;

  m_color = value;
  m_orchestrator->UpdateLink(*this);
}

} // namespace netsimulyzer
} // namespace ns3
