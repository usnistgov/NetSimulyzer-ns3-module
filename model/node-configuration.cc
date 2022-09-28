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

#include "node-configuration.h"
#include <sstream>
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/object-base.h>
#include <ns3/pointer.h>
#include <ns3/string.h>
#include <ns3/log.h>
#include <ns3/optional.h>
#include <cmath>
#include <optional>

namespace {

/**
 * Compare each component in two vectors. If their difference of each component
 * is less than or equal to the given tolerance. Then they are equal.
 * Don't expect any serious precision out of this...
 *
 * \param left
 * The Vector to compare to `right`
 *
 * \param right
 * The Vector to compare to `left`
 *
 * \param tolerance
 * The allowed difference between any two components while still
 * considering them equal
 *
 * \return
 * True if every component is within the tolerance of each other,
 * False otherwise
 */
bool
compareWithTolerance (const ns3::Vector3D &left, const ns3::Vector3D &right, double tolerance)
{
  return (std::abs (left.x - right.x) <= tolerance) && (std::abs (left.y - right.y) <= tolerance) &&
         (std::abs (left.z - right.z) <= tolerance);
}

/**
 * Calculate the angle to rotate the netsimulyzer model to face the direction
 * given by the ray through `last` to `next`
 *
 * \param last
 * The previous position of the Node
 *
 * \param next
 * The position of the Node currently being applied
 *
 * @return
 * The Z orientation (in degrees) to rotate the model to
 */
double
faceForwardAngle (const ns3::Vector3D &last, const ns3::Vector3D &next)
{
  // clang-format off
  return std::atan2((next.y - last.y), (next.x - last.x))
             * 180/3.14 // Convert Radians to Degrees
             + 90.0; // Offset for default NetSimulyzer model orientation TODO: Adjust models to remove this
  // clang-format on
}

} // namespace

namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("NodeConfiguration");
namespace netsimulyzer {

NS_OBJECT_ENSURE_REGISTERED (NodeConfiguration);

NodeConfiguration::NodeConfiguration (Ptr<Orchestrator> orchestrator)
    : m_orchestrator (orchestrator)
{
  NS_LOG_FUNCTION (this << orchestrator);
  m_orchestrator->Register ({this, true});
}

TypeId
NodeConfiguration::GetTypeId (void)
{
  // clang-format off
  // clang-format on
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::NodeConfiguration")
          .SetParent<Object> ()
          .SetGroupName ("netsimulyzer")
          .AddConstructor<NodeConfiguration> ()
          .AddAttribute ("Name", "Name for this Node", StringValue ("Node"),
                         MakeStringAccessor (&NodeConfiguration::m_name), MakeStringChecker ())
          .AddAttribute ("Model", "Filename of the model to represent this Node", StringValue (),
                         MakeStringAccessor (&NodeConfiguration::m_model), MakeStringChecker ())
          .AddAttribute ("Orientation", "Orientation of the Node on each axis in degrees",
                         Vector3DValue (),
                         MakeVector3DAccessor (&NodeConfiguration::GetOrientation,
                                               &NodeConfiguration::SetOrientation),
                         MakeVector3DChecker ())
          .AddAttribute ("Scale",
                         "The percentage to scale the model in all directions (uniform scale)",
                         DoubleValue (1.0), MakeDoubleAccessor (&NodeConfiguration::m_scale),
                         MakeDoubleChecker<double> (0))
          .AddAttribute ("ScaleAxes",
                         "The scale to apply each axis in the order [x, y, z]. "
                         "similar to `Scale`, but allows for non-uniform scales. "
                         "Ignores `KeepRatio`",
                         Vector3DValue (Vector3D{1.0, 1.0, 1.0}),
                         MakeVector3DAccessor (&NodeConfiguration::m_scaleAxes),
                         MakeVector3DChecker ())
          .AddAttribute ("Offset", "Offset from the Node to apply to the model", Vector3DValue (),
                         MakeVector3DAccessor (&NodeConfiguration::m_positionOffset),
                         MakeVector3DChecker ())
          .AddAttribute ("FaceForward",
                         "Automatically change the 2D orientation of "
                         "the Node to face the directions pointed to "
                         "by the ray drawn from the last position to "
                         "the current one",
                         BooleanValue (false),
                         MakeBooleanAccessor (&NodeConfiguration::m_faceForward),
                         MakeBooleanChecker ())
          .AddAttribute ("KeepRatio",
                         "When scaling with the `Height`, `Width`, and `Depth` attributes, "
                         "use only the value that produces the largest model. "
                         "Keeping the scale uniform",
                         BooleanValue (true), MakeBooleanAccessor (&NodeConfiguration::m_keepRatio),
                         MakeBooleanChecker ())
          .AddAttribute ("Height",
                         "Desired height of the rendered model in ns-3 units. "
                         "Applied before `Scale`",
                         OptionalValue<double> (),
                         MakeOptionalAccessor<double> (&NodeConfiguration::m_height),
                         MakeOptionalChecker<double> ())
          .AddAttribute ("Width",
                         "Desired width of the rendered model in ns-3 units. "
                         "Applied before `Scale`",
                         OptionalValue<double> (),
                         MakeOptionalAccessor<double> (&NodeConfiguration::m_width),
                         MakeOptionalChecker<double> ())
          .AddAttribute ("Depth",
                         "Desired depth of the rendered model in ns-3 units. "
                         "Applied before `Scale`",
                         OptionalValue<double> (),
                         MakeOptionalAccessor<double> (&NodeConfiguration::m_depth),
                         MakeOptionalChecker<double> ())
          .AddAttribute ("BaseColor",
                         "The color to use as the primary color in models with configurable colors",
                         OptionalValue<Color3> (),
                         MakeOptionalAccessor<Color3> (&NodeConfiguration::GetBaseColor,
                                                       &NodeConfiguration::SetBaseColor),
                         MakeOptionalChecker<Color3> ())
          .AddAttribute ("HighlightColor",
                         "The color to use as the secondary color"
                         "in models with configurable colors",
                         OptionalValue<Color3> (),
                         MakeOptionalAccessor<Color3> (&NodeConfiguration::GetHighlightColor,
                                                       &NodeConfiguration::SetHighlightColor),
                         MakeOptionalChecker<Color3> ())
          .AddAttribute ("MotionTrailColor",
                         "The color of the optional motion trail"
                         "if unset, uses either the `BaseColor`, `HighlightColor`, or"
                         "the next color in the palette, in that order.",
                         OptionalValue<Color3> (),
                         MakeOptionalAccessor<Color3> (&NodeConfiguration::m_trailColor),
                         MakeOptionalChecker<Color3> ())
          .AddAttribute ("PositionTolerance",
                         "The amount a Node must move to have it's position written again",
                         DoubleValue (0.05),
                         MakeDoubleAccessor (&NodeConfiguration::m_positionTolerance),
                         MakeDoubleChecker<double> (0))
          .AddAttribute ("UsePositionTolerance",
                         "Only write positions when the Node has "
                         "moved beyond the 'PositionTolerance'.",
                         BooleanValue (true),
                         MakeBooleanAccessor (&NodeConfiguration::m_usePositionTolerance),
                         MakeBooleanChecker ())
          .AddAttribute ("Visible", "Defines if the Node is rendered in the visualizer",
                         BooleanValue (true), MakeBooleanAccessor (&NodeConfiguration::m_visible),
                         MakeBooleanChecker ())
          .AddAttribute ("Orchestrator", "Orchestrator that manages this Node", PointerValue (),
                         MakePointerAccessor (&NodeConfiguration::GetOrchestrator,
                                              &NodeConfiguration::SetOrchestrator),
                         MakePointerChecker<Orchestrator> ());
  return tid;
}

void
NodeConfiguration::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_orchestrator = nullptr;
  Object::DoDispose ();
}

