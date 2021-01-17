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

#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include <ns3/ptr.h>
#include <ns3/object.h>
#include <ns3/object.h>
#include <ns3/color.h>
#include <ns3/optional.h>
#include <optional>

namespace ns3 {
namespace visualizer3d {

class Orchestrator;

/**
 * Stream to log messages for display in the
 * `ScenarioLogWidget` in the visualizer
 */
class LogStream : public Object
{
public:
  /**
   * Sets up the LogStream and assigns the ID
   *
   * \param orchestrator
   * The orchestrator to tie this LogStream to
   */
  explicit LogStream (Ptr<Orchestrator> orchestrator);

  /**
   * \brief Get the class TypeId
   *
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Writes`message` out on this log stream at the current time.
   *
   * \param message
   * The message to write on the stream.
   */
  void Write (const std::string &message) const;

protected:
  void DoDispose (void) override;

private:
  /**
   * The Orchestrator that manages this stream.
   */
  Ptr<Orchestrator> m_orchestrator;

  /**
   * Unique ID of the stream. Assigned from the Orchestrator
   */
  uint32_t m_id;

  /**
   * The name of the stream. To show on visualizer dropdown and unified log
   */
  std::string m_name;

  /**
   * Flag indicating this item should appear in visualizer elements
   */
  bool m_visible;

  /**
   * The font color
   */
  std::optional<Color3> m_color;
};

/**
 * Converts `value` to a string and writes it to stream `stream`.
 *
 * \tparam T
 * The type of the value to output. Should have a std::to_string
 * overload. If not, use the `std::string` specialized version
 *
 * \param stream
 * The stream to write to
 *
 * \param value
 * The value to output on `stream` will be converted to a string
 *
 * \return
 * The stream `stream` to chain additional calls
 */
template <class T>
inline LogStream &
operator<< (LogStream &stream, const T &value)
{
  stream.Write (std::to_string (value));
  return stream;
}

/**
 * Writes a string out on `stream` with no conversion to `value`.
 *
 * \param stream
 * The stream to write to
 *
 * \param value
 * The value to write to `stream`
 *
 * \return
 * The stream `stream` to chain additional calls
 */
template <>
inline LogStream &
operator<< (LogStream &stream, const std::string &value)
{
  stream.Write (value);
  return stream;
}
/**
 * Writes a string out on `stream` with no conversion to `value`.
 *
 * \param stream
 * The stream to write to
 *
 * \param value
 * The value to write to `stream`
 *
 * \return
 * The stream `stream` to chain additional calls
 */
template <>
inline LogStream &
operator<< (LogStream &stream, const char &value)
{
  stream.Write (std::string{value});
  return stream;
}

/**
 * Writes a string literal on `stream` with no conversion to `value`
 * (other than to std::string).
 *
 * \param stream
 * The stream to write to
 *
 * \param value
 * The value to write to `stream`
 *
 * \return
 * The stream `stream` to chain additional calls
 */
LogStream &operator<< (LogStream &stream, const char *value);

} // namespace visualizer3d
} // namespace ns3

#endif /*LOG_STREAM_H*/
