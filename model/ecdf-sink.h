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

#ifndef ECDF_SINK_H
#define ECDF_SINK_H

#include <ns3/object.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/string.h>
#include <ns3/timer.h>
#include <ns3/value-axis.h>
#include <ns3/xy-series.h>

#include <vector>

namespace ns3::netsimulyzer
{

class Orchestrator;

/**
 * Helper used to build an updating a graph of an
 * Empirical Cumulative Distribution Function (ECDF).
 *
 * Updates based on the `FlushMode` attribute (default, every time new data is written)
 *
 * Pass measured data points to `Append (double)`
 */
class EcdfSink : public Object
{
  public:
    /**
     * Possible modes for when the data should be used to refresh the
     * graph.
     *
     * `OnWrite`: Updates the graph every time `Append()` is called
     *
     * `Interval`: Updates the graph when the contained timer expires
     *
     * `Manual`: Only updates the graph when `Flush()` is called by the user
     *
     * /see SetInterval() for the `FlushMode::Interval` mode
     */
    enum FlushMode : int
    {
        OnWrite,
        Interval,
        Manual
    };

    /**
     * Constructs the sink with a graph with the name `name`,
     * having a Y axis with the label "Percent",
     * and fixed range from 0 to 1 (inclusive)
     *
     * \param orchestrator
     * The Orchestrator to manage the series
     *
     * \param name
     * The name to use for the generated XYSeries
     */
    EcdfSink(Ptr<Orchestrator> orchestrator, const std::string& name);

    /**
     * \brief Get the class TypeId
     *
     * \return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Convenience method to access the contained series
     *
     * \return
     * A pointer to the contained `XYSeries`
     */
    Ptr<XYSeries> GetSeries(void) const;

    /**
     * Convenience method to access the X axis of the
     * contained series
     *
     * \return
     * A pointer to the `ValueAxis` used for the X axis
     * of the contained series
     */
    Ptr<ValueAxis> GetXAxis(void) const;

    /**
     * Convenience method to access the Y axis of the
     * contained series
     *
     * \return
     * A pointer to the `ValueAxis` used for the Y axis
     * of the contained series
     */
    Ptr<ValueAxis> GetYAxis(void) const;

    /**
     * Gets the current flush mode.
     *
     * \see EcdfSink::FlushMode
     *
     * \return
     * The mode for updating the contained graph
     */
    FlushMode GetFlushMode(void) const;

    /**
     * Sets the current flush mode
     *
     * \see EcdfSink::FlushMode
     *
     * \param mode
     * Which mode to use to update the graph
     */
    void SetFlushMode(FlushMode mode);

    /**
     * Convenience method to get the connection type of the internal graph
     *
     * \return
     * The connection type used by the contained series
     */
    XYSeries::ConnectionType GetConnectionType(void) const;

    /**
     * Convenience method to set the connection type of the internal graph
     *
     * \warning XYSeries::ConnectionType::Spline is unsupported for this helper type
     *
     * \param value
     * The connection type to use for the generated graph
     */
    void SetConnectionType(XYSeries::ConnectionType value);

    /**
     * Sets the interval used to update the graph
     *
     * Only used when `FlushMode::Interval` is used
     *
     * \see FlushMode
     *
     * \param interval
     * The interval to update the graph, should be positive
     */
    void SetInterval(Time interval);

    /**
     * Gets the interval set for the internal flush timer.
     *
     * Only used when `FlushMode::Interval` is used
     *
     * \see FlushMode
     *
     * \return
     * The interval of the contained timer.
     */
    Time GetInterval(void) const;

    /**
     * Convenience method to set up the value (X) axis with a
     * fixed range.
     *
     * This is the equivalent of calling:
     * `ecdf->GetXAxis ()->FixedRange (min, max)`
     *
     * \param min
     * The minimum expected value passed to `Append()`.
     * Should be less than, but not equal to `max`.
     *
     * \param max
     * The maximum expected value passed to `Append()`.
     * Should be greater than, but not equal to `min`
     */
    void SetRangeFixed(double min, double max);

    /**
     * Convenience method to set up the value (X) axis with a
     * scaling range.
     *
     * This is the equivalent of calling:
     * `ecdf->GetXAxis ()->ScalingRange (min, max)`
     *
     * \param min
     * The starting minimum value of the X axis,
     * Should be less than, but not equal to `max`.
     *
     * \param max
     * The starting maximum value of the X axis,
     * Should be greater than, but not equal to `min`
     */
    void SetRangeScaling(double min, double max);

    /**
     * Appends a new measured value to the contained data.
     *
     * if the flush mode is `FlushMode::OnWrite`, then this
     * also triggers a `Flush()`
     *
     * An individual value may appear at most approximately `4.29 · 10^9` times,
     * although this value may be higher on some platforms
     *
     * \param value
     * The measured value to plot
     */
    void Append(double value);

    /**
     * Regenerates the graph based on the contained data and
     * connection mode
     *
     * May be called when using any flush mode, but must be
     * called at some point if using `FlushMode::Manual`
     *
     * \see FlushMode
     */
    void Flush(void);

  protected:
    void DoDispose(void) override;

  private:
    /**
     * Internal structure which stores a given
     * data point and how often it occurs
     *
     * Default sorted by the `frequency` value
     */
    struct PointFrequency
    {
        double point;
        unsigned int frequency = 0u;
        bool operator<(const PointFrequency& other) const;
    };

    /**
     * The series used to generate the graph
     */
    Ptr<XYSeries> m_series;

    /**
     * When to regenerate the graph
     */
    FlushMode m_flushMode;

    /**
     * The timer used when the flush mode is set to `Interval`
     *
     * \see FlushMode
     */
    Timer m_timer;

    /**
     * The vector containing one record of each unique appended data point
     * and the frequency it occurs
     */
    std::vector<PointFrequency> m_data;

    /**
     * Total number of appended points,
     * includes unique and non-unique values
     */
    double m_totalPoints{0.0};
};

} // namespace ns3

#endif // ECDF_SINK_H
