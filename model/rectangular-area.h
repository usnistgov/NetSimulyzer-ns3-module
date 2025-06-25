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

#ifndef RECTANGULAR_AREA_H
#define RECTANGULAR_AREA_H

#include "color.h"
#include "orchestrator.h"

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/rectangle.h"

#include <cstdint>
#include <string>

namespace ns3::netsimulyzer
{

/**
 * A flat, fixed sized marker for an area of some significance to the simulation.
 */
class RectangularArea : public Object
{
  public:
    /**
     * Determines how to draw an element of the Area
     */
    enum DrawMode : int
    {
        Solid,
        Hidden
    };

    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Set up an area with boundaries
     *
     * @param orchestrator
     * The Orchestrator that will manage this area
     *
     * @param bounds
     * The area for this object to cover
     */
    RectangularArea(Ptr<Orchestrator> orchestrator, Rectangle bounds);

    /**
     * Set up an area without boundaries.
     *
     * If this constructor is used, the "Bounds" attribute should be set
     *
     * @param orchestrator
     * The Orchestrator that will manage this area
     */
    explicit RectangularArea(Ptr<Orchestrator> orchestrator);

  protected:
    void DoDispose(void) override;
    void NotifyConstructionCompleted(void) override;

  private:
    /**
     * Unique ID of the area, defined by the orchestrator
     */
    uint32_t m_id;

    /**
     * Pointer to the Orchestrator managing this Area
     */
    ns3::Ptr<Orchestrator> m_orchestrator;

    /**
     * Rectangle which defines the area
     */
    Rectangle m_bounds;

    /**
     * Flag to set `m_bounds` to the value set by the constructor
     */
    bool m_useConstructorBounds{false};

    /**
     * Bounds optionally specified by the constructor.
     *
     * Annoying hack to get around ns-3's overwriting
     * of attribute members after construction...
     */
    Rectangle m_constructorBounds{};

    /**
     * The Z coordinate for the area. In ns-3 units
     */
    double m_height;

    /**
     * Name for the area. For use in visualizer elements
     */
    std::string m_name;

    /**
     * How to draw the border of the area
     */
    DrawMode m_borderMode;

    /**
     * How to draw the fill of the area
     */
    DrawMode m_fillMode;

    /**
     * Color of the body (area within the border) of the area
     */
    Color3 m_fillColor;

    /**
     * Color of the border of the area
     */
    Color3 m_borderColor;
};

} // namespace ns3::netsimulyzer

#endif
