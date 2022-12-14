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
        "/O2",
    ]
else:
    compile_args = [
        "-std=c++11",
        "-Wall",
        "-Weffc++",
        "-Wpedantic",
        "-O2",
        # "-O0",
        # "-g",
    ]
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
setup(ext_modules=ext)
