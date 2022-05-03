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

#ifndef DECORATION_H
#define DECORATION_H

#include <string>
#include <optional>
#include <ns3/type-id.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/event-message.h>
#include <ns3/orchestrator.h>
#include <ns3/vector.h>

namespace ns3::netsimulyzer {

class Orchestrator;

/**
 * Represents a model in the visualizer with no ns-3 behavior, 'for show'
 */
class Decoration : public Object
{
public:
  /**
   * Sets up the Decoration and assigns the ID
   *
   * \param orchestrator
   * The orchestrator to tie this Decoration to
   */
  explicit Decoration (Ptr<Orchestrator> orchestrator);

  /**
   * \brief Get the class TypeId
   *
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \return
   * The current position of the model
   */
  const Vector3D &GetPosition () const;

  /**
   * Sets the position of the model & notifies the orchestrator.
   *
   * \param orientation
   * The new position of the model
   */
  void SetPosition (const Vector3D &position);

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
   * Convenience method for changing the `Scale` attribute
   *
   * \param scale
   * A new value to use for the scale. Must be greater than 0
   */
  void SetScale (double scale);

  /**
   * Convenience method for changing the `ScaleAxes` attribute.
   *
   * \param scale
   * A vector of 3 values to use for scales on each axis
   * in the order [x, y, z]. Must all be greater than 0
   */
  void SetScale (const Vector3D &scale);

  /**
   * Convenience method for changing the `ScaleAxes` attribute.
   *
   * \param scale
   * A vector of 3 values to use for scales on each axis
   * in the order [x, y, z]. Must all be greater than 0
   */
  void SetScaleAxes (const Vector3D &scale);

  /**
   * Convenience method for retrieving the `Scale` attribute. Note that
   * the model may also have other scales applied to it.
   *
   * \return
   * The current uniform scale value.
   *
   * /see GetScaleAxes()
   */
  double GetScale (void) const;

  /**
   * Convenience method for retrieving the `ScaleAxes` attribute. Note that
   * the model may also have other scales applied to it.
   *
   * \return
   * The current non-uniform scale values for each axis,
   * in the order [x, y, z].
   *
   * /see GetScale()
   */
  const Vector3D &GetScaleAxes (void) const;

protected:
  /**
   * \brief Disconnects the referenced Orchestrator
   */
  void DoDispose (void) override;

private:
  /**
   * Unique ID that ties Decorations to their events.
   * Assigned by the `Orchestrator`
   */
  uint32_t m_id;

  /**
   * Pointer to the Orchestrator managing this Decoration
   */
  ns3::Ptr<Orchestrator> m_orchestrator;

  /**
   * Path to the model installed in the visualizer used to represent this Decoration
   */
  std::string m_model;

  /**
   * Orientation of the Decoration on each axis in degrees
   */
  Vector3D m_orientation;

  /**
   * Position of the Decoration
   */
  Vector3D m_position;

  /**
   * Flag for use with `Height`, `Width`, and `Depth`
   * attributes. When set, if more than one of the
   * mentioned attributes are set, then only the
   * largest scale takes effect. Keeping the
   * scale uniform
   */
  bool m_keepRatio;

  /**
   * Desired height of the rendered 3D model
   * in ns-3 units
   */
  std::optional<double> m_height;

  /**
   * Desired width of the rendered 3D model
   * in ns-3 units
   *
   * /see m_keepRatio
   */
  std::optional<double> m_width;

  /**
   * Desired depth of the rendered 3D model
   * in ns-3 units
   *
   * /see m_keepRatio
   */
  std::optional<double> m_depth;

  /**
   * The amount to resize the model with 1.0 being the default size,
   * 0.5 being 1/2 size, etc.
   */
  double m_scale;

  /**
   * Similar to `m_scale`, but for each axis. In the order [x, y, z].
   *
   * A value of [1.25, 1, 1] will scale the model up by 25% on the X
   * axis, and keep the other axes the same size
   *
   * Allows for non-uniform scales
   *
   * \see m_scale
   */
  Vector3D m_scaleAxes;
};

} // namespace ns3::netsimulyzer

#endif /* DECORATION_H */
