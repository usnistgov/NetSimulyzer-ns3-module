/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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

#include "throughput-sink-helper.h"

#include <ns3/enum.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/nstime.h>
#include <ns3/onoff-application.h>
#include <ns3/packet-sink.h>
#include <ns3/udp-client.h>

#include <sstream>
#include <utility>

#ifdef HAS_PSC
#include <ns3/mcptt-ptt-app.h>
#endif

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("NetSimulyzerThroughputCharting");

namespace netsimulyzer
{
ThroughputSinkHelper::ThroughputSinkHelper(Ptr<Orchestrator> orchestrator,
                                           Time interval,
                                           const Time::Unit granularity,
                                           const ThroughputSink::Unit dataUnit)
    : m_orchestrator{orchestrator},
      m_interval{std::move(interval)},
      m_dataUnit{dataUnit},
      m_timeUnit{granularity}
{
#ifdef HAS_PSC
    m_appTypeToTxTraceSourceInfo[psc::McpttPttApp::GetTypeId()] =
        TraceSourceInfo{"ns3::psc::McpttPttApp:TxRxTracedCallback", "Tx"};
#endif
}

std::vector<Ptr<ThroughputSink>>
ThroughputSinkHelper::MakeSinks(const ApplicationContainer& apps, const TraceType type)
{
    std::vector<Ptr<ThroughputSink>> throughputSinks;
    throughputSinks.reserve(apps.GetN());

    for (auto i = 0UL; i < apps.GetN(); i++)
    {
        auto app = apps.Get(i);
        const auto appTypeId = app->GetInstanceTypeId();

        NS_ABORT_MSG_IF(type == TraceType::Tx &&
                            m_txTraceSources.find(appTypeId.GetUid()) == m_txTraceSources.end(),
                        "App Type '" + appTypeId.GetName() + "' not supported for TX traces. ");
        NS_ABORT_MSG_IF(type == TraceType::Rx &&
                            m_rxTraceSources.find(appTypeId.GetUid()) == m_rxTraceSources.end(),
                        "App Type '" + appTypeId.GetName() + "' not supported for RX traces. ");

        Ptr<Node> localNode = app->GetNode();
        Ipv4Address localIpAddr =
            localNode->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();

        std::stringstream ss;

        // The 5 is to remove the ns3:: at the start of name
        ss << appTypeId.GetName().substr(5) << ' ';
        if (type == TraceType::Tx)
            ss << "Throughput(TX) ";
        else
            ss << "Throughput(RX) ";
        ss << "vs Time - Node ";
        ss << localNode->GetId() << ' ';
        ss << '(' << localIpAddr << ')';

        const auto name = ss.str();
        if (type == TraceType::Tx)
        {
            throughputSinks.emplace_back(LinkTxTraces(app, name));
        }
        else
        {
            throughputSinks.emplace_back(LinkRxTraces(app, name));
        }
    }

    return throughputSinks;
}

Time
ThroughputSinkHelper::GetInterval() const
{
    return m_interval;
}

void
ThroughputSinkHelper::SetInterval(const Time interval)
{
    m_interval = interval;
}

Time::Unit
ThroughputSinkHelper::GetGranularity() const
{
    return m_timeUnit;
}

void
ThroughputSinkHelper::SetGranularity(const Time::Unit unit)
{
    m_timeUnit = unit;
}

ThroughputSink::Unit
ThroughputSinkHelper::GetDataUnit() const
{
    return m_dataUnit;
}

void
ThroughputSinkHelper::SetDataUnit(const ThroughputSink::Unit unit)
{
    m_dataUnit = unit;
}

Ptr<ThroughputSink>
ThroughputSinkHelper::LinkTxTraces(Ptr<Application> app, std::string name)
{
    const auto appTypeId = app->GetInstanceTypeId();
    const auto it = m_txTraceSources.find(appTypeId.GetUid());
    if (it == m_txTraceSources.end())
    {
        NS_FATAL_ERROR("App `" + appTypeId.GetName() +
                       "` not supported, add the TX trace to `m_txTraceSources`");
    }
    const auto& [traceSourceType, traceSourceName] = it->second;

    auto sink = CreateObject<ThroughputSink>(m_orchestrator, name);
    sink->SetAttribute("Interval", TimeValue(m_interval));
    sink->SetAttribute("Unit", EnumValue(m_dataUnit));
    sink->SetAttribute("TimeUnit", EnumValue(m_timeUnit));

    if (traceSourceType == "ns3::Packet::TracedCallback")
    {
        app->TraceConnectWithoutContext(
            traceSourceName,
            MakeCallback(&ThroughputSinkHelper::PacketTracedCallback, this, sink));
    }

    else if (traceSourceType == "ns3::psc::McpttPttApp:TxRxTracedCallback")
    {
#ifdef HAS_PSC
        app->TraceConnectWithoutContext(
            traceSourceName,
            MakeCallback(&NetSimulyzerThroughputCharting::McpttPttAppTxRxTracedCallback,
                         this,
                         sink));
#else
        NS_ABORT_MSG(
            "`McpttPttApp` passed to `ThroughputSinkHelper` but PSC module is not available");
#endif
    }
    else
    {
        NS_ABORT_MSG("Unsupported trace source type `" + traceSourceType +
                     " `. Please add a callback for this and attach it in `LinkTxTraces`");
    }

    return sink;
}

Ptr<ThroughputSink>
ThroughputSinkHelper::LinkRxTraces(Ptr<Application> app, std::string name)
{
    const auto appTypeId = app->GetInstanceTypeId();
    const auto it = m_rxTraceSources.find(appTypeId.GetUid());
    if (it == m_rxTraceSources.end())
    {
        NS_FATAL_ERROR("App `" + appTypeId.GetName() +
                       "` not supported, add the RX trace to `m_rxTraceSources`");
    }

    const auto& [traceSourceType, traceSourceName] = it->second;

    auto sink = CreateObject<ThroughputSink>(m_orchestrator, name);
    sink->SetAttribute("Interval", TimeValue(m_interval));
    sink->SetAttribute("Unit", EnumValue(m_dataUnit));
    sink->SetAttribute("TimeUnit", EnumValue(m_timeUnit));

    if (traceSourceName == "ns3::Packet::AddressTracedCallback")
    {
        app->TraceConnectWithoutContext(
            traceSourceName,
            MakeCallback(&ThroughputSinkHelper::PacketAddressTracedCallback, this, sink));
    }
    else
    {
        NS_ABORT_MSG("Unsupported trace source type `" + traceSourceType +
                     " `. Please add a callback for this and attach it in `LinkRxTraces`");
    }

    return sink;
}

void
ThroughputSinkHelper::PacketTracedCallback(Ptr<ThroughputSink> throughputGraph,
                                           Ptr<const Packet> packet)
{
    throughputGraph->AddPacket(packet);
}

void
ThroughputSinkHelper::PacketAddressTracedCallback(Ptr<ThroughputSink> throughputGraph,
                                                  Ptr<const Packet> packet,
                                                  const Address& address)
{
    throughputGraph->AddPacket(packet);
}

#ifdef HAS_PSC
void
ThroughputSinkHelper::McpttPttAppTxRxTracedCallback(Ptr<ThroughputSink> throughputGraph,
                                                    Ptr<const Application> app,
                                                    uint16_t callId,
                                                    Ptr<const Packet> p,
                                                    const TypeId& headerType)
{
    if (headerType == psc::McpttMediaMsg::GetTypeId())
    {
        throughputGraph->AddPacket(p);
    }
}
#endif

} /* namespace netsimulyzer */
} /* namespace ns3 */
