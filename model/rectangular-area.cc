#include "rectangular-area.h"
#include "color-palette.h"
#include <ns3/uinteger.h>
#include <ns3/double.h>
#include <ns3/rectangle.h>
#include <ns3/enum.h>
#include <ns3/string.h>

namespace ns3::netsimulyzer {

TypeId
RectangularArea::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::RectangularArea")
          .SetParent<Object> ()
          .SetGroupName("netsimulyzer")
          .AddAttribute ("Id", "The unique ID of the Area", TypeId::ATTR_GET, UintegerValue (0u),
                         MakeUintegerAccessor (&RectangularArea::m_id),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("Bounds", "Rectangle which defines the boundaries of the area",
                         RectangleValue (), MakeRectangleAccessor (&RectangularArea::m_bounds),
                         MakeRectangleChecker ())
          .AddAttribute ("Height", "The Z level to render the area at", DoubleValue (0.0),
                         MakeDoubleAccessor (&RectangularArea::m_height),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("Border", "How to draw the border of the area",
                         EnumValue (DrawMode::Solid),
                         MakeEnumAccessor (&RectangularArea::m_borderMode),
                         MakeEnumChecker (DrawMode::Solid, "Solid", DrawMode::Hidden, "Hidden"))
          .AddAttribute ("Fill", "How to draw the body of the area", EnumValue (DrawMode::Hidden),
                         MakeEnumAccessor (&RectangularArea::m_fillMode),
                         MakeEnumChecker (DrawMode::Solid, "Solid", DrawMode::Hidden, "Hidden"))
          .AddAttribute ("Name", "Name to represent this area in visualizer elements",
                         StringValue (), MakeStringAccessor (&RectangularArea::m_name),
                         MakeStringChecker ())
          .AddAttribute ("FillColor", "Color of the area within the borders",
                         GRAY_20_VALUE /* light gray (20%) */,
                         MakeColor3Accessor (&RectangularArea::m_fillColor), MakeColor3Checker ())
          .AddAttribute ("BorderColor", "Color of the border surrounding the fill",
                         BLACK_VALUE /* black */,
                         MakeColor3Accessor (&RectangularArea::m_borderColor), MakeColor3Checker ())

      ;
  return tid;
}

RectangularArea::RectangularArea (Ptr<Orchestrator> orchestrator, Rectangle bounds)
    : m_orchestrator (orchestrator),
      m_useConstructorBounds (true), // See: NotifyConstructionCompleted (void)
      m_constructorBounds (bounds)
{
  m_id = m_orchestrator->Register ({this, true});
}

RectangularArea::RectangularArea (Ptr<Orchestrator> orchestrator) : m_orchestrator (orchestrator)
{
  m_id = m_orchestrator->Register ({this, true});
}

void
RectangularArea::DoDispose (void)
{
  m_orchestrator = nullptr;
  Object::DoDispose ();
}

void
RectangularArea::NotifyConstructionCompleted (void)
{
  // Annoying hack to allow the bounds to be set by the
  // constructor.
  // Since ns-3 will supply a default value and overwrite members
  // which are tied to attributes after the constructor has
  // returned
  if (m_useConstructorBounds)
    m_bounds = m_constructorBounds;

  Object::NotifyConstructionCompleted ();
}

} // namespace ns3