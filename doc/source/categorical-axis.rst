Categorical Axis
================

For grouping data based on a category rather then a numeric value use
a ``CategoryAxis``.

.. _category-axis:

CategoryAxis
------------

A string based axis with optional IDs for each value. Axes are
not typically explicitly created, but are provided when a series
is created.

Values may be added by providing just a string, or a string and and associated ID
(See: :ref:`category-axis-value-pair`) using the appropriate ``AddValue()`` call.

Prefer to add all axis values with IDs, or all without IDs.


Providing IDs
^^^^^^^^^^^^^

In order to provide an ID for a value on a ``CategoryAxis``
both values must be packaged in a ``ValuePair``.

.. _category-axis-value-pair:

ValuePair
+++++++++

``ValuePair`` is a simple struct combining the ID and string value of
an item on a ``CategoryAxis``.

``ValuePair`` is a member class of ``CategoryAxis``,
so it is declared as ``CategoryAxis::ValuePair``.

The definition of ``ValuePair`` follows:

.. code-block:: C++

  class CategoryAxis : /* ... */
  {
   public:
    struct ValuePair
    {
      int key;
      std::string value;
    };

   // ...
  };


Adding Values
+++++++++++++

Each string value should be paired with its ID in a ``ValuePair`` and passed to
``AddValue()``

.. code-block:: C++

  enum State { Park, Drive, Reverse };
  netsimulyzer::CategoryAxis::ValuePair parkPair;

  // If State is an `enum class` or `enum struct`
  // use static_cast<int>(State::Park)
  parkPair.key = State::Park;

  // The value that appears on the Axis
  // in the application
  parkPair.value = "Park";

  // Retrieve an axis from a series with one
  PointerValue axisValue;

  // Existing CategoryValueSeries
  series->GetAttribute("YAxis", axisValue);
  auto axis = axisValue.Get<netsimulyzer::CategoryAxis>();

  // some existing axis
  axis->AddValue(parkPair);



Attributes
^^^^^^^^^^

+------+--------+----------------+------------------------------------------------+
| Name | Type   | Default Value  | Description                                    |
+======+========+================+================================================+
| Name | string | Empty String   | Label to apply to this axis in the application |
+------+--------+----------------+------------------------------------------------+
