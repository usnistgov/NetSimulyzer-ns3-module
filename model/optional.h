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

#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <ns3/attribute.h>
#include <ns3/attribute-helper.h>
#include <ns3/log.h>
#include <type_traits>
#include <memory>
#include <typeinfo>
#include <utility>
#include <optional>

namespace ns3::visualizer3d {

/**
 * Attribute wrapper for std::optional,
 * a type to represent a value which may or may not be present.
 *
 * \tparam T
 * The type of value to store within the attribute
 *
 * \see https://en.cppreference.com/w/cpp/utility/optional
 */
template <typename T>
class OptionalValue : public AttributeValue
{
public:
  /**
   * Initialize the attribute with no stored value
   */
  OptionalValue (void) = default;

  /**
   * Make a copy of `other`
   *
   * \param other
   * The attribute to copy, will also copy the stored value
   */
  OptionalValue (const OptionalValue<T> &other) = default;

  /**
   * Initialize the attribute directly from a std::optional.
   * Allows for implicit conversion from a std::optional of
   * the same type.
   *
   * \param value
   * The value to copy into the new attribute.
   */
  OptionalValue (const std::optional<T> &value);

  /**
   * Constructs the contained type in-place, avoiding copies.
   *
   * An example using the `Color3` class follows:
   * \code{.cc}
   * // Calls Color3::Color3(uint8_t, uint8_t, uint8_t)
   * // Never copies the color
   * OptionalValue<Color3> optional{255u, 0u, 0u};
   * \endcode
   *
   * \tparam Args (Deduced)
   * The type of each argument passed
   *
   * \param args
   * The arguments normally passed to the enclosing type's constructor.
   */
  template <typename... Args>
  explicit OptionalValue (Args &&...args);

  /**
   * Sets the contained std::optional
   * by copying `value`. Will overwrite
   * the previously contained value.
   *
   * \param value
   * The value to copy into this attribute.
   */
  void Set (const std::optional<T> &value);

  /**
   * Constructs the value of the std::optional in-place, avoiding copies.
   * See the emplace constructor for more info
   *
   * \tparam Args (Deduced)
   * The type of each argument passed
   *
   * \param args
   * The arguments normally passed to the enclosing type's constructor.
   */
  template <typename... Args>
  void Emplace (Args &&...args);

  /**
   * Retrieves a reference to the contained std::optional,
   * not the actual contained value.
   *
   * \return
   * A reference to the contained `std::optional`
   *
   * \see OptionalValue::GetValue
   */
  const std::optional<T> &Get (void) const;

  /**
   * Returns a reference to actual contained value.
   *
   * \warning Will abort if no value has been set
   *
   * \return
   * A reference to the contained value
   *
   * \see OptionalValue::operator bool()
   * \see OptionalValue::HasValue()
   */
  const T &GetValue (void) const;

  /**
   * Checks if the contained std::optional
   * has a value. Should be checked before
   * attempting to read the value.
   *
   * The method counterpart to the bool operator.
   *
   * \return
   * True if the contained std::optional has a value,
   * false otherwise.
   *
   * \see OptionalValue<T>::operator bool (void)
   */
  bool HasValue (void) const;

  /**
   * Operator allowing the attribute to appear in conditionals.
   *
   * Checks if the contained std::optional holds a value.
   *
   * Allows for usage like the below:
   * \code
   * OptionalValue<int> hasValue{42};
   * if (hasValue)
   *   std::cout << "Will Print\n";
   *
   * OptionalValue<int> hasNoValue;
   * if (hasNoValue)
   *   std::cout << "Will Not Print\n";
   * \endcode
   *
   * \return
   * True if the contained `std::optional` has a value,
   * false otherwise.
   *
   * \see OptionalValue::HasValue()
   */
  explicit operator bool (void) const;

  template <typename V>
  bool GetAccessor (V &value) const;

  /**
   * Copies the attribute with the enclosed value.
   * Does not detect if the enclosed value is reference type.
   *
   * \return
   * A copy of this attribute
   */
  Ptr<AttributeValue> Copy (void) const override;

  /**
   * Unimplemented conversion to string.
   *
   * Required by `AttributeValue`.
   *
   * \warning Will abort if called
   *
   * \param checker
   * Unused
   *
   * \return
   * Empty String
   */
  std::string SerializeToString (Ptr<const AttributeChecker> checker) const override;

