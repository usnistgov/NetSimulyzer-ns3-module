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

#ifndef CATEGORY_VALUE_SERIES_H
#define CATEGORY_VALUE_SERIES_H

#include <string>
#include <cstdint>
#include <vector>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/orchestrator.h>
#include <ns3/value-axis.h>
#include <ns3/category-axis.h>
#include <ns3/color.h>
#include <ns3/deprecated.h>

namespace ns3 {
namespace netsimulyzer {

class CategoryValueSeries : public ns3::Object
{
public:
  /**
   * Get the class TypeId
   *
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Create a series without any categories.
   * Categories must be added to the Y axis before appending values
   *
   * \param orchestrator
   * The `Orchestrator` that manages this series
   */
  explicit CategoryValueSeries (Ptr<Orchestrator> orchestrator);

  /**
   * Creates a series with categories.
   * Generates an ID for each category.
   *
   * \warning Do not duplicate category names,
   * if two categories share the same name IDs
   * must be used to interact with the conflicting names.
   *
   * \param orchestrator
   * The `Orchestrator` that manages this series
   *
   * \param categories
   * The categories to add to this series
   */
  CategoryValueSeries (Ptr<Orchestrator> orchestrator, const std::vector<std::string> &categories);

  /**
   * Creates a series with categories.
   *
   * \warning Category IDs must be unique
   *
   * \warning Do not duplicate category names,
   * if two categories share the same name IDs
   * must be used to interact with the conflicting names.
   *
   * \param orchestrator
   * The `Orchestrator` that manages this series
   *
   * \param categories
   * The categories to add to this series
   */
  CategoryValueSeries (Ptr<Orchestrator> orchestrator,
                       const std::vector<CategoryAxis::ValuePair> &categories);

  /**
   * Changes the current value to `value` within `category`
   * using the category's ID.
   * The Y axis is not checked for the presence of `category`
   *
   * \param category
   * The ID of the category to use
   *
   * \param value
   * The value to plot within `category`
   */
  void Append (int category, double value);

  /**
   * Changes the current value to `value` within `category`
   *
   * \param category
   * The name of the category.
   * If `category` is not on the Y axis
   * this method will abort
   *
   * \param value
   * The value to plot within `category`
   */
  void Append (const std::string &category, double value);

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
  Ptr<CategoryAxis> GetYAxis (void) const;

  /**
   * Replace the Y Axis with `value`.
   * Several series may reference the same axis
   *
   * \param value
   * The new Y Axis for this series
   */
  void SetYAxis (Ptr<CategoryAxis> value);

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
  Ptr<CategoryAxis> m_yAxis{CreateObject<CategoryAxis> ()};

  /**
   * The name to show in visualizer elements & title of the graph
   */
  std::string m_name;

  /**
   * Name for the series that appears in the chart legend
   */
  std::string m_legend;

  /**
   * Enable auto-generation of points on the X axis in the application
   */
  bool m_autoUpdate;

  /**
   * The minimum time between generated points
   */
  Time m_autoUpdateInterval;

  /**
   * The value to add to the previous X value when appending points
   */
  double m_autoUpdateIncrement;

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

} // namespace netsimulyzer
} // namespace ns3
#endif /* CATEGORY_VALUE_SERIES_H */
