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
ruff = ["ruff", "format"]
if check:
    ruff.extend(["--quiet", "--check", "--diff"])
print(*map(shlex.quote, ruff))
ruff_ret = subprocess.run(ruff)

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
    text=True,
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
all_return_zero = all(ret.returncode == 0 for ret in [ruff_ret, clang_format_ret])
if any_cpp_formatting or not all_return_zero:
    sys.exit("Not all files are formatted correctly. Run 'tox -e format'.")
else:
    sys.exit(0)
