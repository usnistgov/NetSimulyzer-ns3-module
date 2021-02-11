#! /usr/bin/env python3
# -*- coding: utf-8 -*-
## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# A list of C++ examples to run in order to ensure that they remain
# buildable and runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run, do_valgrind_run).
#
# See test.py for more information.
cpp_examples = [
    ("mobility-buildings-example", "True", "True"),
    ("lena-radio-link-failure-netsimulyzer --simTime=20 --numberOfEnbs=2 --visual=true", "True", "True"),
    ("outdoor-random-walk-example-netsimulyzer", "True", "True"),
    ("wifi-bianchi-netsimulyzer --trials=1 --nMinStas=10 --nMaxStas=10 --visual=true", "True", "True"),
]

# A list of Python examples to run in order to ensure that they remain
# runnable over time.  Each tuple in the list contains
#
#     (example_name, do_run).
#
# See test.py for more information.
python_examples = []
