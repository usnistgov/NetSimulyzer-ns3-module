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
 * Author: Evan Black <evan.black@nist.gov>
 */

#include <string>
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/netsimulyzer-module.h>

// Example demonstrating mobility output to the NIST visualizer
// Uses models from the Visualization Licensed Resources repository
// * Creates 2 buildings one the top right and the other in bottom left corner
//
// * Creates 4 Nodes in 2 separate groups (Phones & Drones)
// * The Nodes move in a random direction independently
//     For the duration of the simulation
//
// --------------------------
// |                      22| (Two Floor Building)
// |                      22|
// |   ~~~~~~~~~~~~~~~~~~   |
// |   \   Possible     \   |
// |   \     Node       \   |
// |   \   Positions    \   |
// |   \                \   |
// |   ~~~~~~~~~~~~~~~~~~   |
// |11                      |
// |11                      |
// --------------------------
// (One Floor Building)

using namespace ns3;

/**
 * Example application for the StateTransitionSink
 */
class DummyApplication : public Application
{
public:
  // Keep a list of all possible states
  // as strings
  const static std::vector<std::string> States;

  // States may be std::strings or enum/ints
  typedef void (*StateChangedCallback) (const std::string &newState);

  static TypeId
  GetTypeId (void)
  {
    static TypeId tid =
        TypeId ("DummyApplication")
            .SetParent<ns3::Application> ()
            .SetGroupName ("netsimulyzer")
            // Provide some 'StateChanged' trace, or connect to the proper callbacks yourself
            .AddTraceSource ("StateChanged", "Trace called when the application changes states",
                             MakeTraceSourceAccessor (&DummyApplication::m_stateChangedTrace),
                             "DummyApplication::StateChangedCallback");

    return tid;
  }

  void
  Stop (void)
  {
    // "Stopped"
    m_currentState = DummyApplication::States[0];
    m_stateChangedTrace (m_currentState);
  }

  void
  Wait (void)
  {
    // "Waiting"
    m_currentState = DummyApplication::States[1];
    m_stateChangedTrace (m_currentState);
  }

  void
  Transmit (void)
  {
    // "Transmitting"
    m_currentState = DummyApplication::States[2];
    m_stateChangedTrace (m_currentState);
  }

  void
  ChangeState ()
  {
    m_stateChangeCount++;

    // Create a pattern between waiting and sending
    // Toggling between the two
    if (m_stateChangeCount % 2u)
      Wait ();
    else
      Transmit ();

    m_eventId = Simulator::Schedule (Seconds (1), &DummyApplication::ChangeState, this);
  }

private:
  std::string m_currentState{DummyApplication::States[0]};
  unsigned int m_stateChangeCount{0};
  EventId m_eventId;
  TracedCallback<const std::string &> m_stateChangedTrace;

  void
  StartApplication (void) override
  {
    ChangeState ();
  }

  void
  StopApplication (void) override
  {
    Stop ();
    Simulator::Cancel (m_eventId);
  }
};

const std::vector<std::string> DummyApplication::States = {"Stopped", "Waiting", "Transmitting"};

Ptr<netsimulyzer::LogStream> eventLog;

void
CourseChanged (Ptr<const MobilityModel> model)
{
  const auto nodeId = model->GetObject<Node> ()->GetId ();
  const auto position = model->GetPosition ();
  const auto velocity = model->GetVelocity ();

  *eventLog << Simulator::Now ().GetMilliSeconds () << ": Node [" << nodeId
            << "] Course Change Position: [" << position.x << ", " << position.y << ", "
            << position.z << "] "
            << "Velocity [" << velocity.x << ", " << velocity.y << ", " << velocity.z << "]\n";
}

