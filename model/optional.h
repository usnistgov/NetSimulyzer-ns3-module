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

namespace ns3 {
namespace visualizer3d {

template <typename T>
class Optional
{
public:
  Optional () = default;
  Optional (const Optional<T> &other) = default;

  Optional<T> &operator= (const Optional<T> &other);
  Optional<T> &operator= (T &&other);

  template <class... Args>
  explicit Optional (Args &&...args);

  ~Optional ();

  template <class... Args>
  void Emplace (Args &&...args);

  const T &GetValue (void) const;
  const T &operator* (void) const;

  bool HasValue (void) const;
  explicit operator bool (void) const;

  template <typename U>
  T ValueOr (U &&defaultValue) const;

  void Reset ();

private:
  using m_value_t = typename std::aligned_storage<sizeof (T), alignof (T)>::type;
  m_value_t m_value;
  bool m_initialized{false};
};

template <typename T>
Optional<T> &
Optional<T>::operator= (const Optional &other)
{
  Reset ();

  m_initialized = other.m_initialized;
  if (other.m_initialized)
    m_value = other.m_value;

  return *this;
}

template <typename T>
Optional<T> &
Optional<T>::operator= (T &&other)
{
  Reset ();

  new (&m_value) T (std::forward<T> (other));
  m_initialized = true;

  return *this;
}

template <typename T>
template <class... Args>
void
Optional<T>::Emplace (Args &&...args)
{
  Reset ();

  new (&m_value) T (std::forward<Args> (args)...);
  m_initialized = true;
}

template <typename T>
template <typename... Args>
Optional<T>::Optional (Args &&...args) : m_initialized{true}
{
  new (&m_value) T (std::forward<Args> (args)...);
}

template <typename T>
Optional<T>::~Optional ()
{
  Reset ();
}

template <typename T>
Optional<T>::operator bool (void) const
{
  return HasValue ();
}

template <typename T>
bool
Optional<T>::HasValue (void) const
{
  return m_initialized;
}

template <typename T>
const T &
Optional<T>::GetValue (void) const
{
  NS_ABORT_MSG_UNLESS (m_initialized, "Attempted to access uninitialized Optional");
  return *reinterpret_cast<const T *> (&m_value);
}
template <typename T>
const T &
Optional<T>::operator* (void) const
{
  return GetValue ();
}

template <typename T>
template <typename U>
T
Optional<T>::ValueOr (U &&defaultValue) const
{
  return m_initialized ? GetValue () : static_cast<T> (std::forward<U> (defaultValue));
}

template <typename T>
void
Optional<T>::Reset ()
{
  if (!m_initialized)
    return;

  // Call T's destructor
  reinterpret_cast<T *> (&m_value)->~T ();

  // Just in case this gets called multiple times
  m_initialized = false;
}

// ----- ATTRIBUTE -----

template <typename T>
class OptionalValue : public AttributeValue
{
  friend Optional<T>;

public:
  OptionalValue (void) = default;
  OptionalValue (const OptionalValue<T> &other) = default;
  OptionalValue (const Optional<T> &value);

  template <typename... Args>
  explicit OptionalValue (Args &&...args);

  void Set (const Optional<T> &value);

  template <typename... Args>
  void Emplace (Args &&...args);

  const Optional<T> &Get (void) const;
  const T &GetValue (void) const;

  explicit operator bool (void) const;

  template <class V>
  bool
  GetAccessor (V &value) const
  {
    value = V (m_value);
    return true;
  }
  Ptr<AttributeValue> Copy (void) const override;
  std::string SerializeToString (Ptr<const AttributeChecker> checker) const override;
  bool DeserializeFromString (std::string value, Ptr<const AttributeChecker> checker) override;

private:
  Optional<T> m_value;
};

template <typename T>
OptionalValue<T>::OptionalValue (const Optional<T> &value) : m_value{value}
{
}

template <typename T>
template <class... Args>
OptionalValue<T>::OptionalValue (Args &&...args) : m_value{std::forward<Args> (args)...}
{
}

template <typename T>
void
OptionalValue<T>::Set (const Optional<T> &value)
{
  m_value = value;
}

template <typename T>
template <typename... Args>
void
OptionalValue<T>::Emplace (Args &&...args)
{
  m_value.template Emplace (args...);
}

template <typename T>
const Optional<T> &
OptionalValue<T>::Get (void) const
{
  return m_value;
}

template <typename T>
const T &
OptionalValue<T>::GetValue (void) const
{
  return m_value.GetValue ();
}

template <typename T>
OptionalValue<T>::operator bool (void) const
{
  return m_value.HasValue ();
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
  // TODO: Not really sure I want to add this restriction for all types. How necessary is it?
  NS_ABORT_MSG ("Unsupported Operation 'SerializeToString()'");
  return {};
}

template <typename T>
bool
OptionalValue<T>::DeserializeFromString (std::string value, Ptr<const AttributeChecker> checker)
{
  // TODO: Same as `SerializeToString`, but more so
  NS_ABORT_MSG ("Unsupported Operation 'DeserializeFromString()'");
  return false;
}

// ----- ATTRIBUTE ACCESSOR -----

template <typename T0, typename T1>
Ptr<const AttributeAccessor>
MakeOptionalAccessor (T1 a1)
{
  return MakeAccessorHelper<OptionalValue<T0>> (a1);
}

template <typename T0, typename T1, typename T2>
Ptr<const AttributeAccessor>
MakeOptionalAccessor (T1 setter, T2 getter)
{
  return MakeAccessorHelper<OptionalValue<T0>> (setter, getter);
}

// ----- ATTRIBUTE CHECKER -----

template <typename T>
class OptionalChecker : public AttributeChecker
{
public:
  bool
  Check (const AttributeValue &value) const override
  {
    return dynamic_cast<const OptionalValue<T> *> (&value) != nullptr;
  }

  std::string
  GetValueTypeName (void) const override
  {
    return "ns3::visualizer3d::OptionalValue";
  }

  bool
  HasUnderlyingTypeInformation (void) const override
  {
    return false;
  }

  std::string
  GetUnderlyingTypeInformation (void) const override
  {
    // Best possible answer I can give here
    // Probably will not produce a readable name
    // under GCC or Clang
    return std::string{"ns3::visualizer3d::Optional<"} + typeid (T).name () + ">";
  }

  Ptr<AttributeValue>
  Create (void) const override
  {
    // Qualify with the namespace since the
    // function names are the same...
    return ns3::Create<OptionalValue<T>> ();
  }

  bool
  Copy (const AttributeValue &source, AttributeValue &destination) const override
  {
    const auto *src = dynamic_cast<const OptionalValue<T> *> (&source);
    auto *dst = dynamic_cast<OptionalValue<T> *> (&destination);

    if (src == nullptr || dst == nullptr)
      return false;

    *dst = *src;
    return true;
  }
};

template <typename T>
Ptr<const AttributeChecker>
MakeOptionalChecker (void)
{
  return Create<OptionalChecker<T>> ();
}

// TODO: Bounded Checker for Number types

} // namespace visualizer3d
} // namespace ns3

#endif // OPTIONAL_H
