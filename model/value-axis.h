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

#ifndef VALUE_AXIS_H
#define VALUE_AXIS_H

#include "orchestrator.h"

#include "ns3/object.h"
#include "ns3/ptr.h"

#include <cstdint>
#include <string>

namespace ns3::netsimulyzer
{

/**
 * Defines an axis with a linear or logarithmic scale
 */
class ValueAxis : public ns3::Object
{
  public:
    /**
     * The method to scale between tick marks on the axis
     */
    enum Scale
    {
        Linear,
        Logarithmic
    };

    /**
     * How the boundaries of the axis may move given a new value
     */
    enum BoundMode
    {
        Fixed,
        HighestValue
    };

    /**
     * @brief Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Convenience method to set up the axis with a
     * fixed range. In the application, this axis
     * will *not* grow to accommodate new items
     *
     * @param min
     * The minimum value for the range, should be greater than `max`,
     * but not equal to
     *
     * @param max
     * The maximum value for the range, should be less than `min`,
     * but not equal to.
     */
    void FixedRange(double min, double max);

    /**
     * Convenience method to set up the axis with a
     * scaling `HighestValue` range.
     * In the application, this axis will grow
     * to accommodate new items.
     *
     * Provides a default range of 0 to 1.
     * If more precise control is desired,
     * use the overload with `min` and `max`
     * parameters
     *
     * @see ::ScalingRange (double min, double max)
     */
    void ScalingRange();

    /**
     * Convenience method to set up the axis with a
     * scaling `HighestValue` range.
     * In the application, this axis will grow
     * to accommodate new items.
     *
     * @param min
     * The minimum value for the range, should be greater than `max`,
     * but not equal to
     *
     * @param max
     * The maximum value for the range, should be less than `min`,
     * but not equal to.
     */
    void ScalingRange(double min, double max);

  private:
    /**
     * Name for the axis used in visualizer elements
     */
    std::string m_name;

    /**
     * The minimum possible value on the axis.
     * Should be less then the maximum.
     */
    double m_min;

    /**
     * The maximum value to be represented on the axis.
     * Should be grater than minimum.
     */
    double m_max;

    /**
     * The method to scale between tick marks on the axis
     */
    Scale m_scale;

    /**
     * How the boundaries of the axis may move
     */
    BoundMode m_boundMode;
};

} // namespace ns3::netsimulyzer

#endif /* VALUE_AXIS_H */
