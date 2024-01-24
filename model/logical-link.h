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

#ifndef LOGICAL_LINK_H
#define LOGICAL_LINK_H

#include <ns3/color.h>
#include <ns3/node.h>
#include <ns3/object.h>
#include <ns3/optional.h>
#include <ns3/orchestrator.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include <optional>

namespace ns3::netsimulyzer
{

class LogicalLink : public Object
{
  public:
    /**
     * Create an unbound LogicalLink.
     * This constructor is meant for use by the helper.
     *
     * If this constructor is used, then an orchestrator
     * must be set later.
     */
    LogicalLink() = default;

    /**
     * Creates a managed LogicalLink.
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     */
    LogicalLink(Ptr<Orchestrator> orchestrator);

    /**
     * Get the class TypeId
     *
     * \return the TypeId
     */
    static TypeId GetTypeId();

    /**
     * Sets the Orchestrator managing this Logical Link &
     * register this Link with that Orchestrator.
     *
     * Prefer using the Orchestrator constructor as opposed to setting the
     * Orchestrator later
     *
     * \param orchestrator
     * The Orchestrator to register this Logical Link with
     */
    void SetOrchestrator(Ptr<Orchestrator> orchestrator);

    /**
     * \return
     * The Orchestrator managing this Building.
     * nullptr if it is not managed
     */
    [[nodiscard]] Ptr<Orchestrator> GetOrchestrator() const;

    [[nodiscard]] uint32_t GetId() const;

    void SetNodes(const std::pair<Ptr<Node>, Ptr<Node>>& nodes);

    void SetNodes(Ptr<Node> node1, Ptr<Node> node2);
    void SetNodes(uint32_t node1, uint32_t node2);
    void SetNodes(const std::pair<uint32_t, uint32_t>& nodes);

    [[nodiscard]] const std::pair<uint32_t, uint32_t>& GetNodes() const;

  private:
    Ptr<Orchestrator> m_orchestrator;
    unsigned int m_id;
    std::optional<Color3> m_color;
    std::pair<uint32_t, uint32_t> m_nodes{};
};

} // namespace ns3::netsimulyzer

#endif // LOGICAL_LINK_H
