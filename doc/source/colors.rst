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

  // Color types may be freely converted
  Color3 start;
  // Convert to a full opacity(alpha) Color4
  Color4 converted{start};


``Color3`` has a corresponding attribute type ``Color3Value`` which just accepts a ``Color3``.
It's used when setting or retrieving an attribute from a model.

.. code-block:: C++

  Color3 color{/* ... */};

  building->SetAttribute("Color", Color3Value{color});


.. Some code samples were divided by pages, so split up these two on to separate pages
.. raw:: latex

    \clearpage

.. _color4:

Color4
------

Representation of three component color (Red, Green, & Blue) with an alpha (opacity) channel.
Each component may be set by the public member or by the constructor.


.. code-block:: C++

  // Colors are black by default
  Color4 black;

  // Sets red, green, & blue, but not alpha to 204u
  Color4 gray{204u};

  // Sets each color component (red, green, & blue)
  // individually with alpha at 255u
  Color4 allColors{102u, 0u, 200u};

  // Sets each component (red, green, blue, & alpha)
  // individually
  Color4 components{102u, 0u, 200u, 128u};

  // Components may be set as members too
  Color4 memExample;
  memExample.red = 128u;
  memExample.green = 128u;
  memExample.blue = 128u;
  memExample.alpha = 128u;

  // Color types may be freely converted
  Color4 start;
  // Drop opacity and convert to a Color3
  Color3 converted{start};


``Color4``, just like ``Color3``, has a corresponding attribute type ``Color4Value``.
It's used when setting or retrieving an attribute from a model.

.. code-block:: C++

  Color4 color{/* ... */};

  xySeries->SetAttribute("Color", Color4Value{color});

