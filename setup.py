#! /usr/bin/env python
# -*- coding: utf-8 -*-

import glob
import os
import sys

from setuptools import Extension, find_packages, setup


# Compilation arguments are platform-dependent
if sys.platform == "win32":
    compile_args = [
        "/W4",  # Equivalent to -Wall -Wpedantic on GCC/Clang
        "/wd4100",  # don't warn about unreferenced formal parameters
        "/wd4127",  # don't warn about constant conditional expressions
    ]
    if "FN_DEBUG" in os.environ:
        compile_args.append("/Od")
        compile_args.append("/Z7")
    else:
        compile_args.append("/O2")
else:
    compile_args = [
        "-std=c++11",
        "-Wall",
        "-Weffc++",
        "-Wpedantic",
    ]
    if "FN_DEBUG" in os.environ:
        compile_args.append("-O0")
        compile_args.append("-g")
    else:
        compile_args.append("-O2")
    if sys.platform == "darwin":
        compile_args.append("-Wno-c++17-extensions")


ext = [
    Extension(
        "fastnumbers.fastnumbers",
        sorted(glob.glob("src/cpp/*.cpp")),
        include_dirs=[os.path.abspath(os.path.join("include"))],
        extra_compile_args=compile_args,
        extra_link_args=["-lm"],
    )
]

# Define how to build the extension module.
# All other data is in the setup.cfg file.
setup(
    name="fastnumbers",
    version="3.2.1",
    python_requires=">=3.7",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    package_data={"fastnumbers": ["py.typed", "*.pyi"]},
    zip_safe=False,
    ext_modules=ext,
)
