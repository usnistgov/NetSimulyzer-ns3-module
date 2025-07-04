/*
 * Copyright (c) 2018 Fraunhofer ESK
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Vignesh Babu <ns3-dev@esk.fraunhofer.de>
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <array>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <vector>

#ifdef HAS_NETSIMULYZER
#include "ns3/netsimulyzer-module.h"
#endif

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LenaRadioLinkFailure");

// Global values to check the simulation
// behavior during and after the simulation.
uint16_t counterN310FirsteNB = 0; //!< Counter of N310 indications.
Time t310StartTimeFirstEnb;       //!< Time of first N310 indication.
uint32_t ByteCounter = 0;         //!< Byte counter.
uint32_t oldByteCounter = 0;      //!< Old Byte counter,

#ifdef HAS_NETSIMULYZER
// Configuration parameters
bool enableVisualization = true;
double guiResolution = 20; // refresh time in ms
// Visualizer components
Ptr<netsimulyzer::Orchestrator> orchestrator = nullptr;
Ptr<netsimulyzer::LogStream> applicationLog = nullptr;
Ptr<netsimulyzer::LogStream> ueLog = nullptr;
Ptr<netsimulyzer::LogStream> enbLog = nullptr;

// Helper function to simplify writing to logs
void
WriteApplicationLog(std::string message)
{
    *applicationLog << "At " << Simulator::Now().GetSeconds() << " " << message;
}

// Statistics
Ptr<netsimulyzer::ThroughputSink> appRxTraceSeries;
std::map<uint32_t, Ptr<netsimulyzer::StateTransitionSink>> rrcStateMachines;
std::map<uint32_t, Ptr<netsimulyzer::SeriesCollection>> rsrpCollections;
std::map<uint32_t, std::map<uint32_t, Ptr<netsimulyzer::XYSeries>>>
    rsrpSeries; // maps IMSI to map of CellId to series

const std::array<netsimulyzer::Color3Value, 3> g_colors{netsimulyzer::RED_VALUE,
                                                        netsimulyzer::GREEN_VALUE,
                                                        netsimulyzer::BLUE_VALUE};

// Callback for UE PHY measurements
void
NotifyReportUeMeasurements(std::string path,
                           uint16_t rnti,
                           uint16_t cellId,
                           double rsrp,
                           double sinr,
                           bool serving,
                           uint8_t ccId)
{
    std::string uePath = path.substr(0, path.find("/$ns3::LteUeNetDevice"));
    Config::MatchContainer match = Config::LookupMatches(uePath);
    uint64_t imsi = 0;
    if (match.GetN() != 0)
    {
        Ptr<Object> ueNetDevice = match.Get(0);
        NS_LOG_LOGIC("FindImsiFromLteNetDevice: "
                     << path << ", " << ueNetDevice->GetObject<LteUeNetDevice>()->GetImsi());
        imsi = ueNetDevice->GetObject<LteUeNetDevice>()->GetImsi();

        std::map<uint32_t, std::map<uint32_t, Ptr<netsimulyzer::XYSeries>>>::iterator imsiToCellIt =
            rsrpSeries.find(imsi);
        std::map<uint32_t, Ptr<netsimulyzer::XYSeries>>::iterator cellIdToPlotIt;
        if (imsiToCellIt != rsrpSeries.end())
        {
            cellIdToPlotIt = imsiToCellIt->second.find(cellId);
        }
        if (imsiToCellIt == rsrpSeries.end())
        {
            // Create collection to plot RSRP for cells detected by this IMSI
            PointerValue xAxis;
            PointerValue yAxis;
            Ptr<netsimulyzer::SeriesCollection> rsrpCollection =
                CreateObject<netsimulyzer::SeriesCollection>(orchestrator);
            rsrpCollection->SetAttribute("Name", StringValue("RSRP"));
            rsrpCollection->GetAttribute("XAxis", xAxis);
            xAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("Time (s)"));
            rsrpCollection->GetAttribute("YAxis", yAxis);
            yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Name", StringValue("RSRP (dBm)"));
            yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("BoundMode", StringValue("Fixed"));
            yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Maximum", DoubleValue(-110));
            yAxis.Get<netsimulyzer::ValueAxis>()->SetAttribute("Minimum", DoubleValue(-140));
            rsrpCollections[imsi] = rsrpCollection;
        }
        if (imsiToCellIt == rsrpSeries.end() || cellIdToPlotIt == imsiToCellIt->second.end())
        {
            // create new XY series to plot RSRP for this cellId
            std::cout << "Creating new entry for " << imsi << " cellId " << cellId << std::endl;
            rsrpSeries[imsi][cellId] = CreateObject<netsimulyzer::XYSeries>(orchestrator);
            rsrpSeries[imsi][cellId]->SetAttribute(
                "Name",
                StringValue("Cell Id " + std::to_string(cellId)));
            rsrpSeries[imsi][cellId]->SetAttribute("LabelMode", StringValue("Hidden"));
            rsrpSeries[imsi][cellId]->SetAttribute("Connection", StringValue("None"));
            rsrpSeries[imsi][cellId]->SetAttribute("Color", g_colors[cellId % g_colors.size()]);
            rsrpCollections[imsi]->Add(rsrpSeries[imsi][cellId]);
        }
        // Update data point
        rsrpSeries[imsi][cellId]->Append(Simulator::Now().GetSeconds(), rsrp);
    }
    else
    {
        NS_FATAL_ERROR("Lookup " << path << " got no matches");
    }
}
#endif

/**
 * Print the position of a UE with given IMSI.
 *
 * @param imsi The IMSI.
 */
