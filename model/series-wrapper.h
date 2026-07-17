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
 * Author: Andrew Wagger <andrew.wagger@nist.gov>
 */

#ifndef SERIES_WRAPPER_H
#define SERIES_WRAPPER_H

#include "orchestrator.h"
#include "xy-series.h"

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/timer.h"

#include <string>
#include <vector>

namespace ns3::netsimulyzer
{

/**
 * @ingroup netsimulyzer
 * Base class to ensure different structs that wrap a series with context can work together in the
 * SeriesWrapperCollection
 */
class SeriesWrapper : public Object
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    /**
     * Gets the wrapped XYSeires
     */
    Ptr<XYSeries> GetSeries();

    /**
     * Makes a SeriesWrapper from a series
     * @param series
     * The series to wrap
     */
    explicit SeriesWrapper(Ptr<XYSeries> series);

    /**
     * Casts itself as a particular pointer
     */
    template <class T>
    Ptr<T> As()
    {
        return (dynamic_cast<T*>(this));
    };

    /**
     * Shorthand for GetSeries()
     */
    Ptr<XYSeries> operator()();

  private:
    /**
     * The series being wrapped
     */
    Ptr<XYSeries> m_series;
};

/**
 * @ingroup netsimulyzer
 * Appends an accumulating value to the series
 */
class AccumulatorWrapper : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit AccumulatorWrapper(Ptr<XYSeries> series);

    /**
     * Creates a wrapper and sets the time unit to append with
     * @param unit
     * Time unit to use for the x-value
     */
    AccumulatorWrapper(Ptr<XYSeries> series, Time::Unit unit);

    /**
     * Accumulated the value then Appends to the Series
     *
     * @param time
     * The Time the value will be associated with
     * @param add
     * The amount to accumulate by
     */
    void Update(Time time, double add);

    /**
     * Accumulated the value then Appends to the Series at the current time
     *
     * @param add
     * The amount to accumulate by
     */
    void Update(double add);

  private:
    double m_value = 0;
    Time::Unit m_unit = Time::Unit::S;
};

/**
 * @ingroup netsimulyzer
 * Keeps track of a rolling arithmetic average of the given values
 */
class AverageValueWrapper : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit AverageValueWrapper(Ptr<XYSeries> series);

    /**
     * Creates a wrapper and sets the time unit to append with
     *
     * @param unit
     * Time unit to use for the x-value
     */
    AverageValueWrapper(Ptr<XYSeries> series, Time::Unit unit);

    /**
     * Accumulates the average then Appends to the Series
     *
     * @param time
     * The Time to set the new average
     * @param value
     * The amount to add to the average
     */
    void Update(Time time, double value);

    /**
     * Accumulates the average then Appends to the Series
     *
     * @param value
     * The amount to add to the average
     */
    void Update(double value);

  private:
    double m_avg = 0;
    uint32_t m_n = 0;
    Time::Unit m_unit = Time::Unit::S;
};

/**
 * @ingroup netsimulyzer
 * Keeps track of a sliding window for an accumulating value with a bandwidth
 */
