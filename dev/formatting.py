#! /usr/bin/env python

"""
Cross-platform checking if code is appropriately formatted.
INTENDED TO BE CALLED FROM PROJECT ROOT, NOT FROM dev/!
"""

import glob
import shlex
import subprocess
import sys

check = "--check" in sys.argv

# Check that python code is formatted
black = ["black"]
if check:
    black.extend(["--quiet", "--check", "--diff"])
black.append(".")
print(*map(shlex.quote, black))
black_ret = subprocess.run(black)

# Check that C++ code is formatted
clang_format = [
    "clang-format",
    "--style=file:dev/clang-format.cfg",
    "--dry-run" if check else "-i",
]
cpp = glob.glob("src/cpp/*.cpp")
hpp = glob.glob("include/fastnumbers/*.hpp")
hpp += glob.glob("include/fastnumbers/parser/*.hpp")

clang_format = clang_format + cpp + hpp
print(*map(shlex.quote, clang_format))
clang_format_ret = subprocess.run(
    clang_format,
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    universal_newlines=True,
)
print(clang_format_ret.stdout)

# Stop here if not checking
if not check:
    sys.exit(0)

# Did any C++ files get formatted?
any_cpp_formatting = any(
    "clang-format-violations" in line for line in clang_format_ret.stdout.splitlines()
)

# "Roll up" what happened into a single exit code.
all_return_zero = all(ret.returncode == 0 for ret in [black_ret, clang_format_ret])
if any_cpp_formatting or not all_return_zero:
    sys.exit("Not all files are formatted correctly. Run 'tox -e format'.")
else:
    sys.exit(0)
