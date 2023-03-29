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

#include "decoration.h"

#include "optional.h"

#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/pointer.h>
#include <ns3/string.h>
#include <ns3/uinteger.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("Decoration");

namespace netsimulyzer
{

NS_OBJECT_ENSURE_REGISTERED(Decoration);

Decoration::Decoration(Ptr<Orchestrator> orchestrator)
    : m_orchestrator(orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator);
    m_id = orchestrator->Register({this, true});
}

TypeId
Decoration::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::netsimulyzer::Decoration")
            .SetParent<Object>()
            .SetGroupName("netsimulyzer")
            .AddAttribute("Id",
                          "The unique ID of the Decoration",
                          TypeId::ATTR_GET,
                          UintegerValue(0u),
                          MakeUintegerAccessor(&Decoration::m_id),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("Model",
                          "Filename of the model to represent this Decoration",
                          StringValue(),
                          MakeStringAccessor(&Decoration::m_model),
                          MakeStringChecker())
            // clang-format off
            .AddAttribute("Orientation",
                          "Orientation of the Decoration on each axis in degrees",
                          Vector3DValue(),
                          MakeVector3DAccessor(&Decoration::GetOrientation,
                                               &Decoration::SetOrientation),
                          MakeVector3DChecker())
            // clang-format on
            .AddAttribute("KeepRatio",
                          "When scaling with the `Height`, `Width`, and `Depth` attributes, "
                          "use only the value that produces the largest model. "
                          "Keeping the scale uniform",
                          BooleanValue(true),
                          MakeBooleanAccessor(&Decoration::m_keepRatio),
                          MakeBooleanChecker())
            .AddAttribute("Height",
                          "Desired height of the rendered model. Applied before `Scale`",
                          OptionalValue<double>(),
                          MakeOptionalAccessor<double>(&Decoration::m_height),
                          MakeOptionalChecker<double>())
            .AddAttribute("Width",
                          "Desired width of the rendered model in ns-3 units. "
                          "Applied before `Scale`",
                          OptionalValue<double>(),
                          MakeOptionalAccessor<double>(&Decoration::m_width),
                          MakeOptionalChecker<double>())
            .AddAttribute("Depth",
                          "Desired depth of the rendered model in ns-3 units. "
                          "Applied before `Scale`",
                          OptionalValue<double>(),
                          MakeOptionalAccessor<double>(&Decoration::m_depth),
                          MakeOptionalChecker<double>())
            .AddAttribute("Position",
                          "The position of the Decoration",
                          Vector3DValue(),
                          MakeVector3DAccessor(&Decoration::GetPosition, &Decoration::SetPosition),
                          MakeVector3DChecker())
            .AddAttribute("Scale",
                          "The scale to apply to the rendered model",
                          DoubleValue(1.0),
                          MakeDoubleAccessor(&Decoration::m_scale),
                          MakeDoubleChecker<double>(0))
            .AddAttribute("ScaleAxes",
                          "The scale to apply each axis in the order [x, y, z]. "
                          "similar to `Scale`, but allows for non-uniform scales",
                          Vector3DValue(Vector3D{1.0, 1.0, 1.0}),
                          MakeVector3DAccessor(&Decoration::m_scaleAxes),
                          MakeVector3DChecker())
            .AddAttribute("Orchestrator",
                          "Orchestrator that manages this Decoration",
                          PointerValue(),
                          MakePointerAccessor(&Decoration::m_orchestrator),
                          MakePointerChecker<Orchestrator>());
    return tid;
}

const Vector3D&
Decoration::GetPosition() const
{
    NS_LOG_FUNCTION(this);
    return m_position;
}

void
Decoration::SetPosition(const Vector3D& position)
{
    NS_LOG_FUNCTION(this << position);
    m_position = position;

    DecorationMoveEvent event;
    event.time = Simulator::Now();
    event.id = m_id;
    event.position = m_position;

    m_orchestrator->HandlePositionChange(event);
}

const Vector3D&
Decoration::GetOrientation() const
{
    NS_LOG_FUNCTION(this);
    return m_orientation;
}

void
Decoration::SetOrientation(const Vector3D& orientation)
{
    NS_LOG_FUNCTION(this << orientation);
    m_orientation = orientation;

    DecorationOrientationChangeEvent event;
    event.time = Simulator::Now();
    event.id = m_id;
    event.orientation = m_orientation;

    m_orchestrator->HandleOrientationChange(event);
}

void
Decoration::SetScale(double scale)
{
    m_scale = scale;
}

void
Decoration::SetScale(const Vector3D& scale)
{
    SetScaleAxes(scale);
}

void
Decoration::SetScaleAxes(const Vector3D& scale)
{
    m_scaleAxes = scale;
}

double
Decoration::GetScale(void) const
{
    return m_scale;
}

const Vector3D&
Decoration::GetScaleAxes(void) const
{
    return m_scaleAxes;
}

void
Decoration::DoDispose(void)
{
    NS_LOG_FUNCTION(this);
    m_orchestrator = nullptr;
    Object::DoDispose();
}

} // namespace netsimulyzer
} // namespace ns3
