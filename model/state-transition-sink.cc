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
 */

#include "state-transition-sink.h"
#include <ns3/pointer.h>
#include <ns3/enum.h>
#include <ns3/nstime.h>
#include <ns3/string.h>
#include <ns3/boolean.h>
#include <ns3/double.h>

namespace ns3 {
namespace visualizer3d {

StateTransitionSink::StateTransitionSink (Ptr<Orchestrator> orchestrator,
                                          const std::vector<std::string> &states,
                                          const std::string &initialState)
    : m_orchestrator (orchestrator),
      m_series (CreateObject<CategoryValueSeries> (orchestrator, states)),
      m_log (CreateObject<LogStream> (orchestrator))
{
  Init ();
  SetInitialState (initialState);
}

StateTransitionSink::StateTransitionSink (Ptr<Orchestrator> orchestrator,
                                          const std::vector<CategoryAxis::ValuePair> &states,
                                          int initialState)
    : m_orchestrator (orchestrator),
      m_series (CreateObject<CategoryValueSeries> (orchestrator, states)),
      m_log (CreateObject<LogStream> (orchestrator))
{
  Init ();
  SetInitialState (initialState);
}

TypeId
StateTransitionSink::GetTypeId (void)
{
  // clang-format off
  static TypeId tid =
      TypeId ("ns3::visualizer3d::StateTransitionSink")
          .SetParent<ns3::Object> ()
          .SetGroupName ("visualizer3d")
          .AddAttribute ("Name", "Set the names for sub-elements", StringValue (),
                         MakeStringAccessor (&StateTransitionSink::SetNames), MakeStringChecker ())
          .AddAttribute ("Series", "The series tracking the application state", PointerValue (),
                         MakePointerAccessor (&StateTransitionSink::m_series),
                         MakePointerChecker<CategoryValueSeries> ())
          .AddAttribute("Log", "The Log Stream that this helper writes to", PointerValue(),
                        MakePointerAccessor (&StateTransitionSink::m_log),
                        MakePointerChecker<LogStream>())
          .AddAttribute ("LoggingMode", "Mode for connecting points within the series",
                         EnumValue (LoggingMode::StateChanges),
                         MakeEnumAccessor (&StateTransitionSink::SetLoggingMode,
                                           &StateTransitionSink::GetLoggingMode),
                         MakeEnumChecker (
                           LoggingMode::StateChanges, "StateChanges",
                           LoggingMode::All, "All",
                           LoggingMode::None, "None"))
          .AddAttribute ("TimeUnit", "The unit of time to use for the X axis", EnumValue (Time::S),
                         MakeEnumAccessor (&StateTransitionSink::SetTimeUnit,
                                           &StateTransitionSink::GetTimeUnit),
                         MakeEnumChecker (
                             Time::Unit::Y, "Year",
                             Time::Unit::D, "Day",
                             Time::Unit::H, "Hour",
                             Time::Unit::MIN, "Minute",
                             Time::Unit::S, "Second",
                             Time::Unit::MS, "Millisecond",
                             Time::Unit::US, "Microsecond",
                             Time::Unit::NS, "Nanosecond",
                             Time::Unit::PS, "Picosecond",
                             Time::Unit::FS, "Femtosecond"));

  // clang-format on
  return tid;
}

void
StateTransitionSink::SetInitialState (const std::string &state)
{
  const auto &pair = m_categoryAxis->Get (state);
  m_currentState = pair.key;
  m_currentStateLabel = pair.value;
}

void
StateTransitionSink::SetInitialState (int state)
{
  const auto &pair = m_categoryAxis->Get (state);
  m_currentState = pair.key;
  m_currentStateLabel = pair.value;
}

void
StateTransitionSink::StateChangedName (const std::string &newState)
{
  ApplyStateChange (m_categoryAxis->Get (newState));
}

void
StateTransitionSink::StateChangedId (int newState)
{
  ApplyStateChange (m_categoryAxis->Get (newState));
}

void
StateTransitionSink::SetLoggingMode (StateTransitionSink::LoggingMode mode)
{
  m_loggingMode = mode;
  m_log->SetAttribute ("Visible", BooleanValue (mode != LoggingMode::None));
}

StateTransitionSink::LoggingMode
StateTransitionSink::GetLoggingMode (void) const
{
  return m_loggingMode;
}

void
StateTransitionSink::Write (void)
{
  m_series->Append (m_currentState, Simulator::Now ().ToDouble (m_timeUnit));
}

void
StateTransitionSink::DoDispose (void)
{
  m_orchestrator = nullptr;
  m_categoryAxis = nullptr;
  m_series = nullptr;
  m_log = nullptr;

  Object::DoDispose ();
}

void
StateTransitionSink::Init (void)
{
  m_series->SetAttribute ("AutoUpdate", BooleanValue (true));
  // The interval is set for 1 per unit (e.g. 1 per ms for the millisecond time unit)
  // so just adding 1 each time should be fine
  m_series->SetAttribute ("AutoUpdateIncrement", DoubleValue (1.0));

  PointerValue axis;
  m_series->GetAttribute ("YAxis", axis);
  m_categoryAxis = axis.Get<CategoryAxis> ();
  m_categoryAxis->SetAttribute ("Name", StringValue ("State"));
}

void
StateTransitionSink::ApplyStateChange (const CategoryAxis::ValuePair &pair)
{
  // Write points before and after change
  Write ();

  if (m_loggingMode == LoggingMode::All || m_loggingMode == LoggingMode::StateChanges)
    *m_log << "Changing from: " << m_currentStateLabel << " to: " << pair.value << '\n';

  m_currentState = pair.key;
  m_currentStateLabel = pair.value;

  // Write points before and after change
  Write ();
}

void
StateTransitionSink::SetNames (const std::string &name)
{
  m_series->SetAttribute ("Name", StringValue (name));
  m_log->SetAttribute ("Name", StringValue (name));
}

void
StateTransitionSink::SetTimeUnit (Time::Unit unit)
{
  m_timeUnit = unit;
  Time autoUpdateInterval;

  std::string unitLabel;
  switch (unit)
    {
    case Time::Y:
      unitLabel = "y";
      autoUpdateInterval = Years (1.0);
      break;
    case Time::D:
      unitLabel = "d";
      autoUpdateInterval = Days (1.0);
      break;
    case Time::H:
      unitLabel = "h";
      autoUpdateInterval = Hours (1.0);
      break;
    case Time::MIN:
      unitLabel = "min";
      autoUpdateInterval = Minutes (1.0);
      break;
    case Time::S:
      unitLabel = "s";
      autoUpdateInterval = Seconds (1.0);
      break;
    case Time::MS:
      unitLabel = "ms";
      autoUpdateInterval = MilliSeconds (1);
      break;
    case Time::US:
      unitLabel = "us";
      autoUpdateInterval = MicroSeconds (1);
      break;
    case Time::NS:
      unitLabel = "ns";
      autoUpdateInterval = NanoSeconds (1);
      break;
    case Time::PS:
      unitLabel = "ps";
      autoUpdateInterval = PicoSeconds (1);
      break;
    case Time::FS:
      unitLabel = "fs";
      autoUpdateInterval = FemtoSeconds (1);
      break;
    default:
      NS_ABORT_MSG ("Unrecognised time unit: " << unit);
      break;
    }

  PointerValue axis;
  m_series->GetAttribute ("XAxis", axis);
  axis.Get<ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (" + unitLabel + ')'));

  m_series->SetAttribute ("AutoUpdateInterval", TimeValue (autoUpdateInterval));
}

Time::Unit
StateTransitionSink::GetTimeUnit (void) const
{
  return m_timeUnit;
}

} // namespace visualizer3d
} // namespace ns3
