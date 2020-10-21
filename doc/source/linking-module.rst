Linking the Module
==================


Simple Linking
--------------

To link this module to another module, or an example/scenario outside of ``scratch/``
simply add it to the list of connected modules and include it in code.

.. code-block:: python

  # Scenario
  def build(bld):
    obj = bld.create_ns3_program('your-scenario',
                                 ['visualizer3d', ...])

  # Module
  def build(bld):
    module = bld.create_ns3_module('your-module', ['visualizer3d', ...])


Then the module is now available in code.

.. code-block:: C++

  #include <ns3/visualizer3d-module.h>

  // ...

  int main()
  {
    auto orchestrator = CreateObject<visualizer3d::Orchestrator> ("example.json");
    // ...
  }


Guarded Linking
---------------

Waf
^^^

If a module or scenario must be portable to versions of *ns-3* without
the ``visualizer3d`` module, then the  build environment may be checked
for the ``HAS_VISUALIZER3D`` value. The presence of this variable indicates
the module is available in ns-3.


.. code-block:: python

  def build(bld):
    # Create a list of your required modules to link
    # 'core' & 'mobility' are just examples here
    linked_modules = ['core', 'mobility']

    # Check if 'HAS_VISUALIZER3D' was defined during configuration
    if 'HAS_VISUALIZER3D' in bld.env:
        # If it was defined, then the 'visualizer3d' is present and we may link it
        linked_modules.append('visualizer3d')

    # Scenario
    obj = bld.create_ns3_program('your-scenario', linked_modules)

    # Module
    module = bld.create_ns3_module('your-module', linked_modules)


Code
^^^^

The code for the scenario/module must also be guarded, otherwise it would
fail to compile without the module. A C++ preprocessor value also
named ``HAS_VISUALIZER3D`` is defined if the module is present.

.. code-block:: C++

  // Guard the include with the macro
  #ifdef HAS_VISUALIZER3D
  #include <ns3/visualizer3d-module.h>
  #endif

  // ...

  int main ()
  {
    // ...

    // Guard any visualizer references in code with the macro as well
  #ifdef HAS_VISUALIZER3D
    auto orchestrator = CreateObject<visualizer3d::Orchestrator> ("example.json");
    // ...
  #endif
  }