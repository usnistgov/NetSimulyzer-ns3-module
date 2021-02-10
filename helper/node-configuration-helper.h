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

#ifndef NODE_CONFIGURATION_HELPER_H
#define NODE_CONFIGURATION_HELPER_H

#include <ns3/attribute.h>
#include <ns3/node.h>
#include <ns3/node-container.h>
#include <ns3/node-configuration.h>
#include <ns3/node-configuration-container.h>
#include <ns3/object-factory.h>
#include <ns3/orchestrator.h>
#include <ns3/ptr.h>

namespace ns3 {
namespace netsimulyzer {

/**
 * \ingroup netsimulyzer
 * \brief Creates, configures, and installs NodeConfiguration object
 */
class NodeConfigurationHelper
{
public:
  /**
   * Sets the required orchestrator for this helper
   *
   * \param orchestrator The Orchestrator that manages the Nodes
   * configured by this helper. May not be NULL
   */
  explicit NodeConfigurationHelper (Ptr<Orchestrator> orchestrator);

  /**
   * Sets one of the attributes of underlying model
   *
   * \param name Name of attribute to set.
   * \param v Value of the attribute.
   */
  void Set (const std::string &name, const AttributeValue &v);

  /**
   * Aggregates the configured NodeConfiguration object onto a single Node
   *
   * \param node The Node to install the NodeConfiguration onto
   *
   * \return A container containing the newly installed configuration object
   */
  NodeConfigurationContainer Install (Ptr<Node> node) const;

  /**
   * Aggregates a pre-defined NodeConfiguration onto a single Node
   * No new NodeConfiguration objects are created
   *
   * \param node The Node to install 'configuration' onto
   *
   * \param configuration The NodeConfiguration to install onto 'node'
   *
   * \return A container containing only 'configuration'
   */
  NodeConfigurationContainer Install (Ptr<Node> node, Ptr<NodeConfiguration> configuration) const;

  /**
   * Creates and Aggregates a NodeConfiguration object for each Node in 'nodes'
   *
   * \param nodes The collection to iterate over, aggregating configuration objects on each Node
   *
   * \return A collection containing all of the created NodeConfiguration objects
   */
  NodeConfigurationContainer Install (NodeContainer &nodes) const;

  /**
   * \brief Aggregates pre-defined NodeConfiguration objects onto the Nodes in 'nodes'
   *
   * Aggregates pre-defined NodeConfiguration objects onto the Nodes in 'nodes'
   *
   * 'nodes' and 'configurations' should be the same size.
   * No new NodeConfiguration objects will be created by this method
   *
   * If the number of Nodes in 'nodes' exceeds the number of NodeConfiguration objects
   * in 'configurations', this method wil abort
   *
   * If the number of NodeConfiguration objects in 'configurations' exceeds
   * the number of Nodes in 'nodes', then the extra NodeConfiguration objects
   * will be ignored, and a warning will be logged
   *
   * \param nodes The collection to iterate over, aggregating configuration objects on each Node.
   * May not be larger then the size of 'configurations'
   *
   * \param configurations The collection of NodeConfiguration objects to aggregate onto 'nodes'.
   * Should not exceed the size of 'nodes'
   *
   * \return A collection containing each of the NodeConfiguration objects that
   * were aggregated onto a Node
   */
  NodeConfigurationContainer Install (NodeContainer &nodes,
                                      NodeConfigurationContainer &configurations) const;

private:
  /**
   * Factory for producing NodeConfiguration objects
   */
  ObjectFactory m_nodeConfiguration{"ns3::netsimulyzer::NodeConfiguration"};

  /**
   * Orchestrator that manages the Nodes produced by this helper
   */
  Ptr<Orchestrator> m_orchestrator;
};

} // namespace netsimulyzer
} // namespace ns3

#endif /* NODE_CONFIGURATION_HELPER_H */
