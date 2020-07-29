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
 */

#include <ns3/uinteger.h>
#include <ns3/log.h>
#include <ns3/string.h>
#include <ns3/pointer.h>
#include <ns3/boolean.h>
#include "series-collection.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SeriesCollection");

namespace visualizer3d {

NS_OBJECT_ENSURE_REGISTERED (SeriesCollection);

SeriesCollection::SeriesCollection (Ptr<Orchestrator> orchestrator) : m_orchestrator (orchestrator)
{
  m_id = m_orchestrator->NextSeriesId ();
  m_orchestrator->RegisterSeries ({this, true});
}

TypeId
SeriesCollection::GetTypeId (void)
{
  // clang-format off
  static TypeId tid =
      TypeId ("ns3::visualizer3d::SeriesCollection")
          .SetParent<ns3::Object> ()
          .SetGroupName ("visualizer3d")
          .AddAttribute ("Id", "The unique ID of the series",
                         TypeId::ATTR_GET, UintegerValue (0u),
                         MakeUintegerAccessor (&SeriesCollection::m_id),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("XAxis", "The X axis on the graph",
                         PointerValue (),
                         MakePointerAccessor (&SeriesCollection::m_xAxis),
                         MakePointerChecker<ValueAxis> ())
          .AddAttribute ("YAxis", "The Y axis on the graph",
                         PointerValue (),
                         MakePointerAccessor (&SeriesCollection::m_yAxis),
                         MakePointerChecker<ValueAxis> ())
          .AddAttribute ("Name", "Name to represent this series in visualizer elements",
                         StringValue (), MakeStringAccessor (&SeriesCollection::m_name),
                         MakeStringChecker ())
          .AddAttribute("HideAddedSeries", "Set the `Visible` attribute "
                                            "to newly added series to `false`",
                        BooleanValue (true),
                        MakeBooleanAccessor (&SeriesCollection::m_hideAddedSeries),
                        MakeBooleanChecker())
          .AddAttribute ("Orchestrator", "Orchestrator that manages this series",
                         PointerValue (),
                         MakePointerAccessor (&SeriesCollection::m_orchestrator),
                         MakePointerChecker<Orchestrator> ());
  return tid;
  // clang-format on
}

void
SeriesCollection::Add (Ptr<XYSeries> series)
{
  UintegerValue id;
  series->GetAttribute ("Id", id);
  Add (id.Get ());

  if (m_hideAddedSeries)
    series->SetAttribute ("Visible", BooleanValue (false));
}

std::vector<uint32_t>
SeriesCollection::GetSeriesIds (void)
{
  return m_seriesIds;
}

void
SeriesCollection::Commit (void)
{
  if (m_committed)
    {
      NS_LOG_DEBUG ("Ignoring Commit () on already committed model");
      return;
    }

  m_orchestrator->Commit (*this);
  m_committed = true;
}

void
SeriesCollection::DoDispose (void)
{
  m_orchestrator = nullptr;
  Object::DoDispose ();
}

void
SeriesCollection::Add (uint32_t id)
{
  m_seriesIds.emplace_back (id);
}

} // namespace visualizer3d
} // namespace ns3