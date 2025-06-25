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
 * Author: Evan Black <evan.black@nist.gov>
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netsimulyzer-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <string>

// Example demonstrating how to connect the `netsimulyzer::ThroughputSink`
// to the UDP Echo Client & Server.
//
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

// Custom function for the "TxWithAddresses" trace in `UdpEchoServer`
// with the `ThroughputSink` as a bound parameter
void
WriteServerThroughput(Ptr<netsimulyzer::ThroughputSink> sink,
                      Ptr<const Packet> packet,
                      const Address&,
                      const Address&)
{
    sink->AddPacket(packet);

    // If this trace only provided the packet size,
    // instead of the whole packet, then
    // ThroughputSink::AddPacketSize could have
    // been used.
}

int
main(int argc, char* argv[])
{
    double durationUser = 20.0;
    std::string outputFileName = "netsimulyzer-throughput-sink-example.json";

    CommandLine cmd(__FILE__);
    cmd.AddValue("outputFileName",
                 "The name of the file to write the NetSimulyzer trace info",
                 outputFileName);
    cmd.AddValue("duration", "Duration (in Seconds) of the simulation", durationUser);
    cmd.Parse(argc, argv);

    NS_ABORT_MSG_IF(durationUser < 3.0, "Scenario must be at least three seconds long");
    const auto duration = Seconds(durationUser);

    NS_ABORT_MSG_IF(outputFileName.empty(), "`outputFileName` must not be empty");

    NodeContainer nodes{2u};

    // ----- Mobility (Optional) -----
    // Slightly separate the two nodes
    // The positions here are arbitrary
    auto positions = CreateObject<ListPositionAllocator>();
    positions->Add(Vector3D{-1.0, 5.0, 0.0});
    positions->Add(Vector3D{1.0, 5.0, 0.0});

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positions);

    mobility.Install(nodes);

    // ----- Network -----
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("0ms"));

    auto netDevices = pointToPoint.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    auto interfaces = address.Assign(netDevices);

    // ----- Applications -----
    const auto echoPort = 9u;
    UdpEchoServerHelper echoServer{echoPort};

    auto serverApp = echoServer.Install(nodes.Get(1u));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(duration - Seconds(1.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1u), echoPort);

    // Make sure a good number of packets are possible (we won't hit this high number though)
    echoClient.SetAttribute("MaxPackets", UintegerValue(10'000u));

    // Sends a packet every two seconds
    echoClient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024u));

    auto clientApp = echoClient.Install(nodes.Get(0u));
    clientApp.Start(Seconds(2.0));
    clientApp.Stop(duration - Seconds(1.0));

    // ----- NetSimulyzer -----
    auto orchestrator = CreateObject<netsimulyzer::Orchestrator>(outputFileName);

    // Suggest a faster playback, since not too much happens (optional)
    orchestrator->SetTimeStep(MilliSeconds(100), Time::MS);

    // The Nodes don't move during the simulation, so disable mobility polling (Optional)
    orchestrator->SetAttribute("PollMobility", BooleanValue(false));

    // ---- Models (Optional) ----
    netsimulyzer::NodeConfigurationHelper nodeHelper{orchestrator};

    // Client
    nodeHelper.Set("Model", netsimulyzer::models::SMARTPHONE_VALUE);
    nodeHelper.Set("HighlightColor",
                   netsimulyzer::OptionalValue<netsimulyzer::Color3>{
                       netsimulyzer::BLUE}); // Match the plots below
    nodeHelper.Install(nodes.Get(0u));

    // Server
    nodeHelper.Set("Model", netsimulyzer::models::SERVER_VALUE);
    nodeHelper.Set("HighlightColor",
                   netsimulyzer::OptionalValue<netsimulyzer::Color3>{
                       netsimulyzer::RED}); // Match the plots below
    nodeHelper.Install(nodes.Get(1u));

    // ---- Throughput Sinks ----

    // --- Client ---
    auto clientThroughput =
        CreateObject<netsimulyzer::ThroughputSink>(orchestrator, "UDP Echo Client Throughput (TX)");

    // Match the model highlight colors above
    clientThroughput->GetSeries()->SetAttribute("Color", netsimulyzer::BLUE_VALUE);

    // Different from the app's interval to show periods with 0 throughput
    clientThroughput->SetAttribute("Interval", TimeValue(Seconds(1.0)));

    // Optional Unit to display data in, does not change the values passed to the sink
    clientThroughput->SetAttribute("Unit", EnumValue(netsimulyzer::ThroughputSink::Unit::Byte));

    // Unit to group time by, does not affect `Interval`
    clientThroughput->SetAttribute("TimeUnit", EnumValue(Time::Unit::S));

    // Use ThroughputSink::AddPacket if the TX/RX trace passes a pointer to the packet
    // If the model provides a trace with a different signature, see the
    // server configuration below for how to hook it up
    clientApp.Get(0u)->TraceConnectWithoutContext(
        "Tx",
        MakeCallback(&netsimulyzer::ThroughputSink::AddPacket, clientThroughput));

    // --- Server ---
    auto serverThroughput =
        CreateObject<netsimulyzer::ThroughputSink>(orchestrator, "UDP Echo Server Throughput (TX)");

    // Match the model highlight colors above
    serverThroughput->GetSeries()->SetAttribute("Color", netsimulyzer::RED_VALUE);

    // Same basic configuration as the client
    serverThroughput->SetAttribute("Unit", EnumValue(netsimulyzer::ThroughputSink::Unit::Byte));
    serverThroughput->SetAttribute("TimeUnit", EnumValue(Time::Unit::S));

    // Uses a custom function (above main()) to account for the extra parameters in
    // the "TxWithAddresses" trace
    // Note the bound parameter: `serverThroughput`
    serverApp.Get(0u)->TraceConnectWithoutContext(
        "TxWithAddresses",
        MakeBoundCallback(&WriteServerThroughput, serverThroughput));

    // --- Collection (Optional) ---
    // Show both throughput plots together using a SeriesCollection

    auto collection = CreateObject<netsimulyzer::SeriesCollection>(orchestrator);
    collection->SetAttribute("Name", StringValue("Client and Server Throughput (TX)"));

    // Keep the individual series above visible in the application
    collection->SetAttribute("HideAddedSeries", BooleanValue(false));

    // Match the X & Y axes names with the sinks
    // assumes both sinks use the same units
    StringValue name;
    serverThroughput->GetSeries()->GetXAxis()->GetAttribute("Name", name);
    collection->GetXAxis()->SetAttribute("Name", name);

    serverThroughput->GetSeries()->GetYAxis()->GetAttribute("Name", name);
    collection->GetYAxis()->SetAttribute("Name", name);

    // Add the series to the collection
    collection->Add(clientThroughput->GetSeries());
    collection->Add(serverThroughput->GetSeries());

    // ----- Run The Simulation -----
    Simulator::Stop(duration);
    Simulator::Run();
    Simulator::Destroy();
}
