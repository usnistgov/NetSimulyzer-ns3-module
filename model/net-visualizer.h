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

#include "xy-series.h"
#include "series-collection.h"
#include "node-configuration.h"
#include "orchestrator.h"
#include "color.h"
#include "../helper/node-configuration-container.h"
#include "../helper/node-configuration-helper.h"

#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/nstime.h"



#include <string>
#include <unordered_map>
#include <vector>

#ifndef NET_VISUALIZER_H
#define NET_VISUALIZER_H

using namespace ns3;

namespace ns3::netsimulyzer {


namespace visualizer {

// Declare it up here to make everyone happy :)
class Visualizer;


// Base class to ensure different structs that wrap a series with context can work together in the SeriesContainer
struct SeriesWrapper : Object {
    public:
        /**
         * @brief Get the class TypeId
         *
         * @return the TypeId
         */
        static TypeId GetTypeId(void);

        /**
         @brief Gets the wrapped XYSeires        
        */
        Ptr<netsimulyzer::XYSeries> GetSeries(void);


        SeriesWrapper(Ptr<netsimulyzer::XYSeries> series);
    private:
        Ptr<netsimulyzer::XYSeries> m_series;
};

// Keeps track of multiple SeriesWrappers in a single collection
struct SeriesContainer : Object {
    public:

        /**
         * @brief Get the class TypeId
         *
         * @return the TypeId
         */
        static TypeId GetTypeId(void);

        /**
        @brief Append the wrapper as the next in the Wrapper list

        @return 
        self
        */
        SeriesContainer* AddWrapper(Ptr<SeriesWrapper> w);

        /**
        * @brief Shortcut to get a Series from a Wrapper
        * 
        * @param i
        * Series index
        */
        Ptr<netsimulyzer::XYSeries> GetSeries(uint32_t i);
        /**
        @brief Get a Wrapper from the container
        * @param i
        * Wrapper index
        */
        Ptr<SeriesWrapper> GetWrapper(uint32_t i);
        /**
        @brief Get the number of Wrappers in the collection
        */
        uint32_t GetNSeries(void);
        /**
        @brief Get the underlying SeriesCollection
        */
        Ptr<netsimulyzer::SeriesCollection> GetCollection(void);
        SeriesContainer(Ptr<Visualizer> visualizer);
        /**
        @brief Automatically initializes name, x-axis, y-axis in the SeriesCollection

        @param name
        The name to give to the SeriesCollection
        @param x_axis
        The label for the x-axis of the SeriesCollection
        @param y_axis
        The label for the y-axis of the SeriesCollection
        */
        SeriesContainer(Ptr<Visualizer> visualizer, std::string name, std::string x_axis, std::string y_axis);
    private:
        std::vector<Ptr<SeriesWrapper>> m_wrapper;
        Ptr<netsimulyzer::SeriesCollection> m_collection;
};

// State-holder for the entire Netsimulyzer wrapper
// Maintains a map of different eriesCollections to display, as well as NodeConfigs and the Nodes themselves
class Visualizer : public Object {
    public:

        /**
         * @brief Get the class TypeId
         *
         * @return the TypeId
         */
        static TypeId GetTypeId(void);

        Visualizer(std::string outputFileName);
        /**
        @brief Automatically initializes NodeContainer

        @param nodes
        The ns3 Nodes to add to the visualization
        */
        Visualizer(std::string outputFileName, NodeContainer nodes);
        /**
        @brief Initializes NodeContainer
        */
        void SetNodes(NodeContainer nodes);
        /**
        @brief Gets a Container from the Visualizer

        @param index
        The string index of the Collection to get
         */
        Ptr<SeriesContainer> GetContainer(std::string index);
        /**
        @brief Sets a new Container in the Visualizer's map

        @param index
        String index to put the new Container
        @return 
        self
        */
        Visualizer* SetContainer(std::string index,Ptr<SeriesContainer>);

