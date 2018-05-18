#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Std lib imports
import os
import glob

# Non-std lib imports
from setuptools import setup, Extension


# Define how to build the extension module.
# All other data is in the setup.cfg file.
setup(
    name='fastnumbers',
    version='2.0.4',
    ext_modules=[
        Extension(
            'fastnumbers', glob.glob('src/*.c'),
            include_dirs=[os.path.abspath(os.path.join('include'))],
            extra_compile_args=[]
        )
    ],
)