void
PrintUePosition(uint64_t imsi)
{
    for (auto it = NodeList::Begin(); it != NodeList::End(); ++it)
    {
        Ptr<Node> node = *it;
        int nDevs = node->GetNDevices();
        for (int j = 0; j < nDevs; j++)
        {
            Ptr<LteUeNetDevice> uedev = node->GetDevice(j)->GetObject<LteUeNetDevice>();
            if (uedev)
            {
                if (imsi == uedev->GetImsi())
                {
                    Vector pos = node->GetObject<MobilityModel>()->GetPosition();
                    std::cout << "IMSI : " << uedev->GetImsi() << " at " << pos.x << "," << pos.y
                              << std::endl;
                }
            }
        }
    }
}

/**
 * UE Notify connection established.
 *
 * @param context The context.
 * @param imsi The IMSI.
 * @param cellid The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyConnectionEstablishedUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " UE IMSI " << imsi
              << ": connected to cell id " << cellid << " with RNTI " << rnti << std::endl;
}

/**
 * eNB Notify connection established.
 *
 * @param context The context.
 * @param imsi The IMSI.
 * @param cellId The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyConnectionEstablishedEnb(std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " " << context << " eNB cell id " << cellId
              << ": successful connection of UE with IMSI " << imsi << " RNTI " << rnti
              << std::endl;
    // In this example, a UE should experience RLF at least one time in
    // cell 1. For the case, when there is only one eNB with ideal RRC,
    // a UE might reconnects to the eNB multiple times due to more than
    // one RLF. To handle this, we reset the counter here so, even if the UE
    // connects multiple time to cell 1 we count N310
    // indication correctly, i.e., for each RLF UE RRC should receive
    // configured number of N310 indications.
    if (cellId == 1)
    {
        counterN310FirsteNB = 0;
    }

#ifdef HAS_NETSIMULYZER
    if (enableVisualization)
    {
        *enbLog << Simulator::Now().GetSeconds() << " " << context << " eNB cell id " << cellId
                << ": successful connection of UE with IMSI " << imsi << " RNTI " << rnti;
    }
#endif
}

/// Map each of UE RRC states to its string representation.
static const std::string g_ueRrcStateName[LteUeRrc::NUM_STATES] = {"IDLE_START",
                                                                   "IDLE_CELL_SEARCH",
                                                                   "IDLE_WAIT_MIB_SIB1",
                                                                   "IDLE_WAIT_MIB",
                                                                   "IDLE_WAIT_SIB1",
                                                                   "IDLE_CAMPED_NORMALLY",
                                                                   "IDLE_WAIT_SIB2",
                                                                   "IDLE_RANDOM_ACCESS",
                                                                   "IDLE_CONNECTING",
                                                                   "CONNECTED_NORMALLY",
                                                                   "CONNECTED_HANDOVER",
                                                                   "CONNECTED_PHY_PROBLEM",
                                                                   "CONNECTED_REESTABLISHING"};

/**
 * UE state transition tracer.
 *
 * @param imsi The IMSI.
 * @param cellId The Cell ID.
 * @param rnti The RNTI.
 * @param oldState The old state.
 * @param newState The new state.
 */
