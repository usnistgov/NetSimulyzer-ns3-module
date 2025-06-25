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

#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"

#include <cstdint>

namespace
{
ns3::netsimulyzer::Color3
NextLogicalLinkColor()
{
    using namespace ns3::netsimulyzer;
    static auto colorIter = COLOR_PALETTE.begin();

    if (colorIter == COLOR_PALETTE.end())
    {
        colorIter = COLOR_PALETTE.begin();
    }

    const auto& returnColor = *colorIter;
    colorIter++;

    return returnColor.Get();
}
} // namespace

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("LogicalLink");

namespace netsimulyzer
{
NS_OBJECT_ENSURE_REGISTERED(LogicalLink);

LogicalLink::LogicalLink(Ptr<Orchestrator> orchestrator, Ptr<const Node> a, Ptr<const Node> b)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{a->GetId(), b->GetId()},
      m_constructorColor{NextLogicalLinkColor()}
{
    // `CreateLink` in `NotifyConstructionCompleted`
}

LogicalLink::LogicalLink(Ptr<Orchestrator> orchestrator,
                         Ptr<const Node> a,
                         Ptr<const Node> b,
                         const Color3 color)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{a->GetId(), b->GetId()},
      m_constructorColor{color}

{
    // `CreateLink` in `NotifyConstructionCompleted`
}

LogicalLink::LogicalLink(Ptr<Orchestrator> orchestrator, uint32_t nodeIdA, uint32_t nodeIdB)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{nodeIdA, nodeIdB},
      m_constructorColor{NextLogicalLinkColor()}
{
    // `CreateLink` in `NotifyConstructionCompleted`
}

LogicalLink::LogicalLink(Ptr<Orchestrator> orchestrator,
                         uint32_t nodeIdA,
                         uint32_t nodeIdB,
                         const Color3 color)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{nodeIdA, nodeIdB},
      m_constructorColor{color}
{
    // `CreateLink` in `NotifyConstructionCompleted`
}

LogicalLink::LogicalLink(const Ptr<Orchestrator>& orchestrator,
                         const uint32_t nodeIdA,
                         const uint32_t nodeIdB,
                         const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{nodeIdA, nodeIdB},
      m_constructorAttributes{attributes}
{
    // see: `NotifyConstructionCompleted()`
    // for the attribute values
}

LogicalLink::LogicalLink(const Ptr<Orchestrator>& orchestrator,
                         const uint32_t nodeIdA,
                         const uint32_t nodeIdB,
                         const Color3 color,
                         const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes)
    : m_orchestrator{orchestrator},
      m_id{orchestrator->Register({this})},
      m_nodes{nodeIdA, nodeIdB},
      m_constructorColor{color},
      m_constructorAttributes{attributes}
{
    // see: `NotifyConstructionCompleted()`
    // for the attribute values
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
                          MakeUintegerChecker<uint64_t>())
            .AddAttribute("Color",
                          "Color to tint the rendered link "
                          "If unset, uses the next color in the palette",
                          Color3Value{},
                          MakeColor3Accessor(&LogicalLink::GetColor, &LogicalLink::SetColor),
                          MakeColor3Checker())
            .AddAttribute("Active",
                          "Flag to display this link in the application and list "
                          "it as 'Active'",
                          BooleanValue(true),
                          MakeBooleanAccessor(&LogicalLink::IsActive, &LogicalLink::SetActive),
                          MakeBooleanChecker())
            .AddAttribute("Diameter",
                          "Diameter of the link cylinder shown in the application",
                          DoubleValue(0.50),
                          MakeDoubleAccessor(&LogicalLink::GetDiameter, &LogicalLink::SetDiameter),
                          MakeDoubleChecker<double>())
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
    SetNodes(std::make_pair(node1->GetId(), node2->GetId()));
}

void
LogicalLink::SetNodes(uint32_t node1, uint32_t node2)
{
    NS_LOG_FUNCTION(this);
    SetNodes(std::make_pair(node1, node2));
}

void
LogicalLink::SetNodes(const std::pair<Ptr<Node>, Ptr<Node>>& nodes)
{
    NS_LOG_FUNCTION(this << nodes.first << nodes.second);
    SetNodes(std::make_pair(nodes.first->GetId(), nodes.second->GetId()));
}

void
LogicalLink::SetNodes(const std::pair<uint32_t, uint32_t>& nodes)
{
    NS_LOG_FUNCTION(this);
    if (nodes == m_nodes)
    {
        return;
    }

    m_nodes = nodes;

    if (m_ignoreSets)
    {
        return;
    }

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
    {
        return;
    }

    m_active = true;
    m_orchestrator->UpdateLink(*this);
}

void
LogicalLink::Deactivate()
{
    NS_LOG_FUNCTION(this);
    // Already not active, no need to signal a change
    if (!m_active)
    {
        return;
    }

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
LogicalLink::SetActive(const bool value)
{
    NS_LOG_FUNCTION(this);
    if (m_active == value)
    {
        return;
    }

    m_active = value;

    if (m_ignoreSets)
    {
        return;
    }
    m_orchestrator->UpdateLink(*this);
}

Color3
LogicalLink::GetColor() const
{
    NS_LOG_FUNCTION(this);
    return m_color;
}

void
LogicalLink::SetColor(const Color3 value)
{
    NS_LOG_FUNCTION(this);

    if (value == m_color)
    {
        return;
    }

    m_color = value;

    if (m_ignoreSets)
    {
        return;
    }

    m_orchestrator->UpdateLink(*this);
}

double
LogicalLink::GetDiameter() const
{
    return m_diameter;
}

void
LogicalLink::SetDiameter(const double value)
{
    NS_LOG_FUNCTION(this);

    if (value == m_diameter)
    {
        return;
    }

    m_diameter = value;

    if (m_ignoreSets)
    {
        return;
    }

    m_orchestrator->UpdateLink(*this);
}

void
LogicalLink::NotifyConstructionCompleted()
{
    m_ignoreSets = true;
    // Annoying hack to allow the color to be set by the constructor.
    // Since ns-3 will supply a default value and overwrite members
    // which are tied to attributes after the constructor has
    // returned
    m_color = m_constructorColor;

    for (const auto& [name, value] : m_constructorAttributes)
    {
        // In the helper, the color attribute is always converted to
        // the constructor argument, so we don't want the attribute version
        if (name == "Color")
        {
            continue;
        }
        SetAttribute(name, *value);
    }
    m_ignoreSets = false;
    Object::NotifyConstructionCompleted();

    // Handles if we're generated after the simulation starts
    m_orchestrator->CreateLink(*this);
}

} // namespace netsimulyzer
} // namespace ns3
