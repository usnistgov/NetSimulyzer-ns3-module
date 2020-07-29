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

#include "color.h"

/**
 * Verifies the next character in the stream is `separator`,
 * otherwise sets `stream`'s failbit.
 * Also consumes the character.
 *
 * \param stream
 * The input stream to read from
 *
 * \param separator
 * The separator character to check for
 */
static void
checkSeparator (std::istream &stream, const char separator = '|')
{
  char in;
  stream >> in;

  if (in != separator)
    stream.setstate (std::ios::failbit);
}

namespace ns3 {
namespace visualizer3d {

Color4::Color4 (uint8_t component)
    : red (component), green (component), blue (component), alpha (255u)
{
}

Color4::Color4 (uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    : red (red), green (green), blue (blue), alpha (alpha)
{
}

ATTRIBUTE_HELPER_CPP (Color4)

std::ostream &
operator<< (std::ostream &os, const Color4 &color)
{
  os << color.red << '|' << color.green << '|' << color.blue << '|' << color.alpha;
  return os;
}

std::istream &
operator>> (std::istream &is, Color4 &color)
{
  // Be sure to match the << order
  is >> color.red;
  checkSeparator (is);

  is >> color.green;
  checkSeparator (is);

  is >> color.blue;
  checkSeparator (is);

  is >> color.alpha;
  checkSeparator (is);

  return is;
}

Color3::Color3 (uint8_t component) : red (component), green (component), blue (component)
{
}

Color3::Color3 (uint8_t red, uint8_t green, uint8_t blue) : red (red), green (green), blue (blue)
{
}

std::ostream &
operator<< (std::ostream &os, const Color3 &color)
{
  os << color.red << '|' << color.green << '|' << color.blue;
  return os;
}

std::istream &
operator>> (std::istream &is, Color3 &color)
{
  // Be sure to match the << order
  is >> color.red;
  checkSeparator (is);

  is >> color.green;
  checkSeparator (is);

  is >> color.blue;
  checkSeparator (is);

  return is;
}

ATTRIBUTE_HELPER_CPP (Color3)

} // namespace visualizer3d
} // namespace ns3
