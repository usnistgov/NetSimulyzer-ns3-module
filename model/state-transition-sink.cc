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

#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/enum.h>
#include <ns3/nstime.h>
#include <ns3/pointer.h>
#include <ns3/string.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("StateTransitionSink");

namespace netsimulyzer
{

NS_OBJECT_ENSURE_REGISTERED(StateTransitionSink);

StateTransitionSink::StateTransitionSink(Ptr<Orchestrator> orchestrator,
                                         const std::vector<std::string>& states,
                                         const std::string& initialState)
    : m_orchestrator(orchestrator),
      m_series(CreateObject<CategoryValueSeries>(orchestrator, states)),
      m_log(CreateObject<LogStream>(orchestrator))
{
    NS_LOG_FUNCTION(this << orchestrator << initialState);
    Init();
    SetInitialState(initialState);
}

StateTransitionSink::StateTransitionSink(Ptr<Orchestrator> orchestrator,
                                         const std::vector<CategoryAxis::ValuePair>& states,
                                         int initialState)
    : m_orchestrator(orchestrator),
      m_series(CreateObject<CategoryValueSeries>(orchestrator, states)),
      m_log(CreateObject<LogStream>(orchestrator))
{
    NS_LOG_FUNCTION(this << orchestrator << initialState);
    Init();
    SetInitialState(initialState);
}

TypeId
StateTransitionSink::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::netsimulyzer::StateTransitionSink")
            .SetParent<ns3::Object>()
            .SetGroupName("netsimulyzer")
            .AddAttribute("Name",
                          "Set the names for sub-elements",
                          StringValue(),
                          MakeStringAccessor(&StateTransitionSink::SetNames),
                          MakeStringChecker())
            .AddAttribute("Series",
                          "The series tracking the application state",
                          PointerValue(),
                          MakePointerAccessor(&StateTransitionSink::m_series),
                          MakePointerChecker<CategoryValueSeries>())
            .AddAttribute("Log",
                          "The Log Stream that this helper writes to",
                          PointerValue(),
                          MakePointerAccessor(&StateTransitionSink::m_log),
                          MakePointerChecker<LogStream>())
                            .AddAttribute("LoggingMode",
                                          "Mode for connecting points within the series",
                                          EnumValue(LoggingMode::StateChanges),
#ifndef NETSIMULYZER_PRE_NS3_41_ENUM_VALUE
                                          MakeEnumAccessor<LoggingMode>(&StateTransitionSink::SetLoggingMode,
                                                           &StateTransitionSink::GetLoggingMode),
#else
                                           MakeEnumAccessor(&StateTransitionSink::SetLoggingMode,
                                                            &StateTransitionSink::GetLoggingMode),
#endif

                                          MakeEnumChecker(
                                              LoggingMode::StateChanges, "StateChanges",
                                              LoggingMode::All, "All",
                                              LoggingMode::None, "None"))
                            .AddAttribute("TimeUnit",
                                          "The unit of time to use for the X axis",
                                          EnumValue(Time::S),
#ifndef NETSIMULYZER_PRE_NS3_41_ENUM_VALUE
                                          MakeEnumAccessor<Time::Unit>(&StateTransitionSink::SetTimeUnit,
                                                           &StateTransitionSink::GetTimeUnit),
#else
                                               MakeEnumAccessor(&StateTransitionSink::SetTimeUnit,
                                                                &StateTransitionSink::GetTimeUnit),
#endif
                                          MakeEnumChecker(
                                              Time::Unit::Y, "Year",
                                              Time::Unit::D, "Day",
                                              Time::Unit::H, "Hour",
                                              Time::Unit::MIN, "Minute",
                                              Time::Unit::S, "Second",
                                              Time::Unit::MS, "Millisecond",
                                              Time::Unit::US, "Microsecond",
                                              Time::Unit::NS, "Nanosecond",
                                              Time::Unit::PS, "Picosecond",
                                              Time::Unit::FS,"Femtosecond"));

    return tid;
}

void
StateTransitionSink::SetInitialState(const std::string& state)
{
    NS_LOG_FUNCTION(this << state);
    const auto& pair = m_categoryAxis->Get(state);
    m_currentState = pair.key;
    m_currentStateLabel = pair.value;
}

void
StateTransitionSink::SetInitialState(int state)
{
    NS_LOG_FUNCTION(this << state);
    const auto& pair = m_categoryAxis->Get(state);
    m_currentState = pair.key;
    m_currentStateLabel = pair.value;
}

void
StateTransitionSink::StateChangedName(const std::string& newState)
{
    NS_LOG_FUNCTION(this << newState);
    ApplyStateChange(m_categoryAxis->Get(newState));
}

void
StateTransitionSink::StateChangedId(int newState)
{
    NS_LOG_FUNCTION(this << newState);
    ApplyStateChange(m_categoryAxis->Get(newState));
}

