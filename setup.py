#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Std lib imports
import re
from os.path import join, abspath

# Non-std lib imports
from setuptools import setup, Extension


DESCRIPTION = ("Efficiently perform string to number type "
               "conversion with error handling.")
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


# Create a list of all the source files
sourcefiles = [
    'check_number_is_number.c',
    'check_object_is_number.c',
    'check_string_is_number.c',
    'check_unicode_character_is_number.c',
    'convert_number_to_number.c',
    'convert_object_to_number.c',
    'convert_python_string_to_string.c',
    'convert_python_unicode_to_unicode_character.c',
    'convert_string_to_number.c',
    'convert_unicode_character_to_number.c',
    'parse_integer_from_string.c',
    'parse_float_from_string.c',
    'quick_float_might_overflow.c',
    'string_contains_integer.c',
    'string_contains_intlike_float.c',
    'string_contains_float.c',
    'fastnumbers.c',
]
sourcefiles = [join('src', sf) for sf in sourcefiles]


# Extension definition
ext = Extension('fastnumbers', sourcefiles,
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
        'Programming Language :: Python :: 2.6',
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
