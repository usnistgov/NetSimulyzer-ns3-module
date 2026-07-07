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

#ifndef NET_VISUALIZER_H
#define NET_VISUALIZER_H

#include "../helper/node-configuration-container.h"
#include "../helper/node-configuration-helper.h"
#include "color.h"
#include "netsimulyzer-3D-models.h"
#include "node-configuration.h"
#include "orchestrator.h"
#include "series-collection.h"
#include "xy-series.h"

#include "ns3/log.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/timer.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{


class SeriesManager;

/**
 * Base class to ensure different structs that wrap a series with context can work together in the
 * SeriesWrapperCollection
 */
class SeriesWrapper : public Object
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    /**
     * Gets the wrapped XYSeires
     */
    Ptr<XYSeries> GetSeries();

    /**
     * Makes a SeriesWrapper from a series
     * @param series
     * The series to wrap
     */
    explicit SeriesWrapper(Ptr<XYSeries> series);

    /**
     * Casts itself as a particular pointer
     */
    template <class T>
    T* As()
    {
        return (dynamic_cast<T*>(this));
    };

    /**
     * Shorthand for GetSeries()
     */
    Ptr<XYSeries> operator()();

  private:
    /**
     * The series being wrapped
     */
    Ptr<XYSeries> m_series;
};

/**
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
    SeriesWrapperCollection* AddWrapper(Ptr<SeriesWrapper> w);

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
     * Makes a collection from a SeriesManager
     * @param manager
     * The SeriesManager to get the Orchestrator from
     */
    explicit SeriesWrapperCollection(Ptr<SeriesManager> manager);
    /**
     * Automatically initializes name, x-axis, y-axis in the SeriesCollection
     *
     * @param manager
     * The SeriesManager to get the Orchestrator from
     * @param name
     * The name to give to the SeriesCollection
     * @param x_axis
     * The label for the x-axis of the SeriesCollection
     * @param y_axis
     * The label for the y-axis of the SeriesCollection
     */
    SeriesWrapperCollection(Ptr<SeriesManager> manager,
                    std::string name,
                    std::string x_axis,
                    std::string y_axis);

    /**
     * Returns an iterator over the SeriesWrappers
     */
    std::vector<Ptr<SeriesWrapper>>::iterator begin();

    /**
     * Returns an iterator over the SeriesWrappers
     */
    std::vector<Ptr<SeriesWrapper>>::iterator end();

    /**
     * Gets a reference to a particular seriesWrapper
     */
    SeriesWrapper& operator[](std::size_t i);

    /**
     * Casts itself as a particular pointer
     */
    template <class T>
    T* As()
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
 * State-holder for the entire Netsimulyzer wrapper
 * Maintains a map of different eriesCollections to display, as well as NodeConfigs and the Nodes
 * themselves
 */
class SeriesManager : public Object
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    /**
     * Creates a SeriesManager outputting to a particular file
     * @param outputFileName
     * the file to output to
     */
    explicit SeriesManager(std::string outputFileName);
    /**
     * Automatically initializes NodeContainer
     *
     * @param outputFileName
     * the file to output to
     * @param nodes
     * The ns3 Nodes to add to the visualization
     */
    SeriesManager(std::string outputFileName, NodeContainer nodes);
    /**
     * Initializes NodeContainer
     * @param nodes
     * The nodeContainer to set
     */
    void SetNodes(NodeContainer nodes);

    /**
     * Gets a Container from the SeriesManager
     *
     * @param index
     * The string index of the Collection to get
     */
    Ptr<SeriesWrapperCollection> GetCollection(std::string index);

    /**
     * Gets a Container from the SeriesManager as a specific cast
     *
     * @param index
     * The string index of the Collection to get
     */
    template <class T>
    Ptr<T> GetCollectionAs(std::string index)
    {
        return m_collections.at(index)->GetObject<T>();
    };

    /**
     * Gets a reference to a particular SeriesWrapperCollection
     */
    SeriesWrapperCollection& operator[](std::string index);

    /**
     * Sets a new Container in the SeriesManager's map
     *
     * @param index
     * String index to put the new Container
     * @return
     * self
     */
    SeriesManager* SetContainer(std::string index, Ptr<SeriesWrapperCollection> container);

    /**
     * Gets a nodeConfiguration for a specific Node
     * @param i
     * The index of the NodeConfiguration to get
     */
    Ptr<NodeConfiguration> GetConfig(std::size_t i);

    /**
     * Gets the NodeonfigurationHelper
     */
    NodeConfigurationHelper* ConfigHelper();

    /**
     *  Gets a specific Node
     *
     * @param i
     * The index of the Node to get
     */
    Ptr<Node> GetNode(std::size_t i);

    /**
     *  Gets a specific Node by its internal Id
     *
     * @param id
     * The id of the Node to get
     */
    Ptr<Node> GetNodeById(uint32_t id);

    /**
     * Gets the number of nodes in the visualization
     */
    std::size_t GetNNodes();

    /**
     * Gets the Orchestrator for the SeriesManager
     */
    Ptr<Orchestrator> GetOrchestrator() const;

        /**
     * Returns an iterator over the SeriesWrappers
     */
    std::unordered_map<std::string, Ptr<SeriesWrapperCollection>>::iterator begin();

    /**
     * Returns an iterator over the SeriesWrappers
     */
    std::unordered_map<std::string, Ptr<SeriesWrapperCollection>>::iterator end();

  private:
    /**
     * The orchestrator shared among all objects in the SeriesManager
     */
    Ptr<Orchestrator> m_orchestrator;

    /**
     * The container for each node's configuration
     */
    NodeConfigurationContainer m_configContainer;

    /**
     * The helper that manages the defaults of the node's configurations
     */
    NodeConfigurationHelper m_configHelper;

    /**
     * The nodes to keep track of for the visualizations
     */
    NodeContainer m_nodes;

    /**
     * The underlying data structure for the SeriesWrapperCollections
     */
    std::unordered_map<std::string, Ptr<SeriesWrapperCollection>> m_collections;
};