void
UeStateTransition(uint64_t imsi,
                  uint16_t cellId,
                  uint16_t rnti,
                  LteUeRrc::State oldState,
                  LteUeRrc::State newState)
{
    std::cout << Simulator::Now().As(Time::S) << " UE with IMSI " << imsi << " RNTI " << rnti
              << " connected to cell " << cellId << " transitions from " << oldState << " to "
              << newState << std::endl;

#ifdef HAS_NETSIMULYZER
    if (enableVisualization)
    {
        std::map<uint32_t, Ptr<netsimulyzer::StateTransitionSink>>::iterator it =
            rrcStateMachines.find(imsi);
        it->second->StateChangedId(newState);
    }
#endif
}

/**
 * eNB RRC timeout tracer.
 *
 * @param imsi The IMSI.
 * @param rnti The RNTI.
 * @param cellId The Cell ID.
 * @param cause The reason for timeout.
 */
void
EnbRrcTimeout(uint64_t imsi, uint16_t rnti, uint16_t cellId, std::string cause)
{
    std::cout << Simulator::Now().As(Time::S) << " IMSI " << imsi << ", RNTI " << rnti
              << ", Cell id " << cellId << ", ENB RRC " << cause << std::endl;
#ifdef HAS_NETSIMULYZER
    if (enableVisualization)
    {
        *enbLog << Simulator::Now().GetSeconds() << " IMSI " << imsi << ", RNTI " << rnti
                << ", Cell id " << cellId << ", ENB RRC " << cause;
    }
#endif
}

/**
 * Notification of connection release at eNB.
 *
 * @param imsi The IMSI.
 * @param cellId The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyConnectionReleaseAtEnodeB(uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
    std::cout << Simulator::Now() << " IMSI " << imsi << ", RNTI " << rnti << ", Cell id " << cellId
              << ", UE context destroyed at eNodeB" << std::endl;
}

/**
 * PHY sync detection tracer.
 *
 * @param n310 310 data.
 * @param imsi The IMSI.
 * @param rnti The RNTI.
 * @param cellId The Cell ID.
 * @param type The type.
 * @param count The count.
 */
void
PhySyncDetection(uint16_t n310,
                 uint64_t imsi,
                 uint16_t rnti,
                 uint16_t cellId,
                 std::string type,
                 uint8_t count)
{
    std::cout << Simulator::Now().As(Time::S) << " IMSI " << imsi << ", RNTI " << rnti
              << ", Cell id " << cellId << ", " << type << ", no of sync indications: " << +count
              << std::endl;

    if (type == "Notify out of sync" && cellId == 1)
    {
        ++counterN310FirsteNB;
        if (counterN310FirsteNB == n310)
        {
            t310StartTimeFirstEnb = Simulator::Now();
        }
        NS_LOG_DEBUG("counterN310FirsteNB = " << counterN310FirsteNB);
    }
}

/**
 * Radio link failure tracer.
 *
 * @param t310 310 data.
 * @param imsi The IMSI.
 * @param cellId The Cell ID.
 * @param rnti The RNTI.
 */
void
RadioLinkFailure(Time t310, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
    std::cout << Simulator::Now() << " IMSI " << imsi << ", RNTI " << rnti << ", Cell id " << cellId
              << ", radio link failure detected" << std::endl
              << std::endl;

    PrintUePosition(imsi);

    if (cellId == 1)
    {
        NS_ABORT_MSG_IF((Simulator::Now() - t310StartTimeFirstEnb) != t310,
                        "T310 timer expired at wrong time");
    }
}

/**
 * UE Random access error notification.
 *
 * @param imsi The IMSI.
 * @param cellId The Cell ID.
 * @param rnti The RNTI.
 */
void
NotifyRandomAccessErrorUe(uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
    std::cout << Simulator::Now().As(Time::S) << " IMSI " << imsi << ", RNTI " << rnti
              << ", Cell id " << cellId << ", UE RRC Random access Failed" << std::endl;
#ifdef HAS_NETSIMULYZER
    if (enableVisualization)
    {
        *ueLog << Simulator::Now().GetSeconds() << " IMSI " << imsi << ", RNTI " << rnti
               << ", Cell id " << cellId << ", UE RRC Random access Failed";
    }
#endif
}

