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
 * Author: Richard Rouil <richard.rouil@nist.gov>
 */

#ifndef THROUGHPUT_SINK_H
#define THROUGHPUT_SINK_H

#include "orchestrator.h"

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/timer.h"

#include <cstdint>
#include <string>

namespace ns3::netsimulyzer
{

class Orchestrator;

/**
 * Defines a sink for collecting throughput information
 */
class ThroughputSink : public ns3::Object
{
  public:
    /**
     * The unit for the Y axis per second
     */
    enum Unit
    {
        Bit,
        KBit,
        MBit,
        GBit,
        Byte,
        KByte,
        MByte,
        GByte
    };

    /**
     * Sets up the XYSeries and assigns the ID
     *
     * @param m_orchestrator
     * The orchestrator to tie this series to
     */
    explicit ThroughputSink(Ptr<Orchestrator> m_orchestrator, const std::string& name);

    /**
     * @brief Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Set the unit for the Y axis
     * @param unit The unit for the Y axis
     */
    void SetUnit(ThroughputSink::Unit unit);

    /**
     * Set the interval between data points in the XY series
     * @param interval The interval between data points in the XY series
     */
    void SetInterval(Time interval);

    /**
     * Increment the size of data received
     * by the size of `packet`.
     *
     * For manual control of the added size
     * see `AddPacketSize`.
     *
     * @param packet A packet with a size
     * to add to the current throughput size.
     *
     * @see AddPacketSize()
     */
    void AddPacket(Ptr<const Packet> packet);

    /**
     * Increment the size of data received
     * @param size The packet size in bytes
     */
    void AddPacketSize(uint32_t size);

    /**
     * Write the throughput at `Simulation::Now()` time
     */
    void WriteThroughput();

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
     * Gets the underlying series.
     *
     * @return
     * The XYSeries tracking throughput
     */
    Ptr<XYSeries> GetSeries(void) const;

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
     * A pointer to the `ValueAxis` used for the Y axis
     * of the contained series
     */
    Ptr<ValueAxis> GetYAxis(void) const;

    /**
     * Convenience method to set up the throughput (Y) axis with a
     * fixed range.
     *
     * This is the equivalent of calling:
     * `sink->GetYAxis ()->FixedRange (min, max)`
     *
     * @param min
     * The minimum expected value passed to `Append()`.
     * Should be less than, but not equal to `max`.
     *
     * @param max
     * The maximum expected value passed to `Append()`.
     * Should be greater than, but not equal to `min`
     */
    void SetThroughputRangeFixed(double min, double max);

    /**
     * Convenience method to set up the throughput (Y) axis with a
     * scaling range (the default).
     *
     * This is the equivalent of calling:
     * `sink->GetYAxis ()->ScalingRange (min, max)`
     *
     * @param min
     * The starting minimum value of the Y axis,
     * Should be less than, but not equal to `max`.
     *
     * @param max
     * The starting maximum value of the Y axis,
     * Should be greater than, but not equal to `min`
     */
    void SetThroughputRangeScaling(double min, double max);

  protected:
    void DoDispose(void) override;

  private:
    /**
     * Pointer to the Orchestrator that manages the child series
     */
    Ptr<Orchestrator> m_orchestrator;

    /**
     * The data unit used to scale the input for the Y Axis
     */
    Unit m_unit{Unit::KBit}; // Initialized here as it also depends on m_timeUnit being set

    /**
     * Unit of time to use for the X Axis
     */
    Time::Unit m_timeUnit{Time::Unit::S}; // Initialized here as it also depends on m_unit being set

    /**
     * Scale factor based on `m_unit`,
     * applied to `m_total` before it is written
     */
    double m_unitScale{1.0};

    /**
     * The series that tracks the value accumulated in `m_total`
     */
    Ptr<XYSeries> m_series;

    /**
     * Size of data received since last period
     */
    uint32_t m_total{0u};

    /**
     * Interval of time between updates
     */
    Time m_packetsInterval;

    /**
     * Timer for writing values in the XY series
     */
    Timer m_timer;

    /**
     * Update the unit labels on the X & Y axes
     */
    void UpdateAxisLabels(void);
};

} // namespace ns3::netsimulyzer

#endif /* THROUGHPUT_SINK_H */
