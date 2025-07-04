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

#ifndef CATEGORY_AXIS_H
#define CATEGORY_AXIS_H

#include "ns3/object.h"
#include "ns3/ptr.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

class CategoryAxis : public ns3::Object
{
  public:
    struct ValuePair
    {
        int key;
        std::string value;

        /**
         * Compatability constructor for models with states
         * represeneted by a scoped enum (e.g. `enum class`/`enum struct`)
         *
         * @tparam T (deduced)
         * The type of the scoped enum
         *
         * @param key
         * The scoped enum value
         *
         * @param value
         * The string repesenetation of the scoped enum value in `key`
         */
        template <class T>
        ValuePair(const T key, const std::string& value)
            : key{static_cast<int>(key)},
              value{value}
        {
        }

        /**
         * Constructor for unscoped enums (e.g. `enum` without `class`/`struct`)
         *
         * @param key
         * The enum value
         *
         * @param value
         * The string representation of the enum value in `key`
         */
        ValuePair(const int key, const std::string& value)
            : key{key},
              value{value}
        {
        }

        ValuePair() = default;
    };

    /**
     * @brief Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * Construct an axis with no categories
     */
    CategoryAxis(void) = default;

    /**
     * Construct an axis with categories from `values`.
     * Automatically assigns IDs to each provided value.
     *
     * More values may be safely added with `AddValue`
     *
     * @param values
     * The initial categories to have on the axis
     */
    explicit CategoryAxis(const std::vector<std::string>& values);

    /**
     * Construct an axis with categories from `values`
     * with pre-assigned ID's.
     *
     * More values may be safely added with `AddValue`
     *
     * @warning No two categories may have the same ID.
     * This rule is for categories added through both
     * the `ValuePair` constructor and `AddValue (ValuePair)`
     *
     * @param values
     * The initial categories to have on the axis
     */
    explicit CategoryAxis(const std::vector<ValuePair>& values);

    /**
     * Adds a category with an unspecified ID
     *
     * @warning Do not duplicate category names,
     * if two categories share the same name IDs
     * must be used to interact with the conflicting names.
     *
     * @param value
     * A new category to add to the axis
     */
    void AddValue(const std::string& value);

    /**
     * Adds a category with a pre-assigned ID
     *
     * @warning No two categories may have the same ID.
     * This rule is for categories added through both
     * the `ValuePair` constructor and `AddValue (ValuePair)`
     *
     * @param value
     * The category and it's ID to add to the axis.
     */
    void AddValue(const ValuePair& value);

    /**
     * Gives the ID, category mapping used by the axis
     *
     * @return
     * The internal map containing all user added categories
     */
    const std::unordered_map<int, std::string>& GetValues(void) const;

    /**
     * Looks up the ID of a given category.
     * If the category is not found, abort()'s
     *
     * @warning If two categories share the same name
     * the result of this function is undefined
     *
     * @param name
     * The category value, case sensitive
     *
     * @return
     * The ID of the requested category
     */
    int GetId(const std::string& name) const;

    /**
     * Retrieves the name & ID of a category.
     * Will throw std::out_of_range if the category is not found
     *
     * @throw std::out_of_range
     * If `id` is not on the axis
     *
     * @param id
     * The ID of the category to find
     *
     * @return
     * The name & ID of the category
     */
    ValuePair Get(int id);

    /**
     * Retrieves the name & ID of a category.
     * Will abort if the category is not found
     *
     * @param name
     * The name of the category
     *
     * @return
     * The name & ID of the category
     */
    ValuePair Get(const std::string& name);

  private:
    /**
     * Name for the axis used in visualizer elements
     */
    std::string m_name;

    /**
     * Possible values
     */
    std::unordered_map<int, std::string> m_values;

    /**
     * Next ID to assign to values not provided with one.
     * Use `int` since most enums are backed by ints
     */
    int m_nextId{1};
};

} // namespace ns3::netsimulyzer

#endif /* CATEGORY_AXIS_H */
