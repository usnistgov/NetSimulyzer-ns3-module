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
#include <ns3/optional.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LogStream");

namespace netsimulyzer {

NS_OBJECT_ENSURE_REGISTERED (LogStream);

LogStream::LogStream (Ptr<Orchestrator> orchestrator) : m_orchestrator (orchestrator)
{
  NS_LOG_FUNCTION (this << orchestrator);
  m_id = orchestrator->Register ({this, true});
}

TypeId
LogStream::GetTypeId (void)
{
  // clang-format off
  static TypeId tid = TypeId ("ns3::netsimulyzer::LogStream")
    .SetParent<ns3::Object> ()
    .SetGroupName ("netsimulyzer")
    .AddAttribute("Id", "The unique ID of the LogStream", TypeId::ATTR_GET, UintegerValue(0u),
      MakeUintegerAccessor(&LogStream::m_id), MakeUintegerChecker<uint32_t>())
    .AddAttribute ("Orchestrator", "Orchestrator that manages this series", PointerValue (),
      MakePointerAccessor (&LogStream::m_orchestrator), MakePointerChecker<Orchestrator>())
    .AddAttribute ("Name", "Name to represent this stream in visualizer elements",
      StringValue (), MakeStringAccessor (&LogStream::m_name), MakeStringChecker ())
    .AddAttribute ("Color", "The font color", OptionalValue<Color3> (),
                   MakeOptionalAccessor<Color3> (&LogStream::m_color),
                   MakeOptionalChecker<Color3>())
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
  NS_LOG_FUNCTION (this << message);

  LogMessageEvent event;
  event.id = m_id;
  event.time = Simulator::Now ();
  event.message = message;

  m_orchestrator->WriteLogMessage (event);
}

void
LogStream::Commit (void)
{
  NS_LOG_FUNCTION (this);

  if (m_committed)
    {
      NS_LOG_DEBUG ("Ignoring Commit () on already committed model");
      return;
    }

  m_orchestrator->Commit (*this);
  m_committed = true;
}

void
LogStream::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_orchestrator = nullptr;
}

LogStream &
operator<< (LogStream &stream, const char *value)
{
  stream.Write (std::string{value});
  return stream;
}

} // namespace netsimulyzer
} // namespace ns3