void
NodeConfiguration::CourseChange (ns3::Ptr<const MobilityModel> model)
{
  NS_LOG_FUNCTION (this << model);
  CourseChangeEvent event;
  event.time = Simulator::Now ();
  event.nodeId = model->GetObject<Node> ()->GetId ();
  event.position = model->GetPosition ();

  m_orchestrator->HandleCourseChange (event);

  if (m_faceForward)
    SetOrientation (
        {m_orientation.x, m_orientation.y, faceForwardAngle (m_lastPosition, event.position)});

  m_lastPosition = event.position;
}

void
NodeConfiguration::Transmit (Time duration, double targetSize, Color3 color)
{
  NS_LOG_FUNCTION (this << duration << targetSize);

  // If we haven't been aggregated with a Node yet.
  // Assume we're still setting up
  const auto node = GetObject<const Node> ();
  if (!node)
    {
      NS_LOG_DEBUG ("Not triggering NodeTransmit event. No Node aggregated");
      return;
    }

  TransmitEvent event;
  event.time = Simulator::Now ();
  event.nodeId = node->GetId ();
  event.duration = duration;
  event.targetSize = targetSize;
  event.color = color;

  if (lastTransmissionEnd > event.time + event.duration)
    {
      std::stringstream ss;
      ss << "Node ID: " << event.nodeId
         << " transmission event interrupted. Expected end: " << lastTransmissionEnd
         << " Current time: " << event.time;

      NS_LOG_WARN (ss.str ());
    }

  m_orchestrator->HandleTransmit (event);
}

