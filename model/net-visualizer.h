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

#include <string>
#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

namespace visualizer
{

class Visualizer;

/**
 * Base class to ensure different structs that wrap a series with context can work together in the
 * SeriesContainer
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

    explicit SeriesWrapper(Ptr<XYSeries> series);

  private:
    Ptr<XYSeries> m_series;
};

/**
 * Keeps track of multiple SeriesWrappers in a single collection
 */
class SeriesContainer : public Object
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
    SeriesContainer* AddWrapper(Ptr<SeriesWrapper> w);

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
     * Get the number of Wrappers in the collection
     */
    std::size_t GetNSeries();
    /**
     * Get the underlying SeriesCollection
     */
    Ptr<SeriesCollection> GetCollection();
    explicit SeriesContainer(Ptr<Visualizer> visualizer);
    /**
     * Automatically initializes name, x-axis, y-axis in the SeriesCollection
     *
     * @param name
     * The name to give to the SeriesCollection
     * @param x_axis
     * The label for the x-axis of the SeriesCollection
     * @param y_axis
     * The label for the y-axis of the SeriesCollection
     */
    SeriesContainer(Ptr<Visualizer> visualizer,
                    std::string name,
                    std::string x_axis,
                    std::string y_axis);

    std::vector<Ptr<SeriesWrapper>>::iterator begin();
    std::vector<Ptr<SeriesWrapper>>::iterator end();

  private:
    std::vector<Ptr<SeriesWrapper>> m_wrappers;
    Ptr<SeriesCollection> m_collection;
};

/**
 * State-holder for the entire Netsimulyzer wrapper
 * Maintains a map of different eriesCollections to display, as well as NodeConfigs and the Nodes
 * themselves
 */
class Visualizer : public Object
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit Visualizer(std::string outputFileName);
    /**
     * Automatically initializes NodeContainer
     *
     * @param nodes
     * The ns3 Nodes to add to the visualization
     */
    Visualizer(std::string outputFileName, NodeContainer nodes);
    /**
     * Initializes NodeContainer
     */
    void SetNodes(NodeContainer nodes);

    /**
     * Get the default model of the Visualizer Nodes
     *
     * @return The file name of the default model of the Visualizer Nodes
     */
    std::string GetDefaultModel();

    /**
     * Set the default model of the Visualizer Nodes
     * @param model
     * File name of the model
     */
    Visualizer* SetDefaultModel(std::string model);

    /**
     * Get the default scale of the Visualizer Nodes
     */
    double GetDefaultScale();

    /**
     * Set the default scale of the Visualizer Nodes
     * @param scale
     * scale to be set as default
     */
    Visualizer* SetDefaultScale(double scale);

    /**
     * Gets a Container from the Visualizer
     *
     * @param index
     * The string index of the Collection to get
     */
    Ptr<SeriesContainer> GetContainer(std::string index);
    /**
     * Sets a new Container in the Visualizer's map
     *
     * @param index
     * String index to put the new Container
     * @return
     * self
     */
    Visualizer* SetContainer(std::string index, Ptr<SeriesContainer> container);

    /**
     * Gets a nodeConfiguration for a specific Node
     * @param i
     * The index of the NodeConfiguration to get
     */
    Ptr<NodeConfiguration> GetConfig(std::size_t i);
    /**
     * Gets a color from the Visualizer's color list
     * @param i
     * The index to query the color list. Will automatically wrap around
     */
    [[nodiscard]] const Color3 GetColor(std::size_t i) const;
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
     * Gets the Orchestrator for the Visualizer
     */
    Ptr<Orchestrator> GetOrchestrator();
    /**
     * Makes a blank XYSeries
     */
    Ptr<XYSeries> MakeSeries();
    /**
     * Makes a named XYSeries
     * @param name
     * String to name the XYSeries
     */
    Ptr<XYSeries> MakeSeries(std::string name);
    /**
     * Makes a named XYSeries with a particular color
     *
     * @param name
     * String to name the XYSeries
     *
     * @param color
     * The color of the XYSeries
     */
    Ptr<XYSeries> MakeSeries(std::string name, Color3 color);
    /**
     * Makes a named XYSeries with a particular preset color
     *
     * @param name
     * String to name the XYSeries
     * @param col_index
     * The index of the preset Visualizer color of the XYSeries
     */
    Ptr<XYSeries> MakeSeries(std::string name, std::size_t col_index);
    /**
     * Prints some helpful(?) output to cout
     */
    void Dump();

  private:
    Ptr<Orchestrator> m_orchestrator;
    NodeConfigurationContainer m_configContainer;
    NodeConfigurationHelper m_configHelper;

    NodeContainer m_nodes;
    std::vector<Color3> m_colors = {RED,
                                                  ORANGE,
                                                  YELLOW,
                                                  GREEN,
                                                  BLUE,
                                                  PURPLE,
                                                  PINK,
                                                  GRAY_30};
    std::string m_model = models::SINGLE_BOARD_COMPUTER;
    double m_scale = 3.0;

    std::unordered_map<std::string, Ptr<SeriesContainer>> m_containers;
};