/**
 * UE Connection timeout notification.
 *
 * @param imsi The IMSI.
 * @param cellId The Cell ID.
 * @param rnti The RNTI.
 * @param connEstFailCount Connection failure count.
 */
void
NotifyConnectionTimeoutUe(uint64_t imsi, uint16_t cellId, uint16_t rnti, uint8_t connEstFailCount)
{
    std::cout << Simulator::Now().As(Time::S) << " IMSI " << imsi << ", RNTI " << rnti
              << ", Cell id " << cellId << ", T300 expiration counter "
              << (uint16_t)connEstFailCount << ", UE RRC Connection timeout" << std::endl;
#ifdef HAS_NETSIMULYZER
    if (enableVisualization)
    {
        *ueLog << Simulator::Now().GetSeconds() << " IMSI " << imsi << ", RNTI " << rnti
               << ", Cell id " << cellId << ", T300 expiration counter "
               << (uint16_t)connEstFailCount << ", UE RRC Connection timeout";
    }
#endif
}

/**
 * UE RA response timeout notification.
 *
 * @param imsi The IMSI.
 * @param contention Contention flag.
 * @param preambleTxCounter Preamble Tx counter.
 * @param maxPreambleTxLimit Max preamble Ts limit.
 */
void
NotifyRaResponseTimeoutUe(uint64_t imsi,
                          bool contention,
                          uint8_t preambleTxCounter,
                          uint8_t maxPreambleTxLimit)
{
    std::cout << Simulator::Now().As(Time::S) << " IMSI " << imsi << ", Contention flag "
              << contention << ", preamble Tx Counter " << (uint16_t)preambleTxCounter
              << ", Max Preamble Tx Limit " << (uint16_t)maxPreambleTxLimit
              << ", UE RA response timeout" << std::endl;
#ifdef HAS_NETSIMULYZER
    if (enableVisualization)
    {
        *ueLog << Simulator::Now().GetSeconds() << " IMSI " << imsi << ", Contention flag "
               << contention << ", preamble Tx Counter " << (uint16_t)preambleTxCounter
               << ", Max Preamble Tx Limit " << (uint16_t)maxPreambleTxLimit
               << ", UE RA response timeout";

        // Write output, since we're about to crash (from `NS_FATAL_ERROR` below)
        orchestrator->Flush();
    }
#endif
}

/**
 * Receive a packet.
 *
 * @param packet The packet.
 */
void
ReceivePacket(Ptr<const Packet> packet, const Address&)
{
    ByteCounter += packet->GetSize();
#ifdef HAS_NETSIMULYZER
    if (enableVisualization)
    {
        appRxTraceSeries->AddPacketSize(packet->GetSize());
    }
#endif
}

/**
 * Write the throughput to file.
 *
 * @param firstWrite True if first time writing.
 * @param binSize Bin size.
 * @param fileName Output filename.
 */
void
Throughput(bool firstWrite, Time binSize, std::string fileName)
{
    std::ofstream output;

    if (firstWrite)
    {
        output.open(fileName, std::ofstream::out);
        firstWrite = false;
    }
    else
    {
        output.open(fileName, std::ofstream::app);
    }

    // Instantaneous throughput every 200 ms

    double throughput = (ByteCounter - oldByteCounter) * 8 / binSize.GetSeconds() / 1024 / 1024;
    output << Simulator::Now().As(Time::S) << " " << throughput << std::endl;
    oldByteCounter = ByteCounter;
    Simulator::Schedule(binSize, &Throughput, firstWrite, binSize, fileName);
}

/**
 * Sample simulation script for radio link failure.
 * By default, only one eNodeB and one UE is considered for verifying
 * radio link failure. The UE is initially in the coverage of
 * eNodeB and a RRC connection gets established.
 * As the UE moves away from the eNodeB, the signal degrades
 * and out-of-sync indications are counted. When the T310 timer
 * expires, radio link is considered to have failed and UE
 * leaves the CONNECTED_NORMALLY state and performs cell
 * selection again.
 *
 * The example can be run as follows:
 *
 * ./ns3 run "lena-radio-link-failure --numberOfEnbs=1 --simTime=25"
 */
