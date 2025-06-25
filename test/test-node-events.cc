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

#include "ns3/netsimulyzer-ns3-compatibility.h"
#include "netsimulyzer-test-utils.h"

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netsimulyzer-module.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/test.h"

#include <string>
#include <string_view>
#include <vector>

namespace ns3::test
{
using namespace netsimulyzer;

class TestCaseNodePositionEvent : public NetSimulyzerTestCase
{
  public:
    TestCaseNodePositionEvent();

  private:
    void DoRun() override;
};

TestCaseNodePositionEvent::TestCaseNodePositionEvent()
    : NetSimulyzerTestCase("NetSimulyzer - Node Position Event")
{
}

void
TestCaseNodePositionEvent::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    // Disable polling, since we want to control the number of generated events
    o->SetAttribute("PollMobility", BooleanValue{false});

    auto ns3Node = CreateObject<Node>();
    auto nodeConfig = CreateObject<NodeConfiguration>(o);
    ns3Node->AggregateObject(nodeConfig);

    auto mobility = CreateObject<ConstantPositionMobilityModel>();
    mobility->SetAttribute("Position", VectorValue{{0.0, 0.0, 0.0}});
    ns3Node->AggregateObject(mobility);

    Simulator::Stop(MilliSeconds(100UL));

    const Vector3D target{10.0, 0.0, 0.0};
    const auto eventTime = MilliSeconds(25UL);
    Simulator::Schedule(eventTime, [mobility, target]() { mobility->SetPosition(target); });

    Simulator::Run();

    const auto& output = o->GetJson();
    const auto& nodes = output["nodes"];

    // Just in case
    NS_TEST_ASSERT_MSG_EQ(nodes.empty(), false, "'nodes' section should not be empty");

    const auto& node = *nodes.begin();
    const auto& position = node["position"];
    RequiredFields({"x", "y", "z"}, position, "position");
    NS_TEST_ASSERT_MSG_EQ(position["x"].get<double>(), 0.0, "Initial 'x' position should be 0.0");
    NS_TEST_ASSERT_MSG_EQ(position["y"].get<double>(), 0.0, "Initial 'y' position should be 0.0");
    NS_TEST_ASSERT_MSG_EQ(position["z"].get<double>(), 0.0, "Initial 'z' position should be 0.0");

    const auto& events = output["events"];
    NS_TEST_ASSERT_MSG_EQ(events.empty(), false, "'events' section should not be empty");

    const auto& event = *events.begin();
    RequiredFields({"type", "nanoseconds"}, event, "basic event");

    const auto& eventType = event["type"].get<std::string>();
    NS_TEST_ASSERT_MSG_EQ(eventType, "node-position", "Event should be type 'node-position'");
    RequiredFields({"id", "x", "y", "z"}, event, "node-position");

    NS_TEST_ASSERT_MSG_EQ(event["id"].get<uint32_t>(),
                          ns3Node->GetId(),
                          "Event should be tagged with the ID of the Node that made it");

    NS_TEST_ASSERT_MSG_EQ(event["x"].get<double>(),
                          target.x,
                          "Event 'x' position should match target");
    NS_TEST_ASSERT_MSG_EQ(event["y"].get<double>(),
                          target.y,
                          "Event 'y' position should match target");
    NS_TEST_ASSERT_MSG_EQ(event["z"].get<double>(),
                          target.z,
                          "Event 'z' position should match target");

    NS_TEST_ASSERT_MSG_EQ(event["nanoseconds"].get<int64_t>(),
                          eventTime.GetNanoSeconds(),
                          "Event 'nanoseconds' should match scheduled time");

    Simulator::Destroy();
}

class TestCaseNodeOrientationEvent : public NetSimulyzerTestCase
{
  public:
    TestCaseNodeOrientationEvent();

  private:
    void DoRun() override;
};

TestCaseNodeOrientationEvent::TestCaseNodeOrientationEvent()
    : NetSimulyzerTestCase("NetSimulyzer - Node Orientation Event")
{
}

