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

#include "color.h"
#include "event-message.h"
#include "logical-link.h"
#include "optional.h"
#include "orchestrator.h"

#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>

#include <optional>
#include <string>

namespace ns3::netsimulyzer
{

class Orchestrator;
class LogicalLink;

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
    NodeConfiguration(void) = default;

    /**
     * Creates a managed NodeConfiguration.
     *
     * This constructor should be used by users
     *
     * \param orchestrator
     * The Orchestrator to register this Node with
     */
    explicit NodeConfiguration(Ptr<Orchestrator> orchestrator);

    /**
     * \brief Get the class TypeId
     *
     * \return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Callback called when the mobility model attached to a Node
     * triggers the 'CourseChange' trace
     *
     * \param model
     * The mobility model that triggered the trace
     */
    void CourseChange(Ptr<const MobilityModel> model);

    /**
     * Triggers in the application,
     * a bubble to grow out of the center
     * of the Node, which expands for
     * `duration` and until it reaches `targetSize`.
     * To visually indicate a transmission of
     * some sort has occurred.
     *
     * \warning Only one transmission per node
     * may be occurring at once. If another transmission
     * is triggered while one is still ongoing,
     * the ongoing transmission will be cut off, and
     * the new one will begin.
     *
     * \param duration
     * How long the transmission bubble
     * grows.
     *
     * \param targetSize
     * What size the bubble should be
     * when `duration` has passed
     *
     * \param color
     * What color to draw the transmission bubble
     */
    void Transmit(Time duration, double targetSize, Color3 color = GRAY_30);

    Ptr<LogicalLink> Link(Ptr<Node> target);

    /**
     * Called by the Orchestrator during a mobility poll.
     *
     * If `UsePositionTolerance`, is enabled this
     * method checks against the `PositionTolerance`.
     * If the Node has not moved beyond the tolerance,
     * then an empty optional is returned.
     *
     *
     * \return
     * An optional with the location of the associated Node
     * if the position should be used, an unset optional
     * otherwise
     */
    std::optional<Vector3D> MobilityPoll(void);

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
    void SetOrchestrator(Ptr<Orchestrator> orchestrator);

    /**
     * \return
     * The Orchestrator managing this Node.
     * nullptr if it is not managed
     */
    Ptr<Orchestrator> GetOrchestrator(void) const;

    /**
     * Sets the 3D model to use for the
     * Node & notifies the Orchestrator
     *
     * \param value
     * The model to use
     *
     * \see netsimulyzer-3D-models.h
     */
    void SetModel(const std::string& value);

    /**
     *
     * \return
     * The current model, or an empty string
     * if one isn't set
     */
    [[nodiscard]] const std::string& GetModel(void) const;

    /**
     * \return
     * The current orientation of the model in degrees
     */
    const Vector3D& GetOrientation() const;

    /**
     * Sets the orientation of the model & notifies the orchestrator.
     * In degrees
     *
     * \param orientation
     * The new orientation of the model
     */
    void SetOrientation(const Vector3D& orientation);

    /**
     * Gets the "Base" color for the 3D model used
     * to represent this `Node`
     *
     * \return
     * A `std::optional`, potentially containing the base color.
     * If no "Base" color has been set, then the `optional` is empty
     */
    const std::optional<Color3>& GetBaseColor(void) const;

    /**
     * Sets the "Base" color of the 3D model used to represent
     * this `Node`.
     *
     * The "Base" color is typically the color
     * of the largest section of the 3D model.
     *
     * \param value
     * The new value to set for the "Base" color.
     *
     * If the `std::optional` passed in is unset, then
     * the "Base" color is returned to its default value
     */
    void SetBaseColor(const std::optional<Color3>& value);

    /**
     * Gets the "Highlight" color for the 3D model used
     * to represent this `Node`
     *
     * \return
     * A `std::optional`, potentially containing the base color.
     * If no "Highlight" color has been set, then the `optional` is empty
     */
    const std::optional<Color3>& GetHighlightColor(void) const;

    /**
     * Sets the "Highlight" color of the 3D model used to represent
     * this `Node`.
     *
     * The "Highlight" color is typically the color
     * of the smaller details of the 3D model
     *
     * \param value
     * The new value to set for the "Highlight" color.
     *
     * If the `std::optional` passed in is unset, then
     * the "Highlight" color is returned to its default value
     */
    void SetHighlightColor(const std::optional<Color3>& value);

    /**
     * Convenience method for changing the `Scale` attribute
     *
     * \param scale
     * A new value to use for the scale. Must be greater than 0
     */
    void SetScale(double scale);

    /**
     * Convenience method for changing the `ScaleAxes` attribute.
     *
     * \param scale
     * A vector of 3 values to use for scales on each axis
     * in the order [x, y, z]. Must all be greater than 0
     */
    void SetScale(const Vector3D& scale);

    /**
     * Convenience method for changing the `ScaleAxes` attribute.
     *
     * \param scale
     * A vector of 3 values to use for scales on each axis
     * in the order [x, y, z]. Must all be greater than 0
     */
    void SetScaleAxes(const Vector3D& scale);

    /**
     * Convenience method for retrieving the `Scale` attribute. Note that
     * the model may also have other scales applied to it.
     *
     * \return
     * The current uniform scale value.
     *
     * /see GetScaleAxes()
     */
    double GetScale(void) const;

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
    const Vector3D& GetScaleAxes(void) const;

  protected:
    /**
     * \brief Disconnects the referenced Orchestrator
     */
    void DoDispose(void) override;

    void NotifyNewAggregate(void) override;

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
     * Flag to enable displaying the
     * label for this Node in the
     * application if it it
     * set to the 'Enabled Only' mode
     */
    bool m_enableLabel;

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
     * Flag to automatically adjust the orientation
     * of the Node to face the direction given
     * by the last position, and the new position
     */
    bool m_faceForward;

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
     *
     * /see m_keepRatio
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
     * Replacement base color for models with configurable
     * colors
     */
    std::optional<Color3> m_baseColor;

    /**
     * Replacement highlight color for models with configurable
     * colors
     */
    std::optional<Color3> m_highlightColor;

    /**
     * Flag to show/hide the motion trail
     * if the application is set to
     * the 'Enabled Only' mode
     */
    bool m_enableMotionTrail;

    /**
     * Color of the motion trail that
     * follows this Node in the application.
     */
    std::optional<Color3> m_trailColor;

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

    /**
     * Time the last transmission event was supposed to end.
     * Used for logging warnings about truncated transmissions
     */
    Time lastTransmissionEnd{Seconds(-1.0)};
};

} // namespace ns3::netsimulyzer

#endif /* NODE_CONFIGURATION_H */
