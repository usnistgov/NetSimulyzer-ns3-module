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

#include "log-stream.h"
#include <ns3/orchestrator.h>
#include <ns3/uinteger.h>
#include <ns3/boolean.h>
#include <ns3/string.h>
#include <ns3/pointer.h>
#include <ns3/event-message.h>
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/color.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LogStream");

namespace visualizer3d {

NS_OBJECT_ENSURE_REGISTERED (LogStream);

LogStream::LogStream (Ptr<Orchestrator> orchestrator) : m_orchestrator (orchestrator)
{
  m_id = orchestrator->NextLogId ();

  orchestrator->Register ({this, true});
}

TypeId
LogStream::GetTypeId (void)
{
  // clang-format off
  static TypeId tid = TypeId ("ns3::visualizer3d::LogStream")
    .SetParent<ns3::Object> ()
    .SetGroupName ("visualizer3d")
    .AddAttribute("Id", "The unique ID of the LogStream", TypeId::ATTR_GET, UintegerValue(0u),
      MakeUintegerAccessor(&LogStream::m_id), MakeUintegerChecker<uint32_t>())
    .AddAttribute ("Orchestrator", "Orchestrator that manages this series", PointerValue (),
      MakePointerAccessor (&LogStream::m_orchestrator), MakePointerChecker<Orchestrator>())
    .AddAttribute ("Name", "Name to represent this stream in visualizer elements",
      StringValue (), MakeStringAccessor (&LogStream::m_name), MakeStringChecker ())
    .AddAttribute ("Color", "The font color", Color3Value (),
                   MakeColor3Accessor (&LogStream::GetColor, &LogStream::SetColor),
                   MakeColor3Checker ())
    .AddAttribute("Visible", "Flag indicating this item should appear in Visualizer elements",
                  BooleanValue(true), MakeBooleanAccessor(&LogStream::m_visible),
                  MakeBooleanChecker())
    ;
  // clang-format on
  return tid;
}

void
LogStream::Write (const std::string &message) const
{
  LogMessageEvent event;
  event.id = m_id;
  event.time = Simulator::Now ();
  event.message = message;

  m_orchestrator->WriteLogMessage (event);
}

bool
LogStream::IsColorSet (void) const
{
  return m_colorSet;
}

void
LogStream::DoDispose ()
{
  m_orchestrator = nullptr;
}

void
LogStream::SetColor (Color3 value)
{
  // Cheap hack to prevent the initial value from setting the flag
  if (value.red != 0u || value.green != 0u || value.blue != 0u)
    m_colorSet = true;

  m_color = value;
}

Color3
LogStream::GetColor (void) const
{
  return m_color;
}

LogStream &
operator<< (LogStream &stream, const char *value)
{
  stream.Write (std::string{value});
  return stream;
}

} // namespace visualizer3d
} // namespace ns3
