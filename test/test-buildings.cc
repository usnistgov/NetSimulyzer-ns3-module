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
#include <ns3/core-module.h>
#include <ns3/mobility-module.h>
#include <ns3/netsimulyzer-module.h>
#include <ns3/network-module.h>
#include <ns3/nstime.h>
#include <ns3/test.h>

#include <string>
#include <string_view>
#include <vector>

namespace ns3::test {

using namespace netsimulyzer;

class TestCaseDefaultBuilding : public NetSimulyzerTestCase {
public:
  TestCaseDefaultBuilding();

private:
  void DoRun() override;
};

TestCaseDefaultBuilding::TestCaseDefaultBuilding()
    : NetSimulyzerTestCase("NetSimulyzer - Default Building Output") {}

void TestCaseDefaultBuilding::DoRun() {
  auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

  auto ns3Building = CreateObject<Building>();
    auto ns3Bounds = ns3Building->GetBoundaries();

    auto buildingConfig = CreateObject<BuildingConfiguration>(o);
    ns3Building->AggregateObject(buildingConfig);

    Color3Value colorAttribute{};
    buildingConfig->GetAttribute("Color", colorAttribute);
    const auto &ns3Color = colorAttribute.Get();

    BooleanValue visableAttribute{};
    buildingConfig->GetAttribute("Visible", visableAttribute);


    Simulator::Stop(Seconds(100.0));
    Simulator::Run();

    const auto &output = o->GetJson();

    NS_TEST_ASSERT_MSG_EQ(output.contains("buildings"), true, "Output must contain 'buildings' key");

    const auto &buildings = output["buildings"];
    NS_TEST_ASSERT_MSG_EQ(buildings.size(), 1UL, "'buldings' array should contain one building");

    const auto &building = buildings[0];

    RequiredFields({"color", "visible", "id", "floors", "rooms", "bounds"},building, "building");

    CheckColor(building["color"], ns3Color);

    NS_TEST_ASSERT_MSG_EQ(building["visible"].get<bool>(), visableAttribute.Get(), "Output 'visable' must match config");

    NS_TEST_ASSERT_MSG_EQ(building["id"].get<uint32_t>(), ns3Building->GetId(), "Building ID must match output");

    NS_TEST_ASSERT_MSG_EQ(building["floors"].get<uint16_t>(), ns3Building->GetNFloors(), "Output floors much match ns-3");


    const auto rooms = building["rooms"];
    RequiredFields({"x", "y"}, rooms, "rooms");

    NS_TEST_ASSERT_MSG_EQ(rooms["x"].get<uint16_t>(), ns3Building->GetNRoomsX(), "Output X Rooms must match ns3");
    NS_TEST_ASSERT_MSG_EQ(rooms["y"].get<uint16_t>(), ns3Building->GetNRoomsY(), "Output Y Rooms must match ns3");

    const auto &bounds = building["bounds"];
    RequiredFields({"x", "y", "z"}, bounds, "bounds");

    const auto boundsX = bounds["x"];
    RequiredFields({"min", "max"}, boundsX, "bounds['x']");
    NS_TEST_ASSERT_MSG_EQ(boundsX["min"].get<double>(), ns3Bounds.xMin, "Output X bound min must match ns-3");
    NS_TEST_ASSERT_MSG_EQ(boundsX["max"].get<double>(), ns3Bounds.xMax, "Output X bound max must match ns-3");

    const auto boundsY = bounds["y"];
    RequiredFields({"min", "max"}, boundsY, "bounds['y']");
    NS_TEST_ASSERT_MSG_EQ(boundsY["min"].get<double>(), ns3Bounds.yMin, "Output Y bound min must match ns-3");
    NS_TEST_ASSERT_MSG_EQ(boundsY["max"].get<double>(), ns3Bounds.yMax, "Output Y bound max must match ns-3");

    const auto boundsZ = bounds["z"];
    RequiredFields({"min", "max"}, boundsZ, "bounds['z']");
    NS_TEST_ASSERT_MSG_EQ(boundsZ["min"].get<double>(), ns3Bounds.zMin, "Output Z bound min must match ns-3");
    NS_TEST_ASSERT_MSG_EQ(boundsZ["max"].get<double>(), ns3Bounds.zMax, "Output Z bound max must match ns-3");

    Simulator::Destroy();
}

// --------------------------------------------------------------------

class TestCaseBuildingBoundsRooms : public NetSimulyzerTestCase {
public:
    TestCaseBuildingBoundsRooms();

private:
    void DoRun() override;
};

TestCaseBuildingBoundsRooms::TestCaseBuildingBoundsRooms()
    : NetSimulyzerTestCase("NetSimulyzer - Building Bounds and Rooms") {}

void TestCaseBuildingBoundsRooms::DoRun() {
    auto o = CreateObject<Orchestrator>(Orchestrator::MemoryOutputMode::On);

    Box ns3Bounds{-5.0, 5.0, 0.0, 10.0, 15.0, 20.0};
    auto ns3Building = CreateObject<Building>();
    ns3Building->SetBoundaries(ns3Bounds);

    constexpr uint16_t ns3Floors{3U};
    ns3Building->SetNFloors(ns3Floors);

    constexpr uint16_t ns3RoomsX{15U};
    ns3Building->SetNRoomsX(ns3RoomsX);

    constexpr uint16_t ns3RoomsY{10U};
    ns3Building->SetNRoomsY(ns3RoomsY);

    ns3Building->AggregateObject(CreateObject<BuildingConfiguration>(o));

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    const auto &output = o->GetJson();
    const auto &building = output["buildings"][0];

    RequiredFields({"color", "visible", "id", "floors", "rooms", "bounds"},building, "building");

    NS_TEST_ASSERT_MSG_EQ(building["floors"].get<uint16_t>(), ns3Building->GetNFloors(), "Output floors much match ns-3");


    const auto rooms = building["rooms"];
    RequiredFields({"x", "y"}, rooms, "rooms");

    NS_TEST_ASSERT_MSG_EQ(rooms["x"].get<uint16_t>(), ns3RoomsX, "Output X Rooms must match ns3");
    NS_TEST_ASSERT_MSG_EQ(rooms["y"].get<uint16_t>(), ns3RoomsY, "Output Y Rooms must match ns3");

    const auto &bounds = building["bounds"];
    RequiredFields({"x", "y", "z"}, bounds, "bounds");

    const auto boundsX = bounds["x"];
    RequiredFields({"min", "max"}, boundsX, "bounds['x']");
    NS_TEST_ASSERT_MSG_EQ(boundsX["min"].get<double>(), ns3Bounds.xMin, "Output X bound min must match ns-3");
    NS_TEST_ASSERT_MSG_EQ(boundsX["max"].get<double>(), ns3Bounds.xMax, "Output X bound max must match ns-3");

    const auto boundsY = bounds["y"];
    RequiredFields({"min", "max"}, boundsY, "bounds['y']");
    NS_TEST_ASSERT_MSG_EQ(boundsY["min"].get<double>(), ns3Bounds.yMin, "Output Y bound min must match ns-3");
    NS_TEST_ASSERT_MSG_EQ(boundsY["max"].get<double>(), ns3Bounds.yMax, "Output Y bound max must match ns-3");

    const auto boundsZ = bounds["z"];
    RequiredFields({"min", "max"}, boundsZ, "bounds['z']");
    NS_TEST_ASSERT_MSG_EQ(boundsZ["min"].get<double>(), ns3Bounds.zMin, "Output Z bound min must match ns-3");
    NS_TEST_ASSERT_MSG_EQ(boundsZ["max"].get<double>(), ns3Bounds.zMax, "Output Z bound max must match ns-3");

    Simulator::Destroy();
}

class NetsimulyzerBuildingSuite : public TestSuite {
public:
  NetsimulyzerBuildingSuite();
};

NetsimulyzerBuildingSuite::NetsimulyzerBuildingSuite(): TestSuite{"netsimulyzer-buildings"}
{
    using TestDuration = TestCase::TestDuration;

    AddTestCase(new TestCaseDefaultBuilding{}, TestDuration::QUICK);
    AddTestCase(new TestCaseBuildingBoundsRooms{}, TestDuration::QUICK);
};

static NetsimulyzerBuildingSuite g_getsimulyzerBuildingSuite{};

} // namespace ns3::test
