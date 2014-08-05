#! /usr/bin/env python

# Std lib imports
import re
import sys
from os.path import join, abspath

# Non-std lib imports
from setuptools import setup, Extension, find_packages
from setuptools.command.test import test as TestCommand


DESCRIPTION = "Quickly convert strings to number types."
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
        retcode = pytest.main(['--doctest-glob', '"*.so"'])
        sys.exit(retcode | pytest.main(['README.rst']))


# Create a list of all the source files
sourcefiles = ['fast_atoi.c', 'fast_atof.c',
               'fast_atoi_test.c', 'fast_atof_test.c', 
               'convenience.c', 'fastnumbers.c']
sourcefiles = [join('src', sf) for sf in sourcefiles]


# Extension definition
ext = Extension('fastnumbers', sourcefiles,
                include_dirs=[abspath(join('include'))])


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
      tests_require=['pytest'],
      cmdclass={'test': PyTest,},
      classifiers=('Development Status :: 4 - Beta',
                   #'Development Status :: 5 - Production/Stable',
                   'Intended Audience :: Science/Research',
                   'Intended Audience :: Developers',
                   'Operating System :: OS Independent',
                   'License :: OSI Approved :: MIT License',
                   'Natural Language :: English',
                   'Programming Language :: Python :: 2.6',
                   'Programming Language :: Python :: 2.7',
                   'Programming Language :: Python :: 3',
                   'Topic :: Utilities',
                   )
)
