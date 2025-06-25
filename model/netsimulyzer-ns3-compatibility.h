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

#ifndef NETSIMULYZER_NS3_COMPATIBILITY_H
#define NETSIMULYZER_NS3_COMPATIBILITY_H

#include "ns3/core-module.h"

#ifndef NETSIMULYZER_NS3_VERSION
static_assert(
    false,
    "NetSimulyzer: Failed to detect ns-3 version, please specify the `NETSIMULYZER_NS3_VERSION` "
    "environment variable in the form: 'ns-3.xx' e.g 'ns-3.45'");
#endif

namespace ns3::netsimulyzer
{
constexpr auto DEPRECATED_SUPPORT =
#if NETSIMULYZER_NS3_VERSION < 46
    TypeId::SupportLevel::DEPRECATED;
#else
    TypeId::DEPRECATED;
#endif

template <typename T, typename SetT, typename GetT>
Ptr<const AttributeAccessor>
MakeEnumAccessorCompat(SetT set, GetT get)
{
#if NETSIMULYZER_NS3_VERSION >= 41
    return MakeEnumAccessor<T>(set, get);
#else
    return MakeEnumAccessor(set, get);
#endif
}

template <typename T, typename ValueT>
Ptr<const AttributeAccessor>
MakeEnumAccessorCompat(ValueT value)
{
#if NETSIMULYZER_NS3_VERSION >= 41
    return MakeEnumAccessor<T>(value);
#else
    return MakeEnumAccessor(value);
#endif
}

template <typename T>
#if NETSIMULYZER_NS3_VERSION >= 41
EnumValue<T>
#else
EnumValue
#endif
MakeEnumValueCompat()
{
#if NETSIMULYZER_NS3_VERSION >= 41
    return EnumValue<T>{};
#else
    return EnumValue{};
#endif
}

} // namespace ns3::netsimulyzer

#endif // NETSIMULYZER_NS3_COMPATIBILITY_H