  /**
   * Unimplemented conversion from string.
   *
   * Required by `AttributeValue`.
   *
   * \warning Will abort if called
   *
   * \param value
   * Unused
   *
   * \param checker
   * Unused
   *
   * \return
   * False
   */
  bool DeserializeFromString (std::string value, Ptr<const AttributeChecker> checker) override;

private:
  /**
   * The backing store for the attribute
   */
  std::optional<T> m_value;
};

template <typename T>
OptionalValue<T>::OptionalValue (const std::optional<T> &value) : m_value{value}
{
}

template <typename T>
template <typename... Args>
OptionalValue<T>::OptionalValue (Args &&...args)
    : m_value{std::in_place, std::forward<Args> (args)...}
{
}

template <typename T>
void
OptionalValue<T>::Set (const std::optional<T> &value)
{
  m_value = value;
}

template <typename T>
template <typename... Args>
void
OptionalValue<T>::Emplace (Args &&...args)
{
  m_value.template emplace (std::forward<Args> (args)...);
}

template <typename T>
const std::optional<T> &
OptionalValue<T>::Get (void) const
{
  return m_value;
}

template <typename T>
const T &
OptionalValue<T>::GetValue (void) const
{
  return m_value.value ();
}

template <typename T>
bool
OptionalValue<T>::HasValue (void) const
{
  return m_value.has_value ();
}

template <typename T>
OptionalValue<T>::operator bool (void) const
{
  return HasValue ();
}

template <typename T>
template <typename V>
bool
OptionalValue<T>::GetAccessor (V &value) const
{
  value = V (m_value);
  return true;
}

template <typename T>
Ptr<AttributeValue>
OptionalValue<T>::Copy (void) const
{
  return Ptr<AttributeValue> (new OptionalValue<T>{m_value}, false);
}

template <typename T>
std::string
OptionalValue<T>::SerializeToString (Ptr<const AttributeChecker> checker) const
{
  NS_ABORT_MSG ("Unsupported Operation 'SerializeToString()'");
  return {};
}
template <typename T>
bool
OptionalValue<T>::DeserializeFromString (std::string value, Ptr<const AttributeChecker> checker)
{
  NS_ABORT_MSG ("Unsupported Operation 'DeserializeFromString()'");
  return false;
}

// ----- ATTRIBUTE ACCESSOR -----

template <typename T0, typename T1>
Ptr<const AttributeAccessor>
MakeOptionalAccessor (T1 getOrSet)
{
  return MakeAccessorHelper<OptionalValue<T0>> (getOrSet);
}

template <typename T0, typename T1, typename T2>
Ptr<const AttributeAccessor>
MakeOptionalAccessor (T1 setter, T2 getter)
{
  return MakeAccessorHelper<OptionalValue<T0>> (setter, getter);
}

// ----- ATTRIBUTE CHECKER -----

/**
 * Rudimentary checker for optional attributes.
 * Checks if an Attribute is both an OptionalAttribute
 * and encloses the same value.
 *
 * \tparam T
 * The enclosed type of the attribute to check for. It should
 * match the template parameter of the OptionalAttribute
 */
template <typename T>
class OptionalChecker : public AttributeChecker
{
public:
  /**
   * Checks if `value` is an OptionalAttribute enclosing the
   * correct type.
   *
   * \param value
   * The attribute to check
   *
   * \return
   * True if the attribute is an OptionalAttribute enclosing
   * type `T` , false otherwise
   */
  bool Check (const AttributeValue &value) const override;

  /**
   * Notifies ns-3 that we do not have human readable type info
   *
   * \return
   * False
   */
  bool HasUnderlyingTypeInformation (void) const override;

  /**
   * Pulls the name of `T` from `typeid`.
   * Will produce a name, but will probably not be human readable
   *
   * \return
   * The type name from the implementation
   *
   */
  std::string GetUnderlyingTypeInformation (void) const override;

  /**
   * Create an uninitialized `OptionalChecker`
   *
   * \return
   * A new `OptionalChecker`
   */
  Ptr<AttributeValue> Create (void) const override;

  bool Copy (const AttributeValue &source, AttributeValue &destination) const override;
};

template <typename T>
bool
OptionalChecker<T>::Check (const AttributeValue &value) const
{
  return dynamic_cast<const OptionalValue<T> *> (&value) != nullptr;
}

template <typename T>
std::string
OptionalChecker<T>::GetValueTypeName (void) const
{
  return "ns3::visualizer3d::OptionalValue";
}

template <typename T>
bool
OptionalChecker<T>::HasUnderlyingTypeInformation (void) const
{
  return false;
}

template <typename T>
std::string
OptionalChecker<T>::GetUnderlyingTypeInformation (void) const
{
  // Best possible answer I can give here
  // Probably will not produce a readable name
  // under GCC or Clang
  // TODO: Maybe demangle this if people are really interested
  // See: abi::__cxa_demangle
  return std::string{"ns3::visualizer3d::Optional<"} + typeid (T).name () + ">";
}

template <typename T>
Ptr<AttributeValue>
OptionalChecker<T>::Create (void) const
{
  // Almost identical implementation to the helper

  // Qualify with the namespace since the
  // function names are the same...
  return ns3::Create<OptionalValue<T>> ();
}

template <typename T>
bool
OptionalChecker<T>::Copy (const AttributeValue &source, AttributeValue &destination) const
{
  const auto *src = dynamic_cast<const OptionalValue<T> *> (&source);
  auto *dst = dynamic_cast<OptionalValue<T> *> (&destination);

  if (src == nullptr || dst == nullptr)
    return false;

  *dst = *src;
  return true;
}

template <typename T>
Ptr<const AttributeChecker>
MakeOptionalChecker (void)
{
  return Create<OptionalChecker<T>> ();
}

// TODO: Maybe Bounded Checker for Number types

} // namespace ns3::visualizer3d

#endif // OPTIONAL_H
