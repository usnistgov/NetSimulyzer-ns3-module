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

#include "netsimulyzer-test-utils.h"

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netsimulyzer-module.h"
#include "ns3/netsimulyzer-ns3-compatibility.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/test.h"

#include <string>
#include <string_view>
#include <vector>

namespace ns3::test
{

using namespace netsimulyzer;

class TestCaseOutputStructure : public NetSimulyzerTestCase
{
  public:
    TestCaseOutputStructure();

  private:
    void DoRun() override;
};

TestCaseOutputStructure::TestCaseOutputStructure()
    : NetSimulyzerTestCase("NetSimulyzer Orchestrator - Output Structure")
{
}

void
TestCaseOutputStructure::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    Simulator::Stop(MilliSeconds(10UL));
    Simulator::Run();

    const auto& output = o->GetJson();

    NS_TEST_ASSERT_MSG_EQ(output.empty(), false, "Output should not be empty");

    NS_TEST_ASSERT_MSG_EQ(output.contains("configuration"),
                          true,
                          "Output must contain a 'configuration' entry");
    NS_TEST_ASSERT_MSG_EQ(output["configuration"].is_object(),
                          true,
                          "'configuration' must be object type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("nodes"), true, "Output must contain a 'nodes' entry");
    NS_TEST_ASSERT_MSG_EQ(output["nodes"].is_array(), true, "'nodes' must be array type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("events"), true, "Output must contain an 'events' entry");
    NS_TEST_ASSERT_MSG_EQ(output["events"].is_array(), true, "'events' must be array type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("links"), true, "Output must contain a 'links' entry");
    NS_TEST_ASSERT_MSG_EQ(output["links"].is_array(), true, "'links' must be array type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("buildings"),
                          true,
                          "Output must contain a 'buildings' entry");
    NS_TEST_ASSERT_MSG_EQ(output["buildings"].is_array(), true, "'buildings' must be array type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("decorations"),
                          true,
                          "Output must contain a 'decorations' entry");
    NS_TEST_ASSERT_MSG_EQ(output["decorations"].is_array(),
                          true,
                          "'decorations' must be array type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("areas"), true, "Output must contain a 'areas' entry");
    NS_TEST_ASSERT_MSG_EQ(output["areas"].is_array(), true, "'areas' must be array type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("series"), true, "Output must contain a 'series' entry");
    NS_TEST_ASSERT_MSG_EQ(output["series"].is_array(), true, "'series' must be array type");

    NS_TEST_ASSERT_MSG_EQ(output.contains("streams"),
                          true,
                          "Output must contain a 'streams' entry");
    NS_TEST_ASSERT_MSG_EQ(output["streams"].is_array(), true, "'streams' must be array type");

    Simulator::Destroy();
}

class TestCaseNodeInOutput : public NetSimulyzerTestCase
{
  public:
    TestCaseNodeInOutput();

  private:
    void DoRun() override;
};

TestCaseNodeInOutput::TestCaseNodeInOutput()
    : NetSimulyzerTestCase("NetSimulyzer Orchestrator - Node in output")
{
}

void
TestCaseNodeInOutput::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    auto ns3Node = CreateObject<Node>();
    auto nodeConfig = CreateObject<NodeConfiguration>(o);
    ns3Node->AggregateObject(nodeConfig);

    Simulator::Stop(MilliSeconds(10UL));
    Simulator::Run();

    const auto& output = o->GetJson();
    const auto& nodes = output["nodes"];

    NS_TEST_ASSERT_MSG_EQ(nodes.empty(), false, "'nodes' section should not be empty");

    const auto& node = *nodes.begin();

    RequiredFields({"id",
                    "name",
                    "label-enabled",
                    "model",
                    "scale",
                    "trail-enabled",
                    "orientation",
                    "offset",
                    "visible",
                    "position"},
                   node,
                   "node");

    NS_TEST_ASSERT_MSG_EQ(node["type"], "node", "Node type field must be 'node'");

    Simulator::Destroy();
}

class TestCaseNodeMobility : public NetSimulyzerTestCase
{
  public:
    TestCaseNodeMobility();

  private:
    void DoRun() override;
};

TestCaseNodeMobility::TestCaseNodeMobility()
    : NetSimulyzerTestCase("NetSimulyzer - Node Mobility")
{
}

void
TestCaseNodeMobility::DoRun()
{
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    auto ns3Node = CreateObject<Node>();
    auto nodeConfig = CreateObject<NodeConfiguration>(o);
    ns3Node->AggregateObject(nodeConfig);

    auto mobility = CreateObject<ConstantPositionMobilityModel>();
    mobility->SetAttribute("Position", VectorValue{{1.0, 2.0, 3.0}});
    ns3Node->AggregateObject(mobility);

    Simulator::Stop(MilliSeconds(100UL));
    Simulator::Run();

    const auto& output = o->GetJson();
    const auto& nodes = output["nodes"];

    // Just in case
    NS_TEST_ASSERT_MSG_EQ(nodes.empty(), false, "'nodes' section should not be empty");

    const auto& node = *nodes.begin();
    const auto& position = node["position"];
    RequiredFields({"x", "y", "z"}, position, "position");
    NS_TEST_ASSERT_MSG_EQ(position["x"].get<double>(), 1.0, "'x' Position should be 1.0");
    NS_TEST_ASSERT_MSG_EQ(position["y"].get<double>(), 2.0, "'y' Position should be 2.0");
    NS_TEST_ASSERT_MSG_EQ(position["z"].get<double>(), 3.0, "'z' Position should be 3.0");

    Simulator::Destroy();
}

class OrchestratorBasicOutputTestSuite : public TestSuite
{
  public:
    OrchestratorBasicOutputTestSuite();
};

OrchestratorBasicOutputTestSuite::OrchestratorBasicOutputTestSuite()
    : TestSuite("netsimulyzer-orchestrator-outputs", TEST_TYPE_SYSTEM)
{
    AddTestCase(new TestCaseOutputStructure(), TEST_DURATION_QUICK);
    AddTestCase(new TestCaseNodeInOutput(), TEST_DURATION_QUICK);
    AddTestCase(new TestCaseNodeMobility(), TEST_DURATION_QUICK);
}

static OrchestratorBasicOutputTestSuite g_orchestratorBasicOutputTestSuite{};

} // namespace ns3::test
