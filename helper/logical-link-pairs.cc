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

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("LinkPairs");

namespace netsimulyzer
{

LogicalLinkPairs::LogicalLinkPairs(Ptr<Orchestrator> orchestrator, uint32_t nodeCount)
    : m_pairMap(nodeCount),
      m_orchestrator(orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator << nodeCount);
};

Ptr<netsimulyzer::LogicalLink>
LogicalLinkPairs::GetLink(uint32_t i, uint32_t j)
{
    return this->m_pairMap.Get(i, j);
};

void
LogicalLinkPairs::SetLink(uint32_t i,
                          uint32_t j,
                          netsimulyzer::Color3 color,
                          const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes)
{
    if (i != j)
    {
        if (!m_pairMap.Get(i, j))
        {
            m_pairMap.Set(
                i,
                j,
                CreateObject<LogicalLink>(m_orchestrator, i + 1, j + 1, color, attributes));
        }
        else
        {
            auto link = m_pairMap.Get(i, j);
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
}

void
LogicalLinkPairs::SetLink(uint32_t i, uint32_t j, netsimulyzer::Color3 color)
{
    SetLink(i, j, color, {});
}

void
LogicalLinkPairs::SetLink(uint32_t i, uint32_t j)
{
    SetLink(i, j, WHITE);
}

void
LogicalLinkPairs::RemoveLink(uint32_t i, uint32_t j)
{
    if (i != j)
    {
        if (m_pairMap.Get(i, j))
        {
            m_pairMap.Get(i, j)->Deactivate();
        }
    }
}

} // namespace netsimulyzer

} // namespace ns3
