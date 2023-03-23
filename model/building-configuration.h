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

#ifndef BUILDING_CONFIGURATION_H
#define BUILDING_CONFIGURATION_H

#include <ns3/color.h>
#include <ns3/event-message.h>
#include <ns3/object.h>
#include <ns3/orchestrator.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include <string>

namespace ns3::netsimulyzer
{

class Orchestrator;

/**
 * Base configuration properties for an individual building
 * represented in the 3D visualizer
 */
class BuildingConfiguration : public Object
{
  public:
    /**
     * Create an unbound BuildingConfiguration.
     * This constructor is meant for use by the helper.
     *
     * If this constructor is used, then an orchestrator
     * must be set later.
     */
    BuildingConfiguration(void) = default;

    /**
     * Creates a managed BuildingConfiguration.
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this Building with
     */
    explicit BuildingConfiguration(Ptr<Orchestrator> orchestrator);

    /**
     * \brief Get the class TypeId
     *
     * \return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Sets the Orchestrator managing this Building &
     * register this Building with that Orchestrator.
     *
     * Prefer using the Orchestrator constructor as opposed to setting the
     * Orchestrator later
     *
     * \param orchestrator
     * The Orchestrator to register this BuildingConfiguration with
     */
    void SetOrchestrator(Ptr<Orchestrator> orchestrator);

    /**
     * \return
     * The Orchestrator managing this Building.
     * nullptr if it is not managed
     */
    Ptr<Orchestrator> GetOrchestrator(void) const;

  protected:
    /**
     * \brief Disconnects the referenced Orchestrator
     */
    void DoDispose(void) override;

  private:
    /**
     * Pointer to the Orchestrator managing this building
     */
    ns3::Ptr<Orchestrator> m_orchestrator;

    /**
     * The color used to shade the walls of the building
     */
    Color3 m_color;

    /**
     * Flag to show the model in the visualizer or not
     */
    bool m_visible;
};

} // namespace ns3::netsimulyzer

#endif /* BUILDING_CONFIGURATION_H */
