#include "category-value-series.h"
#include <ns3/log.h>
#include <ns3/string.h>
#include <ns3/pointer.h>
#include <ns3/uinteger.h>
#include <ns3/enum.h>
#include <ns3/boolean.h>
#include <ns3/type-id.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CategoryValueSeries");

namespace visualizer3d {

NS_OBJECT_ENSURE_REGISTERED (CategoryValueSeries);

TypeId
CategoryValueSeries::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::visualizer3d::CategoryValueSeries")
          .SetParent<ns3::Object> ()
          .SetGroupName ("visualizer3d")
          .AddAttribute ("Id", "The unique ID of the series", TypeId::ATTR_GET, UintegerValue (0u),
                         MakeUintegerAccessor (&CategoryValueSeries::m_id),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("Name", "Unique name to represent this series in visualizer elements",
                         StringValue (), MakeStringAccessor (&CategoryValueSeries::m_name),
                         MakeStringChecker ())
          .AddAttribute ("Legend", "Name for the series that appears in the chart legend",
                         StringValue (), MakeStringAccessor (&CategoryValueSeries::m_legend),
                         MakeStringChecker ())
          .AddAttribute ("Visible", "Should this series appear in selection elements",
                         BooleanValue (true), MakeBooleanAccessor (&CategoryValueSeries::m_visible),
                         MakeBooleanChecker ())
          .AddAttribute ("XAxis", "The X axis on the graph", PointerValue (),
                         MakePointerAccessor (&CategoryValueSeries::m_xAxis),
                         MakePointerChecker<ValueAxis> ())
          .AddAttribute ("YAxis", "The Y axis on the graph", PointerValue (),
                         MakePointerAccessor (&CategoryValueSeries::m_yAxis),
                         MakePointerChecker<CategoryAxis> ())
          .AddAttribute ("Color", "Color to use for the points and connections", Color4Value (),
                         MakeColor4Accessor (&CategoryValueSeries::m_color), MakeColor4Checker ());

  return tid;
}

CategoryValueSeries::CategoryValueSeries (Ptr<Orchestrator> orchestrator)
    : m_orchestrator (orchestrator)
{
  m_id = m_orchestrator->NextSeriesId ();
  m_orchestrator->Register ({this, true});
}

CategoryValueSeries::CategoryValueSeries (Ptr<Orchestrator> orchestrator,
                                          const std::vector<std::string> &categories)
    : m_orchestrator (orchestrator), m_yAxis (CreateObject<CategoryAxis> (categories))
{
  m_id = m_orchestrator->NextSeriesId ();
  m_orchestrator->Register ({this, true});
}

CategoryValueSeries::CategoryValueSeries (Ptr<Orchestrator> orchestrator,
                                          const std::vector<CategoryAxis::ValuePair> &categories)
    : m_orchestrator (orchestrator), m_yAxis (CreateObject<CategoryAxis> (categories))
{
  m_id = m_orchestrator->NextSeriesId ();
  m_orchestrator->Register ({this, true});
}

void
CategoryValueSeries::Append (int category, double value)
{
  m_orchestrator->AppendCategoryValue (m_id, category, value);
}

void
CategoryValueSeries::Append (const std::string &category, double value)
{
  Append (m_yAxis->GetId (category), value);
}

void
CategoryValueSeries::Commit (void)
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
CategoryValueSeries::DoDispose (void)
{
  m_orchestrator = nullptr;
  Object::DoDispose ();
}

} // namespace visualizer3d
} // namespace ns3
