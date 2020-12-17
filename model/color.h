/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 *
 * Author: Evan Black <evan.black@nist.gov>
 */
#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <ns3/attribute.h>
#include <ns3/attribute-helper.h>

namespace ns3 {
namespace visualizer3d {

class Color3;

/**
 * Represents a 4 component RGB color with an alpha(transparency) channel
 *
 * \see Color3
 */
class Color4
{
public:
  /**
   * Initialize a black color (all components 0) with full opacity (255)
   */
  Color4 (void) = default;

  /**
   * Initialize all color with uniform components & full opacity (255).
   * Allows for implicit conversions.
   *
   * \param component
   * The value to set the color components to
   */
  Color4 (uint8_t component);

  /**
   * Initialize a color, setting each channel
   *
   * \param red
   * The amount of red in the resulting color
   *
   * \param green
   * The amount of green in the resulting color
   *
   * \param blue
   * The amount of blue in the resulting color
   *
   * \param alpha
   * The opacity of the resulting color. (default 255)
   */
  Color4 (uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255u);

  /**
   * Convert a `Color3` to a `Color4` with full opacity
   *
   * \param color
   * A `Color3 to read the RGB components from
   */
  Color4 (const Color3 &color);

  // They're public, sue me

  /**
   * The red component of the resulting color.
   * Range [0 - 255]
   */
  uint8_t red{0u};

  /**
   * The green component of the resulting color.
   * Range [0 - 255]
   */
  uint8_t green{0u};

  /**
   * The blue component of the resulting color.
   * Range [0 - 255]
   */
  uint8_t blue{0u};

  /**
   * The opacity of the resulting color
   * Range [0 - 255]
   */
  uint8_t alpha{255u};
};

// Required for the attribute....
std::ostream &operator<< (std::ostream &os, const Color4 &color);
std::istream &operator>> (std::istream &is, Color4 &color);

ATTRIBUTE_HELPER_HEADER (Color4);

/**
 * Represents a 3 component RGB color.
 * Does not support opacity
 *
 * \see Color4
 */
class Color3
{
public:
  /**
   * Initialize a black color (all components 0)
   */
  Color3 (void) = default;

  /**
   * Initialize all color with uniform components
   * Allows for implicit conversions.
   *
   * \param component
   * The value to set the color components to
   */
  Color3 (uint8_t component);

  /**
   * Initialize a color, setting each channel
   *
   * \param red
   * The amount of red in the resulting color
   *
   * \param green
   * The amount of green in the resulting color
   *
   * \param blue
   * The amount of blue in the resulting color
   */
  Color3 (uint8_t red, uint8_t green, uint8_t blue);

  /**
   * Convert a `Color4` to a `Color3`
   * Will result in the `alpha` being lost
   *
   * \param color
   * A `Color4 to read the RGB components from
   */
  Color3 (const Color4 &color);

  // They're public, sue me

  /**
   * The red component of the resulting color.
   * Range [0 - 255]
   */
  uint8_t red{0u};

  /**
   * The green component of the resulting color.
   * Range [0 - 255]
   */
  uint8_t green{0u};

  /**
   * The blue component of the resulting color.
   * Range [0 - 255]
   */
  uint8_t blue{0u};
};

// Required for the attribute....
std::ostream &operator<< (std::ostream &os, const Color3 &color);
std::istream &operator>> (std::istream &is, Color3 &color);

ATTRIBUTE_HELPER_HEADER (Color3);

} // namespace visualizer3d
} // namespace ns3

#endif /* COLOR_H */
