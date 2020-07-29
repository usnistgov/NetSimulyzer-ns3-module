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

#include "building-configuration.h"
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/object-base.h>
#include <ns3/pointer.h>
#include <ns3/string.h>
#include <ns3/color.h>

namespace ns3 {
namespace visualizer3d {

NS_OBJECT_ENSURE_REGISTERED (BuildingConfiguration);

BuildingConfiguration::BuildingConfiguration (Ptr<Orchestrator> orchestrator)
    : m_orchestrator (orchestrator)
{
  m_orchestrator->Register ({this, false});
}

TypeId
BuildingConfiguration::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::visualizer3d::BuildingConfiguration")
          .SetParent<Object> ()
          .AddConstructor<BuildingConfiguration> ()
          .SetGroupName ("visualizer3d")
          .AddAttribute ("Color", "Color to tint the rendered building",
                         Color3Value (Color3{204u}) /* light gray */,
                         MakeColor3Accessor (&BuildingConfiguration::m_color), MakeColor3Checker ())
          .AddAttribute ("Visible", "Defines if the Building is rendered in the visualizer",
                         BooleanValue (true),
                         MakeBooleanAccessor (&BuildingConfiguration::m_visible),
                         MakeBooleanChecker ())
          .AddAttribute ("Orchestrator", "Orchestrator that manages this Building", PointerValue (),
                         MakePointerAccessor (&BuildingConfiguration::GetOrchestrator,
                                              &BuildingConfiguration::SetOrchestrator),
                         MakePointerChecker<Orchestrator> ());
  return tid;
}
void
BuildingConfiguration::DoDispose (void)
{
  m_orchestrator = nullptr;
  Object::DoDispose ();
}

void
BuildingConfiguration::SetOrchestrator (Ptr<Orchestrator> orchestrator)
{
  m_orchestrator = orchestrator;
  m_orchestrator->Register ({this, false});
}

Ptr<Orchestrator>
BuildingConfiguration::GetOrchestrator (void) const
{
  return m_orchestrator;
}

} // namespace visualizer3d
} // namespace ns3
