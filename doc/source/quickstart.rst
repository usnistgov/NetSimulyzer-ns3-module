Quickstart
==========

Clone the module
++++++++++++++++
Clone the project into a directory called visualizer3d in the `src` folder of a supported version of *ns-3*

.. code-block:: Bash

  git clone https://github.com/usnistgov/ns3-visualizer3d-module.git visualizer3d


Configure waf
+++++++++++++
Re-run the configuration step from a normal *ns-3* build to pickup the new module

.. code-block:: Bash

  ./waf configure


Add to a scenario
+++++++++++++++++

Include the module in your scenario. And optionally use the namespace.

.. code-block:: C++

  #include <ns3/visualizer3d-module.h>

  // For brevity in examples
  using namespace ns3;

  // Optional, not assumed in future examples
  using namespace ns3::visualizer3d;

Create an :doc:`orchestrator`, and pass it a name for the output file.
The :doc:`orchestrator` is the base for all visualizer3d elements.

.. code-block:: C++

  auto orchestrator = CreateObject<visualizer3d::Orchestrator> ("example.json");


Showing Nodes
+++++++++++++

Create a ``NodeConfigurationHelper``, set a model for the Nodes and ``Install()``
on the Nodes you wish to show.

For additional configuration options see :doc:`nodes`.

.. code-block:: C++

  visualizer3d::NodeConfigurationHelper nodeHelper{orchestrator};
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
  visualizer3d::BuildingConfigurationHelper buildingHelper{orchestrator};
  for (auto building = BuildingList::Begin (); building != BuildingList::End (); building++)
    buildingHelper.Install (*building);


Adding Decorations
++++++++++++++++++

For purely visual elements add a :ref:`decoration`. A :ref:`decoration`
is similar to a ``NodeConfiguration`` except its position is set manually.

For more information, see :doc:`decorations`

.. code-block:: C++

  auto decoration = CreateObject<visualizer3d::Decoration>(orchestrator);
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
  Rectangle start{-5.0, 5.0, -5.0, 5.0}
  auto startingArea = CreateObject<visualizer3d::RectangularArea>(orchestrator, start);

  // The Rectangle may be constructed in place as well
  auto finishingArea = CreateObject<visualizer3d::RectangularArea>(orchestrator, Rectangle{10.0, 7.0, 10.0, 7.0});


Adding Log Messages
+++++++++++++++++++

A :ref:`log-stream` may be used to output messages at a given time during the scenario.
A :ref:`log-stream` works similar to a C++ stream (e.g. ``std::cout``).

All messages on a :ref:`log-stream` must end with a newline '\\n'

.. code-block:: C++

  auto infoLog = CreateObject<visualizer3d::LogStream> (orchestrator);

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

  auto xy = CreateObject<visualizer3d::XYSeries> (orchestrator);

  // Optional, but highly recommended
  xy->SetAttribute ("Name", StringValue ("XY Series Example"));

  // Default is `Line` (line graph),
  // there is also `None` (scatter plot)
  // and `Spline` (spline graph)
  xy->SetAttribute ("Connection", EnumValue (visualizer3d::XYSeries::Line));

  // Points are added through `Append (x, y)` calls,
  // and may occur at any time
  // before or during the simulation
  xy->Append (1.0, 1.0);

For the other possible series, see :doc:`series`.