int
main(int argc, char* argv[])
{
    // Configurable parameters
    Time simTime = Seconds(25);
    uint16_t numberOfEnbs = 1;
    double interSiteDistance = 1200;
    uint16_t n311 = 1;
    uint16_t n310 = 1;
    Time t310 = Seconds(1);
    bool useIdealRrc = true;
    bool enableCtrlErrorModel = true;
    bool enableDataErrorModel = true;
    bool enableNsLogs = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("simTime", "Total duration of the simulation (in seconds)", simTime);
    cmd.AddValue("numberOfEnbs", "Number of eNBs", numberOfEnbs);
    cmd.AddValue("n311", "Number of in-synch indication", n311);
    cmd.AddValue("n310", "Number of out-of-synch indication", n310);
    cmd.AddValue("t310", "Timer for detecting the Radio link failure (in seconds)", t310);
    cmd.AddValue("interSiteDistance", "Inter-site distance in meter", interSiteDistance);
    cmd.AddValue("useIdealRrc", "Use ideal RRC protocol", useIdealRrc);
    cmd.AddValue("enableCtrlErrorModel", "Enable control error model", enableCtrlErrorModel);
    cmd.AddValue("enableDataErrorModel", "Enable data error model", enableDataErrorModel);
    cmd.AddValue("enableNsLogs", "Enable ns-3 logging (debug builds)", enableNsLogs);
#ifdef HAS_NETSIMULYZER
    cmd.AddValue("visual", "Generate visualization traces", enableVisualization);
#endif
    cmd.Parse(argc, argv);

    if (enableNsLogs)
    {
        auto logLevel =
            (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_NODE | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
        LogComponentEnable("LteUeRrc", logLevel);
        LogComponentEnable("LteUeMac", logLevel);
        LogComponentEnable("LteUePhy", logLevel);

        LogComponentEnable("LteEnbRrc", logLevel);
        LogComponentEnable("LteEnbMac", logLevel);
        LogComponentEnable("LteEnbPhy", logLevel);

        LogComponentEnable("LenaRadioLinkFailure", logLevel);
    }

    uint16_t numberOfUes = 1;
    uint16_t numBearersPerUe = 1;
    double eNodeB_txPower = 43;

    Config::SetDefault("ns3::LteHelper::UseIdealRrc", BooleanValue(useIdealRrc));
    Config::SetDefault("ns3::LteSpectrumPhy::CtrlErrorModelEnabled",
                       BooleanValue(enableCtrlErrorModel));
    Config::SetDefault("ns3::LteSpectrumPhy::DataErrorModelEnabled",
                       BooleanValue(enableDataErrorModel));

    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(60 * 1024));

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    lteHelper->SetPathlossModelType(TypeId::LookupByName("ns3::LogDistancePropagationLossModel"));
    lteHelper->SetPathlossModelAttribute("Exponent", DoubleValue(3.9));
    lteHelper->SetPathlossModelAttribute("ReferenceLoss",
                                         DoubleValue(38.57)); // ref. loss in dB at 1m for 2.025GHz
    lteHelper->SetPathlossModelAttribute("ReferenceDistance", DoubleValue(1));

    //----power related (equal for all base stations)----
    Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(eNodeB_txPower));
    Config::SetDefault("ns3::LteUePhy::TxPower", DoubleValue(23));
    Config::SetDefault("ns3::LteUePhy::NoiseFigure", DoubleValue(7));
    Config::SetDefault("ns3::LteEnbPhy::NoiseFigure", DoubleValue(2));
    Config::SetDefault("ns3::LteUePhy::EnableUplinkPowerControl", BooleanValue(true));
    Config::SetDefault("ns3::LteUePowerControl::ClosedLoop", BooleanValue(true));
    Config::SetDefault("ns3::LteUePowerControl::AccumulationEnabled", BooleanValue(true));

    //----frequency related----
    lteHelper->SetEnbDeviceAttribute("DlEarfcn", UintegerValue(100));   // 2120MHz
    lteHelper->SetEnbDeviceAttribute("UlEarfcn", UintegerValue(18100)); // 1930MHz
    lteHelper->SetEnbDeviceAttribute("DlBandwidth", UintegerValue(25)); // 5MHz
    lteHelper->SetEnbDeviceAttribute("UlBandwidth", UintegerValue(25)); // 5MHz

    //----others----
    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
    Config::SetDefault("ns3::LteAmc::AmcModel", EnumValue(LteAmc::PiroEW2010));
    Config::SetDefault("ns3::LteAmc::Ber", DoubleValue(0.01));
    Config::SetDefault("ns3::PfFfMacScheduler::HarqEnabled", BooleanValue(true));

    Config::SetDefault("ns3::FfMacScheduler::UlCqiFilter", EnumValue(FfMacScheduler::SRS_UL_CQI));

    // Radio link failure detection parameters
    Config::SetDefault("ns3::LteUeRrc::N310", UintegerValue(n310));
    Config::SetDefault("ns3::LteUeRrc::N311", UintegerValue(n311));
    Config::SetDefault("ns3::LteUeRrc::T310", TimeValue(t310));

    NS_LOG_INFO("Create the internet");
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    // Create a single RemoteHost0x18ab460
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    NS_LOG_INFO("Create eNodeB and UE nodes");
    NodeContainer enbNodes;
    NodeContainer ueNodes;
    enbNodes.Create(numberOfEnbs);
    ueNodes.Create(numberOfUes);

    NS_LOG_INFO("Assign mobility");
    Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator>();

    for (uint16_t i = 0; i < numberOfEnbs; i++)
    {
        positionAllocEnb->Add(Vector(interSiteDistance * i, 0, 0));
    }
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAllocEnb);
    mobility.Install(enbNodes);

    Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator>();

    for (int i = 0; i < numberOfUes; i++)
    {
        positionAllocUe->Add(Vector(200, 0, 0));
    }

    mobility.SetPositionAllocator(positionAllocUe);
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(ueNodes);

    for (int i = 0; i < numberOfUes; i++)
    {
        ueNodes.Get(i)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(
            Vector(30, 0.0, 0.0));
    }

    NS_LOG_INFO("Install LTE Devices in eNB and UEs and fix random number stream");
    NetDeviceContainer enbDevs;
    NetDeviceContainer ueDevs;

    int64_t randomStream = 1;

    enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    randomStream += lteHelper->AssignStreams(enbDevs, randomStream);
    ueDevs = lteHelper->InstallUeDevice(ueNodes);
    randomStream += lteHelper->AssignStreams(ueDevs, randomStream);

    NS_LOG_INFO("Install the IP stack on the UEs");
    internet.Install(ueNodes);
    Ipv4InterfaceContainer ueIpIfaces;
    ueIpIfaces = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueDevs));

    NS_LOG_INFO("Attach a UE to a eNB");
    lteHelper->Attach(ueDevs);

    NS_LOG_INFO("Install and start applications on UEs and remote host");
    uint16_t dlPort = 10000;
    uint16_t ulPort = 20000;

    DataRateValue dataRateValue = DataRate("18.6Mbps");

    uint64_t bitRate = dataRateValue.Get().GetBitRate();

    uint32_t packetSize = 1024; // bytes

    NS_LOG_DEBUG("bit rate " << bitRate);

    double interPacketInterval = static_cast<double>(packetSize * 8) / bitRate;

    Time udpInterval = Seconds(interPacketInterval);

    NS_LOG_DEBUG("UDP will use application interval " << udpInterval.As(Time::S) << " sec");

    for (uint32_t u = 0; u < numberOfUes; ++u)
    {
        Ptr<Node> ue = ueNodes.Get(u);
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(ue->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

        for (uint32_t b = 0; b < numBearersPerUe; ++b)
        {
            ApplicationContainer ulClientApps;
            ApplicationContainer ulServerApps;
            ApplicationContainer dlClientApps;
            ApplicationContainer dlServerApps;

            ++dlPort;
            ++ulPort;

            NS_LOG_LOGIC("installing UDP DL app for UE " << u + 1);
            UdpClientHelper dlClientHelper(ueIpIfaces.GetAddress(u), dlPort);
            dlClientHelper.SetAttribute("Interval", TimeValue(udpInterval));
            dlClientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
            dlClientHelper.SetAttribute("MaxPackets", UintegerValue(1000000));
            dlClientApps.Add(dlClientHelper.Install(remoteHost));

            PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), dlPort));
            dlServerApps.Add(dlPacketSinkHelper.Install(ue));

            NS_LOG_LOGIC("installing UDP UL app for UE " << u + 1);
            UdpClientHelper ulClientHelper(remoteHostAddr, ulPort);
            ulClientHelper.SetAttribute("Interval", TimeValue(udpInterval));
            dlClientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
            ulClientHelper.SetAttribute("MaxPackets", UintegerValue(1000000));
            ulClientApps.Add(ulClientHelper.Install(ue));

            PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), ulPort));
            ulServerApps.Add(ulPacketSinkHelper.Install(remoteHost));

            Ptr<EpcTft> tft = Create<EpcTft>();
            EpcTft::PacketFilter dlpf;
            dlpf.localPortStart = dlPort;
            dlpf.localPortEnd = dlPort;
            tft->Add(dlpf);
            EpcTft::PacketFilter ulpf;
            ulpf.remotePortStart = ulPort;
            ulpf.remotePortEnd = ulPort;
            tft->Add(ulpf);
            EpsBearer bearer(EpsBearer::NGBR_IMS);
            lteHelper->ActivateDedicatedEpsBearer(ueDevs.Get(u), bearer, tft);

            dlServerApps.Start(Seconds(0.27));
            dlClientApps.Start(Seconds(0.27));
            ulServerApps.Start(Seconds(0.27));
            ulClientApps.Start(Seconds(0.27));
