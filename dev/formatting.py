#! /usr/bin/env python3
"""
Cross-platform checking if code is appropriately formatted.

INTENDED TO BE CALLED FROM PROJECT ROOT, NOT FROM dev/!
"""

from __future__ import annotations

import pathlib
import subprocess
import sys

check = "--check" in sys.argv

# Check that python code is formatted
ruff = ["ruff", "format"]
if check:
    ruff.extend(["--check", "--diff"])
ruff_ret = subprocess.run(ruff, check=False, text=True)

# Check that C++ code is formatted
clang_format: list[pathlib.Path | str] = [
    "clang-format",
    "--style=file:dev/clang-format.cfg",
    "--dry-run" if check else "-i",
]
cpp = list(pathlib.Path("src/cpp").glob("*.cpp"))
hpp = list(pathlib.Path("include/fastnumbers").glob("*.hpp"))
hpp.extend(pathlib.Path("include/fastnumbers/parser").glob("*.hpp"))

clang_format += cpp + hpp
clang_format_ret = subprocess.run(
    clang_format,
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    text=True,
    check=False,
)

# Stop here if not checking
if not check:
    sys.exit(0)

# Did any C++ files get formatted?
any_cpp_formatting = any(
    "clang-format-violations" in line for line in clang_format_ret.stdout.splitlines()
)

# "Roll up" what happened into a single exit code.
all_return_zero = all(ret.returncode == 0 for ret in [ruff_ret, clang_format_ret])
if any_cpp_formatting or not all_return_zero:
    sys.exit("Not all files are formatted correctly. Run 'tox run -e format'.")
else:
    sys.exit(0)
