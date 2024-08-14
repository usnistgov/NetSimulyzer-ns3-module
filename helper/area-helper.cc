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
#include "area-helper.h"

#include <ns3/color.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/rectangular-area.h>

namespace ns3::netsimulyzer
{

NS_LOG_COMPONENT_DEFINE("AreaHelper");

AreaHelper::AreaHelper(const Ptr<Orchestrator>& orchestrator)
    : m_orchestrator{orchestrator}
{
}

void
AreaHelper::Set(const std::string& name, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this << name << &v);
    if (name.empty())
    {
        return;
    }

    TypeId::AttributeInformation info;
    if (!m_areaTypeId.LookupAttributeByName(name, &info))
    {
        NS_FATAL_ERROR("Invalid attribute set (" << name << ") on " << m_areaTypeId.GetName());
        return;
    }
    if (const auto attrValue = info.checker->CreateValidValue(v); !attrValue)
    {
        NS_FATAL_ERROR("Invalid value for attribute set (" << name << ") on "
                                                           << m_areaTypeId.GetName());
        return;
    }

    m_attributes[name] = v.Copy();
}

Ptr<RectangularArea>
AreaHelper::Make()
{
    NS_LOG_FUNCTION(this);
    auto area = CreateObject<RectangularArea>(m_orchestrator);
    for (const auto& [name, value] : m_attributes)
    {
        area->SetAttribute(name, *value);
    }

    return area;
}

Ptr<RectangularArea>
AreaHelper::Make(const Rectangle& bounds)
{
    NS_LOG_FUNCTION(this << bounds);

    auto area = CreateObject<RectangularArea>(m_orchestrator, bounds);

    for (const auto& [name, value] : m_attributes)
    {
        // Parameter bounds overrides
        // whatever may have been specified before
        if (name == "Bounds")
            continue;

        area->SetAttribute(name, *value);
    }

    return area;
}

Ptr<RectangularArea>
AreaHelper::MakeSquare(Vector2D center, double size)
{
    NS_LOG_FUNCTION(this << center << size);

    Rectangle bounds{};
    bounds.xMin = center.x - size / 2;
    bounds.xMax = center.x + size / 2;
    bounds.yMin = center.y - size / 2;
    bounds.yMax = center.y + size / 2;

    auto area = CreateObject<RectangularArea>(m_orchestrator, bounds);

    for (const auto& [name, value] : m_attributes)
    {
        // We have a manually specified position/size
        // so ignore whatever has been set before
        if (name == "Bounds")
            continue;

        area->SetAttribute(name, *value);
    }

    return area;
}

Ptr<netsimulyzer::RectangularArea>
AreaHelper::MakeAreaSurroundingNodes(NodeContainer nodes, double width, double vDiff)
{
  if ((nodes.GetN() == 0) || (width < 0) || (vDiff < 0))
  {
    return nullptr;
  }

  double xMin, xMax, yMin, yMax, zMin;
  Vector pos = nodes.Get(0)->GetObject<MobilityModel>()->GetPosition();
  xMin = xMax = pos.x;
  yMin = yMax = pos.y;
  zMin = pos.z;

  // Calculate bounds for the area
  for (uint32_t i = 1; i < nodes.GetN(); i++)
  {
    pos = nodes.Get(i)->GetObject<MobilityModel>()->GetPosition();
    xMin = std::min(xMin, pos.x);
    yMin = std::min(yMin, pos.y);
    zMin = std::min(zMin, pos.z);

    xMax = std::max(xMax, pos.x);
    yMax = std::max(yMax, pos.y);
  }
  
  Ptr<netsimulyzer::RectangularArea> area = CreateObject<netsimulyzer::RectangularArea>(m_orchestrator, 
      ns3::Rectangle{._xMin = xMin-width, ._xMax = xMax+width, ._yMin=yMin-width, ._yMax=yMax+width});

  if (zMin - vDiff < 0)
  {
    if (zMin < 0)
    {  
      area->SetAttribute("Height", DoubleValue(0));
    }
    else
    {
      area->SetAttribute("Height", DoubleValue(zMin));
    }
  }
  else
  {
    area->SetAttribute("Height", DoubleValue(zMin - vDiff));  
  }

  return area;
}


} // namespace ns3::netsimulyzer
