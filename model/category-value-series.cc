#include "category-value-series.h"

#include "color-palette.h"

#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/string.h"
#include "ns3/type-id.h"
#include "ns3/uinteger.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("CategoryValueSeries");

namespace netsimulyzer
{

NS_OBJECT_ENSURE_REGISTERED(CategoryValueSeries);

TypeId
CategoryValueSeries::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::netsimulyzer::CategoryValueSeries")
            .SetParent<ns3::Object>()
            .SetGroupName("netsimulyzer")
            .AddAttribute("Id",
                          "The unique ID of the series",
                          TypeId::ATTR_GET,
                          UintegerValue(0u),
                          MakeUintegerAccessor(&CategoryValueSeries::m_id),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("Name",
                          "Unique name to represent this series in visualizer elements",
                          StringValue(),
                          MakeStringAccessor(&CategoryValueSeries::m_name),
                          MakeStringChecker())
            .AddAttribute("Legend",
                          "Name for the series that appears in the chart legend",
                          StringValue(),
                          MakeStringAccessor(&CategoryValueSeries::m_legend),
                          MakeStringChecker())
            .AddAttribute("Visible",
                          "Should this series appear in selection elements",
                          BooleanValue(true),
                          MakeBooleanAccessor(&CategoryValueSeries::m_visible),
                          MakeBooleanChecker())
            // clang-format off
            .AddAttribute("XAxis",
                          "The X axis on the graph",
                          PointerValue(),
                          MakePointerAccessor(&CategoryValueSeries::GetXAxis,
                                              &CategoryValueSeries::SetXAxis),
                          MakePointerChecker<ValueAxis>())
            .AddAttribute("YAxis",
                          "The Y axis on the graph",
                          PointerValue(),
                          MakePointerAccessor(&CategoryValueSeries::GetYAxis,
                                              &CategoryValueSeries::SetYAxis),
                          MakePointerChecker<CategoryAxis>())
            // clang-format on
            .AddAttribute("Color",
                          "Color to use for the points and connections",
                          BLUE_VALUE,
                          MakeColor3Accessor(&CategoryValueSeries::m_color),
                          MakeColor3Checker())
            .AddAttribute("AutoUpdate",
                          "Automatically append values in the same category (Y value) "
                          "but with a greater X value. Must also set `AutoUpdateInterval` and "
                          "`AutoUpdateValue`",
                          BooleanValue(false),
                          MakeBooleanAccessor(&CategoryValueSeries::m_autoUpdate),
                          MakeBooleanChecker())
            .AddAttribute("AutoUpdateInterval",
                          "The minimum time before appending `AutoUpdateValue` To the series",
                          TimeValue(),
                          MakeTimeAccessor(&CategoryValueSeries::m_autoUpdateInterval),
                          MakeTimeChecker())
            .AddAttribute("AutoUpdateIncrement",
                          "The value to append to the previous X value "
                          "after `AutoUpdateInterval` has passed",
                          DoubleValue(),
                          MakeDoubleAccessor(&CategoryValueSeries::m_autoUpdateIncrement),
                          MakeDoubleChecker<double>());

    return tid;
}

CategoryValueSeries::CategoryValueSeries(Ptr<Orchestrator> orchestrator)
    : m_orchestrator(orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator);
    m_id = m_orchestrator->Register({this, true});
}

CategoryValueSeries::CategoryValueSeries(Ptr<Orchestrator> orchestrator,
                                         const std::vector<std::string>& categories)
    : m_orchestrator(orchestrator),
      m_yAxis(CreateObject<CategoryAxis>(categories))
{
    NS_LOG_FUNCTION(this << orchestrator);
    m_id = m_orchestrator->Register({this, true});
}

CategoryValueSeries::CategoryValueSeries(Ptr<Orchestrator> orchestrator,
                                         const std::vector<CategoryAxis::ValuePair>& categories)
    : m_orchestrator(orchestrator),
      m_yAxis(CreateObject<CategoryAxis>(categories))
{
    NS_LOG_FUNCTION(this << orchestrator);
    m_id = m_orchestrator->Register({this, true});
}

void
CategoryValueSeries::Append(int category, double value)
{
    NS_LOG_FUNCTION(this << category << value);
    m_orchestrator->AppendCategoryValue(m_id, category, value);
}

void
CategoryValueSeries::Append(const std::string& category, double value)
{
    NS_LOG_FUNCTION(this << category << value);
    Append(m_yAxis->GetId(category), value);
}

void
CategoryValueSeries::Commit(void)
{
    NS_LOG_FUNCTION(this);
    if (m_committed)
    {
        NS_LOG_DEBUG("Ignoring Commit () on already committed model");
        return;
    }

    m_orchestrator->Commit(*this);
    m_committed = true;
}

Ptr<ValueAxis>
CategoryValueSeries::GetXAxis(void) const
{
    NS_LOG_FUNCTION(this);
    return m_xAxis;
}

void
CategoryValueSeries::SetXAxis(Ptr<ValueAxis> value)
{
    NS_LOG_FUNCTION(this << value);
    m_xAxis = value;
}

Ptr<CategoryAxis>
CategoryValueSeries::GetYAxis(void) const
{
    NS_LOG_FUNCTION(this);
    return m_yAxis;
}

void
CategoryValueSeries::SetYAxis(Ptr<CategoryAxis> value)
{
    NS_LOG_FUNCTION(this << value);
    m_yAxis = value;
}

void
CategoryValueSeries::DoDispose(void)
{
    NS_LOG_FUNCTION(this);
    m_orchestrator = nullptr;
    m_xAxis = nullptr;
    m_yAxis = nullptr;
    Object::DoDispose();
}

} // namespace netsimulyzer
} // namespace ns3
