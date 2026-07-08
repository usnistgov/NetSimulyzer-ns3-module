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

#ifndef LOGICAL_LINK_PAIRS_H
#define LOGICAL_LINK_PAIRS_H

#include "../model/orchestrator.h"

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/timer.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ns3::netsimulyzer
{

class LogicalLinkPairs : public Object
{
  public:
    LogicalLinkPairs(Ptr<Orchestrator> orchestrator, uint32_t nodeCount);
    /**
     * @brief Gets the L2 link between the two nodes if it exists
     * @param i,j
     * The nodes to check for a connection between. Identical if swapped
     */
    Ptr<netsimulyzer::LogicalLink> GetLink(uint32_t i, uint32_t j);

    /**
     * @brief Creates a node pair Link
     * @param i,j
     * The Nodes to connect. Identical if swapped
     */
    void SetLink(uint32_t i, uint32_t j);

    /**
     * @brief Creates/Sets the color of a node pair Link
     * @param i,j
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     */
    void SetLink(uint32_t i, uint32_t j, Color3 color);

    /**
     * @brief Creates/Sets the color/attributes of a node pair Link
     * @param i,j
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     * @param attributes
     * Attributes to be given to this link
     */
    void SetLink(uint32_t i,
                 uint32_t j,
                 Color3 color,
                 const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes);

    /**
     * @brief Creates of a timed node pair Link
     * @param i,j
     * The Nodes to connect. Identical if swapped
     * @param duration
     * The length the link should stay activated
     */
    void SetLinkBurst(uint32_t i, uint32_t j, Time duration);

    /**
     * @brief Creates/Sets the color of a timed node pair Link
     * @param i,j
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     * @param duration
     * The length the link should stay activated
     */
    void SetLinkBurst(uint32_t i, uint32_t j, Color3 color, Time duration);

    /**
     * @brief Creates/Sets the color/attributes of a timed node pair Link
     * @param i,j
     * The Nodes to connect. Identical if swapped
     * @param color
     * The color to make the link
     * @param attributes
     * Attributes to be given to this link
     * @param duration
     * The length the link should stay activated
     */
    void SetLinkBurst(uint32_t i,
                      uint32_t j,
                      Color3 color,
                      const std::unordered_map<std::string, Ptr<AttributeValue>>& attributes,
                      Time duration);

    /**
     * @brief If a L2 link exists between the two nodes, deactivate it
     * @param i,j
     * The nodes to check for a connection between. Identical if swapped
     */
    void RemoveLink(uint32_t i, uint32_t j);

  private:
    template <typename T>
    class PairMap
    {
      private:
        uint32_t m_n;
        uint32_t m_size;
        T* m_arr;

      public:
        PairMap(void)
            : m_n(0),
              m_size(0),
              m_arr(nullptr) {};

        PairMap(uint32_t n)
            : m_n(n)
        {
            this->m_size = n * (n - 1) / 2;
            this->m_arr = new T[this->m_size];
        };

        ~PairMap()
        {
            delete[] this->m_arr;
        };

        T& Get(uint32_t i, uint32_t j)
        {
            if (i > j)
            {
                return this->Get(j, i);
            }
            else if (i >= this->m_n || j >= this->m_n || i == j)
            {
                throw "oob";
            }
            else
            {
                uint32_t index =
                    (j - (i + 1)) + (this->Length() - ((this->m_n - i) * (this->m_n - i - 1) / 2));
                return this->m_arr[index];
            }
        };

        bool Set(uint32_t i, uint32_t j, T value)
        {
            if (i > j)
            {
                return this->Set(j, i, value);
            }
            else if (i >= this->m_n || j >= this->m_n || i == j)
            {
                return false;
            }
            else
            {
                this->m_arr[(j - (i + 1)) +
                            (this->Length() - ((this->m_n - i) * (this->m_n - i - 1) / 2))] = value;
                return false;
            }
        };

        uint32_t Length()
        {
            return this->m_size;
        };

        uint32_t N()
        {
            return this->m_n;
        };
    };

    /**
     * A data structure that stores the pairwise LogicalLinks
     */
    PairMap<Ptr<netsimulyzer::LogicalLink>> m_pairMap;

    /**
     * for scheduling timed deactivations
     */
    PairMap<Timer> m_timers;

    /**
     * Orchestrator attatched to the LogicalLinks
     */
    Ptr<Orchestrator> m_orchestrator;
};

} // namespace ns3::netsimulyzer

#endif // LOGICAL_LINK_PAIRS_H
