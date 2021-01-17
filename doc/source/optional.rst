Optional Values
===============
Some attributes in the module are allowed to remain entirely empty,
such as the default font color of a :ref:`log-stream`, ans still be valid.
These attributes are wrapped in the ``OptionalValue`` attribute type.

.. _optional-value:

OptionalValue
-------------
Representation of an attribute which may or may not be set.
The actual value of an ``OptionalValue`` is passed as a template
parameter.

Primarily, users should be concerned with how to set these values,
demonstrated below:

.. code-block:: C++

  // Set up a `LogStream`, which has
  // an attribute of this type.
  auto orchestrator = CreateObject<Orchestrator>("example.json");
  auto log = CreateObject<LogStream>(orchestrator);

  // Copy a value
  Color3 logColor{128u, 0u, 0u};
  log->SetAttribute("Color", OptionalValue<Color3>{logColor});


  // In-place construct a value
  // Calls the `Color3` constructor directly,
  // with the same result as the above
  log->SetAttribute("Color", OptionalValue<Color3>{128u, 0u, 0u});



An ``OptionalValue`` should be checked to see if it contains
a value before attempting to read it.

.. code-block:: C++

  OptionalValue<int> hasValue{42};

  // The `OptionalValue` is treated as
  // `true` if it contains a value and
  // `false` if it does not
  if (hasValue) {
    // Prints 42
    std::cout << hasValue.GetValue() << '\n'
  }


  OptionalValue<int> hasNoValue;

  // Treated as `false`, since no value
  // was set
  if (hasNoValue) {
    std::cout << "Will never print\n";
  }

  // The program will abort
  // if a nonexistent value is read
  std::cout << hasNoValue.GetValue();

