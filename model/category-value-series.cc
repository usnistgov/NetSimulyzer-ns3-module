#include "category-value-series.h"
#include <ns3/log.h>
#include <ns3/string.h>
#include <ns3/double.h>
#include <ns3/pointer.h>
#include <ns3/uinteger.h>
#include <ns3/enum.h>
#include <ns3/boolean.h>
#include <ns3/type-id.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CategoryValueSeries");

namespace netsimulyzer {

NS_OBJECT_ENSURE_REGISTERED (CategoryValueSeries);

TypeId
CategoryValueSeries::GetTypeId (void)
{

  // clang-format off
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::CategoryValueSeries")
          .SetParent<ns3::Object> ()
          .SetGroupName ("netsimulyzer")
          .AddAttribute ("Id",
                         "The unique ID of the series",
                         TypeId::ATTR_GET,
                         UintegerValue (0u),
                         MakeUintegerAccessor (&CategoryValueSeries::m_id),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("Name",
                         "Unique name to represent this series in visualizer elements",
                         StringValue (),
                         MakeStringAccessor (&CategoryValueSeries::m_name),
                         MakeStringChecker ())
          .AddAttribute ("Legend",
                         "Name for the series that appears in the chart legend",
                         StringValue (),
                         MakeStringAccessor (&CategoryValueSeries::m_legend),
                         MakeStringChecker ())
          .AddAttribute ("Visible",
                         "Should this series appear in selection elements",
                         BooleanValue (true),
                         MakeBooleanAccessor (&CategoryValueSeries::m_visible),
                         MakeBooleanChecker ())
          .AddAttribute ("XAxis",
                         "The X axis on the graph",
                         PointerValue (),
                         MakePointerAccessor (&CategoryValueSeries::GetXAxis, &CategoryValueSeries::SetXAxis),
                         MakePointerChecker<ValueAxis> ())
          .AddAttribute ("YAxis",
                         "The Y axis on the graph",
                         PointerValue (),
                         MakePointerAccessor (&CategoryValueSeries::GetYAxis,&CategoryValueSeries::SetYAxis),
                         MakePointerChecker<CategoryAxis> ())
          .AddAttribute ("Color",
                         "Color to use for the points and connections",
                         Color4Value (),
                         MakeColor4Accessor (&CategoryValueSeries::m_color),
                         MakeColor4Checker ())
          .AddAttribute ("AutoUpdate",
                         "Automatically append values in the same category (Y value) "
                         "but with a greater X value. Must also set `AutoUpdateInterval` and "
                         "`AutoUpdateValue`",
                         BooleanValue (false),
                         MakeBooleanAccessor (&CategoryValueSeries::m_autoUpdate),
                         MakeBooleanChecker ())
          .AddAttribute ("AutoUpdateInterval",
                         "The minimum time before appending `AutoUpdateValue` To the series",
                         TimeValue(),
                         MakeTimeAccessor(&CategoryValueSeries::m_autoUpdateInterval),
                         MakeTimeChecker())
          .AddAttribute ("AutoUpdateIncrement",
                         "The value to append to the previous X value "
                         "after `AutoUpdateInterval` has passed",
                         DoubleValue(),
                         MakeDoubleAccessor(&CategoryValueSeries::m_autoUpdateIncrement),
                         MakeDoubleChecker<double>());
  // clang-format on

  return tid;
}

CategoryValueSeries::CategoryValueSeries (Ptr<Orchestrator> orchestrator)
    : m_orchestrator (orchestrator)
{
  m_id = m_orchestrator->Register ({this, true});
}

CategoryValueSeries::CategoryValueSeries (Ptr<Orchestrator> orchestrator,
                                          const std::vector<std::string> &categories)
    : m_orchestrator (orchestrator), m_yAxis (CreateObject<CategoryAxis> (categories))
{
  m_id = m_orchestrator->Register ({this, true});
}

CategoryValueSeries::CategoryValueSeries (Ptr<Orchestrator> orchestrator,
                                          const std::vector<CategoryAxis::ValuePair> &categories)
    : m_orchestrator (orchestrator), m_yAxis (CreateObject<CategoryAxis> (categories))
{
  m_id = m_orchestrator->Register ({this, true});
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

Ptr<ValueAxis>
CategoryValueSeries::GetXAxis (void) const
{
  return m_xAxis;
}

void
CategoryValueSeries::SetXAxis (Ptr<ValueAxis> value)
{
  m_xAxis = value;
}

Ptr<CategoryAxis>
CategoryValueSeries::GetYAxis (void) const
{
  return m_yAxis;
}

void
CategoryValueSeries::SetYAxis (Ptr<CategoryAxis> value)
{
  m_yAxis = value;
}

void
CategoryValueSeries::DoDispose (void)
{
  m_orchestrator = nullptr;
  Object::DoDispose ();
}

} // namespace netsimulyzer
} // namespace ns3
