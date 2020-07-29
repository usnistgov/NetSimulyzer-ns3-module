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
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/object-base.h>
#include <ns3/pointer.h>
#include <ns3/string.h>
#include <ns3/log.h>
#include <cmath>

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

namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("NodeConfiguration");
namespace visualizer3d {

NS_OBJECT_ENSURE_REGISTERED (NodeConfiguration);

NodeConfiguration::NodeConfiguration (Ptr<Orchestrator> orchestrator)
    : m_orchestrator (orchestrator)
{
  m_orchestrator->Register ({this, true});
}

TypeId
NodeConfiguration::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::visualizer3d::NodeConfiguration")
          .SetParent<Object> ()
          .SetGroupName ("visualizer3d")
          .AddConstructor<NodeConfiguration> ()
          .AddAttribute ("Name", "Name for this Node", StringValue ("Node"),
                         MakeStringAccessor (&NodeConfiguration::m_name), MakeStringChecker ())
          .AddAttribute ("Model", "Filename of the model to represent this Node", StringValue (),
                         MakeStringAccessor (&NodeConfiguration::m_model), MakeStringChecker ())
          .AddAttribute ("Opacity", "Sets how transparent the model appears", DoubleValue (1.0),
                         MakeDoubleAccessor (&NodeConfiguration::m_opacity),
                         MakeDoubleChecker<double> (0, 1))
          .AddAttribute ("Orientation", "Orientation of the Node on each axis in degrees",
                         Vector3DValue (),
                         MakeVector3DAccessor (&NodeConfiguration::GetOrientation,
                                               &NodeConfiguration::SetOrientation),
                         MakeVector3DChecker ())
          .AddAttribute ("Scale", "The scale to apply to the rendered model", DoubleValue (1.0),
                         MakeDoubleAccessor (&NodeConfiguration::m_scale),
                         MakeDoubleChecker<double> (0))
          .AddAttribute ("Offset", "Offset from the Node to apply to the model", Vector3DValue (),
                         MakeVector3DAccessor (&NodeConfiguration::m_positionOffset),
                         MakeVector3DChecker ())
          .AddAttribute (
              "Height", "Desired height of the rendered model. Applied before `Scale`",
              DoubleValue (),
              MakeDoubleAccessor (&NodeConfiguration::GetHeight, &NodeConfiguration::SetHeight),
              MakeDoubleChecker<double> (0.0))
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
  m_orchestrator = nullptr;
  Object::DoDispose ();
}

void
NodeConfiguration::CourseChange (ns3::Ptr<const MobilityModel> model)
{
  CourseChangeEvent event;
  event.time = Simulator::Now ();
  event.nodeId = model->GetObject<Node> ()->GetId ();
  event.position = model->GetPosition ();

  m_orchestrator->HandleCourseChange (event);
}

MobilityPollEvent
NodeConfiguration::MobilityPoll (void)
{
  auto node = GetObject<Node> ();
  NS_ABORT_MSG_IF (!node, "Mobility poll activated on NodeConfiguration with no associated Node");

  auto mobility = node->GetObject<MobilityModel> ();
  NS_ABORT_MSG_IF (!mobility,
                   "Mobility poll activated on NodeConfiguration with no associated MobilityModel");

  auto position = mobility->GetPosition ();

  MobilityPollEvent event;
  event.position = position;
  if (m_usePositionTolerance &&
      compareWithTolerance (position, m_lastPosition, m_positionTolerance))
    {
      NS_LOG_DEBUG ("Node [ID: " << node->GetId () << "] Within tolerance");
      event.tolerance = MobilityPollEvent::ToleranceStatus::Within;
    }
  else
    {
      NS_LOG_DEBUG ("Node [ID: " << node->GetId () << "] Not Within tolerance");
      event.tolerance = MobilityPollEvent::ToleranceStatus::NotWithin;
      m_lastPosition = position;
    }

  return event;
}

const Vector3D &
NodeConfiguration::GetOrientation () const
{
  return m_orientation;
}
void
NodeConfiguration::SetOrientation (const Vector3D &orientation)
{
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

double
NodeConfiguration::GetHeight (void) const
{
  return m_height;
}

void
NodeConfiguration::SetHeight (double value)
{
  // Do not set the flag for the initial value
  if (value <= 0.0)
    return;

  m_heightSet = true;
  m_height = value;
}

bool
NodeConfiguration::HeightSet (void) const
{
  return m_heightSet;
}

void
NodeConfiguration::SetOrchestrator (Ptr<Orchestrator> orchestrator)
{
  m_orchestrator = orchestrator;
  m_orchestrator->Register ({this, false});
}

Ptr<Orchestrator>
NodeConfiguration::GetOrchestrator (void) const
{
  return m_orchestrator;
}

void
NodeConfiguration::NotifyNewAggregate (void)
{
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
      "CourseChange", MakeCallback (&visualizer3d::NodeConfiguration::CourseChange, this));
  m_attachedMobilityTrace = true;
  Object::NotifyNewAggregate ();
}

} // namespace visualizer3d
} // namespace ns3
