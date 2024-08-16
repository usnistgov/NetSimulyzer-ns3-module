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

#ifndef AREA_HELPER_H
#define AREA_HELPER_H

#include <ns3/node-container.h>
#include <ns3/orchestrator.h>
#include <ns3/ptr.h>
#include <ns3/rectangle.h>
#include <ns3/rectangular-area.h>
#include <ns3/vector.h>

#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

/**
 * \ingroup netsimulyzer
 *  Creates `RectangularArea`s
 */
class AreaHelper
{
  public:
    explicit AreaHelper(const Ptr<Orchestrator>& orchestrator);

    /**
     * Sets one of the attributes of underlying model
     *
     * \param name Name of attribute to set.
     * \param v Value of the attribute.
     */
    void Set(const std::string& name, const AttributeValue& v);

    /**
     * Creates a new `RectangularArea` with bounds defined
     * by the 'Bounds' attribute passed to `Set`
     *
     * If the `Bounds` attribute was not set before calling this
     * then it must be set on the created `RectangularArea`
     * before the simulation starts
     *
     * @return
     * A new `RectangularArea`
     */
    Ptr<RectangularArea> Make();

    /**
     * Creates a new `RectangularArea` with defined bounds
     *
     * @param bounds
     * The dimensions of the `RectangularArea`.
     * Overrides the 'Bounds' attribute
     *
     * @return
     * A complete `RectangularArea`
     */
    Ptr<RectangularArea> Make(const Rectangle& bounds);

    /**
     * Creates a square `RectangularArea` centered around
     * `center`
     *
     * @param center
     * The center of the new area
     *
     * @param size
     * The interior length of each side of the area
     *
     * @return
     * A square `RectangularArea` centered around `center`
     */
    Ptr<RectangularArea> MakeSquare(Vector2D center, double size);

  private:
    const TypeId m_areaTypeId{RectangularArea::GetTypeId()};
    Ptr<Orchestrator> m_orchestrator;
    std::unordered_map<std::string, Ptr<AttributeValue>> m_attributes;
};

} // namespace ns3::netsimulyzer

#endif // AREA_HELPER_H
