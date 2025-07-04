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

#ifndef BUILDING_CONFIGURATION_CONTAINER_H
#define BUILDING_CONFIGURATION_CONTAINER_H

#include "ns3/building-configuration.h"
#include "ns3/ptr.h"

#include <cstdint>
#include <vector>

namespace ns3::netsimulyzer
{

class BuildingConfigurationContainer
{
  public:
    using Iterator = std::vector<Ptr<BuildingConfiguration>>::const_iterator;

    /**
     * Creates an empty BuildingConfigurationContainer
     */
    BuildingConfigurationContainer(void);

    /**
     * Construct a BuildingConfigurationContainer containing configuration
     *
     * Allows implicit conversion from a Ptr to a BuildingConfiguration
     *
     * @param configuration A BuildingConfiguration object to store in the resulting container
     */
    BuildingConfigurationContainer(Ptr<BuildingConfiguration> configuration);

    /**
     * Creates a new BuildingConfigurationContainer by concatenating first and second
     *
     * @param first The container whose elements will appear first
     *
     * @param second The container whose elements will appear after
     * all the elements in 'first'
     */
    BuildingConfigurationContainer(const BuildingConfigurationContainer& first,
                                   const BuildingConfigurationContainer& second);

    /**
     * Returns the iterator to the beginning of the contained vector
     *
     * See the vector rules for iterator invalidation
     *
     * @return A const_iterator to the beginning of the collection
     */
    Iterator Begin(void);

    /**
     * Returns the iterator past the end of the contained vector
     *
     * See the vector rules for iterator invalidation
     *
     * @return  A past-the-end const_iterator for the contained collection
     */
    Iterator End(void);

    /**
     * Returns the size of the collection
     *
     * @return The number of elements in the contained collection
     */
    uint32_t GetN(void);

    /**
     * Returns the element at index i
     *
     * If index i is not within the contained vector,
     * then the behavior is undefined
     *
     * @param i The index of the item to return
     *
     * @return The Ptr at index i
     */
    Ptr<BuildingConfiguration> Get(uint32_t i);

    /**
     * Appends the contents of container c to this one by copy.
     *
     * @param c The container to append the contents of
     */
    void Add(const BuildingConfigurationContainer& c);

    /**
     * Appends a single BuildingConfiguration to the collection.
     *
     * @param configuration The BuildingConfiguration to append.
     * May not be NULL
     */
    void Add(Ptr<BuildingConfiguration> configuration);

    /**
     * Remove all elements from the container
     */
    void Clear(void);

  private:
    std::vector<Ptr<BuildingConfiguration>> m_configurationObjects;
};

// Free 'begin' and 'end' functions allow for range based for loops e.g.:
//
// for (auto ptr: container)
//   {
//     // No need to deference an iterator for the pointer
//     ptr->MemberOfNodeConfiguration
//   }
//
// They MUST reside with the same namespace as the collection itself
// otherwise the ADL will fail

/**
 * Returns the iterator to the beginning of the collection
 *
 * @param c The collection the resulting iterator will reference
 *
 * @return A const_iterator to the beginning of c
 */
BuildingConfigurationContainer::Iterator begin(BuildingConfigurationContainer& c);

/**
 * Returns the  past-the-end iterator for the collection
 *
 * @param c The collection the resulting iterator will reference
 *
 * @return A const_iterator past-the-end of c
 */
BuildingConfigurationContainer::Iterator end(BuildingConfigurationContainer& c);

} // namespace ns3::netsimulyzer

#endif /* BUILDING_CONFIGURATION_CONTAINER_H */
