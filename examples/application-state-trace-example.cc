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
#include <utility>
#include <ns3/core-module.h>
#include <ns3/applications-module.h>
#include <ns3/netsimulyzer-module.h>

// Example demonstrating tracing the state of a custom ns3::Application
// using the StateTransitionSink.
//
// Node placement isn't considered here since it does not
// affect simulation results

using namespace ns3;

/**
 * Example application for the StateTransitionSink
 */
class DummyApplication : public Application
{
  const Time stateChangeDelay;

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

  explicit DummyApplication (Time stateChangeDelay)
      : stateChangeDelay (std::move (stateChangeDelay))
  {
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

    m_eventId = Simulator::Schedule (stateChangeDelay, &DummyApplication::ChangeState, this);
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

const std::vector<std::string> DummyApplication::States{"Stopped", "Waiting", "Transmitting"};

int
main (int argc, char *argv[])
{
  double duration = 100;
  std::string outputFileName = "application-state-trace-example-netsimulyzer.json";

  CommandLine cmd{__FILE__};
  cmd.AddValue ("duration", "Duration (in Seconds) of the simulation", duration);
  cmd.AddValue ("outputFileName", "The name of the file to write the NetSimulyzer trace info",
                outputFileName);
  cmd.Parse (argc, argv);

  NS_ABORT_MSG_IF (duration < 2.0, "Scenario must be at least two seconds long");

  auto node = CreateObject<Node> ();

  auto exampleApplication = CreateObject<DummyApplication> (Seconds (1.0));
  exampleApplication->SetStartTime (Seconds (1.0));
  exampleApplication->SetStopTime (Seconds (duration - 1.0));

  node->AddApplication (exampleApplication);

  auto orchestrator = CreateObject<netsimulyzer::Orchestrator> (outputFileName);

  // No Nodes Move in this scenario
  orchestrator->SetPollMobility(false);

  netsimulyzer::NodeConfigurationHelper nodeHelper{orchestrator};
  nodeHelper.Set("Model", netsimulyzer::models::SERVER_VALUE);
  nodeHelper.Install(node);

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
  Simulator::Destroy ();
}