/**
 * Appends an accumulating value to the series
 */
class AccumulatorWrapper : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit AccumulatorWrapper(Ptr<XYSeries> series);
    /**
     * Accumulated the value then Appends to the Series
     *
     * @param time
     * The Time the value will be associated with
     * @param add
     * The amount to accumulate by
     */
    void Update(Time time, double add);

    /**
     * Accumulated the value then Appends to the Series at the current time
     *
     * @param add
     * The amount to accumulate by
     */
    void Update(double add);

  private:
    double m_value = 0;
};

/**
 * Keeps track of a rolling arithmetic average of the given values
 */
class AverageValueWrapper : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit AverageValueWrapper(Ptr<XYSeries> series);

    /**
     * Accumulates the average then Appends to the Series
     *
     * @param time
     * The Time to set the new average
     * @param value
     * The amount to add to the average
     */
    void Update(Time time, double value);

    /**
     * Accumulates the average then Appends to the Series
     *
     * @param value
     * The amount to add to the average
     */
    void Update(double value);

  private:
    double m_avg = 0;
    uint32_t m_n = 0;
};

/**
 * Keeps track of a sliding window for an accumulating value with a bandwidth
 */
class SlidingValueWrapper : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SlidingValueWrapper(Ptr<XYSeries> series);

    SlidingValueWrapper(Ptr<XYSeries> series, Time interval);
    /**
     * @param window
     * Length of the sliding window in Seconds
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingValueWrapper(Ptr<XYSeries> series, Time window, double max_sample_frequency);

    /**
     * @param window
     * Length of the sliding window
     * @param interval
     * Sets how often the Series should be Appended to
     */
    SlidingValueWrapper(Ptr<XYSeries> series, Time window, Time interval);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param time
     * The time that the value is inserted in
     * @param value
     * The value to be inserted into the sliding value
     */
    void Update(Time time, double value);
    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param value
     * The value to be inserted into the sliding value
     */
    void Update(double value);

    void Flush();
    void Append(Time time);

    /**
     * Gets the value of the current window
     */
    double GetSlidingValue() const;

  private:
    std::vector<std::pair<Time, double>> m_values;
    Time m_window = Seconds(1);
    double m_maxSampleFrequency = 0.1;
    double m_lastSample = 0;
    Timer m_timer;
    bool m_timed = false;
};

/**
 * Keeps track of a sliding window for an accumulating value with a bandwidth
 */
class SlidingLoadWrapper : public SlidingValueWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SlidingLoadWrapper(Ptr<XYSeries> series);

    SlidingLoadWrapper(Ptr<XYSeries> series, Time interval);
    /**
     * @param window
     * Length of the sliding window
     * @param bandwidth
     * Bandwith to compare accumulated value to
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingLoadWrapper(Ptr<XYSeries> series, Time window, double bandwidth, double max_sample_frequency);

    /**
     * @param window
     * Length of the sliding window
     * @param bandwidth
     * Bandwith to compare accumulated value to
     * @param interval
     * Sets how often the Series should be Appended to
     */
    SlidingLoadWrapper(Ptr<XYSeries> series, Time window, double bandwidth, Time interval);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param time
     * The time that the value is inserted in
     * @param value
     * The value to be inserted into the sliding data
     */
    void Update(Time time, double value);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param value
     * The value to be inserted into the sliding data
     */
    void Update(double value);

    /**
     * Gets the value of the current window
     */
    double GetSlidingValue() const;

  private:
    std::vector<std::pair<Time, double>> m_values;
    double m_bandwidth = 100.0;
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

    explicit SeriesWrapperMap(Ptr<SeriesManager> manager);
    SeriesWrapperMap(Ptr<SeriesManager> manager, std::string name, std::string x_axis, std::string y_axis);
    /**
     * Adds a Wrapper to the specified index
     * @param index
     * String index to insert the Wrapper at
     */
    SeriesWrapperMap* AddWrapper(std::string index, Ptr<SeriesWrapper> w);

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

#endif /*NET_VISUALIZER_H*/
