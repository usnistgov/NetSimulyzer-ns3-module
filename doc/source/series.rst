Series
======

The models listed below allow for tracking information which is output to
a chart in the application. Referred to as a 'series'.

Data may be appended onto a series to add points shown in the
application when a given series is selected.

In addition to tracking the data, each series has axes, depending
on the series type, with additional configuration options.



Types of Series
---------------

:ref:`xy-series`

  A Series with numeric X and Y axes (:ref:`value-axis`).

:ref:`category-value-series`

  A Series which has a String based y-axis (:ref:`category-axis`)
  and a numeric x-axis (:ref:`value-axis`).

:ref:`series-collection`

  A collection of :ref:`xy-series` to show as one graph in the application
  with its own two numeric axes (:ref:`value-axis`).


Types of Axes
-------------

:ref:`value-axis`

  A numeric axis which may scale linearly or logarithmically,
  and may expand to fit the data or be fixed to a set range

:ref:`category-axis`

  A string based axis which displays a set of provided strings in
  order. Strings may also be provided optional, mapped IDs.