void
StateTransitionSink::SetLoggingMode(StateTransitionSink::LoggingMode mode)
{
    NS_LOG_FUNCTION(this << mode);
    m_loggingMode = mode;
    m_log->SetAttribute("Visible", BooleanValue(mode != LoggingMode::None));
}

StateTransitionSink::LoggingMode
StateTransitionSink::GetLoggingMode(void) const
{
    NS_LOG_FUNCTION(this);
    return m_loggingMode;
}

Ptr<CategoryValueSeries>
StateTransitionSink::GetSeries(void) const
{
    return m_series;
}

Ptr<ValueAxis>
StateTransitionSink::GetXAxis(void) const
{
    return m_series->GetXAxis();
}

Ptr<CategoryAxis>
StateTransitionSink::GetYAxis(void) const
{
    return m_series->GetYAxis();
}

void
StateTransitionSink::SetTimeRangeFixed(double min, double max)
{
    m_series->GetXAxis()->FixedRange(min, max);
}

void
StateTransitionSink::SetTimeRangeScaling(double min, double max)
{
    m_series->GetXAxis()->ScalingRange(min, max);
}

void
StateTransitionSink::Write(void)
{
    NS_LOG_FUNCTION(this);
    m_series->Append(m_currentState, Simulator::Now().ToDouble(m_timeUnit));
}

void
StateTransitionSink::DoDispose(void)
{
    NS_LOG_FUNCTION(this);
    m_orchestrator = nullptr;
    m_categoryAxis = nullptr;
    m_series = nullptr;
    m_log = nullptr;

    Object::DoDispose();
}

void
StateTransitionSink::Init(void)
{
    NS_LOG_FUNCTION(this);
    m_series->SetAttribute("AutoUpdate", BooleanValue(true));
    // The interval is set for 1 per unit (e.g. 1 per ms for the millisecond time unit)
    // so just adding 1 each time should be fine
    m_series->SetAttribute("AutoUpdateIncrement", DoubleValue(1.0));

    PointerValue axis;
    m_series->GetAttribute("YAxis", axis);
    m_categoryAxis = axis.Get<CategoryAxis>();
    m_categoryAxis->SetAttribute("Name", StringValue("State"));
}

void
StateTransitionSink::ApplyStateChange(const CategoryAxis::ValuePair& pair)
{
    NS_LOG_FUNCTION(this << pair.key << pair.value);
    // Write points before and after change
    Write();

    if (m_loggingMode == LoggingMode::All || m_loggingMode == LoggingMode::StateChanges)
        *m_log << "Changing from: " << m_currentStateLabel << " to: " << pair.value << '\n';

    m_currentState = pair.key;
    m_currentStateLabel = pair.value;

    // Write points before and after change
    Write();
}

void
StateTransitionSink::SetNames(const std::string& name)
{
    NS_LOG_FUNCTION(this << name);
    m_series->SetAttribute("Name", StringValue(name));
    m_log->SetAttribute("Name", StringValue(name));
}

void
StateTransitionSink::SetTimeUnit(Time::Unit unit)
{
    NS_LOG_FUNCTION(this << unit);
    m_timeUnit = unit;
    Time autoUpdateInterval;

    std::string unitLabel;
    switch (unit)
    {
    case Time::Y:
        unitLabel = "y";
        autoUpdateInterval = Years(1.0);
        break;
    case Time::D:
        unitLabel = "d";
        autoUpdateInterval = Days(1.0);
        break;
    case Time::H:
        unitLabel = "h";
        autoUpdateInterval = Hours(1.0);
        break;
    case Time::MIN:
        unitLabel = "min";
        autoUpdateInterval = Minutes(1.0);
        break;
    case Time::S:
        unitLabel = "s";
        autoUpdateInterval = Seconds(1.0);
        break;
    case Time::MS:
        unitLabel = "ms";
        autoUpdateInterval = MilliSeconds(1);
        break;
    case Time::US:
        unitLabel = "us";
        autoUpdateInterval = MicroSeconds(1);
        break;
    case Time::NS:
        unitLabel = "ns";
        autoUpdateInterval = NanoSeconds(1);
        break;
    case Time::PS:
        unitLabel = "ps";
        autoUpdateInterval = PicoSeconds(1);
        break;
    case Time::FS:
        unitLabel = "fs";
        autoUpdateInterval = FemtoSeconds(1);
        break;
    default:
        NS_ABORT_MSG("Unrecognised time unit: " << unit);
        break;
    }

    PointerValue axis;
    m_series->GetAttribute("XAxis", axis);
    axis.Get<ValueAxis>()->SetAttribute("Name", StringValue("Time (" + unitLabel + ')'));

    m_series->SetAttribute("AutoUpdateInterval", TimeValue(autoUpdateInterval));
}

Time::Unit
StateTransitionSink::GetTimeUnit(void) const
{
    NS_LOG_FUNCTION(this);
    return m_timeUnit;
}

} // namespace netsimulyzer
} // namespace ns3
