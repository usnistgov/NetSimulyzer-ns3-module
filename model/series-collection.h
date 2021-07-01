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
 */

#ifndef SERIES_COLLECTION_H
#define SERIES_COLLECTION_H

#include <string>
#include <cstdint>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/orchestrator.h>
#include <ns3/xy-series.h>
#include <ns3/value-axis.h>
#include <ns3/color.h>
#include <ns3/color-palette.h>
#include <cstddef>
#include <vector>

namespace ns3::netsimulyzer {

class Orchestrator;
class XYSeries;

/**
 * Defines a collection of series to be plotted on one graph
 */
class SeriesCollection : public ns3::Object
{
public:
  /**
   * Sets up the SeriesCollection and assigns the ID
   *
   * \param m_orchestrator
   * The orchestrator to tie this collection to
   */
  explicit SeriesCollection (Ptr<Orchestrator> orchestrator);

  /**
   * \brief Get the class TypeId
   *
   * \return the TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Add a series by reference
   * all series in this collection should
   * be tracked by the same `orchestrator`
   *
   * \param series
   * The series to add.
   */
  void Add (Ptr<XYSeries> series);

  /**
   * \return The collection of Series IDs
   */
  std::vector<uint32_t> GetSeriesIds (void);

  /**
   * Gets the current palette of available colors for
   * auto assignment if the `AutoColor` attribute is set.
   * In the order they would be assigned
   *
   * \return
   * The list of colors available for auto-assignment
   */
  const std::vector<Color3Value> &GetAutoColorPalette (void) const;

  /**
   * Replaces the list of colors used when the `AutoColor` attribute
   * is set.
   *
   * These colors are assigned to series in the order they appear in `values`.
   *
   * \param values
   * The list of colors to use for assigned series
   */
  void SetAutoColorPalette (std::vector<Color3Value> values);

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
   * Unique ID that ties this collection to its events.
   * Assigned by the `orchestrator`
   */
  uint32_t m_id;

  /**
   * Pointer to the Orchestrator managing this collection
   */
  Ptr<Orchestrator> m_orchestrator;

  /**
   * The IDs of the series contained in this collection
   */
  std::vector<uint32_t> m_seriesIds;

  /**
   * The X Axis on the graph. Overrides defied axes on child series
   */
  Ptr<ValueAxis> m_xAxis{CreateObject<ValueAxis> ()};

  /**
   * The Y Axis on the graph. Overrides defied axes on child series
   */
  Ptr<ValueAxis> m_yAxis{CreateObject<ValueAxis> ()};

  /**
   * The name to show in visualizer elements & title of the graph
   */
  std::string m_name;

  /**
   * Flag indicating the configuration of this model was finalized and written,
   * and should not be written again
   */
  bool m_committed{false};

  /**
   * Flag indicating added series should have their color overwritten with one of
   * the colors from `m_autoColorPalette`
   *
   * \see m_autoColorPalette
   */
  bool m_autoColor;

  /**
   * The palette used to set added series colors if `m_autoColor` is set.
   *
   * \see m_autoColor
   * \see m_autoColorIndex
   */
  std::vector<Color3Value> m_autoColorPalette{
      BLUE_VALUE,       DARK_BLUE_VALUE,   RED_VALUE,         DARK_RED_VALUE, GREEN_VALUE,
      DARK_GREEN_VALUE, ORANGE_VALUE,      DARK_ORANGE_VALUE, YELLOW_VALUE,   DARK_YELLOW_VALUE,
      PURPLE_VALUE,     DARK_PURPLE_VALUE, PINK_VALUE,        DARK_PINK_VALUE};

  /**
   * The index for the next color in `m_autoColorPalette` to use for an assigned series
   *
   * \see m_autoColor
   * \see m_autoColorPalette
   */
  std::size_t m_autoColorIndex{0u};

  /**
   * Should we set the `Visible`attribute on added series to `false`
   * upon adding them
   */
  bool m_hideAddedSeries;

  /**
   * Add a series by ID.
   * All series in this collection should
   * be tracked by the same `orchestrator`
   *
   * \param id
   * The ID of the series to add.
   */
  void Add (uint32_t id);
};

} // namespace ns3::netsimulyzer

#endif /* SERIES_COLLECTION_H */
