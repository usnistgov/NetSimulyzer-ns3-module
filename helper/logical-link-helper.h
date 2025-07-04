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
#ifndef LOGICAL_LINK_HELPER_H
#define LOGICAL_LINK_HELPER_H

#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/orchestrator.h"
#include "ns3/ptr.h"

#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

/**
 * @ingroup netsimulyzer
 * @brief Creates logical links between Nodes
 */
class LogicalLinkHelper
{
  public:
    explicit LogicalLinkHelper(Ptr<Orchestrator> orchestrator);

    /**
     * Sets one of the attributes of underlying model
     *
     * @param name Name of attribute to set.
     * @param v Value of the attribute.
     */
    void Set(const std::string& name, const AttributeValue& v);

    Ptr<LogicalLink> Link(const NodeContainer& twoNodes) const;
    Ptr<LogicalLink> Link(const NodeContainer& twoNodes, Color3 color) const;
    Ptr<LogicalLink> Link(const Ptr<Node>& node1, const Ptr<Node>& node2) const;
    Ptr<LogicalLink> Link(const Ptr<Node>& node1, const Ptr<Node>& node2, Color3 color) const;
    Ptr<LogicalLink> Link(uint32_t node1, uint32_t node2) const;
    Ptr<LogicalLink> Link(uint32_t node1, uint32_t node2, Color3 color) const;

    /**
     * Creates logical links connecting all nodes in group to baseNode.
     *
     * @param baseNode
     * The Node to link each Node in `group` to.
     *
     * @param group
     * The collection of Nodes to link to `baseNode`
     *
     * @return The collection of `LogicalLink`s from each `group` Node to `baseNode`
     * in order that they appear in `group`
     */
    std::vector<Ptr<LogicalLink>> LinkAllToNode(Ptr<Node> baseNode,
                                                const NodeContainer& group) const;

    /**
     * Creates `LogicalLink`s linking each member of `group`
     * with every other member of `group`
     *
     * @param group
     * The collection of `Node`s to link.
     * If empty, returns an empty `vector`
     *
     * @return The collection of `LogicalLinks` in order with the first `Node` in
     * the link in order that they appear in `group`.
     */
    std::vector<Ptr<LogicalLink>> LinkGroup(const NodeContainer& group) const;

  private:
    const TypeId m_linkTid{LogicalLink::GetTypeId()};
    std::unordered_map<std::string, Ptr<AttributeValue>> m_attributes;

    /**
     * Orchestrator that manages the LogicalLinks produced by this helper
     */
    Ptr<Orchestrator> m_orchestrator;

    [[nodiscard]] Ptr<LogicalLink> Create(Ptr<Orchestrator> orchestrator,
                                          uint32_t nodeIdA,
                                          uint32_t nodeIdB) const;

    [[nodiscard]] Ptr<LogicalLink> Create(Ptr<Orchestrator> orchestrator,
                                          uint32_t nodeIdA,
                                          uint32_t nodeIdB,
                                          Color3 color) const;
};

} // namespace ns3::netsimulyzer
#endif // LOGICAL_LINK_HELPER_H
