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

#include "series-wrapper-collection.h"

#include "series-collection.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/pointer.h"
#include "ns3/ptr.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"

#include <unordered_map>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SeriesWrapperCollection");

namespace netsimulyzer
{

NS_OBJECT_ENSURE_REGISTERED(SeriesWrapperCollection);
NS_OBJECT_ENSURE_REGISTERED(SeriesWrapperMap);

/*          SeriesWrapperCollection           */

TypeId
SeriesWrapperCollection::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SeriesWrapperCollection")
            .SetParent<ns3::Object> ()
            .SetGroupName ("netsimulyzer");
    return tid;
    // clang-format on
}

Ptr<SeriesWrapperCollection>
SeriesWrapperCollection::AddWrapper(Ptr<SeriesWrapper> w)
{
    NS_LOG_FUNCTION(this << w);
    m_wrappers.emplace_back(w);
    m_collection->Add(w->GetSeries());
    return this;
};

Ptr<XYSeries>
SeriesWrapperCollection::GetSeries(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrappers.at(i)->GetSeries();
};

Ptr<SeriesWrapper>
SeriesWrapperCollection::GetWrapper(std::size_t i)
{
    NS_LOG_FUNCTION(this << i);
    return m_wrappers.at(i);
};

std::size_t
SeriesWrapperCollection::GetNSeries()
{
    return m_wrappers.size();
};

Ptr<SeriesCollection>
SeriesWrapperCollection::GetSeriesCollection()
{
    return m_collection;
};

SeriesWrapperCollection::SeriesWrapperCollection(Ptr<Orchestrator> orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator);
    m_collection = CreateObject<SeriesCollection>(orchestrator);
};

SeriesWrapperCollection::SeriesWrapperCollection(Ptr<Orchestrator> orchestrator,
                                                 std::string name,
                                                 std::string x_axis,
                                                 std::string y_axis)
    : SeriesWrapperCollection(orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator << name << x_axis << y_axis);

    m_collection->SetAttribute("Name", StringValue(name));
    m_collection->GetYAxis()->SetAttribute("Name", StringValue(y_axis));
    m_collection->GetXAxis()->SetAttribute("Name", StringValue(x_axis));
};

const std::vector<Ptr<SeriesWrapper>>&
SeriesWrapperCollection::Wrappers()
{
    return this->m_wrappers;
};

SeriesWrapper&
SeriesWrapperCollection::operator[](std::size_t i)
{
    return (*this->GetWrapper(i));
};

/*          SeriesWrapperMap           */

TypeId
SeriesWrapperMap::GetTypeId()
{
    // clang-format off
    static TypeId tid =
        TypeId ("ns3::netsimulyzer::SeriesWrapperMap")
            .SetParent<ns3::netsimulyzer::SeriesWrapperCollection> ()
            .SetGroupName ("netsimulyzer");
    return tid;
    // clang-format on
}

SeriesWrapperMap::SeriesWrapperMap(Ptr<Orchestrator> orchestrator)
    : SeriesWrapperCollection(orchestrator)
{
    NS_LOG_FUNCTION(this << orchestrator);
};

SeriesWrapperMap::SeriesWrapperMap(Ptr<Orchestrator> orchestrator,
                                   std::string name,
                                   std::string x_axis,
                                   std::string y_axis)
    : SeriesWrapperCollection(orchestrator, name, x_axis, y_axis)
{
    NS_LOG_FUNCTION(this << orchestrator << name << x_axis << y_axis);
};

Ptr<SeriesWrapperMap>
SeriesWrapperMap::AddWrapper(std::string index, Ptr<SeriesWrapper> w)
{
    NS_LOG_FUNCTION(this << index << w);
    m_nameMap.insert({index, GetNSeries()});
    SeriesWrapperCollection::AddWrapper(w);
    return this;
};

Ptr<XYSeries>
SeriesWrapperMap::GetSeries(std::string index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesWrapperCollection::GetSeries(m_nameMap.at(index));
};

Ptr<SeriesWrapper>
SeriesWrapperMap::GetWrapper(const std::string& index)
{
    NS_LOG_FUNCTION(this << index);
    return SeriesWrapperCollection::GetWrapper(m_nameMap.at(index));
};

SeriesWrapper&
SeriesWrapperMap::operator[](const std::string& index)
{
    return (*this->GetWrapper(index));
};

} // namespace netsimulyzer

} // namespace ns3