class SlidingValueWrapper : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SlidingValueWrapper(Ptr<XYSeries> series);

    /**
     * Sets a timer to routinely append to the series
     * @param interval
     * How often to append to the series
     */
    SlidingValueWrapper(Ptr<XYSeries> series, Time interval);
    /**
     * @param window
     * Length of the sliding window in Seconds
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingValueWrapper(Ptr<XYSeries> series, Time window, double max_sample_frequency);

    /**
     * @param window
     * Length of the sliding window
     * @param interval
     * Sets how often the Series should be Appended to
     */
    SlidingValueWrapper(Ptr<XYSeries> series, Time window, Time interval);

    SlidingValueWrapper(Ptr<XYSeries> series, Time::Unit unit);

    /**
     * Sets a timer to routinely append to the series
     * @param unit
     * Time unit to use for the x-value
     * @param interval
     * How often to append to the series
     */
    SlidingValueWrapper(Ptr<XYSeries> series, Time::Unit unit, Time interval);
    /**
     * @param unit
     * Time unit to use for the x-value
     * @param window
     * Length of the sliding window in Seconds
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingValueWrapper(Ptr<XYSeries> series,
                        Time::Unit unit,
                        Time window,
                        double max_sample_frequency);

    /**
     * @param unit
     * Time unit to use for the x-value
     * @param window
     * Length of the sliding window
     * @param interval
     * Sets how often the Series should be Appended to
     */
    SlidingValueWrapper(Ptr<XYSeries> series, Time::Unit unit, Time window, Time interval);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param time
     * The time that the value is inserted in
     * @param value
     * The value to be inserted into the sliding value
     */
    void Update(Time time, double value);
    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param value
     * The value to be inserted into the sliding value
     */
    void Update(double value);

    void Flush();
    void Append(Time time);

    /**
     * Gets the value of the current window
     */
    double GetSlidingValue() const;

  private:
    /**
     * Prune's the sliding window values
     * @param time
     * The time to prune from
     */
    void Prune(Time time);

    std::vector<std::pair<Time, double>> m_values;
    Time m_window = Seconds(1);
    double m_maxSampleFrequency = 0.1;
    double m_lastSample = 0;
    Timer m_timer;
    bool m_timed = false;
    Time::Unit m_unit = Time::Unit::S;
};

/**
 * @ingroup netsimulyzer
 * Keeps track of a sliding window for an accumulating value with a bandwidth
 */
class SlidingLoadWrapper : public SlidingValueWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SlidingLoadWrapper(Ptr<XYSeries> series);

    /**
     * Sets a timer to routinely append to the series
     * @param interval
     * How often to append to the series
     */
    SlidingLoadWrapper(Ptr<XYSeries> series, Time interval);
    /**
     * @param window
     * Length of the sliding window
     * @param bandwidth
     * Bandwith to compare accumulated value to
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingLoadWrapper(Ptr<XYSeries> series,
                       Time window,
                       double bandwidth,
                       double max_sample_frequency);

    /**
     * @param window
     * Length of the sliding window
     * @param bandwidth
     * Bandwith to compare accumulated value to
     * @param interval
     * Sets how often the Series should be Appended to
     */
    SlidingLoadWrapper(Ptr<XYSeries> series, Time window, double bandwidth, Time interval);

    SlidingLoadWrapper(Ptr<XYSeries> series, Time::Unit unit);

    /**
     * Sets a timer to routinely append to the series
     * @param unit
     * Time unit to use for the x-value
     * @param interval
     * How often to append to the series
     */
    SlidingLoadWrapper(Ptr<XYSeries> series, Time::Unit unit, Time interval);
    /**
     * @param unit
     * Time unit to use for the x-value
     * @param window
     * Length of the sliding window
     * @param bandwidth
     * Bandwith to compare accumulated value to
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingLoadWrapper(Ptr<XYSeries> series,
                       Time::Unit unit,
                       Time window,
                       double bandwidth,
                       double max_sample_frequency);

    /**
     * @param unit
     * Time unit to use for the x-value
     * @param window
     * Length of the sliding window
     * @param bandwidth
     * Bandwith to compare accumulated value to
     * @param interval
     * Sets how often the Series should be Appended to
     */
    SlidingLoadWrapper(Ptr<XYSeries> series,
                       Time::Unit unit,
                       Time window,
                       double bandwidth,
                       Time interval);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param time
     * The time that the value is inserted in
     * @param value
     * The value to be inserted into the sliding data
     */
    void Update(Time time, double value);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param value
     * The value to be inserted into the sliding data
     */
    void Update(double value);

    /**
     * Gets the value of the current window
     */
    double GetSlidingValue() const;

  private:
    std::vector<std::pair<Time, double>> m_values;
    double m_bandwidth = 100.0;
};

} // namespace ns3::netsimulyzer

#endif // SERIES_WRAPPER_H
