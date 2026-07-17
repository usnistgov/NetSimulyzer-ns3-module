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

#ifndef LOGICAL_LINK_PATHS_H
#define LOGICAL_LINK_PATHS_H

#include "../model/orchestrator.h"
#include "logical-link-helper.h"

#include "ns3/log.h"
#include "ns3/object.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

/**
 * @ingroup netsimulyzer
 * Manages LogicalLinks for multiple paths of links
 */
class LogicalLinkPaths : public Object
{
  public:
    LogicalLinkPaths(Ptr<Orchestrator> orchestrator);

    /**
     * Gets the number of currently created paths
     */
    std::size_t GetNPaths();
    /**
     * Appends a blank path to the list of paths
     */
    std::size_t AddPath();
    /**
     * Appends a path to the list of paths
     * @param path
     * vector of node Ids that represent the path
     */
    std::size_t AddPath(std::vector<uint32_t> path);

    /**
     * Appends a path to the list of paths
     * @param path
     * vector of node Ids that represent the path
     * @param color
     * Color to make all the links in the path
     */
    std::size_t AddPath(std::vector<uint32_t> path, Color3 color);

    /**
     * Appends a path to the list of paths
     * @param path
     * vector of node Ids that represent the path
     * @param color
     * Color to make all the links in the path
     * @param attributes
     * Attributes to be given to all links in the path
     */
    std::size_t AddPath(std::vector<uint32_t> path,
                        Color3 color,
                        const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes);

    /**
     *  Automatically manages the links given a path. Will reuse links of set again
     * @param i
     * index of the app path to set
     * @param path
     * vector of node Ids that represent the path the app is taking
     */
    void SetPath(std::size_t i, std::vector<uint32_t> path);
    /**
     * Automatically manages the links given a path. Will reuse links of set again
     * @param i
     * Index of the app path to set
     * @param path
     * Vector of node Ids that represent the path the app is taking
     * @param color
     * Color to make all the links in the path
     */
    void SetPath(std::size_t i, std::vector<uint32_t> path, Color3 color);
    /**
     * Automatically manages the links given a path. Will reuse links of set again
     * @param i
     * Index of the app path to set
     * @param path
     * Vector of node Ids that represent the path the app is taking
     * @param color
     * Color to make all the links in the path
     * @param attributes
     * Attributes to be given to all links in the path
     */
    void SetPath(std::size_t i,
                 std::vector<uint32_t> path,
                 Color3 color,
                 const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes);

  private:
    /**
     * Gets the next color for a path without a color set
     */
    Color3 NextPathColor();
    /**
     * A vector of vectors containing the LogicalLinks for all paths. vectors may be longer than
     * needed
     */
    std::vector<std::vector<Ptr<netsimulyzer::LogicalLink>>> m_paths;

    /**
     * The Orchestrator the LogicalLinks are attatched to
     */
    Ptr<Orchestrator> m_orchestrator;

    /**
     * Helper to edit logical links
     */
    LogicalLinkHelper m_helper;
};

} // namespace ns3::netsimulyzer

#endif // LOGICAL_LINK_PATHS_H
