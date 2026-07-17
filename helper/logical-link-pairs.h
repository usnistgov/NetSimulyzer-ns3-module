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

#ifndef LOGICAL_LINK_PAIRS_H
#define LOGICAL_LINK_PAIRS_H

#include "../model/orchestrator.h"

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/timer.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

/**
 * @ingroup netsimulyzer
 * Manages LogicalLinks between a within of nodes
 */
class LogicalLinkPairs : public Object
{
  public:
    LogicalLinkPairs(Ptr<Orchestrator> orchestrator);
    /**
     * Gets the link between the two nodes if it exists
     * @param nodeA,nodeB
     * The nodes to check for a connection between. Identical if swapped
     */
    Ptr<netsimulyzer::LogicalLink> GetLink(uint32_t nodeA, uint32_t nodeB);

    /**
     * Creates a node pair Link
     * @param nodeA,nodeB
     * The Nodes to connect. Identical if swapped
     */
    void SetLink(uint32_t nodeA, uint32_t nodeB);

    /**
     * Creates/Sets the color of a node pair Link
     * @param nodeA,nodeB
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     */
    void SetLink(uint32_t nodeA, uint32_t nodeB, Color3 color);

    /**
     * Creates/Sets the color/attributes of a node pair Link
     * @param nodeA,nodeB
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     * @param attributes
     * Attributes to be given to this link
     */
    void SetLink(uint32_t nodeA,
                 uint32_t nodeB,
                 Color3 color,
                 const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes);

    /**
     * Creates of a timed node pair Link
     * @param nodeA,nodeB
     * The Nodes to connect. Identical if swapped
     * @param duration
     * The length the link should stay activated
     */
    void SetLinkBurst(uint32_t nodeA, uint32_t nodeB, Time duration);

    /**
     * Creates/Sets the color of a timed node pair Link
     * @param nodeA,nodeB
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     * @param duration
     * The length the link should stay activated
     */
    void SetLinkBurst(uint32_t nodeA, uint32_t nodeB, Color3 color, Time duration);

    /**
     * Creates/Sets the color/attributes of a timed node pair Link
     * @param nodeA,nodeB
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     * @param attributes
     * Attributes to be given to this link
     * @param duration
     * The length the link should stay activated
     */
    void SetLinkBurst(uint32_t nodeA,
                      uint32_t nodeB,
                      Color3 color,
                      const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes,
                      Time duration);

    /**
     * If a link exists between the two nodes, deactivate it
     * @param nodeA,nodeB
     * The nodes to check for a connection between. Identical if swapped
     */
    void RemoveLink(uint32_t nodeA, uint32_t nodeB);

  private:
    struct pairHash
    {
        std::size_t operator()(const std::pair<uint32_t, uint32_t>& pair) const
        {
            uint32_t nodeA = pair.first;
            uint32_t nodeB = pair.second;
            std::size_t n = 59 * nodeA + 137 * nodeB;
            std::size_t length = n * (n - 1) / 2;

            return (nodeB - (nodeA + 1)) + (length - ((n - nodeA) * (n - nodeA - 1) / 2));
        };
    };

    /**
     * A data structure that stores the pairwise LogicalLinks
     */
    std::unordered_map<std::pair<uint32_t, uint32_t>,
                       Ptr<netsimulyzer::LogicalLink>,
                       LogicalLinkPairs::pairHash>
        m_pairMap;

    /**
     * for scheduling timed deactivations
     */
    std::unordered_map<std::pair<uint32_t, uint32_t>, Timer, LogicalLinkPairs::pairHash> m_timers;

    /**
     * Orchestrator attatched to the LogicalLinks
     */
    Ptr<Orchestrator> m_orchestrator;
};

} // namespace ns3::netsimulyzer

#endif // LOGICAL_LINK_PAIRS_H
