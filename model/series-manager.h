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

#ifndef SERIES_MANAGER_H
#define SERIES_MANAGER_H

#include "../helper/node-configuration-container.h"
#include "../helper/node-configuration-helper.h"
#include "color.h"
#include "netsimulyzer-3D-models.h"
#include "node-configuration.h"
#include "orchestrator.h"
#include "series-collection.h"
#include "series-wrapper-collection.h"
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

/**
 * @ingroup netsimulyzer
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
    Ptr<SeriesManager> SetContainer(std::string index, Ptr<SeriesWrapperCollection> container);

    /**
     * Gets a nodeConfiguration for a specific Node
     * @param i
     * The index of the NodeConfiguration to get
     */
    Ptr<NodeConfiguration> GetConfig(std::size_t i);

    /**
     * Gets the NodeonfigurationHelper
     */
    NodeConfigurationHelper& ConfigHelper();

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
     * returns a constant reference to the internal collection
     */
    const std::unordered_map<std::string, Ptr<SeriesWrapperCollection>>& Collections();

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

} // namespace ns3::netsimulyzer

#endif /*SERIES_MANAGER_H*/
