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

#include "orchestrator.h"
#include "node-configuration.h"
#include "building-configuration.h"
#include "netsimulyzer-version.h"
#include "xy-series.h"
#include <string>
#include <vector>
#include <map>
#include <ns3/abort.h>
#include <ns3/node.h>
#include <ns3/ptr.h>
#include <ns3/mobility-model.h>
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/uinteger.h>
#include <ns3/pointer.h>
#include <ns3/enum.h>
#include <ns3/object-base.h>
#include <ns3/rectangle.h>
#include <ns3/string.h>
#include <ns3/log.h>
#include <ns3/vector.h>
#include <ns3/log-stream.h>
#include <ns3/color.h>
#include <ns3/optional.h>
#include <ns3/point-to-point-channel.h>
#include <ns3/point-to-point-net-device.h>

namespace {
std::string
ScaleToString (int scale)
{
  switch (scale)
    {
    case ns3::netsimulyzer::ValueAxis::Scale::Linear:
      return "linear";
    case ns3::netsimulyzer::ValueAxis::Scale::Logarithmic:
      return "logarithmic";
    default:
      NS_ABORT_MSG ("Unhandled ns3::netsimulyzer::ValueAxis::Scale: " << scale);
    }
}

std::string
BoundModeToString (int mode)
{
  switch (mode)
    {
    case ns3::netsimulyzer::ValueAxis::BoundMode::Fixed:
      return "fixed";
    case ns3::netsimulyzer::ValueAxis::BoundMode::HighestValue:
      return "highest value";
    default:
      NS_ABORT_MSG ("Unhandled ns3::netsimulyzer::ValueAxis::BoundMode: " << mode);
    }
}

nlohmann::json
colorToObject (const ns3::netsimulyzer::Color3 &color)
{
  nlohmann::json object;

  object["red"] = color.red;
  object["green"] = color.green;
  object["blue"] = color.blue;

  return object;
}

nlohmann::json
pointToObject (double x, double y)
{
  nlohmann::json object;

  object["x"] = x;
  object["y"] = y;

  return object;
}

nlohmann::json
makeAxisAttributes (ns3::Ptr<ns3::netsimulyzer::ValueAxis> axis)
{
  nlohmann::json element;

  ns3::StringValue name;
  axis->GetAttribute ("Name", name);
  element["name"] = name.Get ();

  ns3::DoubleValue min;
  axis->GetAttribute ("Minimum", min);
  element["min"] = min.Get ();

  ns3::DoubleValue max;
  axis->GetAttribute ("Maximum", max);
  element["max"] = max.Get ();

  ns3::EnumValue scaleMode;
  axis->GetAttribute ("Scale", scaleMode);
  element["scale"] = ScaleToString (scaleMode.Get ());

  ns3::EnumValue boundMode;
  axis->GetAttribute ("BoundMode", boundMode);
  element["bound-mode"] = BoundModeToString (boundMode.Get ());

  return element;
}

nlohmann::json
makeAxisAttributes (ns3::Ptr<ns3::netsimulyzer::CategoryAxis> axis)
{
  nlohmann::json element;

  ns3::StringValue name;
  axis->GetAttribute ("Name", name);
  element["name"] = name.Get ();

  const auto &values = axis->GetValues ();
  auto jsonValues = nlohmann::json::array ();
  for (const auto &value : values)
    {
      auto object = nlohmann::json::object ();
      object["id"] = value.first;
      object["value"] = value.second;
      jsonValues.emplace_back (object);
    }
  element["values"] = jsonValues;

  return element;
}

} // namespace

namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("Orchestrator");
namespace netsimulyzer {

NS_OBJECT_ENSURE_REGISTERED (Orchestrator);

Orchestrator::Orchestrator (const std::string &output_path) : m_outputPath (output_path)
{
  NS_LOG_FUNCTION (this << output_path);
  m_file.open (output_path);
  NS_ABORT_MSG_IF (!m_file, "Failed to open output file");

  // Preallocate collections since a `Commit ()` call
  // could come at any time
  m_document["series"] = nlohmann::json::array ();
  m_document["streams"] = nlohmann::json::array ();

  // Create the Empty events array, so we can just append to that
  // when the event happens
  m_document["events"] = nlohmann::json::array ();

  Simulator::ScheduleNow (&Orchestrator::SetupSimulation, this);
}

ns3::TypeId
Orchestrator::GetTypeId (void)
{
  // clang-format off
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::Orchestrator")
          .SetParent<ns3::Object> ()
          .SetGroupName ("netsimulyzer")
          .AddAttribute ("TimeStep",
                         "Number of milliseconds a single step in the application will represent",
                         OptionalValue<int> (),
                         MakeOptionalAccessor<int> (&Orchestrator::m_timeStep),
                         MakeOptionalChecker<int> ())
          .AddAttribute ("MobilityPollInterval", "How often to poll Nodes for their position",
                         TimeValue (MilliSeconds (100)),
                         MakeTimeAccessor (&Orchestrator::m_mobilityPollInterval),
                         MakeTimeChecker ())
          .AddAttribute ("PollMobility", "Flag to toggle polling for Node positions",
                         BooleanValue (true), MakeBooleanAccessor (&Orchestrator::GetPollMobility,
                                                                   &Orchestrator::SetPollMobility),
                         MakeBooleanChecker ())
          .AddAttribute ("StartTime", "Beginning of the window to write trace information",
                         TimeValue (), MakeTimeAccessor (&Orchestrator::m_startTime),
                         MakeTimeChecker ())
          .AddAttribute ("StopTime", "End of the window to write trace information", TimeValue (Time::Max()),
                         MakeTimeAccessor (&Orchestrator::m_stopTime), MakeTimeChecker ());

  return tid;
  // clang-format on
}

void
Orchestrator::SetupSimulation (void)
{
  NS_LOG_FUNCTION (this);
  // Header
  auto version = nlohmann::json{};
  version["major"] = VERSION_MAJOR;
  version["minor"] = VERSION_MINOR;
  version["patch"] = VERSION_PATCH;
  version["suffix"] = VERSION_SUFFIX;
  m_document["configuration"]["module-version"] = version;
  if (m_timeStep)
    m_document["configuration"]["time-step"] = m_timeStep.value ();

  // Nodes
  std::multimap<unsigned int, unsigned int> deviceLinkMap;
  auto nodes = nlohmann::json::array ();
  for (const auto &config : m_nodes)
    {
      const auto node = config->GetObject<Node> ();

      nlohmann::json element;
      element["type"] = "node";
      const auto nodeId = node->GetId ();
      element["id"] = nodeId;

      StringValue name;
      config->GetAttribute ("Name", name);
      element["name"] = name.Get ();

      StringValue model;
      config->GetAttribute ("Model", model);
      element["model"] = model.Get ();

      DoubleValue scale;
      config->GetAttribute ("Scale", scale);
      Vector3DValue scaleAxes;
      config->GetAttribute ("ScaleAxes", scaleAxes);
      auto outputScale = nlohmann::json::object ();
      outputScale["x"] = scale.Get () * scaleAxes.Get ().x;
      outputScale["y"] = scale.Get () * scaleAxes.Get ().y;
      outputScale["z"] = scale.Get () * scaleAxes.Get ().z;

      element["scale"] = outputScale;

      auto targetScale = nlohmann::json::object ();
      BooleanValue keepRatio;
      config->GetAttribute ("KeepRatio", keepRatio);
      targetScale["keep-ratio"] = keepRatio.Get ();

      OptionalValue<double> height;
      config->GetAttribute ("Height", height);
      if (height)
        targetScale["height"] = height.GetValue ();

      OptionalValue<double> width;
      config->GetAttribute ("Width", width);
      if (width)
        targetScale["width"] = width.GetValue ();

      OptionalValue<double> depth;
      config->GetAttribute ("Depth", depth);
      if (depth)
        targetScale["depth"] = depth.GetValue ();
      element["target-scale"] = targetScale;

      OptionalValue<Color3> baseColor;
      config->GetAttribute ("BaseColor", baseColor);
      if (baseColor)
        element["base-color"] = colorToObject (baseColor.GetValue ());

      OptionalValue<Color3> highlightColor;
      config->GetAttribute ("HighlightColor", highlightColor);
      if (highlightColor)
        element["highlight-color"] = colorToObject (highlightColor.GetValue ());

      Vector3DValue orientation;
      config->GetAttribute ("Orientation", orientation);
      element["orientation"]["x"] = orientation.Get ().x;
      element["orientation"]["y"] = orientation.Get ().y;
      element["orientation"]["z"] = orientation.Get ().z;

      Vector3DValue offset;
      config->GetAttribute ("Offset", offset);
      element["offset"]["x"] = offset.Get ().x;
      element["offset"]["y"] = offset.Get ().y;
      element["offset"]["z"] = offset.Get ().z;

      BooleanValue visible;
      config->GetAttribute ("Visible", visible);
      element["visible"] = visible.Get ();

      const auto mobility = node->GetObject<MobilityModel> ();
      if (mobility)
        {
          const auto position = mobility->GetPosition ();
          element["position"]["x"] = position.x;
          element["position"]["y"] = position.y;
          element["position"]["z"] = position.z;
        }
      else
        {
          // Without a position, we can't do much better then showing at the origin
          element["position"]["x"] = 0.0;
          element["position"]["y"] = 0.0;
          element["position"]["z"] = 0.0;
        }

      // Check NetDevices for p2p links
      for (auto i = 0u; i < node->GetNDevices (); i++)
        {
          const auto device = node->GetDevice (i);

          // We only support Point-to-Point links for now
          if (!device->IsPointToPoint ())
            continue;

          auto baseChannel = device->GetChannel ();
          if (!baseChannel)
            continue;

          auto p2pChannel = DynamicCast<PointToPointChannel> (baseChannel);
          if (!p2pChannel)
            continue;

          for (auto j = 0u; j < p2pChannel->GetNDevices (); j++)
            {
              auto channelNode = p2pChannel->GetDevice (j)->GetNode ();
              if (channelNode->GetId () == nodeId)
                continue;

              // Check to make sure the remote Node is configured for display
              // If not, then ignore the link as we can't display it
              if (channelNode->GetObject<NodeConfiguration> () == nullptr)
                continue;

              // Check to see if we've already written this link
              // from the other devices perspective
              const auto otherNodeId = channelNode->GetId ();
              auto otherNode = deviceLinkMap.find (otherNodeId);
              // If we've already recorded the other pointing to
              // this node, then there's no need to duplicate
              if (otherNode != deviceLinkMap.end () && otherNode->second == nodeId)
                continue;

              deviceLinkMap.insert ({nodeId, otherNodeId});
            }
        }

      nodes.emplace_back (element);
    }
  m_document["nodes"] = nodes;

  auto links = nlohmann::json::array ();
  for (const auto &[key, value] : deviceLinkMap)
    {
      nlohmann::json element;
      element["type"] = "point-to-point";

      auto linkNodes = nlohmann::json::array ();
      linkNodes.emplace_back (key);
      linkNodes.emplace_back (value);
      element["node-ids"] = linkNodes;

      links.emplace_back (element);
    }
  m_document["links"] = links;

  // Buildings
  auto buildings = nlohmann::json::array ();
  for (const auto &config : m_buildings)
    {
      const auto building = config->GetObject<Building> ();

      nlohmann::json element;
      element["type"] = "building";

      Color3Value color;
      config->GetAttribute ("Color", color);
      element["color"] = colorToObject (color.Get ());

      BooleanValue visible;
      config->GetAttribute ("Visible", visible);
      element["visible"] = visible.Get ();

      element["id"] = building->GetId ();
      element["floors"] = building->GetNFloors ();

      element["rooms"]["x"] = building->GetNRoomsX ();
      element["rooms"]["y"] = building->GetNRoomsY ();

      const auto &bounds = building->GetBoundaries ();

      element["bounds"]["x"]["min"] = bounds.xMin;
      element["bounds"]["x"]["max"] = bounds.xMax;

      element["bounds"]["y"]["min"] = bounds.yMin;
      element["bounds"]["y"]["max"] = bounds.yMax;

      element["bounds"]["z"]["min"] = bounds.zMin;
      element["bounds"]["z"]["max"] = bounds.zMax;
      buildings.emplace_back (element);
    }
  m_document["buildings"] = buildings;

  // Decorations
  auto decorations = nlohmann::json::array ();
  for (const auto &decoration : m_decorations)
    {
      nlohmann::json element;
      element["type"] = "decoration";

      UintegerValue id;
      decoration->GetAttribute ("Id", id);
      element["id"] = id.Get ();

      StringValue model;
      decoration->GetAttribute ("Model", model);
      element["model"] = model.Get ();

      Vector3DValue orientation;
      decoration->GetAttribute ("Orientation", orientation);
      element["orientation"]["x"] = orientation.Get ().x;
      element["orientation"]["y"] = orientation.Get ().y;
      element["orientation"]["z"] = orientation.Get ().z;

      Vector3DValue position;
      decoration->GetAttribute ("Position", position);
      element["position"]["x"] = position.Get ().x;
      element["position"]["y"] = position.Get ().y;
      element["position"]["z"] = position.Get ().z;

      DoubleValue scale;
      decoration->GetAttribute ("Scale", scale);
      Vector3DValue scaleAxes;
      decoration->GetAttribute ("ScaleAxes", scaleAxes);
      auto outputScale = nlohmann::json::object ();
      outputScale["x"] = scale.Get () * scaleAxes.Get ().x;
      outputScale["y"] = scale.Get () * scaleAxes.Get ().y;
      outputScale["z"] = scale.Get () * scaleAxes.Get ().z;

      element["scale"] = outputScale;

      auto targetScale = nlohmann::json::object ();
      BooleanValue keepRatio;
      decoration->GetAttribute ("KeepRatio", keepRatio);
      targetScale["keep-ratio"] = keepRatio.Get ();

      OptionalValue<double> height;
      decoration->GetAttribute ("Height", height);
      if (height)
        targetScale["height"] = height.GetValue ();

      OptionalValue<double> width;
      decoration->GetAttribute ("Width", width);
      if (width)
        targetScale["width"] = width.GetValue ();

      OptionalValue<double> depth;
      decoration->GetAttribute ("Depth", depth);
      if (depth)
        targetScale["depth"] = depth.GetValue ();
      element["target-scale"] = targetScale;

      decorations.emplace_back (element);
    }
  m_document["decorations"] = decorations;

  // Series
  for (const auto &xYSeries : m_xYSeries)
    {
      xYSeries->Commit ();
    }
  for (const auto &categorySeries : m_categorySeries)
    {
      categorySeries->Commit ();
    }
  for (const auto &seriesCollection : m_seriesCollections)
    {
      seriesCollection->Commit ();
    }

  // Streams
  for (const auto &stream : m_streams)
    {
      stream->Commit ();
    }

  // Areas
  auto areas = nlohmann::json::array ();
  for (const auto &area : m_areas)
    {
      nlohmann::json element;
      element["type"] = "rectangular-area";

      UintegerValue id;
      area->GetAttribute ("Id", id);
      element["id"] = id.Get ();

      StringValue name;
      area->GetAttribute ("Name", name);
      if (name.Get ().empty ())
        element["name"] = "Area: " + std::to_string (id.Get ());
      else
        element["name"] = name.Get ();

      RectangleValue boundsValue;
      area->GetAttribute ("Bounds", boundsValue);
      auto bounds = boundsValue.Get ();

      auto points = nlohmann::json::array ();

      // Counter clockwise order is important here
      // 1        4
      //   |----|
      // | |    | ^
      // V |    | |
      //   |----|
      // 2  ->    3

      // Left Top (1)
      points.emplace_back (pointToObject (bounds.xMin, bounds.yMax));

      // Left Bottom (2)
      points.emplace_back (pointToObject (bounds.xMin, bounds.yMin));

      // Right Bottom (3)
      points.emplace_back (pointToObject (bounds.xMax, bounds.yMin));

      // Right Top (4)
      points.emplace_back (pointToObject (bounds.xMax, bounds.yMax));

      element["points"] = points;

      DoubleValue height;
      area->GetAttribute ("Height", height);
      element["height"] = height.Get ();

      EnumValue fillMode;
      area->GetAttribute ("Fill", fillMode);

      switch (fillMode.Get ())
        {
        case RectangularArea::DrawMode::Solid:
          element["fill-mode"] = "solid";
          break;
        case RectangularArea::DrawMode::Hidden:
          element["fill-mode"] = "hidden";
          break;
        default:
          NS_ABORT_MSG ("Unhandled RectangularArea::DrawMode: " << fillMode.Get ());
          break;
        }

      EnumValue borderMode;
      area->GetAttribute ("Border", borderMode);

      switch (borderMode.Get ())
        {
        case RectangularArea::DrawMode::Solid:
          element["border-mode"] = "solid";
          break;
        case RectangularArea::DrawMode::Hidden:
          element["border-mode"] = "hidden";
          break;
        default:
          NS_ABORT_MSG ("Unhandled RectangularArea::DrawMode: " << borderMode.Get ());
          break;
        }

      Color3Value fillColor;
      area->GetAttribute ("FillColor", fillColor);
      element["fill-color"] = colorToObject (fillColor.Get ());

      Color3Value borderColor;
      area->GetAttribute ("BorderColor", borderColor);
      element["border-color"] = colorToObject (borderColor.Get ());

      areas.emplace_back (element);
    }

  m_document["areas"] = areas;

  // We're out of the initial config now
  // Everything else is an event
  m_currentSection = Orchestrator::Section::Events;

  NS_ABORT_MSG_IF (m_startTime > m_stopTime, "StopTime must be after StartTime");

  // This method should be called immediately after the simulation starts,
  // so using the Start Time as the delay should be fine
  if (m_pollMobility && !m_mobilityPollEvent.has_value ())
    m_mobilityPollEvent = Simulator::Schedule (m_startTime, &Orchestrator::PollMobility, this);

  Simulator::ScheduleDestroy (&Orchestrator::Flush, this);
}

void
Orchestrator::SetPollMobility (bool enable)
{
  NS_LOG_FUNCTION (this);
  m_pollMobility = enable;

  if (m_pollMobility && !m_mobilityPollEvent.has_value ())
    {
      if (Simulator::Now () > m_stopTime)
        return;
      else if (Simulator::Now () >= m_startTime)
        m_mobilityPollEvent = Simulator::ScheduleNow (&Orchestrator::PollMobility, this);
      else
        m_mobilityPollEvent = Simulator::Schedule (Simulator::Now () - m_startTime,
                                                   &Orchestrator::PollMobility, this);
    }

  else if (!m_pollMobility && m_mobilityPollEvent.has_value ())
    {
      Simulator::Cancel (m_mobilityPollEvent.value ());
      m_mobilityPollEvent.reset ();
    }
}

bool
Orchestrator::GetPollMobility () const
{
  NS_LOG_FUNCTION (this);
  return m_pollMobility;
}

void
Orchestrator::PollMobility (void)
{
  NS_LOG_FUNCTION (this);
  // Stop the polling if we've passed StopTime
  // StartTime addressed in scheduling
  if (Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("PollMobility() Activated past StopTime, Ignoring");
      m_mobilityPollEvent.reset ();
      return;
    }

  for (const auto &config : m_nodes)
    {
      const auto node = config->GetObject<Node> ();

      auto position = config->MobilityPoll ();
      if (position)
        WritePosition (node->GetId (), Simulator::Now (), position.value ());
    }

  m_mobilityPollEvent =
      Simulator::Schedule (m_mobilityPollInterval, &Orchestrator::PollMobility, this);
}

void
Orchestrator::WritePosition (uint32_t nodeId, Time time, Vector3D position)
{
  NS_LOG_FUNCTION (this << nodeId << time << position);
  nlohmann::json element;
  element["type"] = "node-position";
  element["milliseconds"] = time.GetMilliSeconds ();
  element["id"] = nodeId;
  element["x"] = position.x;
  element["y"] = position.y;
  element["z"] = position.z;
  m_document["events"].emplace_back (element);
}

void
Orchestrator::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Flush ();
  m_xYSeries.clear ();
  m_categorySeries.clear ();
  m_seriesCollections.clear ();
  m_decorations.clear ();
  m_nodes.clear ();
  m_buildings.clear ();
  m_streams.clear ();
  m_areas.clear ();
  Object::DoDispose ();
}

