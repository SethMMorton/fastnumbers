"""\
Copies doctest.py from the stdlib to the current directory,
and modifies it so that

  a) It will load "*.so" files as modules just like a "*.py" file
  b) It recognizes functions defined in "*.so" files

With these enhancements, doctest can be run on a C python extension module.
"""
from __future__ import print_function

import inspect
import doctest

# Get the source file location
dt_location = inspect.getsourcefile(doctest)

# Read the module into memory
with open(dt_location) as fl:
    doctest_str = fl.read()

# Add a search for the .so extension when inspecting the input files
# so that extension modules will be loaded properly.
doctest_str = doctest_str.replace(
    'if filename.endswith(".py"):',
    'if filename.endswith(".py") or filename.endswith(".so"):'
)

# inspect.isfunction does not work for functions written in C,
# so we have to replace that with an inspect.isbuiltin check when
# looking for functions with docstrings.
doctest_str = doctest_str.replace(
    'if ((inspect.isfunction(val) or inspect.isclass(val)) and',
    'if ((inspect.isbuiltin(val) or inspect.isclass(val)) and'
)

# Open up the new output file and write the modified input to it.
with open('doctest.py', 'w') as fl:
    print(doctest_str, file=fl, end='')
