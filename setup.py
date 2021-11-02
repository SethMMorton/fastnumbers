#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Std lib imports
import glob
import os

# Non-std lib imports
from setuptools import Extension, find_packages, setup


# Define how to build the extension module.
# All other data is in the setup.cfg file.
setup(
    name="fastnumbers",
    version="3.2.0",
    python_requires=">=3.6",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    package_data={"fastnumbers": ["py.typed", "*.pyi"]},
    zip_safe=False,
    ext_modules=[
        Extension(
            "fastnumbers.fastnumbers",
            sorted(glob.glob("src/*.c")),
            include_dirs=[os.path.abspath(os.path.join("include"))],
            extra_compile_args=[],
        )
    ],
)
