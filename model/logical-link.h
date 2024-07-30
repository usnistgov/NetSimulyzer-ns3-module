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

#include "color.h"
#include "optional.h"
#include "orchestrator.h"

#include <ns3/node.h>
#include <ns3/object.h>
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
     * Creates a managed LogicalLink. Using the next color in the palette
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     *
     * \param a
     * The first Node in the link
     * \param b
     * The second Node in the link
     *
     */
    LogicalLink(Ptr<Orchestrator> orchestrator, Ptr<const Node> a, Ptr<const Node> b);

    /**
     * Creates a managed LogicalLink.
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     *
     * \param a
     * The first Node in the link
     * \param b
     * The second Node in the link
     * \param color
     * The initial color of the link
     *
     */
    LogicalLink(Ptr<Orchestrator> orchestrator, Ptr<const Node> a, Ptr<const Node> b, Color3 color);

    /**
     * Creates a managed LogicalLink. Using the next color in the palette
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     *
     * \param nodeIdA
     * The first Node in the link
     * \param nodeIdB
     * The second Node in the link
     *
     */
    LogicalLink(Ptr<Orchestrator> orchestrator, uint32_t nodeIdA, uint32_t nodeIdB);

    /**
     * Creates a managed LogicalLink.
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     *
     * \param nodeIdA
     * The first Node in the link
     * \param nodeIdB
     * The second Node in the link
     * \param color
     * The initial color of the link
     *
     */
    LogicalLink(Ptr<Orchestrator> orchestrator, uint32_t nodeIdA, uint32_t nodeIdB, Color3 color);

    /**
     * Creates a managed LogicalLink.
     *
     * This constructor is used by the helper,
     * `m_ignoreSets` must be set to `true` and
     * `m_orchestrator->CreateLink(*this)` must be called by hand
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     * \param nodeIdA
     * The first Node in the link
     * \param nodeIdB
     * The second Node in the link
     * \param color
     * The initial color of the link
     * \param attributes
     * The attributes specified to the helper
     *
     */
    LogicalLink(const Ptr<Orchestrator>& orchestrator,
                uint32_t nodeIdA,
                uint32_t nodeIdB,
                Color3 color,
                const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes);

    /**
     * Creates a managed LogicalLink.
     *
     * This constructor is used by the helper,
     * `m_ignoreSets` must be set to `true` and
     * `m_orchestrator->CreateLink(*this)` must be called by hand
     *
     * \param nodeIdA
     * The first Node in the link
     *
     * \param nodeIdB
     * The second Node in the link
     *
     * \param orchestrator
     * The Orchestrator to register this LogicalLink with
     *
     * \param attributes
     * The attributes specified to the helper
     *
     */
    LogicalLink(const Ptr<Orchestrator>& orchestrator,
                uint32_t nodeIdA,
                uint32_t nodeIdB,
                const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes);

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

    [[nodiscard]] Color3 GetColor() const;
    void SetColor(Color3 value);

    [[nodiscard]] double GetDiameter() const;
    void SetDiameter(double value);

  protected:
    void NotifyConstructionCompleted() override;

  private:
    // The TypeID setting default attributes will trigger update event
    // so ignore until we get to `NotifyConstructionCompleted()`
    bool m_ignoreSets{true};
    Ptr<Orchestrator> m_orchestrator;
    unsigned long m_id;
    bool m_active{true};
    Color3 m_color{};
    double m_diameter{}; // Initialized by attribute "Diameter"
    std::pair<uint32_t, uint32_t> m_nodes{};
    /**
     * \see LogicalLink::NotifyConstructionCompleted
     */
    Color3 m_constructorColor{};
    /**
     * \see LogicalLink::NotifyConstructionCompleted
     */
    std::unordered_map<std::string, Ptr<AttributeValue>> m_constructorAttributes{};

    // Allow access to `m_ignoreSets`
    friend LogicalLinkHelper;
};

} // namespace ns3::netsimulyzer

#endif // LOGICAL_LINK_H
