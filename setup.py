#! /usr/bin/env python
# -*- coding: utf-8 -*-

import glob
import os

try:
    from Cython.Build import cythonize
except ImportError:
    USING_CYTHON = False
else:
    USING_CYTHON = True
from setuptools import Extension, find_packages, setup


if USING_CYTHON:
    ext = cythonize(
        [
            Extension(
                "fastnumbers.fastnumbers",
                sorted(glob.glob("src/cython/*.pyx") + glob.glob("src/*.c")),
                include_dirs=[os.path.abspath(os.path.join("include"))],
                extra_compile_args=[],
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
