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
 * Author: Richard Rouil <richard.rouil@nist.gov>
 */

#include "throughput-sink.h"
#include <ns3/log.h>
#include <ns3/enum.h>
#include <ns3/string.h>
#include <ns3/double.h>
#include <ns3/pointer.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ThroughputSink");

namespace visualizer3d {

NS_OBJECT_ENSURE_REGISTERED (ThroughputSink);

ThroughputSink::ThroughputSink (Ptr<Orchestrator> orchestrator, const std::string &name)
    : m_orchestrator (orchestrator)
{
  //Create the series and assign known information
  m_series = CreateObject<visualizer3d::XYSeries> (orchestrator);
  m_series->SetAttribute ("Name", StringValue (name));

  PointerValue xAxis;
  m_series->GetAttribute ("XAxis", xAxis);
  xAxis.Get<visualizer3d::ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (s)"));

  //Set callback for data dump
  m_timer.SetFunction (&ThroughputSink::WriteThroughput, this);
}

TypeId
ThroughputSink::GetTypeId (void)
{
  // clang-format off
  static TypeId tid =
    TypeId ("ns3::visualizer3d::ThroughputSink")
    .SetParent<ns3::Object> ()
    .SetGroupName ("visualizer3d")
    .AddAttribute ("XYSeries", "The XY Series", TypeId::ATTR_GET, PointerValue (),
                   MakePointerAccessor (&ThroughputSink::GetSeries), MakePointerChecker<XYSeries> ())
    .AddAttribute ("Interval", "Time between updates", TimeValue (Seconds (1)),
                   MakeTimeAccessor (&ThroughputSink::SetInterval), MakeTimeChecker ())
    .AddAttribute (
      "Unit", "The unit for the throughput plot",
      EnumValue (ThroughputSink::Unit::KBit), MakeEnumAccessor (&ThroughputSink::SetUnit),
      MakeEnumChecker (ThroughputSink::Bit, "b/s", ThroughputSink::KBit, "kb/s",
                       ThroughputSink::MBit, "Mb/s", ThroughputSink::GBit, "Gb/s",
                       ThroughputSink::Byte, "B/s", ThroughputSink::KByte, "KB/s",
                       ThroughputSink::MByte, "MB/s", ThroughputSink::GByte, "GB/s"))
    .AddAttribute (
      "TimeUnit", "The unit of time to use for the X axis",
      EnumValue (Time::S),
      MakeEnumAccessor (&ThroughputSink::SetTimeUnit,
        &ThroughputSink::GetTimeUnit),
      MakeEnumChecker (Time::Unit::Y, "Year",
                       Time::Unit::D, "Day",
                       Time::Unit::H, "Hour",
                       Time::Unit::MIN, "Minute",
                       Time::Unit::S, "Second",
                       Time::Unit::MS, "Millisecond",
                       Time::Unit::US, "Microsecond",
                       Time::Unit::NS, "Nanosecond",
                       Time::Unit::PS, "Picosecond",
                       Time::Unit::FS, "Femtosecond"));
  // clang-format on
  return tid;
}

void
ThroughputSink::SetUnit (ThroughputSink::Unit unit)
{
  NS_LOG_FUNCTION (this << unit);
  m_unit = unit;

  switch (m_unit)
    {
    case ThroughputSink::Bit:
      m_unitScale = 8.0;
      break;
    case ThroughputSink::KBit:
      m_unitScale = 8 / 1e3;
      break;
    case ThroughputSink::MBit:
      m_unitScale = 8 / 1e6;
      break;
    case ThroughputSink::GBit:
      m_unitScale = 8 / 1e9;
      break;
    case ThroughputSink::Byte:
      m_unitScale = 1;
      break;
    case ThroughputSink::KByte:
      m_unitScale = 1 / 1e3;
      break;
    case ThroughputSink::MByte:
      m_unitScale = 1 / 1e6;
      break;
    case ThroughputSink::GByte:
      m_unitScale = 1 / 1e9;
      break;
    default:
      NS_ABORT_MSG ("Invalid unit");
    }

  UpdateAxisLabels ();
}

void
ThroughputSink::SetInterval (Time interval)
{
  NS_LOG_FUNCTION (this << interval);
  NS_ASSERT_MSG (interval.GetSeconds () > 0, "Needs interval greater than 0");
  if (m_timer.IsRunning ())
    {
      m_timer.Cancel ();
    }
  m_packetsInterval = interval;
  m_timer.SetDelay (m_packetsInterval);
  m_timer.Schedule ();
}

void
ThroughputSink::AddPacketSize (uint32_t size)
{
  NS_LOG_FUNCTION (this << size);
  m_total += size;
}

void
ThroughputSink::WriteThroughput ()
{
  NS_LOG_FUNCTION (this);

  double y = (m_total * m_unitScale) / m_packetsInterval.GetSeconds ();
  m_series->Append (Simulator::Now ().GetSeconds (), y);
  m_total = 0;
  m_timer.Schedule ();
}

void
ThroughputSink::SetTimeUnit (Time::Unit unit)
{
  m_timeUnit = unit;
  UpdateAxisLabels ();
}

Time::Unit
ThroughputSink::GetTimeUnit (void) const
{
  return m_timeUnit;
}

Ptr<XYSeries>
ThroughputSink::GetSeries (void) const
{
  return m_series;
}

void
ThroughputSink::DoDispose (void)
{
  m_orchestrator = nullptr;
  m_series = nullptr;
  m_timer.Cancel();
  Object::DoDispose ();
}

void
ThroughputSink::UpdateAxisLabels (void)
{
  std::string timeUnitLabel;
  switch (m_timeUnit)
    {
    case Time::Y:
      timeUnitLabel = "y";
      break;
    case Time::D:
      timeUnitLabel = "d";
      break;
    case Time::H:
      timeUnitLabel = "h";
      break;
    case Time::MIN:
      timeUnitLabel = "min";
      break;
    case Time::S:
      timeUnitLabel = "s";
      break;
    case Time::MS:
      timeUnitLabel = "ms";
      break;
    case Time::US:
      timeUnitLabel = "us";
      break;
    case Time::NS:
      timeUnitLabel = "ns";
      break;
    case Time::PS:
      timeUnitLabel = "ps";
      break;
    case Time::FS:
      timeUnitLabel = "fs";
      break;
    default:
      NS_ABORT_MSG ("Unrecognised time unit: " << timeUnitLabel);
    }

  PointerValue xAxis;
  m_series->GetAttribute ("XAxis", xAxis);
  xAxis.Get<ValueAxis> ()->SetAttribute ("Name", StringValue ("Time (" + timeUnitLabel + ')'));

  std::string dataUnitLabel;
  switch (m_unit)
    {
    case ThroughputSink::Bit:
      dataUnitLabel = "b";
      break;
    case ThroughputSink::KBit:
      dataUnitLabel = "Kb";
      break;
    case ThroughputSink::MBit:
      dataUnitLabel = "Mb";
      break;
    case ThroughputSink::GBit:
      dataUnitLabel = "Gb";
      break;
    case ThroughputSink::Byte:
      dataUnitLabel = "B";
      break;
    case ThroughputSink::KByte:
      dataUnitLabel = "KB";
      break;
    case ThroughputSink::MByte:
      dataUnitLabel = "MB";
      break;
    case ThroughputSink::GByte:
      dataUnitLabel = "GB";
      break;
    default:
      NS_ABORT_MSG ("Unrecognised data unit: " << m_unit);
    }

  PointerValue yAxis;
  m_series->GetAttribute ("YAxis", yAxis);
  yAxis.Get<ValueAxis> ()->SetAttribute (
      "Name", StringValue ("Throughput (" + dataUnitLabel + '/' + timeUnitLabel + ')'));
}

} // namespace visualizer3d
} // namespace ns3