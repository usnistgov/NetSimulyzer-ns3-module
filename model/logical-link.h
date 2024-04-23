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
class LogicalLinkHelper;
class Orchestrator;

class LogicalLink : public Object
{
  public:
    /**
     * Creates a managed LogicalLink.
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     *
     * \param a
     * \param b
     *
     * \param
     */
    LogicalLink(Ptr<Orchestrator> orchestrator,
                Ptr<const Node> a,
                Ptr<const Node> b);

    LogicalLink(Ptr<Orchestrator> orchestrator,
                uint32_t nodeIdA,
                uint32_t nodeIdB);

    /**
     * Get the class TypeId
     *
     * \return the TypeId
     */
    static TypeId GetTypeId();

    /**
     * \return
     * The Orchestrator managing this Link.
     */
    [[nodiscard]] Ptr<Orchestrator> GetOrchestrator() const;

    [[nodiscard]] uint32_t GetId() const;

    void SetNodes(Ptr<Node> node1, Ptr<Node> node2);
    void SetNodes(uint32_t node1, uint32_t node2);
    void SetNodes(const std::pair<Ptr<Node>, Ptr<Node>>& nodes);
    void SetNodes(const std::pair<uint32_t, uint32_t>& nodes);

    [[nodiscard]] const std::pair<uint32_t, uint32_t>& GetNodes() const;

    void Activate();
    void Deactivate();
    void Toggle();

    [[nodiscard]] bool IsActive() const;
    void SetActive(bool value);

    [[nodiscard]] const std::optional<Color3>& GetColor() const;
    void SetColor(std::optional<Color3> value);

  private:
    bool m_ignoreSets{false};
    Ptr<Orchestrator> m_orchestrator;
    unsigned long m_id;
    bool m_active{true};
    std::optional<Color3> m_color;
    std::pair<uint32_t, uint32_t> m_nodes{};

    // Allow access to `m_ignoreSets`
    friend LogicalLinkHelper;
};

} // namespace ns3::netsimulyzer

#endif // LOGICAL_LINK_H
