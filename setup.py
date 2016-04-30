#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Std lib imports
import re
import sys
import os
from os.path import join, abspath
from fnmatch import filter as ffilter
from glob import glob
from shutil import rmtree

# Non-std lib imports
from setuptools import setup, Extension, find_packages, Command
from setuptools.command.test import test as TestCommand


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


class PyTest(TestCommand):
    '''Define how to use pytest to test the code'''

    def finalize_options(self):
        TestCommand.finalize_options(self)
        self.test_args = []
        self.test_suite = True

    def run_tests(self):
        #import here, cause outside the eggs aren't loaded
        import pytest
        if sys.version[:3] == '2.6':
            sys.exit(pytest.main(['tests/test_fastnumbers_examples.py',
                                  'README.rst']))
        else:
            sys.exit(pytest.main(['--doctest-glob', 'README.rst']))


class Distclean(Command):
    description = "custom clean command that fully cleans directory tree"
    user_options = []

    def initialize_options(self):
        self.cwd = None

    def finalize_options(self):
        self.cwd = os.getcwd()

    def run(self):
        dirs = glob("*.egg-info") + ['build', 'dist']
        files = glob("*.so") + ['doctest.py']
        for root, dirnames, filenames in os.walk(os.getcwd()):
            for filename in ffilter(filenames, '*.py[co]'):
                files.append(os.path.join(root, filename))
            for dirname in ffilter(dirnames, '__pycache__')+ffilter(dirnames, 'xml'):
                dirs.append(os.path.join(root, dirname))
        
        for f in files:
            try:
                os.remove(f)
            except OSError:
                pass
        
        for d in dirs:
            try:
                rmtree(d)
            except OSError:
                pass


# Create a list of all the source files
sourcefiles = ['parse_integer_from_string.c',
               'parse_float_from_string.c',
               'string_contains_integer.c',
               'string_contains_intlike_float.c', 
               'string_contains_float.c', 
               'string_contains_non_overflowing_float.c', 
               'parsing.c',
               'py_to_char.c',
               'py_shortcuts.c',
               'fastnumbers.c',
               ]
sourcefiles = [join('src', sf) for sf in sourcefiles]


# Define what the tests require.
test_requires = ['pytest']
if sys.version[:3] != '2.6':
    test_requires.append('hypothesis')


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
      tests_require=test_requires,
      cmdclass={'test': PyTest, 'distclean': Distclean},
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
        'Topic :: Scientific/Engineering :: Information Analysis',
        'Topic :: Utilities',
        'Topic :: Text Processing',
        'Topic :: Text Processing :: Filters',
    )
)