/**
 * Appends an accumulating value to the series
 */
class Accumulator : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit Accumulator(Ptr<XYSeries> series);
    /**
     * Accumulated the value then Appends to the Series
     *
     * @param add
     * The amount to accumulate by
     */
    void Update(Time now, double add);

  private:
    double m_value = 0;
};

/**
 * Keeps track of a rolling arithmetic average of the given values
 */
class AverageValue : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit AverageValue(Ptr<XYSeries> series);
    /**
     * Accumulates the average then Appends to the Series
     *
     * @param value
     * The amount to add to the average
     */
    void Update(Time now, double value);

  private:
    double m_avg = 0;
    uint32_t m_n = 0;
};

/**
 * Keeps track of a sliding window for an accumulating value with a bandwidth
 */
class SlidingValue : public SeriesWrapper
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SlidingValue(Ptr<XYSeries> series);
    /**
     * @param window
     * Length of the sliding window in Seconds
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingValue(Ptr<XYSeries> series, double window, double max_sample_frequency);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param now
     * The time that the value is inserted in
     * @param value
     * The value to be inserted into the sliding value
     */
    void Update(Time now, double value);
    /**
     * Gets the value of the current window
     */

    double GetSlidingValue();

  private:
    std::vector<std::pair<Time, double>> m_values;
    double m_window = 1;
    double m_maxSampleFrequency = 0.1;
    double m_lastSample = 0;
};

/**
 * Keeps track of a sliding window for an accumulating value with a bandwidth
 */
class SlidingLoad : public SlidingValue
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SlidingLoad(Ptr<XYSeries> series);
    /**
     * @param window
     * Length of the sliding window in Seconds
     * @param bandwidth
     * Bandwith to compare accumulated value to
     * @param max_sample_frequency
     * Caps how quickly in sucession the Series will be appended to in Seconds
     * Any more frequent calls will update the sliding value, but not append to the Series
     */
    SlidingLoad(Ptr<XYSeries> series,
                double window,
                double bandwidth,
                double max_sample_frequency);

    /**
     * Adds a new value for the sliding data and updates the Series
     *
     * @param now
     * The time that the value is inserted in
     * @param value
     * The value to be inserted into the sliding data
     */
    void Update(Time now, double value);
    /**
     * Gets the value of the current window
     */
    double GetSlidingValue();

  private:
    std::vector<std::pair<Time, double>> m_values;
    double m_bandwidth = 100;
};

/**
 * Adds a String map over the Container
 */
class SeriesMap : public SeriesContainer
{
  public:
    /**
     * Get the class TypeId
     *
     * @return the TypeId
     */
    static TypeId GetTypeId();

    explicit SeriesMap(Ptr<Visualizer> visualizer);
    SeriesMap(Ptr<Visualizer> visualizer, std::string name, std::string x_axis, std::string y_axis);
    /**
     * Adds a Wrapper to the specified index
     * @param index
     * String index to insert the Wrapper at
     */
    SeriesMap* AddWrapper(std::string index, Ptr<SeriesWrapper> w);

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

  private:
    std::unordered_map<std::string, std::size_t> m_nameMap;
};

} // namespace visualizer

} // namespace ns3::netsimulyzer

#endif /*NET_VISUALIZER_H*/