void
TestCaseNodeOrientationEvent::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    auto ns3Node = CreateObject<Node>();
    auto nodeConfig = CreateObject<NodeConfiguration>(o);
    ns3Node->AggregateObject(nodeConfig);

    Vector3DValue initialOrientation;
    nodeConfig->GetAttribute("Orientation", initialOrientation);
    NS_TEST_ASSERT_MSG_EQ(initialOrientation.Get(),
                          Vector3D(0.0, 0.0, 0.0),
                          "Initial orientation should be 0.0, 0.0, 0.0");

    nodeConfig->SetAttribute("Orientation", Vector3DValue({30.0, 60.0, 90.0}));

    Simulator::Stop(MilliSeconds(100UL));

    const Vector3D target{10.0, 20.0, 25.0};
    const auto eventTime = MilliSeconds(25UL);
    Simulator::Schedule(eventTime, [nodeConfig, target]() {
        nodeConfig->SetAttribute("Orientation", Vector3DValue{target});
    });

    Simulator::Run();

    const auto& output = o->GetJson();
    const auto& nodes = output["nodes"];

    // Just in case
    NS_TEST_ASSERT_MSG_EQ(nodes.empty(), false, "'nodes' section should not be empty");

    const auto& node = *nodes.begin();
    const auto& outputInitalOrientation = node["orientation"];
    RequiredFields({"x", "y", "z"}, outputInitalOrientation, "orientation");
    NS_TEST_ASSERT_MSG_EQ(outputInitalOrientation["x"].get<double>(),
                          30.0,
                          "Initial 'x' orientation should be 30.0");
    NS_TEST_ASSERT_MSG_EQ(outputInitalOrientation["y"].get<double>(),
                          60.0,
                          "Initial 'y' orientation should be 60.0");
    NS_TEST_ASSERT_MSG_EQ(outputInitalOrientation["z"].get<double>(),
                          90.0,
                          "Initial 'z' orientation should be 90.0");

    const auto& events = output["events"];
    NS_TEST_ASSERT_MSG_EQ(events.empty(), false, "'events' section should not be empty");

    const auto& event = *events.begin();
    RequiredFields({"type", "nanoseconds"}, event, "basic event");

    const auto& eventType = event["type"].get<std::string>();
    NS_TEST_ASSERT_MSG_EQ(eventType, "node-orientation", "Event should be type 'node-orientation'");
    RequiredFields({"id", "x", "y", "z"}, event, "node-orientation");

    NS_TEST_ASSERT_MSG_EQ(event["id"].get<uint32_t>(),
                          ns3Node->GetId(),
                          "Event should be tagged with the ID of the Node that made it");

    NS_TEST_ASSERT_MSG_EQ(event["x"].get<double>(),
                          target.x,
                          "Event 'x' orientation should match target");
    NS_TEST_ASSERT_MSG_EQ(event["y"].get<double>(),
                          target.y,
                          "Event 'y' orientation should match target");
    NS_TEST_ASSERT_MSG_EQ(event["z"].get<double>(),
                          target.z,
                          "Event 'z' orientation should match target");

    NS_TEST_ASSERT_MSG_EQ(event["nanoseconds"].get<int64_t>(),
                          eventTime.GetNanoSeconds(),
                          "Event 'nanoseconds' should match scheduled time");

    Simulator::Destroy();
}

class TestCaseNodeColorChangeEvent : public NetSimulyzerTestCase
{
  public:
    TestCaseNodeColorChangeEvent();

  private:
    void DoRun() override;
};

TestCaseNodeColorChangeEvent::TestCaseNodeColorChangeEvent()
    : NetSimulyzerTestCase("NetSimulyzer - Node Color Change Event")
{
}

