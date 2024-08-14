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

#ifndef THROUGHPUT_CHARTING_H
#define THROUGHPUT_CHARTING_H

#include <ns3/application-container.h>
#include <ns3/nstime.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "ns3/orchestrator.h"
#include "ns3/throughput-sink.h"


namespace ns3::netsimulyzer
{

using namespace std;

/*
 * traceSourceType refers to a string representation of the trace sink type
 * The last param to AddTraceSource conventionally - although can use a user defined string here
 * 
 * traceSourceName refers to the name that corresponds to the Tx/Rx trace source propery name used in a TraceConnect
 */
struct TraceSourceInfo
{
  std::string traceSourceType;
  std::string traceSourceName;
};

/*
 * Class that creates throughput graphs for each of the supplied apps 
 * ASSUMPTION: ip address is of type ipv4 (takes the first ip address in list for node)
 *
 * Tx supported app types:
 * - OnOffApp
 * - UdpClient
 * - psc::McpttPttApp (MediaTypeMsg)
 *
 * Rx supported app types
 * - PacketSink (tested with UdpSocketFactory)
 *
 * To add support for additional apps modify the following methods:
 * - SetUpTxTraceSourceTypeMap/SetUpRxTraceSourceTypeMap
 * - LinkTxTraces/LinkRxTraces
 * - If the trace source type is not the same as another app will require an additional trace sink callback to be added into the class
 *   (modifying both the header and source code files)
 */
class NetSimulyzerThroughputCharting
{
public:
  NetSimulyzerThroughputCharting();

  /*
   *  \returns 1 on succesful setup
   *           0 (immediately) if called after a succesful set up, ignoring invocation
   *           -1 on unsuccessful setup (orchestrator = nullptr)
   */
  int SetUp(Ptr<Orchestrator> orchestrator);

  /*
   * Setup parameters for the throughput graphs
   * \param interval the interval (in seconds)
   *
   * \returns 1 on success
   *          -1 on failure (not SetUp() properly | interval <= 0)
   */
  int SetParams(double interval, ThroughputSink::Unit dataUnit, Time::Unit timeUnit);

  /*
   * Creates throughput graphs for each app in apps
   * graph[i] = nullptr if apps[i] is an unsupported app
   * \param type - "RX" or "TX"
   *
   * \return the vector of created throughput graphs (graph[i] is the graphs for apps[i])
   */
  std::vector<Ptr<ThroughputSink>> AddApps(ApplicationContainer apps, std::string type);

private:
  /* 
   * Creates Tx tput graph and links to the app's tx trace source
   *
   * \return the throughput sink created
   *         nullptr if app is not supported
   */
  Ptr<ThroughputSink> LinkTxTraces(Ptr<Application> app, std::string name);
  
  /* 
   * Creates Rx tput graph and links to the app's rx trace source
   *
   * \return the throughput sink created
   *         nullptr if app is not supported
   */
  Ptr<ThroughputSink> LinkRxTraces(Ptr<Application> app, std::string name); 

  /*
   * The orchestrator used to create graphs
   */
  Ptr<Orchestrator> m_orchestrator;

  /*
   * Flag verifying correct setup
   */
  bool m_isSetUp;

  /*
   * interval for tput graph
   */
  double m_interval;
  /*
   * data unit for tput graph
   */
  ThroughputSink::Unit m_dataUnit;
  /*
   * time unit for tput graph
   */
  Time::Unit m_timeUnit;

  /*
   * Boolean flag to check if maps regarding supported app types are set up or not
   */
  bool m_areMapsSetUp;
  /*
   * Setup m_appTypeToRemoteProperty
   */
  void SetUpRemoteTypeMap();
  /*
   * Setup m_appTypeToTxTraceSourceInfo (list of suppported tx maps)
   */
  void SetUpTxTraceSourceTypeMap();
  /*
   * Setup m_appTypeToRxTraceSourceInfo (list of supported rx maps)
   */
  void SetUpRxTraceSourceTypeMap();
  /*
   * Helper to SetUpRemoteTypeMap (adds an entry into the map)
   */
  void AddRemoteTypeEntry(ns3::TypeId, std::string);
  /*
   * Helper to SetUpTxTraceSourceTypeMap (adds an entry into the map)
   */
  void AddTxTraceSourceInfoEntry(ns3::TypeId typeId, std::string txTraceSourceType, std::string txTraceSourceName);
  /*
   * Helper to SetUpRxTraceSourceTypeMap (adds an entry into the map)
   */
  void AddRxTraceSourceInfoEntry(ns3::TypeId typeId, std::string rxTraceSourceType, std::string rxTraceSourceName);

  /*
   * Maps an app type id to the string name for its remote address
   */
  std::map<ns3::TypeId, std::string> m_appTypeToRemoteProperty;
  /*
   * Maps an app type id to TraceSourceInfo object regarding its Tx trace source 
   */
  std::map<ns3::TypeId, TraceSourceInfo> m_appTypeToTxTraceSourceInfo;
  /*
   * Maps an app type id to TraceSourceInfo object regarding its Rx trace source 
   */
  std::map<ns3::TypeId, TraceSourceInfo> m_appTypeToRxTraceSourceInfo;


  // Conventional naming: last param in AddTraceSource call for an app's Tx/Rx trace source but without module naming
  /*
   * Represents a callback for sinks of the type Packet::TracedCallback
   */
  void PacketTracedCallback(Ptr<ThroughputSink> throughputGraph, Ptr<const Packet> packet);
  /*
   * Represents a callback for sinks of the type Packet::AddressTracedCallback
   */
  void PacketAddressTracedCallback(Ptr<ThroughputSink> throughputGraph, Ptr<const Packet> packet, const Address& address);
  /*
   * Represents a callback for sinks of the type McpttPttApp::TxRxTracedCallback
   */
  void McpttPttAppTxRxTracedCallback(Ptr<ThroughputSink> throughputGraph,
                                      Ptr<const Application> app,
                                      uint16_t callId,
                                      Ptr<const Packet> p,
                                      const TypeId& headerType);
};

} /* namespace ns3::netsimulyzer */

#endif /* THROUGHPUT_CHARTING_H */
