#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Std lib imports
import re
import glob
from os.path import join, abspath

# Non-std lib imports
from setuptools import setup, Extension


DESCRIPTION = "Super-fast and clean conversions to numbers."
try:
    with open('README.rst') as fl:
        LONG_DESCRIPTION = fl.read()
except IOError:
    LONG_DESCRIPTION = DESCRIPTION


def current_version():
    '''Get the current version number'''
    VERSIONFILE = join('include', 'version.h')
    with open(VERSIONFILE, "rt") as fl:
        versionstring = fl.read()
    m = re.search(r"#define \w+_VERSION \"(.*)\"", versionstring)
    if m:
        return m.group(1)
    else:
        s = "Unable to locate version string in {0}"
        raise RuntimeError(s.format(VERSIONFILE))


# Extension definition
ext = Extension('fastnumbers', glob.glob('src/*.c'),
                include_dirs=[abspath(join('include'))],
                extra_compile_args=[])


# Define the build
setup(name='fastnumbers',
      version=current_version(),
      author='Seth M. Morton',
      author_email='drtuba78@gmail.com',
      url='https://github.com/SethMMorton/fastnumbers',
      license='MIT',
      ext_modules=[ext],
      description=DESCRIPTION,
      long_description=LONG_DESCRIPTION,
      classifiers=(
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Science/Research',
        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',
        'Intended Audience :: System Administrators',
        'Intended Audience :: Financial and Insurance Industry',
        'Operating System :: OS Independent',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Topic :: Scientific/Engineering :: Information Analysis',
        'Topic :: Utilities',
        'Topic :: Text Processing',
        'Topic :: Text Processing :: Filters',
    )
)
