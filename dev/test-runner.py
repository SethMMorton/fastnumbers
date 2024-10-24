#! /usr/bin/env python3
"""
Run tests using gdb as a means to catch segfaults.

If gdb is not installed, it just runs the tests.
"""

from __future__ import annotations

import os
import sys

my_python = sys.argv[1]
other_args = sys.argv[2:]
try:
    # Don't use gdb unless requesting debugging mode
    if "FN_DEBUG" not in os.environ:
        raise OSError  # noqa: TRY301

    # Attempt to run pytest with debugger
    # fmt: off
    os.execlp(
        "gdb",
        "gdb",
        "-ex", "run",
        "-ex", "bt",
        "-ex", "quit",
        "--args", my_python,
        "-m", "pytest",
        "--doctest-glob=README.rst",
        *other_args,
    )
    # fmt: on
except OSError:
    # No debugger installed, just run pytest directly
    os.execl(
        my_python, my_python, "-m", "pytest", "--doctest-glob=README.rst", *other_args
    )
