/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 * Copyright (c) 2019, University of Padova, Dep. of Information Engineering, SIGNET lab
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es> for the code adapted from the lena-dual-stripe.cc example
 * Author: Michele Polese <michele.polese@gmail.com> for this version
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#include "ns3/buildings-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"

#ifdef HAS_NETSIMULYZER
#include "ns3/netsimulyzer-module.h"
#endif

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("OutdoorRandomWalkExampleNetSimulyzer");

void
PrintGnuplottableBuildingListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  uint32_t index = 0;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {
      ++index;
      Box box = (*it)->GetBoundaries ();
      outFile << "set object " << index
              << " rect from " << box.xMin  << "," << box.yMin
              << " to "   << box.xMax  << "," << box.yMax
              << std::endl;
    }
}

#ifdef HAS_NETSIMULYZER
void
//Define callback function to track node mobility
CourseChanged (Ptr<netsimulyzer::XYSeries> posSeries, Ptr<netsimulyzer::LogStream> eventLog, std::string context, Ptr<const MobilityModel> model)
{
  const auto position = model->GetPosition ();
  //Write coordinates to log
  *eventLog << Simulator::Now ().GetSeconds () << " Course Change Position: [" 
            << position.x << ", " << position.y << ", " << position.z << "]\n";
  //Add data point to XYSeries
  posSeries->Append (position.x, position.y);
}
#endif


/**
 * This is an example on how to use the RandomWalk2dOutdoorMobilityModel class.
 * The script outdoor-random-walk-example.sh can be used to visualize the 
 * positions visited by the random walk.
 */
int
main (int argc, char *argv[])
{
  LogComponentEnable ("RandomWalk2dOutdoor", LOG_LEVEL_LOGIC);
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  // create a grid of buildings
  double buildingSizeX = 100;       // m
  double buildingSizeY = 50; // m
  double streetWidth = 25; // m
  double buildingHeight = 10; // m
  uint32_t numBuildingsX = 10;
  uint32_t numBuildingsY = 10;
  double maxAxisX = (buildingSizeX + streetWidth) * numBuildingsX;
  double maxAxisY = (buildingSizeY + streetWidth) * numBuildingsY;

  std::vector<Ptr<Building> > buildingVector;
  for (uint32_t buildingIdX = 0; buildingIdX < numBuildingsX; ++buildingIdX)
    {
      for (uint32_t buildingIdY = 0; buildingIdY < numBuildingsY; ++buildingIdY)
        {
          Ptr < Building > building;
          building = CreateObject<Building> ();

          building->SetBoundaries (Box (buildingIdX * (buildingSizeX + streetWidth),
                                        buildingIdX * (buildingSizeX + streetWidth) + buildingSizeX,
                                        buildingIdY * (buildingSizeY + streetWidth),
                                        buildingIdY * (buildingSizeY + streetWidth) + buildingSizeY,
                                        0.0, buildingHeight));
          building->SetNRoomsX (1);
          building->SetNRoomsY (1);
          building->SetNFloors (1);
          buildingVector.push_back (building);
        }
    }

  // print the list of buildings to file
  PrintGnuplottableBuildingListToFile ("buildings.txt");

  // create one node
  NodeContainer nodes;
  nodes.Create (1);

  // set the RandomWalk2dOutdoorMobilityModel mobility model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::RandomWalk2dOutdoorMobilityModel",
                             "Bounds", RectangleValue (
                               Rectangle (-streetWidth, maxAxisX, -streetWidth, maxAxisY)));
  // create an OutdoorPositionAllocator and set its boundaries to match those of the mobility model
  Ptr<OutdoorPositionAllocator> position = CreateObject<OutdoorPositionAllocator> ();
  Ptr<UniformRandomVariable> xPos = CreateObject<UniformRandomVariable>();
  xPos->SetAttribute ("Min", DoubleValue (-streetWidth));
  xPos->SetAttribute ("Max", DoubleValue (maxAxisX));
  Ptr<UniformRandomVariable> yPos = CreateObject<UniformRandomVariable>();
  yPos->SetAttribute ("Min", DoubleValue (-streetWidth));
  yPos->SetAttribute ("Max", DoubleValue (maxAxisY));
  position->SetAttribute ("X", PointerValue (xPos));
  position->SetAttribute ("Y", PointerValue (yPos));
  mobility.SetPositionAllocator (position);
  // install the mobility model
  mobility.Install (nodes.Get (0));

  // enable the traces for the mobility model
  AsciiTraceHelper ascii;
  MobilityHelper::EnableAsciiAll (ascii.CreateFileStream ("mobility-trace-example.mob"));

#ifdef HAS_NETSIMULYZER
  auto orchestrator = CreateObject<netsimulyzer::Orchestrator> ("outdoor-random-walk-example.json");
  //Use helper to define model for visualizing nodes and aggregate to Node object
  netsimulyzer::NodeConfigurationHelper nodeHelper{orchestrator};
  nodeHelper.Set ("Model", StringValue ("models/smartphone.obj"));
  nodeHelper.Set ("Scale", DoubleValue (4));
  nodeHelper.Install(nodes);
  //Use helper to configure buildings and export them
  netsimulyzer::BuildingConfigurationHelper buildingHelper{orchestrator};
  for (auto building = buildingVector.begin(); building != buildingVector.end(); building++)
      buildingHelper.Install(*building);
  //Create a LogStream to output mobility events
  Ptr<netsimulyzer::LogStream> eventLog = CreateObject<netsimulyzer::LogStream> (orchestrator);
  //Create XYSeries that will be used to display mobility (similar to a 2D plot)
  Ptr<netsimulyzer::XYSeries> posSeries = CreateObject <netsimulyzer::XYSeries>(orchestrator);
  posSeries->SetAttribute ("Name", StringValue("Node position" ));
  posSeries->SetAttribute ("LabelMode", StringValue("Hidden"));
  PointerValue xAxis;
  posSeries->GetAttribute ("XAxis", xAxis);
  xAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue("X position (m)"));
  PointerValue yAxis;
  posSeries->GetAttribute ("YAxis", yAxis);
  yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue("Y position (m)"));
  //Tie together the callback function, LogStream, and XYSeries
  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeBoundCallback (&CourseChanged, posSeries, eventLog));
#endif


  Simulator::Stop (Seconds (1e4));
  Simulator::Run ();
  Simulator::Destroy ();
}