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
 */

#ifndef XY_SERIES_H
#define XY_SERIES_H

#include <string>
#include <cstdint>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/orchestrator.h>
#include <ns3/value-axis.h>
#include <ns3/color.h>

namespace ns3::netsimulyzer {

class Orchestrator;
class ValueAxis;

class XYSeries : public ns3::Object
{
public:
  /**
   * The type of connection to form between points
   */
  enum ConnectionType : int { None, Line, Spline };

  /**
   * Possible configurations for point labels
   */
  enum LabelMode : int { Hidden, Shown };

  /**
   * Sets up the XYSeries and assigns the ID
   *
   * \param orchestrator
   * The orchestrator to tie this series to
   */
  explicit XYSeries (Ptr<Orchestrator> orchestrator);

  /**
   * \brief Get the class TypeId
   *
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Add a single point that will be plotted at `Simulation::Now()` time
   *
   * \param x
   * The value to plot on the first axis
   *
   * \param y
   * The value to plot on the second axis
   */
  void Append (double x, double y);

  /**
   * Finalizes configuration of the series.
   *
   * Only necessary to call manually if a series is created
   * after the simulation starts.
   *
   * A series may only be committed once. Additional commit calls have no effect.
   */
  void Commit (void);

  /**
   * \return
   * A pointer to the X Axis for this series
   */
  Ptr<ValueAxis> GetXAxis (void) const;

  /**
   * Replace the X Axis with `value`.
   * Several series may reference the same axis
   *
   * \param value
   * The new X Axis for this series
   */
  void SetXAxis (Ptr<ValueAxis> value);

  /**
   * \return
   * A pointer to the Y Axis for this series
   */
  Ptr<ValueAxis> GetYAxis (void) const;

  /**
   * Replace the Y Axis with `value`.
   * Several series may reference the same axis
   *
   * \param value
   * The new Y Axis for this series
   */
  void SetYAxis (Ptr<ValueAxis> value);

protected:
  void DoDispose (void) override;

private:
  /**
   * Unique ID that ties series to their events.
   * Assigned by the `orchestrator`
   */
  uint32_t m_id;

  /**
   * Pointer to the Orchestrator managing this series
   */
  Ptr<Orchestrator> m_orchestrator;

  /**
   * The X Axis on the graph
   */
  Ptr<ValueAxis> m_xAxis{CreateObject<ValueAxis> ()};

  /**
   * The Y Axis on the graph
   */
  Ptr<ValueAxis> m_yAxis{CreateObject<ValueAxis> ()};

  /**
   * The method of connecting points on the graph
   */
  ConnectionType m_connection;

  /**
   * The current display mode for the point labels in this series
   */
  LabelMode m_labelMode;

  /**
   * The name to show in visualizer elements & title of the graph
   */
  std::string m_name;

  /**
   * Name for the series that appears in the chart legend
   */
  std::string m_legend;

  /**
   * Flag indicating if this series should appear individually in visualiser elements
   */
  bool m_visible;

  /**
   * The color of the points & connections
   */
  Color3 m_color;

  /**
   * Flag indicating the configuration of this model was finalized and written,
   * and should not be written again
   */
  bool m_committed{false};
};

} // namespace ns3::netsimulyzer

#endif /* XY_SERIES_H */
