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

namespace ns3::netsimulyzer {

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

Ptr<const AttributeChecker>
MakeColor3Checker (void)
{
  return MakeSimpleAttributeChecker<Color3Value, Color3Checker> ("Color3"
                                                                 "Value",
                                                                 "Color3");
};

Color3Value::Color3Value (const Color3 &value) : m_value (value)
{
}

void
Color3Value::Set (const Color3 &v)
{
  m_value = v;
}

Color3
Color3Value::Get (void) const
{
  return m_value;
}

Ptr<AttributeValue>
Color3Value::Copy (void) const
{
  return Ptr<AttributeValue>{new Color3Value{m_value}, false};
}

std::string
Color3Value::SerializeToString (Ptr<const AttributeChecker> checker) const
{
  std::ostringstream oss;
  oss << m_value;
  return oss.str ();
}

bool
Color3Value::DeserializeFromString (std::string value, Ptr<const AttributeChecker> checker)
{
  std::istringstream iss;
  iss.str (value);
  iss >> m_value;
  NS_ABORT_MSG_UNLESS (iss.eof (), "Attribute value\"" << value << "\" is not properly formatted");
  return !iss.bad () && !iss.fail ();
}

} // namespace ns3