#ifdef HAS_NETSIMULYZER
            if (enableVisualization)
            {
                std::ostringstream oss;
                oss << "Client " << (u + 1) << " starting\n";
                Simulator::Schedule(Seconds(0.27), &WriteApplicationLog, oss.str());
            }
#endif
        }
    }
    NS_LOG_INFO("Enable Lte traces and connect custom trace sinks");

    lteHelper->EnableTraces();
    Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats();
    rlcStats->SetAttribute("EpochDuration", TimeValue(Seconds(0.05)));
    Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats();
    pdcpStats->SetAttribute("EpochDuration", TimeValue(Seconds(0.05)));

    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                    MakeCallback(&NotifyConnectionEstablishedEnb));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                    MakeCallback(&NotifyConnectionEstablishedUe));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteUeRrc/StateTransition",
                                  MakeCallback(&UeStateTransition));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteUeRrc/PhySyncDetection",
                                  MakeBoundCallback(&PhySyncDetection, n310));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteUeRrc/RadioLinkFailure",
                                  MakeBoundCallback(&RadioLinkFailure, t310));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteEnbRrc/NotifyConnectionRelease",
                                  MakeCallback(&NotifyConnectionReleaseAtEnodeB));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteEnbRrc/RrcTimeout",
                                  MakeCallback(&EnbRrcTimeout));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteUeRrc/RandomAccessError",
                                  MakeCallback(&NotifyRandomAccessErrorUe));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionTimeout",
                                  MakeCallback(&NotifyConnectionTimeoutUe));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/"
                                  "ComponentCarrierMapUe/*/LteUeMac/RaResponseTimeout",
                                  MakeCallback(&NotifyRaResponseTimeoutUe));