std::optional<Vector3D>
NodeConfiguration::MobilityPoll (void)
{
  NS_LOG_FUNCTION (this);
  auto node = GetObject<Node> ();
  NS_ABORT_MSG_IF (!node, "Mobility poll activated on NodeConfiguration with no associated Node");

  auto mobility = node->GetObject<MobilityModel> ();
  if (!mobility)
    {
      NS_LOG_DEBUG ("Mobility poll activated on Node with no Mobility Model, ignoring");
      return {};
    }

  const auto position = mobility->GetPosition ();

  if (m_usePositionTolerance &&
      compareWithTolerance (position, m_lastPosition, m_positionTolerance))
    {
      NS_LOG_DEBUG ("Node [ID: " << node->GetId () << "] Within tolerance");
      return {};
    }

  if (m_faceForward)
    SetOrientation (
        {m_orientation.x, m_orientation.y, faceForwardAngle (m_lastPosition, position)});
  m_lastPosition = position;

  return {position};
}

const Vector3D &
NodeConfiguration::GetOrientation () const
{
  NS_LOG_FUNCTION (this);
  return m_orientation;
}

void
NodeConfiguration::SetOrientation (const Vector3D &orientation)
{
  NS_LOG_FUNCTION (this << orientation);
  m_orientation = orientation;
  // If we haven't been aggregated with a Node yet.
  // Assume we're still setting up
  auto node = GetObject<Node> ();
  if (!node)
    {
      NS_LOG_DEBUG ("Not triggering NodeOrientationChanged event. No Node aggregated");
      return;
    }

  NodeOrientationChangeEvent event;
  event.time = Simulator::Now ();
  event.nodeId = node->GetId ();
  event.orientation = m_orientation;

  m_orchestrator->HandleOrientationChange (event);
}

void
NodeConfiguration::SetOrchestrator (Ptr<Orchestrator> orchestrator)
{
  NS_LOG_FUNCTION (this << orchestrator);
  m_orchestrator = orchestrator;
  m_orchestrator->Register ({this, true});
}

Ptr<Orchestrator>
NodeConfiguration::GetOrchestrator (void) const
{
  NS_LOG_FUNCTION (this);
  return m_orchestrator;
}

const std::optional<Color3> &
NodeConfiguration::GetBaseColor (void) const
{
  NS_LOG_FUNCTION (this);
  return m_baseColor;
}

void
NodeConfiguration::SetBaseColor (const std::optional<Color3> &value)
{
  NS_LOG_FUNCTION (this);
  if (m_baseColor == value)
    return;

  m_baseColor = value;

  auto node = GetObject<Node> ();
  if (!node)
    {
      NS_LOG_DEBUG ("Not triggering NodeColorChangeEvent event. No Node aggregated");
      return;
    }

  NodeColorChangeEvent event;
  event.time = Simulator::Now ();
  event.id = node->GetId ();
  event.type = NodeColorChangeEvent::ColorType::Base;
  event.color = value;

  m_orchestrator->HandleColorChange (event);
}

const std::optional<Color3> &
NodeConfiguration::GetHighlightColor (void) const
{
  NS_LOG_FUNCTION (this);
  return m_highlightColor;
}

void
NodeConfiguration::SetHighlightColor (const std::optional<Color3> &value)
{
  NS_LOG_FUNCTION (this);
  if (m_highlightColor == value)
    return;

  m_highlightColor = value;

  auto node = GetObject<Node> ();
  if (!node)
    {
      NS_LOG_DEBUG ("Not triggering NodeColorChangeEvent event. No Node aggregated");
      return;
    }

  NodeColorChangeEvent event;
  event.time = Simulator::Now ();
  event.id = node->GetId ();
  event.type = NodeColorChangeEvent::ColorType::Highlight;
  event.color = value;

  m_orchestrator->HandleColorChange (event);
}

void
NodeConfiguration::SetScale (double scale)
{
  m_scale = scale;
}

void
NodeConfiguration::SetScale (const Vector3D &scale)
{
  SetScaleAxes (scale);
}

void
NodeConfiguration::SetScaleAxes (const Vector3D &scale)
{
  m_scaleAxes = scale;
}

double
NodeConfiguration::GetScale (void) const
{
  return m_scale;
}

const Vector3D &
NodeConfiguration::GetScaleAxes (void) const
{
  return m_scaleAxes;
}

void
NodeConfiguration::NotifyNewAggregate (void)
{
  NS_LOG_FUNCTION (this);
  // Make sure we don't attach the trace more than once
  if (m_attachedMobilityTrace)
    {
      Object::NotifyNewAggregate ();
      return;
    }

  // Just in case we don't have a Node
  auto node = GetObject<Node> ();
  if (!node)
    {
      Object::NotifyNewAggregate ();
      return;
    }

  auto mobility = node->GetObject<MobilityModel> ();
  if (!mobility)
    {
      Object::NotifyNewAggregate ();
      return;
    }

  mobility->TraceConnectWithoutContext (
      "CourseChange", MakeCallback (&netsimulyzer::NodeConfiguration::CourseChange, this));
  m_attachedMobilityTrace = true;
  Object::NotifyNewAggregate ();
}

} // namespace netsimulyzer
} // namespace ns3
