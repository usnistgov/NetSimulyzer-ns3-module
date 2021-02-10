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

#ifndef BUILDING_CONFIGURATION_HELPER_H
#define BUILDING_CONFIGURATION_HELPER_H

#include <ns3/attribute.h>
#include <ns3/building.h>
#include <ns3/building-configuration.h>
#include <ns3/building-container.h>
#include <ns3/building-configuration-container.h>
#include <ns3/object-factory.h>
#include <ns3/orchestrator.h>
#include <ns3/ptr.h>

namespace ns3 {
namespace netsimulyzer {

/**
 * \ingroup netsimulyzer
 * \brief Creates, configures, and installs BuildingConfiguration objects
 */
class BuildingConfigurationHelper
{
public:
  /**
   * Sets the required orchestrator for this helper
   *
   * \param orchestrator The Orchestrator that manages the Buildings
   * configured by this helper. May not be NULL
   */
  explicit BuildingConfigurationHelper (Ptr<Orchestrator> orchestrator);

  /**
   * Sets one of the attributes of underlying model
   *
   * \param name Name of attribute to set.
   * \param v Value of the attribute.
   */
  void Set (const std::string &name, const AttributeValue &v);

  /**
   * Aggregates the configured BuildingConfiguration object onto a single Building
   *
   * \param building The Building to install the BuildingConfiguration onto
   *
   * \return A container containing the newly installed configuration object
   */
  BuildingConfigurationContainer Install (Ptr<Building> building) const;

  /**
   * Aggregates a pre-defined BuildingConfiguration onto a single Building
   * No new BuildingConfiguration objects are created
   *
   * \param building The Building to install 'configuration' onto
   *
   * \param configuration The BuildingConfiguration to install onto 'building'
   *
   * \return A container containing only 'configuration'
   */
  BuildingConfigurationContainer Install (Ptr<Building> building,
                                          Ptr<BuildingConfiguration> configuration) const;

  /**
   * Creates and Aggregates a BuildingConfiguration object for each Building in 'buildings'
   *
   * \param buildings The collection to iterate over, aggregating configuration objects on each Building
   *
   * \return A collection containing all of the created BuildingConfiguration objects
   */
  BuildingConfigurationContainer Install (BuildingContainer &buildings) const;

  /**
   * \brief Aggregates pre-defined BuildingConfiguration objects onto the Buildings in 'buildings'
   *
   * Aggregates pre-defined BuildingConfiguration objects onto the Buildings in 'buildings'
   *
   * 'buildings' and 'configurations' should be the same size.
   * No new BuildingConfiguration objects will be created by this method
   *
   * If the number of Buildings in 'buildings' exceeds the number of BuildingConfiguration objects
   * in 'configurations', this method wil abort
   *
   * If the number of BuildingConfiguration objects in 'configurations' exceeds
   * the number of Buildings in 'buildings', then the extra BuildingConfiguration objects
   * will be ignored, and a warning will be logged
   *
   * \param buildings The collection to iterate over, aggregating configuration objects on each Building.
   * May not be larger then the size of 'configurations'
   *
   * \param configurations The collection of BuildingConfiguration objects to aggregate onto 'buildings'.
   * Should not exceed the size of 'buildings'
   *
   * \return A collection containing each of the BuildingConfiguration objects that
   * were aggregated onto a Building
   */
  BuildingConfigurationContainer Install (BuildingContainer &buildings,
                                          BuildingConfigurationContainer &configurations) const;

private:
  /**
   * Factory for producing BuildingConfiguration objects
   */
  ObjectFactory m_buildingConfiguration{"ns3::netsimulyzer::BuildingConfiguration"};

  /**
   * Orchestrator that manages the Buildings produced by this helper
   */
  Ptr<Orchestrator> m_orchestrator;
};

} // namespace netsimulyzer
} // namespace ns3

#endif /* BUILDING_CONFIGURATION_HELPER_H */
