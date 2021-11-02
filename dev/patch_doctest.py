"""
Copies doctest.py from the stdlib to the current directory,
and modifies it so that

  a) It will load "*.so" files as modules just like a "*.py" file
  b) It recognizes functions defined in "*.so" files
  c) Remove the configuration extension from the "*.so" files (on Python3)

With these enhancements, doctest can be run on a C python extension module.
"""
from __future__ import print_function

import doctest
import inspect
import sys

# Get the source file location
dt_location = inspect.getsourcefile(doctest)

# Read the module into memory
with open(dt_location) as fl:
    doctest_str = fl.read()

# Let's add the glob module.
# Also define a function to detect if we could import this module name.
doctest_str = doctest_str.replace(
    "import __future__",
    "import __future__\nimport glob\n"
    "def can_import_module(name):\n"
    "    try:\n"
    "        __import__(name)\n"
    "    except ImportError:\n"
    "        return False\n"
    "    else:\n"
    "        return True\n",
)

# Add a search for the .so extension when inspecting the input files
# so that extension modules will be loaded properly.
doctest_str = doctest_str.replace(
    'if filename.endswith(".py"):',
    'if filename.endswith((".py", ".so", ".pyd")) or can_import_module(filename):',
)

# inspect.isfunction does not work for functions written in C,
# so we have to replace that with an inspect.isbuiltin check when
# looking for functions with docstrings.
doctest_str = doctest_str.replace(
    "if ((inspect.isfunction(val) or inspect.isclass(val)) and",
    "if ((inspect.isfunction(val) or inspect.isbuiltin(val) "
    "or inspect.isclass(val)) and",
)

# Replace the configuration extension with nothing on Python3
if sys.version[0] == "3":
    doctest_str = doctest_str.replace(
        "m = __import__(filename[:-3])",
        'm = __import__(filename[:-3] if filename.endswith(".py") '
        'else filename.replace(get_config_var("EXT_SUFFIX"), ""))',
    )
    # We need to import the get_config_var variable.
    doctest_str = doctest_str.replace(
        "def _test():", "from sysconfig import get_config_var\ndef _test():"
    )
else:
    doctest_str = doctest_str.replace(
        "m = __import__(filename[:-3])",
        "m = __import__(filename[:-3] "
        'if filename.endswith((".py", ".so", ".pyd")) else filename)',
    )

# To silence mypy
doctest_str = "# type: ignore\n" + doctest_str

# Open up the new output file and write the modified input to it.
with open("doctest.py", "w") as fl:
    print(doctest_str, file=fl, end="")
