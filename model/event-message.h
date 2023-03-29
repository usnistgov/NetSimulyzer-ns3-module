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

#ifndef EVENT_MESSAGE_H
#define EVENT_MESSAGE_H

#include <ns3/color-palette.h>
#include <ns3/color.h>
#include <ns3/nstime.h>
#include <ns3/vector.h>

#include <cstdint>
#include <optional>

namespace ns3::netsimulyzer
{

struct CourseChangeEvent
{
    Time time;
    uint32_t nodeId;
    Vector position;
};

struct TransmitEvent
{
    Time time;
    uint32_t nodeId;
    Time duration;
    double targetSize;
    Color3 color;
};

struct NodeOrientationChangeEvent
{
    Time time;
    uint32_t nodeId;
    Vector3D orientation;
};

struct NodeColorChangeEvent
{
    enum class ColorType
    {
        Base,
        Highlight
    };

    Time time;
    uint32_t id;
    ColorType type;
    std::optional<Color3> color;
};

struct DecorationMoveEvent
{
    Time time;
    uint32_t id;
    Vector3D position;
};

struct DecorationOrientationChangeEvent
{
    Time time;
    uint32_t id;
    Vector3D orientation;
};

struct LogMessageEvent
{
    Time time;
    uint32_t id;
    std::string message;
};

} // namespace ns3::netsimulyzer

#endif /* EVENT_MESSAGE_H */
