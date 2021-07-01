Series Collections
==================

To view and potentially compare several :ref:`xy-series` in one chart,
they may be grouped under a ``SeriesCollection``.

.. _series-collection:

SeriesCollection
----------------

A collection of :ref:`xy-series` which are displayed as a single
graph in the application with its own axes.

A ``SeriesCollection`` cannot have values appended to it like other
series, but rather it shows the values of its child series.


Example
^^^^^^^

.. code-block:: C++

  // Make some XY Series to add to the collection
  // Names & colors are pretty useful to differentiate
  // these in the actual chart
  auto xyChild1 = CreateObject<XYSeries>(/* orchestrator* /);
  xyChild1->SetAttribute("Name", StringValue("Child One"));
  xyChild1->SetAttribute("Color", RED_VALUE);

  auto xyChild2 = CreateObject<XYSeries>(/* orchestrator */);
  xyChild2->SetAttribute("Name", StringValue("Child Two"));
  xyChild2->SetAttribute("Color", GREEN_VALUE);


  // Create the collection
  auto collection = CreateObject<SeriesCollection>(/* orchestrator */);

  // Add the series to the collection
  collection->Add(xyChild1);
  collection->Add(xyChild2);


  // Add data to the child series to see it in
  // the collection

  xyChild1->Append(1.0, 1.0);
  xyChild1->Append(2.0, 2.0);

  xyChild2->Append(1.0, 2.0);
  xyChild2->Append(2.0, 1.0);

  // Append data is visible from both before
  // and after the Series is added to the
  // collection


Automatic Color Assignment
^^^^^^^^^^^^^^^^^^^^^^^^^^

If the ``AutoColor`` is set to ``true`` (default ``false``). the collection
will assign a color from its internal palette to each ``XYSeries`` added
to it. This palette may be accessed with the ``GetAutoColorPalette ()``
and ``SetAutoColorPalette ()`` methods.


Attributes
^^^^^^^^^^

+----------------------+-------------------+---------------------------+-------------------------------------------------------------+
| Name                 | Type              | Default Value             | Description                                                 |
+======================+===================+===========================+=============================================================+
| XAxis                | :ref:`value-axis` | Default configured        | The X Axis shown on the graph                               |
|                      |                   | :ref:`value-axis`         |                                                             |
+----------------------+-------------------+---------------------------+-------------------------------------------------------------+
| YAxis                | :ref:`value-axis` | Default configured        | The Y Axis shown on the graph                               |
|                      |                   | :ref:`value-axis`         |                                                             |
+----------------------+-------------------+---------------------------+-------------------------------------------------------------+
| Name                 | string            | "Series Collection: {ID}" | Name to use for this Series in application elements         |
+----------------------+-------------------+---------------------------+-------------------------------------------------------------+
| HideAddedSeries      | bool              | true                      | Hides series added to this collection.See the ``Visible``   |
|                      |                   |                           | attribute for :ref:`xy-series` for more info                |
+----------------------+-------------------+---------------------------+-------------------------------------------------------------+
| AutoColor            | bool              | false                     | Overwrites the ``Color`` attribute of added series with one |
|                      |                   |                           | selected from the internal palette.                         |
+----------------------+-------------------+---------------------------+-------------------------------------------------------------+





