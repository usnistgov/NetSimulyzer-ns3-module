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

#include <string>
#include <ns3/core-module.h>
#include <ns3/mobility-module.h>
#include <ns3/applications-module.h>
#include <ns3/network-module.h>
#include <ns3/internet-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/netsimulyzer-module.h>

// Example demonstrating how to plot an
// Empirical Cumulative Distribution Function (ECDF)
// of transmitted packet sizes using the `netsimulyzer::EcdfSink`
// and the `UdpTraceClient`
//
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

void
macTxTrace (Ptr<netsimulyzer::EcdfSink> ecdf, Ptr<const Packet> packet)
{
  ecdf->Append (packet->GetSize ());
}

int
main (int argc, char *argv[])
{
  double duration = 20.0;
  std::string connectionTypeUser = "Line";
  CommandLine cmd{__FILE__};

  cmd.AddValue ("duration", "Duration (in Seconds) of the simulation", duration);
  cmd.AddValue ("ConnectionType",
                R"(Type of connection to use for the plot. Possible values: ["Line", "None"])",
                connectionTypeUser);

  cmd.Parse (argc, argv);

  auto connectionType = netsimulyzer::XYSeries::ConnectionType::Line;
  if (connectionTypeUser == "Line")
    connectionType = netsimulyzer::XYSeries::ConnectionType::Line;
  else if (connectionTypeUser == "None")
    connectionType = netsimulyzer::XYSeries::ConnectionType::None;
  else
    {
      NS_ABORT_MSG ("Unrecognised 'ConnectionType': " + connectionTypeUser);
    }

  NS_ABORT_MSG_IF (duration < 1.0, "Scenario must be at least one second long");

  // ----- Nodes -----
  NodeContainer nodes{2u};

  // ----- Mobility (Optional) -----
  // Slightly separate the two nodes
  // The positions here are arbitrary
  auto positions = CreateObject<ListPositionAllocator> ();
  positions->Add (Vector3D{-1.0, 5.0, 0.0});
  positions->Add (Vector3D{1.0, 5.0, 0.0});

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positions);

  mobility.Install (nodes);

  // ----- Network -----
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0ms"));

  auto netDevices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  auto interfaces = address.Assign (netDevices);

  // ----- Application -----

  // Client
  auto u = CreateObject<UdpTraceClient> ();
  u->SetStartTime (Seconds (0.5));
  u->SetStopTime (Seconds (duration - 1.0));

  // The destination does not particularly matter,
  // as we don't set up anything to receive these packets
  u->SetRemote (interfaces.GetAddress (1u));

  // Load the default trace
  u->SetTraceFile ("");

  nodes.Get (0u)->AddApplication (u);

  // ---- NetSimulyzer ----
  auto orchestrator = CreateObject<netsimulyzer::Orchestrator> ("ecdf-sink-example.json");

  // Show the two Nodes using the server model
  netsimulyzer::NodeConfigurationHelper nodeHelper{orchestrator};
  nodeHelper.Set ("Model", netsimulyzer::models::SERVER_VALUE);
  nodeHelper.Install (nodes);

  auto ecdf = CreateObject<netsimulyzer::EcdfSink> (orchestrator, "UdpTraceClient Packet Size");
  ecdf->SetAttribute ("Connection", EnumValue (connectionType));
  ecdf->GetXAxis ()->SetAttribute ("Name", StringValue ("Packet Size (Bytes)"));

  // ---- Callback ----
  // The `UdpTraceClient` doesn't offer a 'Tx' trace,
  // so we get the size from the 'MacTx' one.
  netDevices.Get (0u)->TraceConnectWithoutContext ("MacTx", MakeBoundCallback (&macTxTrace, ecdf));

  Simulator::Stop (Seconds (duration));
  Simulator::Run ();
  Simulator::Destroy ();
}
