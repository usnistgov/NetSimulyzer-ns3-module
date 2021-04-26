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

#include "category-axis.h"
#include <algorithm>
#include <ns3/log.h>
#include <ns3/string.h>
#include <ns3/pointer.h>
#include <ns3/abort.h>

namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("CategoryAxis");
namespace netsimulyzer {

NS_OBJECT_ENSURE_REGISTERED (CategoryAxis);

TypeId
CategoryAxis::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::netsimulyzer::CategoryAxis")
          .SetParent<ns3::Object> ()
          .SetGroupName ("netsimulyzer")
          .AddAttribute ("Name", "Unique name to represent this axis in visualizer elements",
                         StringValue (), MakeStringAccessor (&CategoryAxis::m_name),
                         MakeStringChecker ());

  return tid;
}

CategoryAxis::CategoryAxis (const std::vector<std::string> &values)
{
  NS_LOG_FUNCTION (this);
  for (const auto &value : values)
    AddValue (value);
}

CategoryAxis::CategoryAxis (const std::vector<ValuePair> &values)
{
  NS_LOG_FUNCTION (this);
  for (const auto &value : values)
    AddValue (value);
}

void
CategoryAxis::AddValue (const std::string &value)
{
  NS_LOG_FUNCTION (this << value);
  AddValue ({m_nextId++, value});
}

void
CategoryAxis::AddValue (const CategoryAxis::ValuePair &value)
{
  NS_LOG_FUNCTION (this << value.key << value.value);
  m_values.emplace (value.key, value.value);

  // We have no guarantee that the user won't still add
  // keys with no values, so keep our next key ahead of
  // the largest
  if (value.key > m_nextId)
    m_nextId = value.key + 1;
}

const std::unordered_map<int, std::string> &
CategoryAxis::GetValues (void) const
{
  NS_LOG_FUNCTION (this);
  return m_values;
}

int
CategoryAxis::GetId (const std::string &name) const
{
  NS_LOG_FUNCTION (this << name);
  // TODO: When/If ns3 updates to C++ 14, use auto instead
  const auto iter =
      std::find_if (m_values.begin (), m_values.end (),
                    [&name] (const std::unordered_map<int, std::string>::value_type &value) {
                      return value.second == name;
                    });

  if (iter == m_values.end ())
    NS_ABORT_MSG ("Name: " << name << " not registered with CategoryAxis");

  return iter->first;
}

CategoryAxis::ValuePair
CategoryAxis::Get (int id)
{
  NS_LOG_FUNCTION (this << id);
  return {id, m_values.at (id)};
}

CategoryAxis::ValuePair
CategoryAxis::Get (const std::string &name)
{
  NS_LOG_FUNCTION (this << name);
  return {GetId (name), name};
}

} // namespace netsimulyzer
} // namespace ns3
