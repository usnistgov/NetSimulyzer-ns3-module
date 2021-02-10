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
                                 ['netsimulyzer', ...])

  # Module
  def build(bld):
    module = bld.create_ns3_module('your-module', ['netsimulyzer', ...])


Then the module is now available in code.

.. code-block:: C++

  #include <ns3/netsimulyzer-module.h>

  // ...

  int main()
  {
    auto orchestrator = CreateObject<netsimulyzer::Orchestrator> ("example.json");
    // ...
  }


Guarded Linking
---------------

Waf
^^^

If a module or scenario must be portable to versions of *ns-3* without
the ``netsimulyzer`` module, then the  build environment may be checked
for the ``HAS_NETSIMULYZER`` value. The presence of this variable indicates
the module is available in ns-3.


.. code-block:: python

  def build(bld):
    # Create a list of your required modules to link
    # 'core' & 'mobility' are just examples here
    linked_modules = ['core', 'mobility']

    # Check if 'HAS_NETSIMULYZER' was defined during configuration
    if 'HAS_NETSIMULYZER' in bld.env:
        # If it was defined, then the 'netsimulyzer' is present and we may link it
        linked_modules.append('netsimulyzer')

    # Scenario
    obj = bld.create_ns3_program('your-scenario', linked_modules)

    # Module
    module = bld.create_ns3_module('your-module', linked_modules)


Code
^^^^

The code for the scenario/module must also be guarded, otherwise it would
fail to compile without the module. A C++ preprocessor value also
named ``HAS_NETSIMULYZER`` is defined if the module is present.

.. code-block:: C++

  // Guard the include with the macro
  #ifdef HAS_NETSIMULYZER
  #include <ns3/netsimulyzer-module.h>
  #endif

  // ...

  int main ()
  {
    // ...

    // Guard any visualizer references in code with the macro as well
  #ifdef HAS_NETSIMULYZER
    auto orchestrator = CreateObject<netsimulyzer::Orchestrator> ("example.json");
    // ...
  #endif
  }