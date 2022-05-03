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

#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <unordered_map>
#include <ns3/building-list.h>
#include <ns3/building.h>
#include <ns3/building-configuration.h>
#include <ns3/decoration.h>
#include <ns3/node-list.h>
#include <ns3/node-configuration.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/rectangular-area.h>
#include <ns3/simulator.h>
#include <ns3/series-collection.h>
#include <ns3/event-message.h>
#include <ns3/xy-series.h>
#include <ns3/category-value-series.h>
#include <ns3/value-axis.h>
#include <ns3/json.hpp>
#include <ns3/optional.h>
#include "event-message.h"

namespace ns3::netsimulyzer {

class RectangularArea;
class Decoration;
class NodeConfiguration;
class BuildingConfiguration;
class LogStream;
class SeriesCollection;
struct XYPoint;
class XYSeries;
class CategoryValueSeries;
class ValueAxis;

class Orchestrator : public ns3::Object
{
public:
  /**
   * Current section of the JSON document
   */
  enum class Section { Header, Events };

  /**
   * \brief Constructs an Orchestrator and opens an output handle at output_path
   *
   * \param output_path
   * The file to write all output to. If the file exists it will be overwritten
   */
  explicit Orchestrator (const std::string &output_path);

  /**
   * \brief Get the class TypeId
   *
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Sets the suggested time step for playback in
   * the application.
   *
   * \param step
   * The amount of time to pass per frame.
   *
   * \param granularity
   * The level of granularity to show in the application.
   * Must be one of the following:
   * `Unit::MS` (Milliseconds)
   * `Unit::US` (Microseconds)
   * `Unit::NS` (Nanoseconds)
   */
  void SetTimeStep (Time step, Time::Unit granularity);

  /**
   * Unsets the time step set by `SetTimeStep`
   */
  void ClearTimeStep (void);

  /**
   * A pair containing the current time step
   * and suggested granularity
   */
  struct TimeStepPair
  {
    Time timeStep;
    Time::Unit granularity;
  };

  /**
   * Gets the suggested time step and granularity.
   * If no time set was set, returns an empty `optional`
   *
   * \return
   * An `optional`, set if `SetTimeStep` was called, contains
   * the current suggested time step and unit.
   */
  std::optional<TimeStepPair> GetTimeStep (void) const;

  /**
   * \brief Collect Global & Node/Building configs, Schedule Polls
   *
   * Write the global configuration.
   *
   * Transverses the NodeList and BuildingList, and outputs
   * each item and its configuration
   *
   * Schedules polling methods.
   */
  void SetupSimulation (void);

  /**
   * Set if the mobility poll is enabled/disabled
   * and schedule it if it was enabled
   *
   * \param enable
   * True: enable & schedule the mobility poll,
   * False: disable the mobility poll & cancel the poll event
   */
  void SetPollMobility (bool enable);

  /**
   * Gets in the mobility poll is enabled or not
   *
   * \return
   * True: the mobility poll is enabled,
   * False: the mobility poll is not enabled
   */
  bool GetPollMobility (void) const;

  /**
   * \brief Writes positions of configured Nodes
   *
   * Traverses the NodeList and writes the position of
   * all Nodes with aggregated Configuration
   */
  void PollMobility (void);

  /**
   * \brief Trace sink for the 'CourseChange' trace. Writes the event info to the output.
   *
   * Receives the 'CourseChange' trace from an associated NodeConfiguration
   * and writes the event information to the output
   *
   * \param event The event information from the triggered 'CourseChange' event
   */
  void HandleCourseChange (const CourseChangeEvent &event);

  /**
   * Trace sink for when a Decoration's position changes.
   */
  void HandlePositionChange (const DecorationMoveEvent &event);

  /**
   * Trace sink for when a Node's orientation has changed.
   *
   * \param event The event information from the triggered 'NodeOrientationChangeEvent' event
   */
  void HandleOrientationChange (const NodeOrientationChangeEvent &event);

  /**
   * Trace sink for when a Decoration's orientation has changed.
   *
   * \param event The event information from the triggered 'DecorationOrientationChangeEvent' event
   */
  void HandleOrientationChange (const DecorationOrientationChangeEvent &event);

  /**
   * Trace sink for when a Node's Base/Highlight color has changed.
   *
   * \param event The event info for the color change event
   */
  void HandleColorChange (const NodeColorChangeEvent &event);

  /**
   * Trace sink for when a Node has indicated it's transmitting.
   *
   * Prefer to use `NodeConfiguration::Transmit`, instead
   * of calling this directly.
   *
   * \param event The event info for the transmit event
   * \see NodeConfiguration::Transmit
   */
  void HandleTransmit (const TransmitEvent &event);

  /**
   * \brief Flag a Decoration to be tracked.
   *
   * Called by the Decoration constructor, so users should not call
   * this function directly.
   *
   * \param decoration
   * The Decoration to track with this Orchestrator
   *
   * \return
   * The ID to use for the new Decoration
   */
  uint32_t Register (Ptr<Decoration> decoration);

