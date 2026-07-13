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

LogicalLinkPairs::LogicalLinkPairs(Ptr<Orchestrator> orchestrator)
    : m_orchestrator(orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator);
};

Ptr<netsimulyzer::LogicalLink>
LogicalLinkPairs::GetLink(std::size_t i, std::size_t j)
{
    if (i > j)
    {
        auto k = i;
        i = j;
        j = k;
    }
    if (this->m_pairMap.contains({i, j}))
    {
        return this->m_pairMap.at({i, j});
    }
    return nullptr;
};

void
LogicalLinkPairs::SetLink(std::size_t i,
                          std::size_t j,
                          Color3 color,
                          const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes)
{
    if (i > j)
    {
        auto k = i;
        i = j;
        j = k;
    }
    if (!m_pairMap.contains({i, j}))
    {
        m_pairMap.insert(
            {{i, j}, CreateObject<LogicalLink>(m_orchestrator, i + 1, j + 1, color, attributes)});
    }
    else
    {
        auto link = m_pairMap.at({i, j});
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
LogicalLinkPairs::SetLink(std::size_t i, std::size_t j, Color3 color)
{
    SetLink(i, j, color, {});
}

void
LogicalLinkPairs::SetLink(std::size_t i, std::size_t j)
{
    SetLink(i, j, WHITE);
}

void
LogicalLinkPairs::SetLinkBurst(
    std::size_t i,
    std::size_t j,
    Color3 color,
    const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes,
    Time duration)
{
    if (i > j)
    {
        auto k = i;
        i = j;
        j = k;
    }
    if (!m_timers.contains({i, j}))
    {
        m_timers.insert({{i, j}, {}});
    }
    Timer& timer = m_timers.at({i, j});
    SetLink(i, j, color, attributes);
    if (timer.IsRunning())
        timer.Cancel();
    timer.SetDelay(duration);
    if (timer.GetDelay().IsPositive())
    {
        timer.SetFunction(&LogicalLinkPairs::RemoveLink, this);
        timer.SetArguments<std::size_t, std::size_t>(i, j);
        timer.Schedule();
    }
}

void
LogicalLinkPairs::SetLinkBurst(std::size_t i, std::size_t j, Color3 color, Time duration)
{
    SetLinkBurst(i, j, color, {}, duration);
}

void
LogicalLinkPairs::SetLinkBurst(std::size_t i, std::size_t j, Time duration)
{
    SetLinkBurst(i, j, WHITE, {}, duration);
}

void
LogicalLinkPairs::RemoveLink(std::size_t i, std::size_t j)
{
    if (i > j)
    {
        auto k = i;
        i = j;
        j = k;
    }
    if (m_pairMap.contains({i, j}))
    {
        m_pairMap.at({i, j})->Deactivate();
    }
}

} // namespace netsimulyzer

} // namespace ns3