void
Orchestrator::HandleCourseChange (const CourseChangeEvent &event)
{
  NS_LOG_FUNCTION (this);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("HandleCourseChange() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  WritePosition (event.nodeId, event.time, event.position);
}

void
Orchestrator::HandlePositionChange (const DecorationMoveEvent &event)
{
  NS_LOG_FUNCTION (this);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("HandlePositionChange() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  if (m_currentSection != Section::Events)
    {
      // We'll get the final orientation when we write the head info
      NS_LOG_DEBUG ("DecorationMoveEvent ignored. Not in Events section");
      return;
    }

  nlohmann::json element;
  element["type"] = "decoration-position";
  element["milliseconds"] = event.time.GetMilliSeconds ();
  element["id"] = event.id;
  element["x"] = event.position.x;
  element["y"] = event.position.y;
  element["z"] = event.position.z;

  m_document["events"].emplace_back (element);
}

void
Orchestrator::HandleOrientationChange (const NodeOrientationChangeEvent &event)
{
  NS_LOG_FUNCTION (this);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("HandleOrientationChange() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  if (m_currentSection != Section::Events)
    {
      // We'll get the final orientation when we write the head info
      NS_LOG_DEBUG ("NodeOrientationChangeEvent ignored. Not in Events section");
      return;
    }

  nlohmann::json element;
  element["type"] = "node-orientation";
  element["milliseconds"] = event.time.GetMilliSeconds ();
  element["id"] = event.nodeId;
  element["x"] = event.orientation.x;
  element["y"] = event.orientation.y;
  element["z"] = event.orientation.z;

  m_document["events"].emplace_back (element);
}

void
Orchestrator::HandleOrientationChange (const DecorationOrientationChangeEvent &event)
{
  NS_LOG_FUNCTION (this);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("HandleOrientationChange() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  if (m_currentSection != Section::Events)
    {
      // We'll get the final orientation when we write the head info
      NS_LOG_DEBUG ("DecorationOrientationChangeEvent ignored. Not in Events section");
      return;
    }

  nlohmann::json element;
  element["type"] = "decoration-orientation";
  element["milliseconds"] = event.time.GetMilliSeconds ();
  element["id"] = event.id;
  element["x"] = event.orientation.x;
  element["y"] = event.orientation.y;
  element["z"] = event.orientation.z;
  m_document["events"].emplace_back (element);
}

void
Orchestrator::HandleColorChange (const NodeColorChangeEvent &event)
{
  NS_LOG_FUNCTION (this);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("HandleColorChange() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  if (m_currentSection != Section::Events)
    {
      // We'll get the final color when we write the head info
      NS_LOG_DEBUG ("NodeColorChangeEvent ignored. Not in Events section");
      return;
    }

  nlohmann::json element;
  element["type"] = "node-color";
  element["milliseconds"] = event.time.GetMilliSeconds ();
  element["id"] = event.id;
  switch (event.type)
    {
    case NodeColorChangeEvent::ColorType::Base:
      element["color-type"] = "base";
      break;
    case NodeColorChangeEvent::ColorType::Highlight:
      element["color-type"] = "highlight";
      break;
    default:
      NS_ABORT_MSG ("Unhandled ColorType passed to HandleColorChange ()");
      break;
    }

  if (event.color.has_value ())
    element["color"] = colorToObject (event.color.value ());

  m_document["events"].emplace_back (element);
}

void
Orchestrator::HandleTransmit (const TransmitEvent &event)
{
  NS_LOG_FUNCTION (this);

  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("HandleTransmit() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  nlohmann::json element;
  element["type"] = "node-transmit";
  element["milliseconds"] = event.time.GetMilliSeconds ();
  element["id"] = event.nodeId;
  element["duration"] = event.duration.GetMilliSeconds();
  element["target-size"] = event.targetSize;
  element["color"] = colorToObject (event.color);

  m_document["events"].emplace_back (element);
}

uint32_t
Orchestrator::Register (Ptr<Decoration> decoration)
{
  NS_LOG_FUNCTION (this << decoration);
  m_decorations.emplace_back (decoration);

  // Just in case
  return static_cast<uint32_t> (m_decorations.size ());
}

uint32_t
Orchestrator::Register (Ptr<XYSeries> series)
{
  NS_LOG_FUNCTION (this << series);
  m_xYSeries.emplace_back (series);

  return m_nextSeriesId++;
}

uint32_t
Orchestrator::Register (Ptr<CategoryValueSeries> series)
{
  NS_LOG_FUNCTION (this << series);
  m_categorySeries.emplace_back (series);

  return m_nextSeriesId++;
}

uint32_t
Orchestrator::Register (Ptr<SeriesCollection> series)
{
  NS_LOG_FUNCTION (this << series);
  m_seriesCollections.emplace_back (series);

  return m_nextSeriesId++;
}

void
Orchestrator::Register (Ptr<NodeConfiguration> nodeConfiguration)
{
  NS_LOG_FUNCTION (this << nodeConfiguration);
  m_nodes.emplace_back (nodeConfiguration);
}

void
Orchestrator::Register (Ptr<BuildingConfiguration> buildingConfiguration)
{
  NS_LOG_FUNCTION (this << buildingConfiguration);
  m_buildings.emplace_back (buildingConfiguration);
}

uint32_t
Orchestrator::Register (Ptr<LogStream> stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_streams.emplace_back (stream);

  return static_cast<uint32_t> (m_streams.size ());
}

uint32_t
Orchestrator::Register (Ptr<RectangularArea> area)
{
  NS_LOG_FUNCTION (this << area);
  m_areas.emplace_back (area);

  return static_cast<uint32_t> (m_areas.size ());
}

void
Orchestrator::Commit (XYSeries &series)
{
  NS_LOG_FUNCTION (this);
  nlohmann::json element;
  element["type"] = "xy-series";

  UintegerValue id;
  series.GetAttribute ("Id", id);
  element["id"] = id.Get ();

  StringValue name;
  series.GetAttribute ("Name", name);
  // Handle blank names somewhat gracefully
  if (name.Get ().empty ())
    element["name"] = "XY Series: " + std::to_string (id.Get ());
  else
    element["name"] = name.Get ();

  StringValue legend;
  series.GetAttribute ("Legend", legend);
  // Use the name if we don't have a specific legend name
  if (legend.Get ().empty ())
    element["legend"] = element["name"].get<std::string> ();
  else
    element["legend"] = legend.Get ();

  BooleanValue visible;
  series.GetAttribute ("Visible", visible);
  element["visible"] = visible.Get ();

  EnumValue connection;
  series.GetAttribute ("Connection", connection);
  switch (connection.Get ())
    {
    case XYSeries::ConnectionType::None:
      element["connection"] = "none";
      break;
    case XYSeries::ConnectionType::Line:
      element["connection"] = "line";
      break;
    case XYSeries::ConnectionType::Spline:
      element["connection"] = "spline";
      break;
    default:
      NS_ABORT_MSG ("Unhandled XY Series connection type: " << connection.Get ());
      break;
    }

  EnumValue labelMode;
  series.GetAttribute ("LabelMode", labelMode);
  switch (labelMode.Get ())
    {
    case XYSeries::LabelMode::Hidden:
      element["labels"] = "hidden";
      break;
    case XYSeries::LabelMode::Shown:
      element["labels"] = "shown";
      break;
    }

  Color3Value color;
  series.GetAttribute ("Color", color);
  element["color"] = colorToObject (color.Get ());

  // X Axis
  PointerValue xAxisAttr;
  series.GetAttribute ("XAxis", xAxisAttr);
  auto xAxis = xAxisAttr.Get<ValueAxis> ();
  element["x-axis"] = makeAxisAttributes (xAxis);

  // Y Axis
  PointerValue yAxisAttr;
  series.GetAttribute ("YAxis", yAxisAttr);
  auto yAxis = yAxisAttr.Get<ValueAxis> ();
  element["y-axis"] = makeAxisAttributes (yAxis);

  m_document["series"].emplace_back (element);
}

void
Orchestrator::Commit (SeriesCollection &series)
{
  NS_LOG_FUNCTION (this);
  nlohmann::json element;
  element["type"] = "series-collection";

  UintegerValue id;
  series.GetAttribute ("Id", id);
  element["id"] = id.Get ();

  StringValue name;
  series.GetAttribute ("Name", name);
  // Handle blank names somewhat gracefully
  if (name.Get ().empty ())
    element["name"] = "Series Collection: " + std::to_string (id.Get ());
  else
    element["name"] = name.Get ();

  // X Axis
  PointerValue xAxisAttr;
  series.GetAttribute ("XAxis", xAxisAttr);
  auto xAxis = xAxisAttr.Get<ValueAxis> ();
  element["x-axis"] = makeAxisAttributes (xAxis);

  // Y Axis
  PointerValue yAxisAttr;
  series.GetAttribute ("YAxis", yAxisAttr);
  auto yAxis = yAxisAttr.Get<ValueAxis> ();
  element["y-axis"] = makeAxisAttributes (yAxis);

  element["child-series"] = series.GetSeriesIds ();

  m_document["series"].emplace_back (element);
}

void
Orchestrator::Commit (CategoryValueSeries &series)
{
  NS_LOG_FUNCTION (this);
  nlohmann::json element;
  element["type"] = "category-value-series";

  UintegerValue id;
  series.GetAttribute ("Id", id);
  element["id"] = id.Get ();

  StringValue name;
  series.GetAttribute ("Name", name);
  // Handle blank names somewhat gracefully
  if (name.Get ().empty ())
    element["name"] = "Category Value Series: " + std::to_string (id.Get ());
  else
    element["name"] = name.Get ();

  StringValue legend;
  series.GetAttribute ("Legend", legend);
  // Use the name if we don't have a specific legend name
  if (legend.Get ().empty ())
    element["legend"] = element["name"].get<std::string> ();
  else
    element["legend"] = legend.Get ();

  BooleanValue visible;
  series.GetAttribute ("Visible", visible);
  element["visible"] = visible.Get ();

  Color3Value color;
  series.GetAttribute ("Color", color);
  element["color"] = colorToObject (color.Get ());

  // X Axis
  PointerValue xAxisAttr;
  series.GetAttribute ("XAxis", xAxisAttr);
  auto xAxis = xAxisAttr.Get<ValueAxis> ();
  element["x-axis"] = makeAxisAttributes (xAxis);

  // Y Axis
  PointerValue yAxisAttr;
  series.GetAttribute ("YAxis", yAxisAttr);
  auto yAxis = yAxisAttr.Get<CategoryAxis> ();
  element["y-axis"] = makeAxisAttributes (yAxis);

  BooleanValue autoUpdate;
  series.GetAttribute ("AutoUpdate", autoUpdate);
  element["auto-update"] = autoUpdate.Get ();
  if (autoUpdate.Get ())
    {
      TimeValue interval;
      series.GetAttribute ("AutoUpdateInterval", interval);
      element["auto-update-interval"] = interval.Get ().GetMilliSeconds ();

      DoubleValue value;
      series.GetAttribute ("AutoUpdateIncrement", value);
      element["auto-update-increment"] = value.Get ();
    }

  m_document["series"].emplace_back (element);
}

void
Orchestrator::Commit (LogStream &logStream)
{
  NS_LOG_FUNCTION (this);
  nlohmann::json element;
  element["type"] = "stream";

  UintegerValue id;
  logStream.GetAttribute ("Id", id);
  element["id"] = id.Get ();

  StringValue name;
  logStream.GetAttribute ("Name", name);
  if (name.Get ().empty ())
    element["name"] = std::string ("Log: ") + std::to_string (id.Get ());
  else
    element["name"] = name.Get ();

  OptionalValue<Color3> color;
  logStream.GetAttribute ("Color", color);
  if (color)
    element["color"] = colorToObject (color.GetValue ());

  BooleanValue visible;
  logStream.GetAttribute ("Visible", visible);
  element["visible"] = visible.Get ();

  m_document["streams"].emplace_back (element);
}

void
Orchestrator::AppendXyValue (uint32_t id, double x, double y)
{
  NS_LOG_FUNCTION (this << id << x << y);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("AppendXyValue() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  nlohmann::json element;
  element["type"] = "xy-series-append";
  element["milliseconds"] = Simulator::Now ().GetMilliSeconds ();
  element["series-id"] = id;
  element["x"] = x;
  element["y"] = y;
  m_document["events"].emplace_back (element);
}

void
Orchestrator::AppendXyValues (uint32_t id, const std::vector<XYPoint> &points)
{
  NS_LOG_FUNCTION (this << id);

  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("AppendXyValue() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  nlohmann::json element;
  element["type"] = "xy-series-append-array";
  element["milliseconds"] = Simulator::Now ().GetMilliSeconds ();
  element["series-id"] = id;

  auto elementArray = nlohmann::json::array ();
  for (const auto &point : points)
    {
      elementArray.emplace_back (nlohmann::json{
          {"x", point.x},
          {"y", point.y},
      });
    }

  element["points"] = elementArray;
  m_document["events"].emplace_back (element);
}

void
Orchestrator::ClearXySeries (uint32_t id)
{
  NS_LOG_FUNCTION (this << id);

  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("ClearXySeries() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  nlohmann::json element;
  element["type"] = "xy-series-clear";
  element["milliseconds"] = Simulator::Now ().GetMilliSeconds ();
  element["series-id"] = id;
  m_document["events"].emplace_back (element);
}

void
Orchestrator::AppendCategoryValue (uint32_t id, int category, double value)
{
  NS_LOG_FUNCTION (this << id << category << value);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("AppendCategoryValue() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  nlohmann::json element;
  element["type"] = "category-series-append";
  element["milliseconds"] = Simulator::Now ().GetMilliSeconds ();
  element["series-id"] = id;
  element["category"] = category;
  element["value"] = value;
  m_document["events"].emplace_back (element);
}

void
Orchestrator::WriteLogMessage (const LogMessageEvent &event)
{
  NS_LOG_FUNCTION (this);
  if (Simulator::Now () < m_startTime || Simulator::Now () > m_stopTime)
    {
      NS_LOG_DEBUG ("WriteLogMessage() Activated outside (StartTime, StopTime), Ignoring");
      return;
    }

  nlohmann::json element;
  element["milliseconds"] = event.time.GetMilliSeconds ();
  element["type"] = "stream-append";
  element["stream-id"] = event.id;
  element["data"] = event.message;

  m_document["events"].emplace_back (element);
}
void
Orchestrator::Flush (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_file.is_open () || !m_file.good ())
    {
      NS_LOG_DEBUG ("Flush() called on closed file");
      return;
    }

  // Make all models are in the document
  CommitAll ();

  // Inform the application of the actual end time
  // using the Stop Time if it was set
  m_document["configuration"]["max-time-ms"] =
      std::min (m_stopTime.GetMilliSeconds (), Simulator::Now ().GetMilliSeconds ());

  m_file << m_document;
  m_file.close ();
}

void
Orchestrator::CommitAll (void)
{
  NS_LOG_FUNCTION (this);
  // Just go through and commit everything
  // since extra commits have no effect

  for (const auto &xYSeries : m_xYSeries)
    {
      xYSeries->Commit ();
    }
  for (const auto &categorySeries : m_categorySeries)
    {
      categorySeries->Commit ();
    }
  for (const auto &seriesCollection : m_seriesCollections)
    {
      seriesCollection->Commit ();
    }

  for (const auto &stream : m_streams)
    {
      stream->Commit ();
    }
}

} // namespace netsimulyzer
} // namespace ns3
