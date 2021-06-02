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

#include "xy-series.h"
#include "color-palette.h"
#include <ns3/log.h>
#include <ns3/enum.h>
#include <ns3/boolean.h>
#include <ns3/uinteger.h>
#include <ns3/pointer.h>
#include <ns3/string.h>

namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("XYSeries");
namespace netsimulyzer {

NS_OBJECT_ENSURE_REGISTERED (XYSeries);

XYSeries::XYSeries (Ptr<Orchestrator> orchestrator) : m_orchestrator (orchestrator)
{
  NS_LOG_FUNCTION (this << m_orchestrator);
  // There doesn't seem to be a Ptr from `this`, so this is as close as we can get
  m_id = orchestrator->Register ({this, true});
}

TypeId
XYSeries::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::XYSeries")
          .SetParent<ns3::Object> ()
          .SetGroupName ("netsimulyzer")
          .AddAttribute ("Id", "The unique ID of the series", TypeId::ATTR_GET, UintegerValue (0u),
                         MakeUintegerAccessor (&XYSeries::m_id), MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("XAxis", "The X axis on the graph", PointerValue (),
                         MakePointerAccessor (&XYSeries::GetXAxis, &XYSeries::SetXAxis),
                         MakePointerChecker<ValueAxis> ())
          .AddAttribute ("YAxis", "The Y axis on the graph", PointerValue (),
                         MakePointerAccessor (&XYSeries::GetYAxis, &XYSeries::SetYAxis),
                         MakePointerChecker<ValueAxis> ())
          .AddAttribute ("Orchestrator", "Orchestrator that manages this series", PointerValue (),
                         MakePointerAccessor (&XYSeries::m_orchestrator),
                         MakePointerChecker<Orchestrator> ())
          .AddAttribute ("Connection", "Type of connection to form between points in the series",
                         EnumValue (XYSeries::ConnectionType::Line),
                         MakeEnumAccessor (&XYSeries::m_connection),
                         MakeEnumChecker (XYSeries::ConnectionType::None, "None",
                                          XYSeries::ConnectionType::Line, "Line",
                                          XYSeries::ConnectionType::Spline, "Spline"))
          .AddAttribute ("LabelMode", "How the point labels are shown",
                         EnumValue (XYSeries::LabelMode::Hidden),
                         MakeEnumAccessor (&XYSeries::m_labelMode),
                         MakeEnumChecker (XYSeries::LabelMode::Hidden, "Hidden",
                                          XYSeries::LabelMode::Shown, "Shown"))
          .AddAttribute ("Name", "Name to represent this series in visualizer elements",
                         StringValue (), MakeStringAccessor (&XYSeries::m_name),
                         MakeStringChecker ())
          .AddAttribute ("Legend", "Name for the series that appears in the chart legend",
                         StringValue (), MakeStringAccessor (&XYSeries::m_legend),
                         MakeStringChecker ())
          .AddAttribute ("Visible", "Should this series appear in selection elements",
                         BooleanValue (true), MakeBooleanAccessor (&XYSeries::m_visible),
                         MakeBooleanChecker ())
          .AddAttribute ("Color", "Color to use for the points and connections", BLUE_VALUE,
                         MakeColor3Accessor (&XYSeries::m_color), MakeColor3Checker ());
  return tid;
}

void
XYSeries::Append (double x, double y)
{
  NS_LOG_FUNCTION (this << x << y);
  m_orchestrator->AppendXyValue (m_id, x, y);
}

void
XYSeries::Append (const XYPoint &point)
{
  NS_LOG_FUNCTION (this << point.x << point.y);
  m_orchestrator->AppendXyValue (m_id, point.x, point.y);
}

void
XYSeries::Append (const std::vector<XYPoint> &points)
{
  NS_LOG_FUNCTION (this);
  m_orchestrator->AppendXyValues (m_id, points);
}

void
XYSeries::Clear (void)
{
  NS_LOG_FUNCTION (this);
  m_orchestrator->ClearXySeries (m_id);
}

void
XYSeries::Commit (void)
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

Ptr<ValueAxis>
XYSeries::GetXAxis (void) const
{
  NS_LOG_FUNCTION (this);
  return m_xAxis;
}

void
XYSeries::SetXAxis (Ptr<ValueAxis> value)
{
  NS_LOG_FUNCTION (this << value);
  m_xAxis = value;
}

Ptr<ValueAxis>
XYSeries::GetYAxis (void) const
{
  NS_LOG_FUNCTION (this);
  return m_yAxis;
}

void
XYSeries::SetYAxis (Ptr<ValueAxis> value)
{
  NS_LOG_FUNCTION (this << value);
  m_yAxis = value;
}
void
XYSeries::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_orchestrator = nullptr;
  m_xAxis = nullptr;
  m_yAxis = nullptr;
  Object::DoDispose ();
}

} // namespace netsimulyzer
} // namespace ns3
