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

#ifndef APPLICATION_STATE_SINK_H
#define APPLICATION_STATE_SINK_H

#include "category-value-series.h"
#include "log-stream.h"
#include "orchestrator.h"

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/timer.h"

#include <string>

namespace ns3::netsimulyzer
{

/**
 * Sink class for use with an `ns3::Application`.
 *
 * Creates a `CategoryValueSeries`, & `LogStream`
 * for tracking an application's state.
 *
 * Provide a list of possible states, optionally with IDs, &
 * Connect a model's custom state trace to
 * `StateChangedName` (If the application state is a std::string)
 * or `StateChangedId` (If the application state is an enum/int)
 */
class StateTransitionSink : public Object
{
  public:
    /**
     * Behaviors for writing to the `LogStream`
     */
    enum LoggingMode : int
    {
        /**
         * Only write a message when the application
         * changes to a new state
         */
        StateChanges,

        /**
         * Write all possible log messages
         */
        All,

        /**
         * Write no log messages
         */
        None
    };

    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Sets up the sink with a list of the possible application states.
     * Automatically generates IDs for each state in `states`.
     *
     * @warning Do not duplicate state names,
     * if two states share the same name, IDs
     * must be used to interact with the conflicting names.
     *
     * @warning State IDs must be unique
     *
     * @param orchestrator
     * The `Orchestrator` that will manage the sub-elements
     *
     * @param states
     * List of unique state names with IDs
     *
     * @param initialState
     * The initial state of the model to be connected
     * to one of the StateChanged callbacks.
     */
    StateTransitionSink(Ptr<Orchestrator> orchestrator,
                        const std::vector<std::string>& states,
                        const std::string& initialState);

    /**
     * Sets up the sink with a list of the possible application states
     * with IDs
     *
     * @warning Do not duplicate state names,
     * if two states share the same name, IDs
     * must be used to interact with the conflicting names.
     *
     * @param orchestrator
     * The `Orchestrator` that will manage the sub-elements
     *
     * @param states
     * List of unique state names with IDs
     *
     * @param initialState
     * The initial state of the model to be connected
     * to one of the StateChanged callbacks.
     */
    StateTransitionSink(Ptr<Orchestrator> orchestrator,
                        const std::vector<CategoryAxis::ValuePair>& states,
                        int initialState);

    /**
     * Sets up the sink with a list of the possible application states
     * with IDs
     *
     * @warning Do not duplicate state names,
     * if two states share the same name, IDs
     * must be used to interact with the conflicting names.
     *
     * @tparam T (deduced)
     * The type of the scoped enum
     *
     * @param orchestrator
     * The `Orchestrator` that will manage the sub-elements
     *
     * @param states
     * List of unique state names with IDs
     *
     * @param initialState
     * The initial state of the model to be connected
     * to one of the StateChanged callbacks.
     */
    template <class T>
    StateTransitionSink(Ptr<Orchestrator> orchestrator,
                        const std::vector<CategoryAxis::ValuePair>& states,
                        T initialState);

    /**
     * Sets the starting state of the application.
     * Does not write a state change.
     *
     * If state is not a provided state, this method will abort
     *
     * @param state
     * The starting state of the attached model
     */
    void SetInitialState(const std::string& state);

    /**
     * Sets the starting state of the application.
     * Does not write a state change.
     *
     * If `state` is not a provided state, this method will abort
     *
     * @param state
     * The starting state of the attached model
     */
    void SetInitialState(int state);

    /**
     * Sets the starting state of the application.
     * Does not write a state change.
     *
     * If `state` is not a provided state, this method will abort
     *
     * @tparam T (deduced)
     * The type of the scoped enum
     *
     * @param state
     * The starting state of the attached model
     */
    template <class T>
    void SetInitialState(T state);

    /**
     * Callback to connect to a given subclass of `ns3::Application`.
     * Use this one if you track your application state with a string
     *
     * Call this when your application's state changes, or
     * connect your own StateChanged callback to this.
     *
     * @param newState
     * The state the application is changing to.
     */
    void StateChangedName(const std::string& newState);

    /**
     * Callback to connect to a given subclass of `ns3::Application`.
     * Use this one if you track your application state with a scoped enum
     *
     * Call this when your application's state changes.
     *
     * @tparam T (deduced)
     * The type of the scoped enum
     *
     * @param newState
     * The state the application is changing to.
     */
    template <class T>
    void StateChangedId(T newState);

    /**
     * Callback to connect to a given subclass of `ns3::Application`.
     * Use this one if you track your application state with an unscoped enum/int
     *
     * Call this when your application's state changes, or
     * connect your own StateChanged callback to this.
     *
     * @param newState
     * The state the application is changing to.
     */
    void StateChangedId(int newState);

