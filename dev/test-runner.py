#! /usr/bin/env python
"""
This script will run tests using gdb as a means to catch segfaults.

If gdb is not installed, it just runs the tests.
"""

import os
import sys

my_python = sys.argv[1]
other_args = sys.argv[2:]
try:
    # Don't use gdb unless requesting debugging mode
    if "FN_DEBUG" not in os.environ:
        raise OSError

    # Attempt to run pytest with debugger
    os.execlp(
        "gdb",
        "gdb",
        "-ex",
        "run",
        "-ex",
        "bt",
        "-ex",
        "quit",
        "--args",
        my_python,
        "-m",
        "pytest",
        "--doctest-glob=README.rst",
        *other_args,
    )
except OSError:
    # No debugger installed, just run pytest directly
    os.execl(
        my_python, my_python, "-m", "pytest", "--doctest-glob=README.rst", *other_args
    )
