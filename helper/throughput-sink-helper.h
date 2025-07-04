/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 *
 * Author: Nihar Kapasi <niharkkapasi@gmail.com>
 */

#pragma once

#include "ns3/application-container.h"
#include "ns3/nstime.h"
#include "ns3/onoff-application.h"
#include "ns3/orchestrator.h"
#include "ns3/packet-sink.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/throughput-sink.h"
#include "ns3/type-id.h"
#include "ns3/udp-client.h"

#include <cstdint>
#include <unordered_map>

namespace ns3::netsimulyzer
{

/**
 * Helper that creates throughput graphs for each of the supplied apps.
 * Only works with IPv4 (for now)
 *
 * Tx supported app types:
 * - OnOffApp
 * - UdpClient
 * - psc::McpttPttApp (MediaTypeMsg)
 *
 * Rx supported app types
 * - PacketSink (tested with UdpSocketFactory)
 */
class ThroughputSinkHelper
{
  public:
    enum class TraceType
    {
        Tx,
        Rx
    };

    ThroughputSinkHelper(Ptr<Orchestrator> orchestrator,
                         Time interval,
                         Time::Unit granularity,
                         ThroughputSink::Unit dataUnit);

    /**
     * Creates a `ThroughputSink` for each Application in `apps`
     * Using `orchestrator`, `interval`, `granularity`, & `dataUnit`
     *
     * If an Application with an unknown TX/RX trace is passed in `apps`,
     * then this method aborts
     *
     * @param apps
     * Applications to attach a `ThroughputSink` to.
     *
     * @param type
     * If the created `ThroughputSink` for that `Application` should attach to the
     * known TX or RX trace
     *
     * @return
     * A `ThroughputSink` for each app in order of Applications in `apps`
     *
     * @see m_txTraceSources
     * The mapping from App -> TX trace source
     *
     * @see m_rxTraceSources
     * The mapping from App -> RX trace source
     */
    std::vector<Ptr<ThroughputSink>> MakeSinks(const ApplicationContainer& apps, TraceType type);

    [[nodiscard]] Time GetInterval() const;
    void SetInterval(Time interval);

    [[nodiscard]] Time::Unit GetGranularity() const;
    void SetGranularity(Time::Unit unit);

    [[nodiscard]] ThroughputSink::Unit GetDataUnit() const;
    void SetDataUnit(ThroughputSink::Unit unit);

  private:
    /**
     * Pair of values representing a packet
     * trace
     */
    struct TraceSourceInfo
    {
        /**
         * The fully qualified name of the trace type
         * e.g. "ns3::Packet::TracedCallback"
         */
        std::string type;

        /**
         * The name or the trace
         * e.g. "Tx"
         */
        std::string name;
    };

    /**
     * Method which attaches the TX trace in `m_txTraceSources`
     * to the created `ThroughputSink`
     *
     * If you add a new TX trace type, connect it here
     *
     * @see m_txTraceSources
     */
    Ptr<ThroughputSink> LinkTxTraces(Ptr<Application> app, std::string name);

    /**
     * Method which attaches the RX trace in `m_rxTraceSources`
     * to the created `ThroughputSink`
     *
     * If you add a new RX trace type, connect it here
     *
     * @see m_rxTraceSources
     */
    Ptr<ThroughputSink> LinkRxTraces(Ptr<Application> app, std::string name);

    /**
     * The Orchestrator that manages the created sinks
     */
    Ptr<Orchestrator> m_orchestrator;

    /**
     * Interval for created `ThroughputSink`s
     */
    Time m_interval;

    /**
     * Data unit for created `ThroughputSink`s
     */
    ThroughputSink::Unit m_dataUnit;

    /**
     * Time unit for created `ThroughputSink`s
     */
    Time::Unit m_timeUnit;

    /// We use the uid of the `TypeId` to match it in the map
    /// this is just a wrapper to make it clear what that integer is.
    /// Private, since this is just for our maps
    using TypeUid = uint16_t;

    /**
     * Applications with 'known' TX traces.
     *
     * To add another Application,
     * add an entry to this map following this pattern:
     * `{Application's Type ID UID, {"Type of the trace", "Name of the trace"}}`
     *
     * @see LinkTxTraces
     */
    std::unordered_map<TypeUid, TraceSourceInfo> m_txTraceSources{
        {UdpClient::GetTypeId().GetUid(), {"ns3::Packet::TracedCallback", "Tx"}},
        {OnOffApplication::GetTypeId().GetUid(), {"ns3::Packet::TracedCallback", "Tx"}}};

    /**
     * Applications with 'known' RX traces.
     *
     * To add another Application,
     * add an entry to this map following this pattern:
     * `{Application's Type ID UID, {"Type of the trace", "Name of the trace"}}`
     *
     * @see LinkRxTraces
     */
    std::unordered_map<TypeUid, TraceSourceInfo> m_rxTraceSources{
        {PacketSink::GetTypeId().GetUid(), {"ns3::Packet::AddressTracedCallback", "Rx"}}};

    /// Callbacks for each tx/rx trace type above
    /// @see m_txTraceSources
    /// @see m_rxTraceSources

    // Packet::TracedCallback
    void PacketTracedCallback(Ptr<ThroughputSink> throughputGraph, Ptr<const Packet> packet);
    // Packet::AddressTracedCallback
    void PacketAddressTracedCallback(Ptr<ThroughputSink> throughputGraph,
                                     Ptr<const Packet> packet,
                                     const Address& address);

    // McpttPttApp::TxRxTracedCallback
    void McpttPttAppTxRxTracedCallback(Ptr<ThroughputSink> throughputGraph,
                                       Ptr<const Application> app,
                                       uint16_t callId,
                                       Ptr<const Packet> p,
                                       const TypeId& headerType);
};

} /* namespace ns3::netsimulyzer */
