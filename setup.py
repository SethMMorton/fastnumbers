"""Rules for compilation of C++ extension module."""

from __future__ import annotations

import os
import pathlib
import sys

from setuptools import Extension, setup

# Compilation arguments are platform-dependent
link_args = ["-lm"]
if sys.platform == "win32":
    compile_args = [
        "/std:c++17",
        "/W4",  # Equivalent to -Wall -Wpedantic on GCC/Clang
        "/wd4100",  # don't warn about unreferenced formal parameters
        "/wd4127",  # don't warn about constant conditional expressions
    ]
    if "FN_DEBUG" in os.environ or "FN_COV" in os.environ:
        compile_args.append("/Od")
        compile_args.append("/Z7")
    if "FN_WARNINGS_AS_ERRORS" in os.environ:
        compile_args.append("/WX")
else:
    compile_args = [
        "-std=c++17",
        "-Wall",
        "-Weffc++",
        "-Wpedantic",
    ]
    if sys.platform == "darwin":
        compile_args.append("-mmacosx-version-min=10.13")
    if "FN_DEBUG" in os.environ or "FN_COV" in os.environ:
        compile_args.append("-Og")
        compile_args.append("-g")
        if "FN_COV" in os.environ:
            compile_args.append("--coverage")
            link_args.append("--coverage")
    if "FN_WARNINGS_AS_ERRORS" in os.environ:
        compile_args.append("-Werror")


ext = [
    Extension(
        "fastnumbers.fastnumbers",
        sorted(map(str, pathlib.Path("src/cpp").glob("*.cpp"))),
        include_dirs=[str(pathlib.Path("include").resolve())],
        extra_compile_args=compile_args,
        extra_link_args=link_args,
    )
]

# Define how to build the extension module.
# All other data is in the pyproject.toml file.
setup(ext_modules=ext)
