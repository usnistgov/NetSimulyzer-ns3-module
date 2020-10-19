Numeric Axis
============

Most models output some form of numeric information. To display this
data use a series with a ``ValueAxis``


.. _value-axis:

ValueAxis
---------

A numeric axis with a configurable scale and boundaries. Axes are
not typically explicitly created, but are provided when a series
is created.

Scale
^^^^^

By default a ``ValueAxis`` will use a linear scale for tick marks,
but a logarithmic scale may also be used. See the ``Scale`` attribute
for more info.

Boundaries
^^^^^^^^^^

By default, the axis will define both the minimum and maximum
to be 0.0, and scale them to the values appended to the series.
If this behavior is undesired, an explicit minimum and maximum
may be set (See: ``Minimum`` and ``Maximum`` attributes) and
scaling may be disabled (See: ``BoundMode`` attribute).


Attributes
^^^^^^^^^^
+----------------------+--------------------------+--------------------+---------------------------------------------------------------------------------------------------+
| Name                 | Type                     | Default Value      | Description                                                                                       |
+======================+==========================+====================+===================================================================================================+
| Name                 | string                   | Empty String       | Label to apply to this axis in the application                                                    |
+----------------------+--------------------------+--------------------+---------------------------------------------------------------------------------------------------+
| Minimum              | double                   | 0.0                | Smallest value shown on the axis by default                                                       |
+----------------------+--------------------------+--------------------+---------------------------------------------------------------------------------------------------+
| Maximum              | double                   | 0.0                | Largest value shown on the axis by default                                                        |
+----------------------+--------------------------+--------------------+---------------------------------------------------------------------------------------------------+
| BoundMode            | ``ValueAxis::BoundMode`` | ``HighestValue``   | Method of scaling the ends of the axis                                                            |
|                      |                          |                    |                                                                                                   |
|                      |                          |                    | * ``Fixed`` The endpoints of the axis do not move regardless of appended values                   |
|                      |                          |                    | * ``HighestValue`` (Default) Each endpoint is moved to slightly beyond the min/max appended value |
+----------------------+--------------------------+--------------------+---------------------------------------------------------------------------------------------------+
| Scale                | ``ValueAxis::Scale``     | ``Linear``         | Method scale between tick marks on the axis                                                       |
|                      |                          |                    |                                                                                                   |
|                      |                          |                    | * ``Linear`` (Default) Scale the same amount between ticks                                        |
|                      |                          |                    | * ``Logarithmic`` Scale the tick marks using a logarithmic scale                                  |
+----------------------+--------------------------+--------------------+---------------------------------------------------------------------------------------------------+





