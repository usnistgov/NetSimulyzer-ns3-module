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

#ifndef NETSIMULYZER_3D_MODELS_H
#define NETSIMULYZER_3D_MODELS_H

#include <string>
#include <ns3/string.h>

namespace ns3::netsimulyzer::models {

const std::string SMARTPHONE{"models/smartphone.obj"};
const StringValue SMARTPHONE_VALUE{SMARTPHONE};

const std::string LAND_DRONE{"models/land_drone.obj"};
const StringValue LAND_DRONE_VALUE{LAND_DRONE};

const std::string SERVER{"models/server.obj"};
const StringValue SERVER_VALUE{SERVER};

const std::string CELL_TOWER_POLE{"models/cell_tower_pole.obj"};
const StringValue CELL_TOWER_POLE_VALUE{CELL_TOWER_POLE};

// Simple Shapes

const std::string CUBE{"models/simple/cube.obj"};
const StringValue CUBE_VALUE{CUBE};

const std::string DIAMOND{"models/simple/diamond.obj"};
const StringValue DIAMOND_VALUE{DIAMOND};

const std::string SPHERE{"models/simple/sphere.obj"};
const StringValue SPHERE_VALUE{SPHERE};

const std::string SQUARE_PYRAMID{"models/simple/square_pyramid.obj"};
const StringValue SQUARE_PYRAMID_VALUE{SQUARE_PYRAMID};

} // namespace ns3::netsimulyzer::models

#endif //NETSIMULYZER_3D_MODELS_H
