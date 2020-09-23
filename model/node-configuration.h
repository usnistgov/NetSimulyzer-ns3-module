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

#ifndef NODE_CONFIGURATION_H
#define NODE_CONFIGURATION_H

#include <string>
#include <ns3/type-id.h>
#include <ns3/object.h>
#include <ns3/mobility-model.h>
#include <ns3/ptr.h>
#include <ns3/event-message.h>
#include <ns3/orchestrator.h>
#include <ns3/vector.h>

namespace ns3 {
namespace visualizer3d {

class Orchestrator;

/**
 * Base configuration properties for an individual node
 * represented in the 3D visualizer
 */
class NodeConfiguration : public Object
{
public:
  /**
   * Create an unbound NodeConfiguration.
   * This constructor is meant for use by the helper.
   *
   * If this constructor is used, then an orchestrator
   * must be set later.
   */
  NodeConfiguration (void) = default;

  /**
   * Creates a managed NodeConfiguration.
   *
   * This constructor should be used by users
   *
   * \param orchestrator
   * The Orchestrator to register this Node with
   */
  explicit NodeConfiguration (Ptr<Orchestrator> orchestrator);

  /**
   * \brief Get the class TypeId
   *
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   *
   * \param model
   */
  void CourseChange (Ptr<const MobilityModel> model);

  /**
   * Called by the Orchestrator during a mobility poll.
   *
   * \return
   * The location of the associated Node
   */
  MobilityPollEvent MobilityPoll (void);

  /**
   * Sets the Orchestrator managing this Node &
   * register this Node with that Orchestrator.
   *
   * Prefer using the Orchestrator constructor as opposed to setting the
   * Orchestrator later
   *
   * \param orchestrator
   * The Orchestrator to register this NodeConfiguration with
   */
  void SetOrchestrator (Ptr<Orchestrator> orchestrator);

  /**
   * \return
   * The Orchestrator managing this Node.
   * nullptr if it is not managed
   */
  Ptr<Orchestrator> GetOrchestrator (void) const;

  /**
   * \return
   * The current orientation of the model in degrees
   */
  const Vector3D &GetOrientation () const;

  /**
   * Sets the orientation of the model & notifies the orchestrator.
   * In degrees
   *
   * \param orientation
   * The new orientation of the model
   */
  void SetOrientation (const Vector3D &orientation);

  /**
   * \return
   * The set desired height if `heightSet()` is true.
   * The default value otherwise
   */
  double GetHeight (void) const;

  /**
   * Set the desired height & mark that the value should be written
   *
   * \param value
   * The desired height of the model in ns-3 units
   */
  void SetHeight (double value);

  /**
   * \return
   * True if `Height` was set and should be read. False otherwise
   */
  bool HeightSet (void) const;

protected:
  /**
   * \brief Disconnects the referenced Orchestrator
   */
  void DoDispose (void) override;

  void NotifyNewAggregate (void) override;

private:
  /**
   * Pointer to the Orchestrator managing this node
   */
  ns3::Ptr<Orchestrator> m_orchestrator;

  /**
   * Name used for this node
   */
  std::string m_name;

  /**
   * Path to the model installed in the visualizer used to represent this node
   */
  std::string m_model;

  /**
   * Orientation of the Node on each axis in degrees
   */
  Vector3D m_orientation;

  /**
   * The last written position of the Node.
   */
  Vector3D m_lastPosition;

  /**
   * The offset from Node's position
   * to apply to the rendered model
   */
  Vector3D m_positionOffset;

  /**
   * Desired height of the rendered 3D model
   * in ns-3 units
   */
  double m_height;

  /**
   * Flag indicating the optional attribute `m_height`
   * was set and should be output
   */
  bool m_heightSet{false};

  /**
   * The amount to resize the model with 1.0 being the default size,
   * 0.5 being 1/2 size, etc.
   */
  double m_scale;

  /**
   * The amount a Node must move to have it's position written again.
   * Used only if m_usePositionTolerance it true.
   */
  double m_positionTolerance;

  /**
   * Use m_positionTolerance to only write different Node positions.
   * Set to false to write positions on every poll.
   */
  bool m_usePositionTolerance;

  /**
   * Flag to show the model in the visualizer or not
   */
  bool m_visible;

  /**
   * Flag tracking if we've connected the CourseChanged callback to a MobilityModel
   */
  bool m_attachedMobilityTrace = false;
};

} // namespace visualizer3d
} // namespace ns3

#endif /* NODE_CONFIGURATION_H */
