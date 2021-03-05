Quickstart
==========

Clone the module
++++++++++++++++
Clone the project into a directory called netsimulyzer in the `src` folder of a supported version of *ns-3*

.. code-block:: Bash

  git clone https://github.com/usnistgov/NetSimulyzer-ns3-module.git netsimulyzer


Configure waf
+++++++++++++
Re-run the configuration step from a normal *ns-3* build to pickup the new module

.. code-block:: Bash

  ./waf configure


Add to a scenario
+++++++++++++++++

Include the module in your scenario. And optionally use the namespace.

.. code-block:: C++

  #include <ns3/netsimulyzer-module.h>

  // For brevity in examples
  using namespace ns3;

  // Optional, not assumed in future examples
  using namespace ns3::netsimulyzer;

For more complex linking, see: :doc:`linking-module`


Create an :doc:`orchestrator`, and pass it a path to an output file to create.
The :doc:`orchestrator` is the base for all netsimulyzer elements.

.. code-block:: C++

  auto orchestrator = CreateObject<netsimulyzer::Orchestrator> ("example.json");

For more configuration options, see: :doc:`orchestrator`


Showing Nodes
+++++++++++++

Create a ``NodeConfigurationHelper``, set a model for the Nodes and ``Install()``
on the Nodes you wish to be displayed in the application.

For additional configuration options see :doc:`nodes`.

.. code-block:: C++

  netsimulyzer::NodeConfigurationHelper nodeHelper{orchestrator};
  nodeHelper.Set ("Model",
                  StringValue ("relative/path/from/resource/dir/model.obj"));

  // Shows every Node in the scenario
  for (auto node = NodeList::Begin (); node != NodeList::End (); node++)
     nodeHelper.Install (*node);

  // Or install on a container
  NodeContainer containerNodes;
  containerNodes.Create (2);
  nodeHelper.Install (containerNodes);


Showing Buildings
+++++++++++++++++

Buildings have a similar setup to Nodes, only there is no requirement for a model.

For additional properties, see :doc:`buildings`

.. code-block:: C++

  // Show every building in the scenario
  netsimulyzer::BuildingConfigurationHelper buildingHelper{orchestrator};
  for (auto building = BuildingList::Begin (); building != BuildingList::End (); building++)
    buildingHelper.Install (*building);


Adding Decorations
++++++++++++++++++

For purely visual elements add a :ref:`decoration`. A :ref:`decoration`
is similar to a ``NodeConfiguration`` except its position is set manually.

For more information, see :doc:`decorations`

.. code-block:: C++

  auto decoration = CreateObject<netsimulyzer::Decoration>(orchestrator);
  decoration.SetAttribute ("Model", StringValue ("relative/path/from/resource/dir/model.obj"));
  decoration.SetPosition ({5.0, 5.0, 0.0});


Marking Areas
+++++++++++++

To draw attention to certain areas in the topology, it may be defined as an area.
A :ref:`rectangular-area` will draw a rectangle with a border at some defined coordinates

For additional configuration, see :doc:`areas`

.. code-block:: C++

  // ns-3 Rectangle from the Mobility Model
  // 5x5 area around the origin
  Rectangle start{-5.0, 5.0, -5.0, 5.0};
  auto startingArea = CreateObject<netsimulyzer::RectangularArea>(orchestrator, start);

  // The Rectangle may be constructed in place as well
  auto finishingArea = CreateObject<netsimulyzer::RectangularArea>(orchestrator, Rectangle{10.0, 7.0, 10.0, 7.0});


Adding Log Messages
+++++++++++++++++++

A :ref:`log-stream` may be used to output messages at a given time during the scenario.
A :ref:`log-stream` works similar to a C++ stream (e.g. ``std::cout``).

All messages on a :ref:`log-stream` must end with a newline '\\n'

.. code-block:: C++

  auto infoLog = CreateObject<netsimulyzer::LogStream> (orchestrator);

  // Optional, but highly recommended you set a name for each stream
  infoLog->SetAttribute ("Name", StringValue ("Info"));

  // Use like std::cout
  // Note the * at the beginning
  // and '\n' at the end of the message
  *infoLog << "Hello "
           << "world!\n";

  int number = 5;
  *infoLog << "Logs convert numbers to strings for you\n"
           << "See: " << number << '\n';

Adding Series
+++++++++++++

A :doc:`series` is a collection of points which may be displayed
on a chart in the application.

A series may be added to as the scenario runs and points
are added at the same time during playback as they were added in
the simulation.

There are several types of series, but the simplest is the :ref:`xy-series`:

.. code-block:: C++

  auto xy = CreateObject<netsimulyzer::XYSeries> (orchestrator);

  // Optional, but highly recommended
  xy->SetAttribute ("Name", StringValue ("XY Series Example"));

  // Default is `Line` (line graph),
  // there is also `None` (scatter plot)
  // and `Spline` (spline graph)
  xy->SetAttribute ("Connection", EnumValue (netsimulyzer::XYSeries::Line));

  // Points are added through `Append (x, y)` calls,
  // and may occur at any time
  // before or during the simulation
  xy->Append (1.0, 1.0);

For the other possible series, see :doc:`series`.