#ifdef HAS_NETSIMULYZER
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/"
                    "LteUePhy/ReportUeMeasurements",
                    MakeCallback(&NotifyReportUeMeasurements));
#endif

    // Trace sink for the packet sink of UE
    std::ostringstream oss;
    oss << "/NodeList/" << ueNodes.Get(0)->GetId() << "/ApplicationList/0/$ns3::PacketSink/Rx";
    Config::ConnectWithoutContext(oss.str(), MakeCallback(&ReceivePacket));

    bool firstWrite = true;
    std::string rrcType = useIdealRrc ? "ideal_rrc" : "real_rrc";
    std::string fileName = "rlf_dl_thrput_" + std::to_string(enbNodes.GetN()) + "_eNB_" + rrcType;
    Time binSize = Seconds(0.2);
    Simulator::Schedule(Seconds(0.47), &Throughput, firstWrite, binSize, fileName);

#ifdef HAS_NETSIMULYZER
    /* Define graphs to visualize */
    if (enableVisualization)
    {
        NS_LOG_INFO("Enabling visualization...");

        orchestrator = CreateObject<netsimulyzer::Orchestrator>("lena-radio-link-failure.json");
        orchestrator->SetAttribute("MobilityPollInterval", TimeValue(MilliSeconds(guiResolution)));

        // Configure nodes
        netsimulyzer::NodeConfigurationHelper nodeConfigHelper(orchestrator);
        nodeConfigHelper.Set("Scale", DoubleValue(5));
        for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
        {
            nodeConfigHelper.Set("Model", netsimulyzer::models::LAND_DRONE_VALUE);
            nodeConfigHelper.Set("Name", StringValue("UE " + std::to_string(i)));
            nodeConfigHelper.Install(ueNodes.Get(i));

            // Add state machine graphs
            std::vector<netsimulyzer::CategoryAxis::ValuePair> rrcStates;
            netsimulyzer::CategoryAxis::ValuePair rrcStateValue;

            for (uint32_t j = 0; j < LteUeRrc::NUM_STATES; j++)
            {
                rrcStateValue.key = j;
                rrcStateValue.value = g_ueRrcStateName[j];
                rrcStates.push_back(rrcStateValue);
            }

            Ptr<netsimulyzer::StateTransitionSink> rrcStateGraph =
                CreateObject<netsimulyzer::StateTransitionSink>(orchestrator, rrcStates, 0);
            rrcStateGraph->SetAttribute("Name",
                                        StringValue("UE " + std::to_string(i + 1) + " RRC State"));
            rrcStateGraph->SetAttribute("LoggingMode", StringValue("None"));
            rrcStateMachines.insert(std::pair<uint32_t, Ptr<netsimulyzer::StateTransitionSink>>(
                ueDevs.Get(i)->GetObject<LteUeNetDevice>()->GetImsi(),
                rrcStateGraph));
            PointerValue stateSeries;
            rrcStateGraph->GetAttribute("Series", stateSeries);
            stateSeries.Get<netsimulyzer::CategoryValueSeries>()->SetAttribute(
                "Color",
                netsimulyzer::BLUE_VALUE);
        }

        // Network
        for (uint32_t i = 0; i < enbNodes.GetN(); ++i)
        {
            nodeConfigHelper.Set("Model", netsimulyzer::models::CELL_TOWER_POLE_VALUE);
            nodeConfigHelper.Set("Name", StringValue("Cell tower " + std::to_string(i)));
            nodeConfigHelper.Set("Height", netsimulyzer::OptionalValue<double>(10));
            nodeConfigHelper.Set("Orientation", Vector3DValue(Vector3D(0, 0, 0)));
            nodeConfigHelper.Install(enbNodes.Get(i));
        }

        // Logs
        applicationLog = CreateObject<netsimulyzer::LogStream>(orchestrator);
        applicationLog->SetAttribute("Name", StringValue("Application log"));
        applicationLog->SetAttribute(
            "Color",
            netsimulyzer::OptionalValue<netsimulyzer::Color3>{netsimulyzer::GREEN});

        ueLog = CreateObject<netsimulyzer::LogStream>(orchestrator);
        ueLog->SetAttribute("Name", StringValue("UE log"));
        ueLog->SetAttribute("Color",
                            netsimulyzer::OptionalValue<netsimulyzer::Color3>{netsimulyzer::BLUE});

        enbLog = CreateObject<netsimulyzer::LogStream>(orchestrator);
        enbLog->SetAttribute("Name", StringValue("eNodeB log"));
        enbLog->SetAttribute("Color",
                             netsimulyzer::OptionalValue<netsimulyzer::Color3>{netsimulyzer::RED});

        // Statistics
        appRxTraceSeries = CreateObject<netsimulyzer::ThroughputSink>(orchestrator, "Rx");
        appRxTraceSeries->SetAttribute("Unit", StringValue("Mb/s"));
        appRxTraceSeries->SetAttribute("Interval", TimeValue(Seconds(0.2)));
        PointerValue rxXySeries;
        appRxTraceSeries->GetAttribute("XYSeries", rxXySeries);
        rxXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("LabelMode", StringValue("Hidden"));
        rxXySeries.Get<netsimulyzer::XYSeries>()->SetAttribute("Color", netsimulyzer::BLUE_VALUE);
    }
#endif

    NS_LOG_INFO("Starting simulation...");

    Simulator::Stop(simTime);

    Simulator::Run();

    NS_ABORT_MSG_IF(counterN310FirsteNB != n310,
                    "UE RRC should receive " << n310
                                             << " out-of-sync indications in Cell 1."
                                                " Total received = "
                                             << counterN310FirsteNB);

    Simulator::Destroy();

    return 0;
}
