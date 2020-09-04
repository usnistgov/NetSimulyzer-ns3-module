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
#include "visualizer3d-version.h"
#include <string>
#include <vector>
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

namespace {
std::string
ScaleToString (int scale)
{
  switch (scale)
    {
    case ns3::visualizer3d::ValueAxis::Scale::Linear:
      return "linear";
    case ns3::visualizer3d::ValueAxis::Scale::Logarithmic:
      return "logarithmic";
    default:
      NS_ABORT_MSG ("Unhandled ns3::visualizer3d::ValueAxis::Scale: " << scale);
    }
}

std::string
BoundModeToString (int mode)
{
  switch (mode)
    {
    case ns3::visualizer3d::ValueAxis::BoundMode::Fixed:
      return "fixed";
    case ns3::visualizer3d::ValueAxis::BoundMode::HighestValue:
      return "highest value";
    default:
      NS_ABORT_MSG ("Unhandled ns3::visualizer3d::ValueAxis::BoundMode: " << mode);
    }
}

nlohmann::json
colorToObject (const ns3::visualizer3d::Color3 &color)
{
  nlohmann::json object;

  object["red"] = color.red;
  object["green"] = color.green;
  object["blue"] = color.blue;

  return object;
}

nlohmann::json
colorToObject (const ns3::visualizer3d::Color4 &color)
{
  nlohmann::json object;

  object["red"] = color.red;
  object["green"] = color.green;
  object["blue"] = color.blue;
  object["alpha"] = color.alpha;

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
makeAxisAttributes (ns3::Ptr<ns3::visualizer3d::ValueAxis> axis)
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
makeAxisAttributes (ns3::Ptr<ns3::visualizer3d::CategoryAxis> axis)
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

namespace visualizer3d {

NS_OBJECT_ENSURE_REGISTERED (Orchestrator);

Orchestrator::Orchestrator (const std::string &output_path) : m_outputPath (output_path)
{
  m_file.open (output_path);
  NS_ABORT_MSG_IF (!m_file, "Failed to open output file");

  // Preallocate collections since a `Commit ()` call
  // could come at any time
  m_document["series"] = nlohmann::json::array ();

  // Create the Empty events array, so we can just append to that
  // when the event happens
  m_document["events"] = nlohmann::json::array ();

  Simulator::ScheduleNow (&Orchestrator::SetupSimulation, this);
}

ns3::TypeId
Orchestrator::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::visualizer3d::Orchestrator")
          .SetParent<ns3::Object> ()
          .SetGroupName ("visualizer3d")
          .AddAttribute ("MillisecondsPerFrame",
                         "Number of milliseconds a single frame in the visualizer will represent",
                         DoubleValue (),
                         MakeDoubleAccessor (&Orchestrator::m_millisecondsPerFrame),
                         MakeDoubleChecker<double> (0.0), TypeId::DEPRECATED,
                         "Set playback speed in the application 'Settings' dialog")
          .AddAttribute ("MobilityPollInterval", "How often to poll Nodes for their position",
                         TimeValue (MilliSeconds (100)),
                         MakeTimeAccessor (&Orchestrator::m_mobilityPollInterval),
                         MakeTimeChecker ())
          .AddAttribute ("PollMobility", "Flag to toggle polling for Node positions",
                         BooleanValue (true), MakeBooleanAccessor (&Orchestrator::m_pollMobility),
                         MakeBooleanChecker ())
          .AddAttribute ("StartTime", "Beginning of the window to write trace information",
                         TimeValue (), MakeTimeAccessor (&Orchestrator::m_startTime),
                         MakeTimeChecker ())
          .AddAttribute ("StopTime", "End of the window to write trace information", TimeValue (),
                         MakeTimeAccessor (&Orchestrator::m_stopTime), MakeTimeChecker ());

  return tid;
}

void
Orchestrator::SetupSimulation (void)
{
  // Header
  auto version = nlohmann::json{};
  version["major"] = VERSION_MAJOR;
  version["minor"] = VERSION_MINOR;
  version["patch"] = VERSION_PATCH;
  m_document["configuration"]["module-version"] = version;

  // Nodes
  auto nodes = nlohmann::json::array ();
  for (const auto &config : m_nodes)
    {
      const auto node = config->GetObject<Node> ();

      nlohmann::json element;
      element["type"] = "node";
      element["id"] = node->GetId ();

      StringValue name;
      config->GetAttribute ("Name", name);
      element["name"] = name.Get ();

      StringValue model;
      config->GetAttribute ("Model", model);
      element["model"] = model.Get ();

      DoubleValue scale;
      config->GetAttribute ("Scale", scale);
      element["scale"] = scale.Get ();

      if (config->HeightSet ())
        {
          DoubleValue height;
          config->GetAttribute ("Height", height);
          element["height"] = height.Get ();
        }

      DoubleValue opacity;
      config->GetAttribute ("Opacity", opacity);
      element["opacity"] = opacity.Get ();

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
      nodes.emplace_back (element);
    }
  m_document["nodes"] = nodes;

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

      DoubleValue opacity;
      decoration->GetAttribute ("Opacity", opacity);
      element["opacity"] = opacity.Get ();

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
      element["scale"] = scale.Get ();

      if (decoration->HeightSet ())
        {
          DoubleValue height;
          decoration->GetAttribute ("Height", height);
          element["height"] = height.Get ();
        }

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
  auto streams = nlohmann::json::array ();
  for (const auto &stream : m_streams)
    {
      nlohmann::json element;
      element["type"] = "stream";

      UintegerValue id;
      stream->GetAttribute ("Id", id);
      element["id"] = id.Get ();

      StringValue name;
      stream->GetAttribute ("Name", name);
      if (name.Get ().empty ())
        element["name"] = std::string ("Log: ") + std::to_string (id.Get ());
      else
        element["name"] = name.Get ();

      if (stream->IsColorSet ())
        {
          Color3Value color;
          stream->GetAttribute ("Color", color);
          element["color"] = colorToObject (color.Get ());
        }

      BooleanValue visible;
      stream->GetAttribute ("Visible", visible);
      element["visible"] = visible.Get ();

      streams.emplace_back (element);
    }
  m_document["streams"] = streams;

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

  // Write all the spooled logs
  for (const auto &logEvent : m_earlyLogs)
    {
      WriteLogMessage (logEvent);
    }
  m_earlyLogs.clear ();
  // Just in case we had lots
  m_earlyLogs.shrink_to_fit ();

  Simulator::ScheduleNow (&Orchestrator::PollMobility, this);

  Simulator::ScheduleDestroy (&Orchestrator::Flush, this);
}

void
Orchestrator::PollMobility (void)
{
  for (const auto &config : m_nodes)
    {
      const auto node = config->GetObject<Node> ();

      auto event = config->MobilityPoll ();
      if (event.tolerance == MobilityPollEvent::ToleranceStatus::NotWithin)
        {
          WritePosition (node->GetId (), Simulator::Now (), event.position);
        }
    }

  Simulator::Schedule (m_mobilityPollInterval, &Orchestrator::PollMobility, this);
}

void
Orchestrator::WritePosition (uint32_t nodeId, Time time, Vector3D position)
{
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
  WritePosition (event.nodeId, event.time, event.position);
}

void
Orchestrator::HandlePositionChange (const DecorationMoveEvent &event)
{
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

uint32_t
Orchestrator::Register (Ptr<Decoration> decoration)
{
  m_decorations.emplace_back (decoration);

  return m_nextDecorationId++;
}

void
Orchestrator::RegisterSeries (Ptr<XYSeries> series)
{
  m_xYSeries.emplace_back (series);
}

void
Orchestrator::Register (Ptr<CategoryValueSeries> series)
{
  m_categorySeries.emplace_back (series);
}

void
Orchestrator::RegisterSeries (Ptr<SeriesCollection> series)
{
  m_seriesCollections.emplace_back (series);
}

void
Orchestrator::Register (Ptr<NodeConfiguration> nodeConfiguration)
{
  m_nodes.emplace_back (nodeConfiguration);
}

void
Orchestrator::Register (Ptr<BuildingConfiguration> buildingConfiguration)
{
  m_buildings.emplace_back (buildingConfiguration);
}

void
Orchestrator::Register (Ptr<LogStream> stream)
{
  m_streams.emplace_back (stream);
}

void
Orchestrator::Register (Ptr<RectangularArea> area)
{
  m_areas.emplace_back (area);
}

void
Orchestrator::Commit (XYSeries &series)
{
  using namespace ns3;
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
    case visualizer3d::XYSeries::ConnectionType::None:
      element["connection"] = "none";
      break;
    case visualizer3d::XYSeries::ConnectionType::Line:
      element["connection"] = "line";
      break;
    case visualizer3d::XYSeries::ConnectionType::Spline:
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
    case visualizer3d::XYSeries::LabelMode::Hidden:
      element["labels"] = "hidden";
      break;
    case visualizer3d::XYSeries::LabelMode::Shown:
      element["labels"] = "shown";
      break;
    }

  Color4Value color;
  series.GetAttribute ("Color", color);
  element["color"] = colorToObject (color.Get ());

  // X Axis
  PointerValue xAxisAttr;
  series.GetAttribute ("XAxis", xAxisAttr);
  auto xAxis = xAxisAttr.Get<visualizer3d::ValueAxis> ();
  element["x-axis"] = makeAxisAttributes (xAxis);

  // Y Axis
  PointerValue yAxisAttr;
  series.GetAttribute ("YAxis", yAxisAttr);
  auto yAxis = yAxisAttr.Get<visualizer3d::ValueAxis> ();
  element["y-axis"] = makeAxisAttributes (yAxis);

  m_document["series"].emplace_back (element);
}

void
Orchestrator::Commit (SeriesCollection &series)
{
  using namespace ns3;
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
  auto xAxis = xAxisAttr.Get<visualizer3d::ValueAxis> ();
  element["x-axis"] = makeAxisAttributes (xAxis);

  // Y Axis
  PointerValue yAxisAttr;
  series.GetAttribute ("YAxis", yAxisAttr);
  auto yAxis = yAxisAttr.Get<visualizer3d::ValueAxis> ();
  element["y-axis"] = makeAxisAttributes (yAxis);

  element["child-series"] = series.GetSeriesIds ();

  m_document["series"].emplace_back (element);
}

void
Orchestrator::Commit (CategoryValueSeries &series)
{
  using namespace ns3;
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

  EnumValue connectionMode;
  series.GetAttribute ("ConnectionMode", connectionMode);
  switch (connectionMode.Get ())
    {
    case visualizer3d::CategoryValueSeries::ConnectionMode::ConnectAll:
      element["connection-mode"] = "all";
      break;
    case visualizer3d::CategoryValueSeries::ConnectionMode::ConnectInCategory:
      element["connection-mode"] = "in category";
      break;
    default:
      // Just in case
      NS_ABORT_MSG ("Unhandled CategoryValueSeries::ConnectionMode: " << connectionMode.Get ());
      break;
    }

  Color4Value color;
  series.GetAttribute ("Color", color);
  element["color"] = colorToObject (color.Get ());

  // X Axis
  PointerValue xAxisAttr;
  series.GetAttribute ("XAxis", xAxisAttr);
  auto xAxis = xAxisAttr.Get<visualizer3d::ValueAxis> ();
  element["x-axis"] = makeAxisAttributes (xAxis);

  // Y Axis
  PointerValue yAxisAttr;
  series.GetAttribute ("YAxis", yAxisAttr);
  auto yAxis = yAxisAttr.Get<visualizer3d::CategoryAxis> ();
  element["y-axis"] = makeAxisAttributes (yAxis);

  m_document["series"].emplace_back (element);
}

uint32_t
Orchestrator::NextSeriesId (void)
{
  return m_nextSeriesId++;
}

uint32_t
Orchestrator::NextLogId (void)
{
  return m_nextLogId++;
}

uint32_t
Orchestrator::NextAreaId (void)
{
  return m_nextAreaId++;
}

void
Orchestrator::AppendXyValue (uint32_t id, double x, double y)
{
  nlohmann::json element;
  element["type"] = "xy-series-append";
  element["milliseconds"] = Simulator::Now ().GetMilliSeconds ();
  element["series-id"] = id;
  element["x"] = x;
  element["y"] = y;
  m_document["events"].emplace_back (element);
}

void
Orchestrator::AppendCategoryValue (uint32_t id, int category, double value)
{
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
  if (m_currentSection != Section::Events)
    {
      // We can't write the message until we're in the `Events` section
      // so store it and write later
      NS_LOG_DEBUG ("Spooling Log until simulation starts");

      m_earlyLogs.emplace_back (event);
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

  m_file << m_document;
  m_file.close ();
}

void
Orchestrator::CommitAll (void)
{
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
}

} // namespace visualizer3d
} // namespace ns3
