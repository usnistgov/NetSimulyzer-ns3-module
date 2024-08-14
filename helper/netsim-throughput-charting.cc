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

#include "netsim-throughput-charting.h"

#include <ns3/enum.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/nstime.h>
#include <ns3/onoff-application.h>
#include <ns3/packet-sink.h>
#include <ns3/udp-client.h>

namespace ns3
{

namespace netsimulyzer
{

//TODO: put inside the netsimulyzer namespace

NS_LOG_COMPONENT_DEFINE("NetSimulyzerThroughputCharting");

//using namespace psc;
using namespace std;

NetSimulyzerThroughputCharting::NetSimulyzerThroughputCharting()
{
  m_isSetUp = false;
  m_areMapsSetUp = false;
  m_orchestrator = nullptr;

  m_interval = 0.2;
  m_dataUnit = ThroughputSink::Unit::KByte;
  m_timeUnit = Time::Unit::S;
}

int
NetSimulyzerThroughputCharting::SetUp(Ptr<Orchestrator> orchestrator)
{
  if (m_isSetUp == true)
  {
    return 0;
  }

  if (orchestrator == nullptr)
  {
    m_isSetUp = false;
    return -1;
  }

  m_orchestrator = orchestrator; 
  m_isSetUp = true;
  return 1;
}

int
NetSimulyzerThroughputCharting::SetParams(double interval, ThroughputSink::Unit dataUnit, Time::Unit timeUnit)
{
  if ((m_isSetUp == false) || (interval <= 0))
  {
    return -1;
  }

  m_interval = interval;
  m_dataUnit = dataUnit;
  m_timeUnit = timeUnit;

  return 1;
}


std::vector<Ptr<ThroughputSink>>
NetSimulyzerThroughputCharting::AddApps(ApplicationContainer apps, std::string type)
{
  std::vector<Ptr<ThroughputSink>> tputGraphs;

  if (((type != "TX") || (type != "RX")) || (m_isSetUp == false) || (apps.GetN() == 0))
  {
    return tputGraphs;
  }

  if (m_areMapsSetUp == false)
  {
    SetUpRemoteTypeMap();
    SetUpTxTraceSourceTypeMap();
    SetUpRxTraceSourceTypeMap(); 

    m_areMapsSetUp = true;
  }

  for (uint32_t i = 0; i < apps.GetN(); i++)
  {
    Ptr<Application> app = apps.Get(i);
    TypeId appTypeId = app->GetInstanceTypeId();

    Ptr<Node> localNode = app->GetNode();
    Ipv4Address localIpAddr = localNode->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();

    std::ostringstream os;
    localIpAddr.Print(os);
    std::string localIpAddrString = os.str();
    
    // The 5 is to remove the ns3:: at the start of name
    std::string name = appTypeId.GetName().substr(5) + " Throughput(" + type + ") vs Time - Node " + std::to_string(localNode->GetId()) 
      + " (" + localIpAddrString + ")";
    
    if (type == "TX")
    {
      tputGraphs.push_back(LinkTxTraces(app, name));
    }
    else
    {
      tputGraphs.push_back(LinkRxTraces(app, name));
    }
  }

  return tputGraphs;
}

Ptr<ThroughputSink>
NetSimulyzerThroughputCharting::LinkTxTraces(Ptr<Application> app, std::string name)
{
  TypeId appTypeId = app->GetInstanceTypeId();
  auto it = m_appTypeToTxTraceSourceInfo.find(appTypeId);
  if (it != m_appTypeToTxTraceSourceInfo.end())
  {
    auto tputGraph = CreateObject<ThroughputSink>(m_orchestrator, name);
    tputGraph->SetAttribute("Interval", TimeValue(Seconds(m_interval)));
    tputGraph->SetAttribute("Unit", EnumValue(m_dataUnit));
    tputGraph->SetAttribute("TimeUnit", EnumValue(m_timeUnit));

    std::string txTraceSourceType = it->second.traceSourceType;
    std::string txTraceSourceName = it->second.traceSourceName;

    if (txTraceSourceType == "ns3::Packet::TracedCallback")
    {    
      app->TraceConnectWithoutContext(txTraceSourceName, MakeCallback(&NetSimulyzerThroughputCharting::PacketTracedCallback, this, tputGraph));
    }
#ifdef HAS_PSC
    else if(txTraceSourceType == "ns3::psc::McpttPttApp:TxRxTracedCallback")
    {
      app->TraceConnectWithoutContext(txTraceSourceName, MakeCallback(&NetSimulyzerThroughputCharting::McpttPttAppTxRxTracedCallback, this, tputGraph)); 
    }
#endif
    else
    {
      NS_FATAL_ERROR("TxTraceSourceInfo map setup incorrect - entry added without supported Tx trace type");
    }

    return tputGraph;
  }

  NS_LOG_WARN(appTypeId.GetName() + "being skipped. No supported Tx trace exists.");
  return nullptr; 
}

Ptr<ThroughputSink>
NetSimulyzerThroughputCharting::LinkRxTraces(Ptr<Application> app, std::string name)
{
  TypeId appTypeId = app->GetInstanceTypeId();
  auto it = m_appTypeToRxTraceSourceInfo.find(appTypeId);
  if (it != m_appTypeToRxTraceSourceInfo.end())
  {
    auto tputGraph = CreateObject<ThroughputSink>(m_orchestrator, name);
    tputGraph->SetAttribute("Interval", TimeValue(Seconds(m_interval)));
    tputGraph->SetAttribute("Unit", EnumValue(m_dataUnit));
    tputGraph->SetAttribute("TimeUnit", EnumValue(m_timeUnit));

    std::string rxTraceSourceType = it->second.traceSourceType;
    std::string rxTraceSourceName = it->second.traceSourceName;

    if (rxTraceSourceType == "ns3::Packet::AddressTracedCallback")
    {    
      app->TraceConnectWithoutContext(rxTraceSourceName, MakeCallback(&NetSimulyzerThroughputCharting::PacketAddressTracedCallback, this, tputGraph));
    }
    else
    {
      NS_FATAL_ERROR("RxTraceSourceInfo map setup incorrect - entry added without supported Rx trace type");
    }

    return tputGraph;
  }
  
  NS_LOG_WARN(appTypeId.GetName() + "being skipped. No supported Rx trace exists.");
  return nullptr; 

}

void
NetSimulyzerThroughputCharting::PacketTracedCallback(Ptr<ThroughputSink> throughputGraph, Ptr<const Packet> packet)
{
  throughputGraph->AddPacket(packet);
}

void
NetSimulyzerThroughputCharting::PacketAddressTracedCallback(Ptr<ThroughputSink> throughputGraph, Ptr<const Packet> packet, const Address& address)
{
  throughputGraph->AddPacket(packet);
}

#ifdef HAS_PSC
void
NetSimulyzerThroughputCharting::McpttPttAppTxRxTracedCallback(Ptr<ThroughputSink> throughputGraph,
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

void
NetSimulyzerThroughputCharting::SetUpTxTraceSourceTypeMap()
{
  AddTxTraceSourceInfoEntry(ns3::UdpClient::GetTypeId(), "ns3::Packet::TracedCallback", "Tx");
  AddTxTraceSourceInfoEntry(ns3::OnOffApplication::GetTypeId(), "ns3::Packet::TracedCallback", "Tx");
#ifdef HAS_PSC
  AddTxTraceSourceInfoEntry(ns3::psc::McpttPttApp::GetTypeId(), "ns3::psc::McpttPttApp:TxRxTracedCallback", "Tx");
#endif
}

void
NetSimulyzerThroughputCharting::AddTxTraceSourceInfoEntry(ns3::TypeId typeId, std::string txTraceSourceType, std::string txTraceSourceName)
{
  m_appTypeToTxTraceSourceInfo.insert
  (
    std::pair<ns3::TypeId, TraceSourceInfo>
    (typeId, {.traceSourceType=txTraceSourceType, .traceSourceName=txTraceSourceName})
  );
}

void
NetSimulyzerThroughputCharting::SetUpRxTraceSourceTypeMap()
{
  AddRxTraceSourceInfoEntry(ns3::PacketSink::GetTypeId(), "ns3::Packet::AddressTracedCallback", "Rx");
}

void
NetSimulyzerThroughputCharting::AddRxTraceSourceInfoEntry(ns3::TypeId typeId, std::string txTraceSourceType, std::string txTraceSourceName)
{
  m_appTypeToRxTraceSourceInfo.insert
  (
    std::pair<ns3::TypeId, TraceSourceInfo>
    (typeId, {.traceSourceType=txTraceSourceType, .traceSourceName=txTraceSourceName})
  );
}

void
NetSimulyzerThroughputCharting::SetUpRemoteTypeMap()
{
  AddRemoteTypeEntry(ns3::UdpClient::GetTypeId(), "RemoteAddress");
  AddRemoteTypeEntry(ns3::OnOffApplication::GetTypeId(), "Remote");
}

void
NetSimulyzerThroughputCharting::AddRemoteTypeEntry(ns3::TypeId typeId, std::string remotePropertyName)
{
  m_appTypeToRemoteProperty.insert
  (
    std::pair<ns3::TypeId, std::string>
    {typeId, remotePropertyName}
  );
}

} /* namespace netsimulyzer */
} /* namespace ns3 */