        /**
        @brief Gets a nodeConfiguration for a specific Node

        @param i
        The index of the NodeConfiguration to get
         */
        Ptr<netsimulyzer::NodeConfiguration> GetConfig(uint32_t i);
        /**
        @brief Gets a color from the Visualyzer's color list
        @param i
        The index to query the color list. Will automatically wrap around
        */
        netsimulyzer::Color3 GetColor(uint32_t i);
        /**
        @brief  Gets a specific Node

        @param i
        The index of the Node to get
         */
        Ptr<Node> GetNode(uint32_t i);

         /**
        @brief  Gets a specific Node by its internal Id

        @param id
        The id of the Node to get
         */
        Ptr<Node> GetNodeById(uint32_t id);

        /**
         * @brief Gets the number of nodes in the visualization
         */
        uint32_t GetNNodes();

        /**
        @brief Gets the Orchestrator for the Visualyzer
        */
        Ptr<netsimulyzer::Orchestrator> GetOrchestrator(void);
        /**
        @brief Makes a blank XYSeries
        */
        Ptr<netsimulyzer::XYSeries>MakeSeries(void);
        /** 
            @brief Makes a named XYSeries
            @param name
            String to name the XYSeries

        */
        Ptr<netsimulyzer::XYSeries>MakeSeries(std::string name);
        /** 
            @brief Makes a named XYSeries with a particular color
            @param name
            String to name the XYSeries
            @param color
            The color of the XYSeries

        */
        Ptr<netsimulyzer::XYSeries>MakeSeries(std::string name,netsimulyzer::Color3 color);
        /** 
            @brief Makes a named XYSeries with a particular preset color
            @param name
            String to name the XYSeries
            @param col_index
            The index of the preset Visualyzer color of the XYSeries
        */
        Ptr<netsimulyzer::XYSeries>MakeSeries(std::string name,uint32_t col_index);
        /**
        @brief Prints some helpful(?) output to cout
         */
        void Dump(void);
        
    private:
        Ptr<netsimulyzer::Orchestrator> m_orchestrator;
        netsimulyzer::NodeConfigurationContainer m_configContainer;
        netsimulyzer::NodeConfigurationHelper m_configHelper;
        
        NodeContainer m_nodes;
        std::vector<netsimulyzer::Color3> m_colors = {
            netsimulyzer::RED,
            netsimulyzer::ORANGE,
            netsimulyzer::YELLOW,
            netsimulyzer::GREEN,
            netsimulyzer::BLUE,
            netsimulyzer::PURPLE,
            netsimulyzer::PINK,
            netsimulyzer::GRAY_30
        };
        std::unordered_map<std::string,Ptr<SeriesContainer>> m_containers;

};

// Appends an accumulating value to the series
struct Accumulator : SeriesWrapper {
    public:

        /**
         * @brief Get the class TypeId
         *
         * @return the TypeId
         */
        static TypeId GetTypeId(void);

        Accumulator(Ptr<netsimulyzer::XYSeries> series);
        /**
         @brief Accumulated the value then Appends to the Series

         @param add
         The amount to accumulate by
         */
        void Update(Time now, uint32_t add);
    private:
        uint32_t m_value = 0;
};

// Adds a String map over the Container
struct SeriesMap : SeriesContainer{
    public:

        /**
         * @brief Get the class TypeId
         *
         * @return the TypeId
         */
        static TypeId GetTypeId(void);
        
        SeriesMap(Ptr<Visualizer> visualizer);
        SeriesMap(Ptr<Visualizer> visualizer, std::string name, std::string x_axis, std::string y_axis);
        /**
         @brief Adds a Wrapper to the specified index
         @param index
         String index to insert the Wrapper at
         */
        SeriesMap* AddWrapper(std::string index, Ptr<SeriesWrapper> w);

         /**
        @brief Shortcut for getting a Series from a Wrapper
        * @param indec
        * String index of the Wrapper to get the series of
        */
        Ptr<netsimulyzer::XYSeries> GetSeries(std::string index);
        
         /**
        @brief Get a Wrapper from the container
        * @param indec
        * String index of the Wrapper to get
        */
        Ptr<SeriesWrapper> GetWrapper(std::string index);
    private:
        std::unordered_map<std::string,uint32_t> m_nameMap;
};

} //namespace visualizer

} //namespace netsimulyzer

#endif /*NET_VISUALIZER_H*/