void
TestCaseNodeColorChangeEvent::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    auto ns3Node = CreateObject<Node>();
    auto nodeConfig = CreateObject<NodeConfiguration>(o);
    ns3Node->AggregateObject(nodeConfig);

    Simulator::Stop(MilliSeconds(100UL));

    const auto target{RED_OPTIONAL_VALUE};
    const auto eventTime = MilliSeconds(25UL);
    Simulator::Schedule(eventTime,
                        [nodeConfig, target]() { nodeConfig->SetAttribute("BaseColor", target); });

    Simulator::Run();

    const auto& output = o->GetJson();
    const auto& nodes = output["nodes"];

    // Just in case
    NS_TEST_ASSERT_MSG_EQ(nodes.empty(), false, "'nodes' section should not be empty");

    const auto& node = *nodes.begin();
    NS_TEST_ASSERT_MSG_EQ(node.contains("base-color"), false, "Node should not have base color");
    NS_TEST_ASSERT_MSG_EQ(node.contains("highlight-color"),
                          false,
                          "Node should not have highlight color");

    const auto& events = output["events"];
    NS_TEST_ASSERT_MSG_EQ(events.empty(), false, "'events' section should not be empty");

    const auto& event = *events.begin();
    RequiredFields({"type", "nanoseconds"}, event, "basic event");

    const auto& eventType = event["type"].get<std::string>();
    NS_TEST_ASSERT_MSG_EQ(eventType, "node-color", "Event should be type 'node-color'");
    RequiredFields({"id", "color-type", "color"}, event, "node-color");

    NS_TEST_ASSERT_MSG_EQ(event["id"].get<uint32_t>(),
                          ns3Node->GetId(),
                          "Event should be tagged with the ID of the Node that made it");

    NS_TEST_ASSERT_MSG_EQ(event["color-type"].get<std::string>(),
                          "base",
                          "Color change event should change 'base' color");

    CheckColor(event["color"], target.GetValue());

    NS_TEST_ASSERT_MSG_EQ(event["nanoseconds"].get<int64_t>(),
                          eventTime.GetNanoSeconds(),
                          "Event 'nanoseconds' should match scheduled time");

    Simulator::Destroy();
}

class TestCaseNodeModelChangeEvent : public NetSimulyzerTestCase
{
  public:
    TestCaseNodeModelChangeEvent();

  private:
    void DoRun() override;
};

TestCaseNodeModelChangeEvent::TestCaseNodeModelChangeEvent()
    : NetSimulyzerTestCase("NetSimulyzer - Node Model Change Event")
{
}

void
TestCaseNodeModelChangeEvent::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    auto ns3Node = CreateObject<Node>();
    auto nodeConfig = CreateObject<NodeConfiguration>(o);
    ns3Node->AggregateObject(nodeConfig);

    const auto inital = models::CUBE_VALUE;
    nodeConfig->SetAttribute("Model", inital);

    Simulator::Stop(MilliSeconds(100UL));

    const StringValue target{models::LAND_DRONE_VALUE};
    const auto eventTime = MilliSeconds(25UL);
    Simulator::Schedule(eventTime,
                        [nodeConfig, target]() { nodeConfig->SetAttribute("Model", target); });

    Simulator::Run();

    const auto& output = o->GetJson();
    const auto& nodes = output["nodes"];

    // Just in case
    NS_TEST_ASSERT_MSG_EQ(nodes.empty(), false, "'nodes' section should not be empty");

    const auto& node = *nodes.begin();
    NS_TEST_ASSERT_MSG_EQ(node["model"].get<std::string>(),
                          inital.Get(),
                          "Initial model should be `CUBE_VALUE`");

    const auto& events = output["events"];
    NS_TEST_ASSERT_MSG_EQ(events.empty(), false, "'events' section should not be empty");

    const auto& event = *events.begin();
    RequiredFields({"type", "nanoseconds"}, event, "basic event");

    const auto& eventType = event["type"].get<std::string>();
    NS_TEST_ASSERT_MSG_EQ(eventType,
                          "node-model-change",
                          "Event should be type 'node-model-change'");
    RequiredFields({"id", "model"}, event, "node-model-change");

    NS_TEST_ASSERT_MSG_EQ(event["id"].get<uint32_t>(),
                          ns3Node->GetId(),
                          "Event should be tagged with the ID of the Node that made it");

    NS_TEST_ASSERT_MSG_EQ(event["model"].get<std::string>(),
                          target.Get(),
                          "Event 'model' should match target");

    NS_TEST_ASSERT_MSG_EQ(event["nanoseconds"].get<int64_t>(),
                          eventTime.GetNanoSeconds(),
                          "Event 'nanoseconds' should match scheduled time");

    Simulator::Destroy();
}

