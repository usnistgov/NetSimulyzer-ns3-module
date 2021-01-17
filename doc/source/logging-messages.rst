Logging Messages
================

.. _log-stream:

LogStream
---------

``LogStream`` objects allow for messages to be printed directly
to the application. Messages printed to a ``LogStream``
are output during playback at the time they were emitted.
``LogStream`` objects may be useful for printing scenario information
such as marking the beginning of important events.


A ``Name`` may be provided to a ``LogStream`` to more
clearly identify it in the application.


Messages
--------

A ``LogStream`` message **must** end in a newline character '\\n',
and may contain single characters (e.g. 'c'), other strings
(e.g. "Hello world!"), and anything which may be passed to
`std::to_string <https://en.cppreference.com/w/cpp/string/basic_string/to_string>`_.
If greater control is desired for number formatting, then
the user must convert the number to a string by hand.


Example
-------

.. code-block:: C++

  auto stream = CreateObject<LogStream> (/* the orchestrator */);

  // Note the '\n'
  // and the *
  *stream << "Hello World\n";

  // A Message may be divided up
  // as long as it ends in a
  // newline '\n'
  *stream << "Hello " << "World!" << '\n';

  int randomNumber = 4;
  *stream << "Conversion for numbers"
             " happens automatically."
             " See: " << randomNumber
             << '\n';



Attributes
----------

+----------------------+----------------------------------------+--------------------+------------------------------------------+
| Name                 | Type                                   | Default Value      | Description                              |
+======================+========================================+====================+==========================================+
| Name                 | string                                 |                n/a | Name to use for this ``LogSteam`` in     |
|                      |                                        |                    | application elements                     |
+----------------------+----------------------------------------+--------------------+------------------------------------------+
| Color                |:ref:`optional-value` <:ref:`color3`>   | n/a - (decided by  | The font color used in the application.  |
|                      |                                        | the application)   | If not provided, then drawn from the     |
|                      |                                        |                    | application's Qt theme                   |
+----------------------+----------------------------------------+--------------------+------------------------------------------+
| Visible              | bool                                   |              true  | Defines if the ``LogStream`` may be      |
|                      |                                        |                    | selected individually in the application |
+----------------------+----------------------------------------+--------------------+------------------------------------------+