    /**
     * Sets the names of the attached series & log
     *
     * @param name
     * The name to use
     */
    void SetNames(const std::string& name);

    /**
     * Sets the unit of time for the X axis.
     * Also sets the label for the axis
     *
     * @param unit
     * The unit of time to use.
     */
    void SetTimeUnit(Time::Unit unit);

    /**
     * Get the current time unit for the X Axis
     *
     * @return
     * The current time unit
     */
    Time::Unit GetTimeUnit(void) const;

    /**
     * Sets what messages are printed to the log.
     * If set to `LoggingMode::None` also hides the log
     * If set to anything other than `LoggingMode::None` show the log
     *
     * @param mode
     * What messages to print to the log
     */
    void SetLoggingMode(LoggingMode mode);

    /**
     * Gets the current logging mode
     *
     * @return
     * The logging mode
     */
    LoggingMode GetLoggingMode(void) const;

    /**
     * Convenience method to access the contained series
     *
     * @return
     * A pointer to the contained `CategoryValueSeries`
     */
    Ptr<CategoryValueSeries> GetSeries(void) const;

    /**
     * Convenience method to access the X axis of the
     * contained series
     *
     * @return
     * A pointer to the `ValueAxis` used for the X axis
     * of the contained series
     */
    Ptr<ValueAxis> GetXAxis(void) const;

    /**
     * Convenience method to access the Y axis of the
     * contained series
     *
     * @return
     * A pointer to the `CategoryAxis` used for the Y axis
     * of the contained series
     */
    Ptr<CategoryAxis> GetYAxis(void) const;

    /**
     * Convenience method to set up the time (X) axis with a
     * fixed range.
     *
     * This is the equivalent of calling:
     * `sink->GetXAxis ()->FixedRange (min, max)`
     *
     * @param min
     * The minimum expected value passed to `Append()`.
     * Should be less than, but not equal to `max`.
     *
     * @param max
     * The maximum expected value passed to `Append()`.
     * Should be greater than, but not equal to `min`
     */
    void SetTimeRangeFixed(double min, double max);

    /**
     * Convenience method to set up the time (X) axis with a
     * scaling range (the default).
     *
     * This is the equivalent of calling:
     * `sink->GetXAxis ()->ScalingRange (min, max)`
     *
     * @param min
     * The starting minimum value of the X axis,
     * Should be less than, but not equal to `max`.
     *
     * @param max
     * The starting maximum value of the X axis,
     * Should be greater than, but not equal to `min`
     */
    void SetTimeRangeScaling(double min, double max);

    /**
     * Write the current application state to the series
     */
    void Write(void);

  protected:
    void DoDispose(void) override;

  private:
    /**
     * Initialize common members between the constructors
     * should not be called by the user
     */
    void Init(void);

    /**
     * The `Orchestrator` that manages the visualizer elements
     */
    Ptr<Orchestrator> m_orchestrator;

    /**
     * The series that tracks the application state vs time
     */
    Ptr<CategoryValueSeries> m_series;

    /**
     * The axis that contains the application state
     */
    Ptr<CategoryAxis> m_categoryAxis;

    /**
     * The log for application events,
     * controlled by the `LoggingMode`
     */
    Ptr<LogStream> m_log;

    /**
     * Flag that determines what log messages are
     * emitted on `m_log`
     */
    LoggingMode m_loggingMode;

    /**
     * The ID of the current state,
     *
     * should be set by the user, but
     * 0 is the closest to a reasonable
     * default we have
     */
    int m_currentState{0};

    /**
     * The string representation of the current state.
     *
     * Should be set by the user
     */
    std::string m_currentStateLabel{"Unset Initial State"};

    /**
     * Unit of time to use for the X Axis
     */
    Time::Unit m_timeUnit;

    /**
     * Change the current state & write to series & logs
     *
     * @param pair
     * The name/ID of the new state
     */
    void ApplyStateChange(const CategoryAxis::ValuePair& pair);
};

template <class T>
StateTransitionSink::StateTransitionSink(Ptr<Orchestrator> orchestrator,
                                         const std::vector<CategoryAxis::ValuePair>& states,
                                         T initialState)
    : m_orchestrator(orchestrator),
      m_series(CreateObject<CategoryValueSeries>(orchestrator, states)),
      m_log(CreateObject<LogStream>(orchestrator))
{
    Init();
    SetInitialState(static_cast<int>(initialState));
}

template <class T>
void
StateTransitionSink::SetInitialState(T state)
{
    SetInitialState(static_cast<int>(state));
}

template <class T>
void
StateTransitionSink::StateChangedId(T newState)
{
    StateChangedId(static_cast<int>(newState));
}

} // namespace ns3::netsimulyzer

#endif
