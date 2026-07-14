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
 * Author: Andrew Wagger <andrew.wagger@nist.gov>
 */

#ifndef SERIES_WRAPPER_COLLECTION_H
#define SERIES_WRAPPER_COLLECTION_H

#include "color.h"
#include "orchestrator.h"
#include "series-collection.h"
#include "series-wrapper.h"
#include "xy-series.h"

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/ptr.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

/**
 * @ingroup netsimulyzer
 * Keeps track of multiple SeriesWrappers in a single collection
 */
class SeriesWrapperCollection : public Object
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    /**
     * Append the wrapper as the next in the Wrapper list
     *
     * @return
     * self
     */
    Ptr<SeriesWrapperCollection> AddWrapper(Ptr<SeriesWrapper> w);

    /**
     * Shortcut to get a Series from a Wrapper
     *
     * @param i
     * Series index
     */
    Ptr<XYSeries> GetSeries(std::size_t i);

    /**
     * Get a Wrapper from the container
     * @param i
     * Wrapper index
     */
    Ptr<SeriesWrapper> GetWrapper(std::size_t i);

    /**
     * Get a Wrapper from the container as a specific class
     * @param i
     * Wrapper index
     */
    template <class T>
    Ptr<T> GetWrapperAs(std::size_t i)
    {
        return m_wrappers.at(i)->GetObject<T>();
    };

    /**
     * Get the number of Wrappers in the collection
     */
    std::size_t GetNSeries();
    /**
     * Get the underlying SeriesCollection
     */
    Ptr<SeriesCollection> GetSeriesCollection();

    /**
     * Makes a collection from a Orchestrator
     * @param orchestrator
     * The Orchestrator for the Collection
     */
    explicit SeriesWrapperCollection(Ptr<Orchestrator> orchestrator);
    /**
     * Automatically initializes name, x-axis, y-axis in the SeriesCollection
     *
     * @param orchestrator
     * The Orchestrator for the Collection
     * @param name
     * The name to give to the SeriesCollection
     * @param x_axis
     * The label for the x-axis of the SeriesCollection
     * @param y_axis
     * The label for the y-axis of the SeriesCollection
     */
    SeriesWrapperCollection(Ptr<Orchestrator> orchestrator,
                            std::string name,
                            std::string x_axis,
                            std::string y_axis);

    /**
     * returns a constant reference to the internal vector of Wrappers
     */
    const std::vector<Ptr<SeriesWrapper>>& Wrappers();

    /**
     * Gets a reference to a particular seriesWrapper
     */
    SeriesWrapper& operator[](std::size_t i);

    /**
     * Casts itself as a particular pointer
     */
    template <class T>
    Ptr<T> As()
    {
        return (dynamic_cast<T*>(this));
    };

  private:
    /**
     * Vector of the wrappers it contains
     */
    std::vector<Ptr<SeriesWrapper>> m_wrappers;

    /**
     * The SeriesCollection being wrapped
     */
    Ptr<SeriesCollection> m_collection;
};

/**
 * Adds a String map over the Container
 */
class SeriesWrapperMap : public SeriesWrapperCollection
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SeriesWrapperMap(Ptr<Orchestrator> orchestrator);
    SeriesWrapperMap(Ptr<Orchestrator> orchestrator,
                     std::string name,
                     std::string x_axis,
                     std::string y_axis);
    /**
     * Adds a Wrapper to the specified index
     * @param index
     * String index to insert the Wrapper at
     */
    Ptr<SeriesWrapperMap> AddWrapper(std::string index, Ptr<SeriesWrapper> w);

    /**
     * Shortcut for getting a Series from a Wrapper
     * @param index
     * String index of the Wrapper to get the series of
     */
    Ptr<XYSeries> GetSeries(std::string index);

    /**
     * Get a Wrapper from the container
     * @param index
     * String index of the Wrapper to get
     */
    Ptr<SeriesWrapper> GetWrapper(const std::string& index);

    /**
     * Get a Wrapper from the container as a specific cast
     * @param index
     * String index of the Wrapper to get
     */
    template <class T>
    Ptr<T> GetWrapperAs(const std::string& index)
    {
        return SeriesWrapperCollection::GetWrapperAs<T>(m_nameMap.at(index));
    };

    SeriesWrapper& operator[](const std::string& index);

  private:
    std::unordered_map<std::string, std::size_t> m_nameMap;
};

} // namespace ns3::netsimulyzer

#endif // SERIES_WRAPPER_COLLECTION_H
