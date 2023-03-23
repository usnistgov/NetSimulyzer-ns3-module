/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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

#include "node-configuration-container.h"

#include <ns3/assert.h>
#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NodeConfigurationContainer");

namespace netsimulyzer
{

NodeConfigurationContainer::NodeConfigurationContainer(void)
{
    NS_LOG_FUNCTION(this);
}

NodeConfigurationContainer::NodeConfigurationContainer(Ptr<NodeConfiguration> configuration)
{
    NS_LOG_FUNCTION(this << configuration);
    NS_ASSERT(configuration != nullptr);
    m_configurationObjects.push_back(configuration);
}

NodeConfigurationContainer::NodeConfigurationContainer(const NodeConfigurationContainer& first,
                                                       const NodeConfigurationContainer& second)
{
    NS_LOG_FUNCTION(this << &first << &second);
    m_configurationObjects = first.m_configurationObjects;

    m_configurationObjects.insert(m_configurationObjects.end(),
                                  second.m_configurationObjects.begin(),
                                  second.m_configurationObjects.end());
}

NodeConfigurationContainer::Iterator
NodeConfigurationContainer::Begin(void)
{
    NS_LOG_FUNCTION(this);
    return m_configurationObjects.begin();
}

NodeConfigurationContainer::Iterator
NodeConfigurationContainer::End(void)
{
    NS_LOG_FUNCTION(this);
    return m_configurationObjects.end();
}

uint32_t
NodeConfigurationContainer::GetN(void)
{
    NS_LOG_FUNCTION(this);
    return m_configurationObjects.size();
}

Ptr<NodeConfiguration>
NodeConfigurationContainer::Get(uint32_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_configurationObjects[i];
}

void
NodeConfigurationContainer::Add(const NodeConfigurationContainer& c)
{
    NS_LOG_FUNCTION(this << &c);
    m_configurationObjects.insert(m_configurationObjects.end(),
                                  c.m_configurationObjects.begin(),
                                  c.m_configurationObjects.end());
}

void
NodeConfigurationContainer::Add(Ptr<NodeConfiguration> configuration)
{
    NS_LOG_FUNCTION(this << &configuration);
    NS_ASSERT(configuration != nullptr);
    m_configurationObjects.push_back(configuration);
}

void
NodeConfigurationContainer::Clear(void)
{
    NS_LOG_FUNCTION(this);
    m_configurationObjects.clear();
}

NodeConfigurationContainer::Iterator
begin(NodeConfigurationContainer& c)
{
    return c.Begin();
}

NodeConfigurationContainer::Iterator
end(NodeConfigurationContainer& c)
{
    return c.End();
}

} // namespace netsimulyzer
} // namespace ns3
