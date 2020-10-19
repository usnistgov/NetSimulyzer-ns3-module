Categorized Numeric Data
========================

For numeric data that may be binned into descrete catagories
(e.g. application state vs time) a ``CategoryValueSeries``
may be used.


.. _category-value-series:

CategoryValueSeries
-------------------

The ``CategoryValueSeries`` plots a numeric value (x-axis) to a
String value (y-axis). It is useful for tracking the state of
an Application or some other model.

A list of Strings to populate the y-axis may be provided
at creation time, or provided later to ``YAxis``.
Unique, integer IDs may be provided for each String
value (i.e. ``enum`` values). If IDs are not provided
then they will be generated. Ideally IDs are
sequential, ascending values.

Values are added with ``Append()`` calls, like :ref:`xy-series`, but
the provided Y value may be a String, or the ID corresponding
to the desired String using a :ref:`category-axis-value-pair`.


The basic setup for a ``CategoryValueSeries`` is as follows:

.. code-block:: C++

  const std::vector<std::string> categories{"First", "Second", "Third", "Last"}
  auto cvSeries = CreateObject<visualizer3d::CategoryValueSeries>(/* orchestrator */, categories);

  cvSeries->Append("First", 1.0);


To provide IDs with the String values, use :ref:`category-axis-value-pair` from ``ValueAxis`` like so:

.. code-block:: C++

  using namespace ns3;
  using namespace visualizer3d;

  enum States { First, Second, Third, Last };

  // ----- Long Way -----

  //Create a `ValuePair` for each enum value
  CategoryAxis::ValuePair first{States::First, "First"};
  // ...

  // Put them all in a vector (or construct one in the `CreateObject` call later)
  const std::vector<CategoryAxis::ValuePair> categories{first, /* ... */};

  // Create the series
  auto series = CreateObject<CategoryValueSeries>(/* orchestrator */, categories);


  // ----- Short Way -----

  // Build the vector passing in `ValuePair`s for every enum value
  const std::vector<CategoryAxis::ValuePair> categories{ {First, "First"}, /* ... */};

  // Pass the orchestrator & categories to the series
  auto series = CreateObject<CategoryValueSeries>(/* orchestrator */, categories);


  // ----- Really Short Way -----

  // The vector may be constructed like the pairs in the Short Way
  auto series = CreateObject<CategoryValueSeries>(/* orchestrator */, { {First, "First"}, /* ... */} );


Then the series may be appended to using the ID (enum) value

.. code-block:: C++

  // From above
  // enum States { First, Second, Third, Last };

  // Translates enum values to their String values
  series->Append(States::First, 1.0);
  series->Append(States::Second, 2.0);
  series->Append(States::Third, 3.0);
  series->Append(States::Last, 4.0);

  // Using String values is still supported
  series->Append("First", 5.0);
  series->Append("Second", 6.0);
  series->Append("Third", 7.0);
  series->Append("Last", 8.0);


Attributes
^^^^^^^^^^

+----------------------+----------------------+----------------------+-------------------------------------------------------+
| Name                 | Type                 | Default Value        | Description                                           |
+======================+======================+======================+=======================================================+
| XAxis                | :ref:`value-axis`    | Default configured   | The X Axis shown on the graph                         |
|                      |                      | :ref:`value-axis`    |                                                       |
+----------------------+----------------------+----------------------+-------------------------------------------------------+
| YAxis                | :ref:`category-axis` | Default configured   | The Y Axis shown on the graph. Stores the String/ID   |
|                      |                      | :ref:`category-axis` | pairs.                                                |
+----------------------+----------------------+----------------------+-------------------------------------------------------+
| Name                 | string               | "Category Value      | Name to use for this Series in application elements   |
|                      |                      | Series: {ID}"        |                                                       |
+----------------------+----------------------+----------------------+-------------------------------------------------------+
| Legend               | string               | Value of ``Name``    | Name for the series that appears in the chart legend. |
+----------------------+----------------------+----------------------+-------------------------------------------------------+
| Visible              | bool                 | ``true``             | Defines if this series appears in                     |
|                      |                      |                      | application elements.                                 |
+----------------------+----------------------+----------------------+-------------------------------------------------------+
| Color                | :ref:`color4`        | n/a                  | The color to use for point connections                |
+----------------------+----------------------+----------------------+-------------------------------------------------------+


