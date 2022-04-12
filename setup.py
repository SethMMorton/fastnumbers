#! /usr/bin/env python
# -*- coding: utf-8 -*-

import glob
import os
import sys

try:
    from Cython.Build import cythonize
except ImportError:
    USING_CYTHON = False
else:
    USING_CYTHON = True
from setuptools import Extension, find_packages, setup


# Compilation arguments are platform-dependent
if sys.platform == "win32":
    compile_args = [
        "/Wall",
        "/O2",
    ]
else:
    compile_args = [
        "-std=c++11",
        "-Wall",
        "-Weffc++",
        "-Wpedantic",
        "-O2",
    ]
    if sys.platform == "darwin":
        compile_args.append("-Wno-c++17-extensions")


if USING_CYTHON:
    ext = cythonize(
        [
            Extension(
                "fastnumbers.fastnumbers",
                sorted(glob.glob("src/cython/main.pyx") + glob.glob("src/cpp/*.cpp")),
                include_dirs=[os.path.abspath(os.path.join("include"))],
                extra_compile_args=compile_args,
                extra_link_args=["-lm"],
            )
        ],
        language_level="3",
        compiler_directives={
            "always_allow_keywords": False,
        },
    )
else:
    ext = None

# Define how to build the extension module.
# All other data is in the setup.cfg file.
setup(
    name="fastnumbers",
    version="3.2.1",
    python_requires=">=3.6",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    package_data={"fastnumbers": ["py.typed", "*.pyi"]},
    zip_safe=False,
    ext_modules=ext,
)
