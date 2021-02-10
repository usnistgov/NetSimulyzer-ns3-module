Showing Buildings
=================

To show a ``ns3::Building`` in the application aggregate a ``BuildingConfiguration`` onto it.
Any buildings in the scenario with a ``BuildingConfiguration`` will be tracked.


.. _building-configuration:

BuildingConfiguration
---------------------

A class which allows for display settings for a ``ns3::Building`` to
be set.

The bounds of each ``ns3::Building`` with a ``BuildingConfiguration``
are read at the beginning of the simulation (``Simulation::Start()`` time),
and the model mill not detect any changes to the building afterwards.

A building is rendered as a semi-transparent rectangular prism with a configurable color.


Examples
^^^^^^^^

Installing on One Building
++++++++++++++++++++++++++

The basic setup for showing a single ``ns3::Building`` is as follows:

.. code-block:: C++

  netsimulyzer::BuildingConfigurationHelper buildingHelper{/* orchestrator */};
  buildingHelper.Install (/* Ptr or Collection */);


Installing on All Buildings
+++++++++++++++++++++++++++

Since buildings often all share a similar configuration, looping through the ``ns3::BuildingList`` and
configuring every ``ns3::Building`` right before the scenario starts
is the easiest way to get them all to show up.

.. code-block:: C++

  netsimulyzer::BuildingConfigurationHelper helper{orchestrator};
  for (auto building = BuildingList::Begin (); building != BuildingList::End (); building++)
    helper.Install (*building);

  // Simulator::Run ();

Attributes
^^^^^^^^^^

+----------------------+---------------+-------------------+--------------------------------------------+
| Name                 | Type          | Default Value     | Description                                |
+======================+===============+===================+============================================+
| Color                | :ref:`color3` | Light Gray (204u) | The tint for the walls of the ``Building`` |
+----------------------+---------------+-------------------+--------------------------------------------+
| Visible              | bool          |              true | Defines if the ``Building``                |
|                      |               |                   | is rendered in the application             |
+----------------------+---------------+-------------------+--------------------------------------------+
