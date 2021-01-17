Orchestrator
============

The ``Orchestrator`` manages module wide ``visualizer3d`` features, receives events, and
manages output from the module. All ``visualizer3d`` models require a reference
to an ``Orchestrator`` to function.

The `Orchestrator` must be constructed **before** the simulation begins.

To construct an `Orchestrator` pass a path to a file to output. Absolute & relative
paths are accepted.

Relative paths (e.g. ``../example.json``) will be relative to the current working directory.

If just a filename is provided (e.g. ``example.json``), then it will be treated as a
path to the current working directory, like so: ``./example.json``.

The output file is in JSON format. For the schema for the output file
see the `schema.json <https://github.com/usnistgov/ns3-visualizer3d/blob/master/schema.json>`_
for the application. The '.json' extension is not required, but strongly encouraged.

.. code-block:: C++

  auto orchestrator = CreateObject<visualizer3d::Orchestrator> ("filename.json");

Most ``Orchestrator`` events are based on the simulation's time, so it may be safely placed at
the global scope if desired.

.. code-block:: C++

  using namespace ns3;
  auto orchestrator = CreateObject<visualizer3d::Orchestrator> ("global.json");

  int
  main ()
  { /* ... */ }


The output file is created when the ``Orchestrator`` is constructed, but is written to
at the **end** of the simulation.


.. _orchestrator-mobility-polling:

Mobility Polling
----------------

If the ``PollMobility`` attribute is true, then the ``Orchestrator`` will poll
all of its child ``NodeConfiguration`` objects for their current location on the interval defined
by ``MobilityPollInterval``.

If the child ``NodeConfiguration`` has ``UsePositionTolerance`` set to true, then,
the aggregated ``Node`` will be checked if its position is within its ``PositionTolerance``,
and if so the ``Orchestrator`` will not write the position change until
it is beyond the ``PositionTolerance`` in the ``NodeConfiguration``.

If ``NodeConfiguration::UsePositionTolerance`` is false, then the ``Orchestrator``
will always write the position when a ``NodeConfiguration`` is polled.


Properties
----------

+----------------------+--------------------------------+--------------------+------------------------------------------+
| Name                 | Type                           | Default Value      | Description                              |
+======================+================================+====================+==========================================+
| MillisecondsPerFrame | :ref:`optional-value` <double> |                n/a | Optional hint to the application for     |
|                      |                                |                    | the number of milliseconds to advance    |
|                      |                                |                    | the simulation by for one frame          |
+----------------------+--------------------------------+--------------------+------------------------------------------+
| MobilityPollInterval | Time                           | MilliSeconds (100) | How often to poll each child`            |
|                      |                                |                    | ``NodeConfiguration`` for their          |
|                      |                                |                    | current position. Only enabled if        |
|                      |                                |                    | ``PollMobility`` is true                 |
+----------------------+--------------------------------+--------------------+------------------------------------------+
| PollMobility         | bool                           |               true | Flag to toggle polling                   |
|                      |                                |                    | for Node positions                       |
+----------------------+--------------------------------+--------------------+------------------------------------------+
| StartTime            | Time                           |               n/a  | Optional start of the time window to     |
|                      |                                |                    | capture events in.                       |
|                      |                                |                    | Events outside the window will           |
|                      |                                |                    | be ignored                               |
+----------------------+--------------------------------+--------------------+------------------------------------------+
| EndTime              | Time                           |               n/a  | Optional end of the time window to       |
|                      |                                |                    | capture events in.                       |
|                      |                                |                    | Events outside the window will           |
|                      |                                |                    | be ignored                               |
+----------------------+--------------------------------+--------------------+------------------------------------------+
