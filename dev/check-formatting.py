#! /usr/bin/env python

"""
Cross-platform checking if code is appropriately formatted.
INTENDED TO BE CALLED FROM PROJECT ROOT, NOT FROM dev/!
"""

import shlex
import subprocess
import sys

# Check that python code is formatted
black = ["black", "--quiet", "--check", "--diff", "."]
print(*map(shlex.quote, black))
black_ret = subprocess.run(["black", "--quiet", "--check", "--diff", "."])

# Check that C code is formatted
astyle = [
    "astyle",
    "--recursive",
    "--options=dev/astyle.cfg",
    "--suffix=none",
    "--dry-run",
]
astyle_c = astyle + ["src/*.c"]
print(*map(shlex.quote, astyle_c))
astyle_c_ret = subprocess.run(
    astyle_c, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True
)
print(astyle_c_ret.stdout)

# Check that headers are formatted
astyle_h = astyle + [
    "--exclude=docstrings.h",
    "--exclude=fn_bool.h",
    "--exclude=options.h",
    "--exclude=pstdint.h",
    "include/fastnumbers/*.h",
]
print(*map(shlex.quote, astyle_h))
astyle_h_ret = subprocess.run(
    astyle_h, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True
)
print(astyle_h_ret.stdout)

# Did any C files get formatted?
any_c_formatting = any(
    line.startswith("Formatted") for line in astyle_c_ret.stdout.splitlines()
)
any_h_formatting = any(
    line.startswith("Formatted") for line in astyle_h_ret.stdout.splitlines()
)

# "Roll up" what happened into a single exit code.
all_return_zero = all(
    ret.returncode == 0 for ret in [black_ret, astyle_c_ret, astyle_h_ret]
)
if any_c_formatting or not all_return_zero:
    sys.exit("Not all files are formatted correctly. Run 'tox -e format'.")
else:
    sys.exit(0)