  /**
   * \brief Register a Node to be tracked.
   *
   * Called by the NodeConfiguration constructor, Orchestrator setter,
   * or helper, so users should not call this function directly
   *
   * No return since ns-3 supplies Node IDs
   *
   * \param nodeConfiguration
   * The new NodeConfiguration to register
   */
  void Register (Ptr<NodeConfiguration> nodeConfiguration);

  /**
   * \brief Register a Building to be tracked.
   *
   * Called by the buildingConfiguration constructor, Orchestrator setter,
   * or helper, so users should not call this function directly
   *
   * No return since ns-3 supplies Building IDs
   *
   * \param buildingConfiguration
   * The new buildingConfiguration to register
   */
  void Register (Ptr<BuildingConfiguration> buildingConfiguration);

  /**
   * \brief Flag a series to be tracked by this Orchestrator.
   *
   * This is called by the series constructors by default, so users
   * should not need to call this function directly.
   *
   * \param series
   * The series to begin tracking
   *
   * \return
   * The ID to use for the new series
   */
  uint32_t Register (Ptr<XYSeries> series);

  /**
   * \brief Flag a series to be tracked by this Orchestrator.
   *
   * This is called by the series constructors by default, so users
   * should not need to call this function directly.
   *
   * \param series
   * The series to begin tracking
   *
   * \return
   * The ID to use for the new series
   */
  uint32_t Register (Ptr<CategoryValueSeries> series);

  /**
   * \brief Flag a series to be tracked by this Orchestrator.
   *
   * This is called by the series constructors by default, so users
   * should not need to call this function directly.
   *
   * \param series
   * The series to begin tracking
   *
   * \return
   * The ID to use for the new series
   */
  uint32_t Register (Ptr<SeriesCollection> series);

  /**
   * \brief Flag a stream to be tracked by this Orchestrator.
   *
   * This is called by the series constructors by default, so users
   * should not need to call this function directly.
   *
   * \param stream
   * The LogStream to begin tracking
   *
   * \return
   * The ID to use for the new LosStream
   */
  uint32_t Register (Ptr<LogStream> stream);

  /**
   * \brief Flag an area to be tracked by this Orchestrator.
   *
   * This is called by the area constructors by default, so users
   * should not need to call this function directly.
   *
   * \param area
   * The area to begin tracking
   *
   * \return
   * The ID to use for the new area
   */
  uint32_t Register (Ptr<RectangularArea> area);

  /**
   * Commit a series created while the simulation is running.
   *
   * Called after the series is fully configured, as no more
   * configuration may be performed afterwards.
   *
   * At the beginning of the simulation, all series created before it
   * started are automatically committed.
   *
   * Users should call the `Commit ()` method on the object
   * they are committing.
   *
   * \param series
   * The configured series to write. Should be a registered to this
   * Orchestrator
   */
  void Commit (XYSeries &series);

  /**
   * Commit a series created while the simulation is running.
   *
   * Called after the series is fully configured, as no more
   * configuration may be performed afterwards.
   *
   * At the beginning of the simulation, all series created before it
   * started are automatically committed.
   *
   * Users should call the `Commit ()` method on the object
   * they are committing.
   *
   * \param series
   * The configured series to write. Should be a registered to this
   * Orchestrator
   */
  void Commit (SeriesCollection &series);

  /**
   * Commit a series created while the simulation is running.
   *
   * Called after the series is fully configured, as no more
   * configuration may be performed afterwards.
   *
   * Users should call the `Commit ()` method on the object
   * they are committing.
   *
   * \param series
   * The configured series to write. Should be a registered to this
   * Orchestrator
   */
  void Commit (CategoryValueSeries &series);

  /**
   * Commit a LogStream created while the simulation is running.
   *
   * Called after the LogStream is fully configured, as no more
   * configuration may be performed afterwards.
   *
   * At the beginning of the simulation, all LogStreams created before it
   * started are automatically committed.
   *
   * Users should call the `Commit ()` method on the object
   * they are committing.
   *
   * \param logStream
   * The configured LogStream to write. Should be a registered to this
   * Orchestrator
   */
  void Commit (LogStream &logStream);

  /**
   * Add a single point that will be plotted at `Simulation::Now()` time.
   * Written immediately.
   *
   * \param id
   * The ID of the series to append to
   *
   * \param x
   * The value to plot on the first axis
   *
   * \param y
   * The value to plot on the second axis
   */
  void AppendXyValue (uint32_t id, double x, double y);

  /**
   * Add several points that will be plotted at `Simulation::Now()` time.
   * Written immediately as a single event.
   *
   * \param id
   * The ID of the series to append to
   *
   * \param points
   * The points to append to the series
   */
  void AppendXyValues (uint32_t id, const std::vector<XYPoint> &points);

  /**
   * Hides all of the points currently shown
   * for the series at `Simulation::Now()` time.
   *
   * Users should call `Clear()` on the individual series,
   * rather than this method
   *
   * \param id
   * The ID of the XYSeries to write the clear event for.
   *
   * \see XYSeries::Clear()
   */
  void ClearXySeries (uint32_t id);