int
main (int argc, char *argv[])
{
  double minNodePosition = -100;
  double maxNodePosition = 100;
  // These must remain positive (since the RandomDirection2dMobilityModel only accepts positive values)
  double minSpeed = .1;
  double maxSpeed = 5;
  double duration = 100;
  std::string outputFileName = "netsimulyzer-mobility-buildings-example.json";
  std::string phoneModelPath = netsimulyzer::models::SMARTPHONE;
  std::string droneModelPath = netsimulyzer::models::LAND_DRONE;

  CommandLine cmd;
  cmd.AddValue ("minNodePosition", "Minimum X/Y position a Node may move to", minNodePosition);
  cmd.AddValue ("maxNodePosition", "Maximum X/Y position a Node may move to", maxNodePosition);
  cmd.AddValue ("minSpeed", "Minimum X/Y speed a Node may move", minSpeed);
  cmd.AddValue ("maxSpeed", "Maximum X/Y speed a Node may move", maxSpeed);
  cmd.AddValue ("outputFileName", "The name of the file to write the visualizer trace info",
                outputFileName);
  cmd.AddValue ("phoneModelPath", "The path to the model file to represent the Phone Nodes",
                phoneModelPath);
  cmd.AddValue ("droneModelPath", "The path to the model file to represent the Drone Nodes",
                droneModelPath);
  cmd.AddValue ("duration", "Duration (in Seconds) of the simulation", duration);
  cmd.Parse (argc, argv);

  NS_ABORT_MSG_IF (duration < 1.0, "Scenario must be at least one second long");

  // ---- Nodes ----
  NodeContainer phones;
  phones.Create (2);

  NodeContainer drones;
  drones.Create (2);

  auto positionAllocator = CreateObject<RandomBoxPositionAllocator> ();
  auto positionStream = CreateObject<UniformRandomVariable> ();
  positionStream->SetAttribute ("Min", DoubleValue (minNodePosition));
  positionStream->SetAttribute ("Max", DoubleValue (maxNodePosition));

  // (Hopefully) start the Nodes at different positions
  positionAllocator->SetX (positionStream);
  positionAllocator->SetY (positionStream);
  positionAllocator->SetAttribute ("Z", StringValue ("ns3::ConstantRandomVariable[Constant=0.0])"));

  // Show the Nodes moving at different speeds
  auto velocityStream = CreateObject<UniformRandomVariable> ();
  velocityStream->SetAttribute ("Min", DoubleValue (minSpeed));
  velocityStream->SetAttribute ("Max", DoubleValue (maxSpeed));

  MobilityHelper mobility;
  mobility.SetMobilityModel (
      "ns3::RandomDirection2dMobilityModel", "Bounds",
      RectangleValue ({minNodePosition, maxNodePosition, minNodePosition, maxNodePosition}),
      "Speed", PointerValue (velocityStream), "Pause",
      StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  mobility.SetPositionAllocator (positionAllocator);
  mobility.Install (phones);
  mobility.Install (drones);

  for (auto iter = NodeList::Begin (); iter != NodeList::End (); iter++)
    {
      auto m = (*iter)->GetObject<MobilityModel> ();
      if (!m)
        continue;
      m->TraceConnectWithoutContext ("CourseChange", MakeCallback (&CourseChanged));
    }

  // ---- Buildings ----
  BuildingContainer buildings;

  Ptr<Building> simpleBuilding = CreateObject<Building> ();
  simpleBuilding->SetBoundaries ({-150.0, -130.0, -150.0, -130.0, 0.0, 20.0});
  buildings.Add (simpleBuilding);

  Ptr<Building> twoFloorBuilding = CreateObject<Building> ();
  twoFloorBuilding->SetBoundaries ({150.0, 130.0, 150.0, 130.0, 0.0, 40.0});
  twoFloorBuilding->SetNFloors (2);
  buildings.Add (twoFloorBuilding);

  // ---- Visualization ----
  auto orchestrator = CreateObject<netsimulyzer::Orchestrator> (outputFileName);

  // Mark possible Node locations
  auto possibleNodeLocations = CreateObject<netsimulyzer::RectangularArea> (
      orchestrator, Rectangle{minNodePosition, maxNodePosition, minNodePosition, maxNodePosition});

  // Identify the area
  possibleNodeLocations->SetAttribute ("Name", StringValue ("Possible Node Locations"));

  // Move the area indicator slightly below the feet of the Nodes
  possibleNodeLocations->SetAttribute ("Height", DoubleValue (-0.5));

  // Mark with a light green color
  possibleNodeLocations->SetAttribute ("FillColor", netsimulyzer::Color3Value ({204u, 255u, 204u}));

  auto infoLog = CreateObject<netsimulyzer::LogStream> (orchestrator);
  eventLog = CreateObject<netsimulyzer::LogStream> (orchestrator);

  // Log the base configuration for the scenario
  *infoLog << "----- Scenario Settings -----\n";
  *infoLog << "Node Position Range: [" << minNodePosition << ',' << maxNodePosition << "]\n";
  *infoLog << "Node Speed Range: [" << minSpeed << ',' << maxSpeed << "]\n";
  *infoLog << "Models: Phone [" << phoneModelPath << "], Drone [" << droneModelPath << "]\n";
  *infoLog << "Scenario Duration (Seconds): " << duration << '\n';

  netsimulyzer::NodeConfigurationHelper nodeConfigHelper (orchestrator);

  nodeConfigHelper.Set ("Model", StringValue (phoneModelPath));
  nodeConfigHelper.Install (phones);

  nodeConfigHelper.Set ("Model", StringValue (droneModelPath));
  nodeConfigHelper.Install (drones);

  // Only explicitly configured items will be shown
  // so, even if we don't have options to set
  // the buildings bust be configured
  netsimulyzer::BuildingConfigurationHelper buildingConfigHelper (orchestrator);
  buildingConfigHelper.Install (buildings);

  // StateTraceSink
  auto exampleApplication = CreateObject<DummyApplication> ();
  exampleApplication->SetStartTime (Seconds (1.0));
  exampleApplication->SetStopTime (Seconds (duration - 1.0));

  // Doesn't really matter what Node it's on
  phones.Get (0u)->AddApplication (exampleApplication);

  auto exampleStateSink = CreateObject<netsimulyzer::StateTransitionSink> (
      orchestrator, // Orchestrator for series & log
      DummyApplication::States, // Possible States (with optional IDs)
      DummyApplication::States[0] // Initial state
  );

  exampleStateSink->SetAttribute ("Name", StringValue ("Dummy Application"));

  // Use StateChangedName for string states
  // & StateChangedId for enum/int states
  exampleApplication->TraceConnectWithoutContext (
      "StateChanged",
      MakeCallback (&netsimulyzer::StateTransitionSink::StateChangedName, exampleStateSink));

  Simulator::Stop (Seconds (duration));
  Simulator::Run ();

  *infoLog << "Scenario Finished\n";
  Simulator::Destroy ();
}