class TestCaseNodeTransmitEvent : public NetSimulyzerTestCase
{
  public:
    TestCaseNodeTransmitEvent();

  private:
    void DoRun() override;
};

TestCaseNodeTransmitEvent::TestCaseNodeTransmitEvent()
    : NetSimulyzerTestCase("NetSimulyzer - Node Transmit Event")
{
}

void
TestCaseNodeTransmitEvent::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    auto ns3Node = CreateObject<Node>();
    auto nodeConfig = CreateObject<NodeConfiguration>(o);
    ns3Node->AggregateObject(nodeConfig);

    const auto inital = models::CUBE_VALUE;
    nodeConfig->SetAttribute("Model", inital);

    Simulator::Stop(MilliSeconds(100UL));

    constexpr auto transmitSize = 5.0;
    const auto transmitDuration = Seconds(2.0);
    const auto transmitColor = RED;

    const auto eventTime = MilliSeconds(25UL);
    Simulator::Schedule(eventTime, [nodeConfig, transmitDuration, transmitColor]() {
        nodeConfig->Transmit(transmitDuration, transmitSize, transmitColor);
    });

    Simulator::Run();

    const auto& output = o->GetJson();

    const auto& events = output["events"];
    NS_TEST_ASSERT_MSG_EQ(events.empty(), false, "'events' section should not be empty");

    const auto& event = *events.begin();
    RequiredFields({"type", "nanoseconds"}, event, "basic event");

    const auto& eventType = event["type"].get<std::string>();
    NS_TEST_ASSERT_MSG_EQ(eventType, "node-transmit", "Event should be type 'node-transmit'");
    RequiredFields({"id", "duration", "target-size", "color"}, event, "node-transmit");

    NS_TEST_ASSERT_MSG_EQ(event["id"].get<uint32_t>(),
                          ns3Node->GetId(),
                          "Event should be tagged with the ID of the Node that made it");

    NS_TEST_ASSERT_MSG_EQ(event["duration"].get<int64_t>(),
                          transmitDuration.GetNanoSeconds(),
                          "Event duration should matct");
    NS_TEST_ASSERT_MSG_EQ(event["target-size"].get<double>(),
                          transmitSize,
                          "Event size should match");
    const auto& eventColor = event["color"];

    CheckColor(eventColor, transmitColor);

    NS_TEST_ASSERT_MSG_EQ(event["nanoseconds"].get<int64_t>(),
                          eventTime.GetNanoSeconds(),
                          "Event 'nanoseconds' should match scheduled time");

    Simulator::Destroy();
}

class NodeEventsTestSuite : public TestSuite
{
  public:
    NodeEventsTestSuite();
};

NodeEventsTestSuite::NodeEventsTestSuite()
    : TestSuite("netsimulyzer-node-events", TEST_TYPE_SYSTEM)
{
    AddTestCase(new TestCaseNodePositionEvent(), TEST_DURATION_QUICK);
    AddTestCase(new TestCaseNodeOrientationEvent{}, TEST_DURATION_QUICK);
    AddTestCase(new TestCaseNodeColorChangeEvent{}, TEST_DURATION_QUICK);
    AddTestCase(new TestCaseNodeModelChangeEvent{}, TEST_DURATION_QUICK);
    AddTestCase(new TestCaseNodeTransmitEvent{}, TEST_DURATION_QUICK);
}

static NodeEventsTestSuite g_nodeEventsTestSuite{};

} // namespace ns3::test
