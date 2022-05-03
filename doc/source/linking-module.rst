Linking the Module
==================


Simple Linking
--------------

To link this module to another module, or an example/scenario outside of ``scratch/``
simply add it to the list of connected modules and include it in code.

CMake
^^^^^
.. code-block:: cmake

  # Module
  build_lib(
    LIBNAME your-module
    SOURCE_FILES
      # ...
    HEADER_FILES
      # ...
    LIBRARIES_TO_LINK
      ${libnetsimulyzer}
      # ...
  )

  # Example
  build_lib_example(
    NAME your-scenario
    SOURCE_FILES scenario.cc
    LIBRARIES_TO_LINK
      ${libnetsimulyzer}
      # ...
  )


Waf (Legacy)
^^^^^^^^^^^^
.. code-block:: python

  # Scenario
  def build(bld):
    obj = bld.create_ns3_program('your-scenario',
                                 ['netsimulyzer', ...])

  # Module
  def build(bld):
    module = bld.create_ns3_module('your-module', ['netsimulyzer', ...])


Code
^^^^
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

If a module or scenario must be portable to versions of *ns-3* without
the ``netsimulyzer`` module, then the  build environment may be checked
for the ``HAS_NETSIMULYZER`` value (C++/Waf), or the `netsimulyzer` in the
enabled modules list (CMake). The presence of either of these variable indicates
the module is available in ns-3.

.. _cmake-guarded-linking:

CMake
^^^^^
Note: if the module linking to the NetSimulyzer module is in the `src/` directory,
then you'll need to add the `HAS_NETSIMULYZER` C++ define
yourself when you check for the presence of the module.
See the N.B. comment in the example below

.. code-block:: cmake

  # Create a list of your required modules to link
  # 'core' & 'mobility' are just examples here
  set(libraries_to_link "${libcore};${libmobility}")

  # Check if the `netsimulyzer` module is in the enabled modules list
  if("netsimulyzer" IN_LIST ns3-all-enabled-modules)
      # If it's there, then it's safe to add to the library list
      list(APPEND libraries_to_link ${libnetsimulyzer})

     # N.B if the module you're linking to is in the `src/` directory
     # of ns-3, then (at least for now),
     # you must also add the C++ define
     # yourself, like this.
     #
     # There's no harm in repeated definitions
     # of the same value, so there's no
     # need to guard this statement
     add_definitions(-DHAS_NETSIMULYZER)
  endif()

  # Use the `libraries_to_link` list as your dependency list

  # Module
  build_lib(
    LIBNAME your-module
    SOURCE_FILES
      # ...
    HEADER_FILES
      # ...
    LIBRARIES_TO_LINK
      ${libraries_to_link}
  )

  # Example
  build_lib_example(
    NAME your-scenario
    SOURCE_FILES scenario.cc
    LIBRARIES_TO_LINK
      ${libraries_to_link}
  )

Waf
^^^
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

Note, if you're using CMake and the module is in the ``src/`` directory, you may have to add
this definition yourself (``scratch/`` and module examples are fine).
See :ref:`the CMake Guarded Linking section <cmake-guarded-linking>` for an example of this.

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