  /**
   * Add a single point that will be plotted at `Simulation::Now()` time.
   * Written immediately.
   *
   * \param id
   * The ID of the series to append to
   *
   * \param category
   * The category to place `value` in
   *
   * \param value
   * The value to plot on the value axis
   */
  void AppendCategoryValue (uint32_t id, int category, double value);

  /**
   * Write a message for the LogStream specified by `event.id`.
   *
   * Users should use the output operator `<<` on `LogStream`
   * to write their messages, rather than this method.
   *
   * \param event
   * The event emitted by `LogStream`
   */
  void WriteLogMessage (const LogMessageEvent &event);

  /**
   * Write the document and close the output file.
   * Called when `Simulator::Stop()` is fired,
   * or when the Orchestrator is destroyed
   *
   * This function should only be called once.
   * Subsequent calls will have no affect
   */
  void Flush (void);

protected:
  void DoDispose (void) override;

private:
  /**
   * Gets the time step in a way that's compatible with the
   * deprecated `TimeStep` attribute
   *
   * \return
   * A `std::optional`, set if a time step was set,
   * potentially containing the current time step in
   * milliseconds
   */
  std::optional<int> GetTimeStepCompat (void) const;

  /**
   * Sets the time step in a way that's compatible with the
   * deprecated `TimeStep` attribute.
   *
   * Set the time step to `milliseconds` and the
   * granularity to `Time::Unit::MS` (Milliseconds)
   *
   * \param milliseconds
   * An optional value for the time step for use in the application
   * in milliseconds. If this optional is empty, then
   * the existing time step is cleared
   */
  void SetTimeStepCompat (const std::optional<int> &milliseconds);

  /**
   * Path to output JSON to
   */
  std::string m_outputPath;

  /**
   * Output file handle
   */
  std::ofstream m_file;

  /**
   * The document to serialize
   */
  nlohmann::json m_document = nlohmann::json::object ();

  /**
   * The section of the JSON document the writer is currently in
   */
  Section m_currentSection = Section::Header;

  /**
   * The beginning of the window to write information.
   *
   * Polls/Traces may activate before this time,
   * but no output will be generated
   */
  Time m_startTime;

  /**
   * The end of the window to write information.
   *
   * Polls/Traces may activate after this time,
   * but no output will be generated
   */
  Time m_stopTime;

  /**
   * Flag indicating weather or not we'll schedule and run the PollMobility() method
   */
  bool m_pollMobility;

  /**
   * Flag indicated we've hit the `SetupSimulation` method
   * which runs at the start of the simulation
   */
  bool m_simulationStarted{false};

  /**
   * Event handle for a scheduled mobility poll.
   * Will be unset if no event is scheduled
   */
  std::optional<EventId> m_mobilityPollEvent;

  /**
   * How often to traverse the NodeList for Node positions
   */
  Time m_mobilityPollInterval;

  /**
   * Amount of ns-3 time to pass per step in the application
   * in nanoseconds
   */
  std::optional<Time> m_timeStep;

  /**
   * The suggested unit for the `m_timeStep`
   */
  std::optional<Time::Unit> m_timeStepGranularity;

  /**
   * The ID to assign to the next series that requests it.
   * Separate member since all series share the same ID space
   */
  uint32_t m_nextSeriesId{1u};

  /**
   * Collection of tracked Decorations
   */
  std::vector<Ptr<Decoration>> m_decorations;

  /**
   * Collection of tracked Nodes
   */
  std::vector<Ptr<NodeConfiguration>> m_nodes;

  /**
   * Collection of tracked Buildings
   */
  std::vector<Ptr<BuildingConfiguration>> m_buildings;

  /**
   * Collection of tracked XYSeries for this Orchestrator.
   */
  std::vector<Ptr<XYSeries>> m_xYSeries;

  /**
   * Collection of tracked CategoryValueSeries for this Orchestrator.
   */
  std::vector<Ptr<CategoryValueSeries>> m_categorySeries;

  /**
   * Collection of tracked SeriesCollections for this Orchestrator.
   */
  std::vector<Ptr<SeriesCollection>> m_seriesCollections;

  /**
   * Collection of streams for this Orchestrator.
   */
  std::vector<Ptr<LogStream>> m_streams;

  /**
   * Collection of areas tracked by this Orchestrator.
   */
  std::vector<Ptr<RectangularArea>> m_areas;

  /**
   * Write a `position` event to the output file given it is different than the
   * previous written position
   *
   * \param nodeId
   * The `Node` that should receive the event
   *
   * \param time
   * The time the event occurred
   *
   * \param position
   * The `Node` specified by nodeId's new position
   *
   * \return
   * True if the position was written to the output, false otherwise
   */
  void WritePosition (uint32_t nodeId, Time time, Vector3D position);

  /**
   * Commit all items tracked by this Orchestrator.
   *
   * Used to collect any series the user failed to commit.
   */
  void CommitAll (void);
};

} // namespace ns3::netsimulyzer

#endif /* ORCHESTRATOR_H */
