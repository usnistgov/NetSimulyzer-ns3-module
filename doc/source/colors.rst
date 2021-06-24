Color Types
===========

Many of the netsimulyzer models have configurable colors which are set using the following classes.


.. _color3:

Color3
------

Representation of three component color (Red, Green, and Blue).
Each component may be set by the public member or by the constructor.


.. code-block:: C++

  // Colors are black by default
  Color3 black;

  // Sets all components to 204u
  Color3 gray{204u};

  // Sets each component (red, green, & blue)
  // individually
  Color3 all{102u, 0u, 200u};

  // Components may be set as members too
  Color3 memExample;
  memExample.red = 128u;
  memExample.green = 128u;
  memExample.blue = 128u;


``Color3`` has a corresponding attribute type ``Color3Value`` which accepts the
same constructor as a ``Color3``, or just accepts a ``Color3``.
It's used when setting or retrieving an attribute from a model. As well as
corresponding ``Color3OptionalValue`` types for attributes backed by ``OptionalValue``.

.. code-block:: C++

  // Red Color
  building->SetAttribute("Color", Color3Value{255u, 0u, 0u});

  // Results as the same as above
  Color3 color{255u, 0u, 0u};
  building->SetAttribute("Color", Color3Value{color});

  // Use of the palette constants is highly recommended
  building->SetAttribute("Color", RED_VALUE);

  // Palette constants for OptionalValue attributes exist as well
  node->SetAttribute("BaseColor", BLUE_OPTIONAL_VALUE);

.. _color-palette:

Color Palette
-------------
.. image:: _static/color-palette.png
  :alt: The colors available in the module

To make selecting colors easier, the model comes with a pre-defined palette of colors
defined in ``color-palette.h``. Each color has a ``Color3``, ``Color3Value``, and ``Color3OptionalValue``
constants associated with it in the following style:

.. code-block:: C++

  const Color3 RED{245u, 61u, 0u};
  const Color3Value RED_VALUE{RED};
  const OptionalValue<Color3> RED_OPTIONAL_VALUE{RED};


