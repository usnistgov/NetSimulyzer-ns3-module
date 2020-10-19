Adding Decorations
==================

To add purely visual elements to a scenario, such as fire hydrants, non-networked cars, doors, etc. use
a ``Decoration``.


.. _decoration:

Decoration
----------

A decoration is a model with no corresponding *ns-3* item outside of the module.
A ``Decoration`` is configured in much the same was a :ref:`node-configuration` is.
Except the location of a ``Decoration`` is set directly without a mobility model.

A ``Decoration`` may have its position and orientation changed during
the simulation.

A basic setup for a ``Decoration`` is as follows:

.. code-block:: C++

  auto decoration = CreateObject<visualizer3d::Decoration>(/* the orchestrator */);

  decoration.SetAttribute ("Model", StringValue ("path/model.obj"));
  decoration.SetAttribute ("Height", DoubleValue (2.0));

  // May also use SetAttribute("Position")
  decoration.SetPosition ({5.0, 5.0, 0.0});


Attributes
^^^^^^^^^^

+----------------------+-------------------+---------------+--------------------------------------------------------------+
| Name                 | Type              | Default Value | Description                                                  |
+======================+===================+===============+==============================================================+
| Model                | string            | n/a           | Relative path from the application's ``Resource``            |
|                      |                   |               | directory to the model to show for this ``Decoration``       |
+----------------------+-------------------+---------------+--------------------------------------------------------------+
| Orientation          | ``ns3::Vector3D`` | (0, 0, 0)     | Orientation of this ``Decoration`` on each axis, in degrees. |
+----------------------+-------------------+---------------+--------------------------------------------------------------+
| Scale                | double            | 1.00          | A multiplicative scale to apply to the model.                |
|                      |                   |               | Applied after ``Height``                                     |
+----------------------+-------------------+---------------+--------------------------------------------------------------+
| Height               | double            | n/a           | Desired height of the model in ns-3 units.                   |
|                      |                   |               | Applied before ``Scale``                                     |
+----------------------+-------------------+---------------+--------------------------------------------------------------+
| Position             | ``ns3::Vector3D`` | (0, 0, 0)     | Position to show this ``Decoration`` in the scenario.        |
+----------------------+-------------------+---------------+--------------------------------------------------------------+
