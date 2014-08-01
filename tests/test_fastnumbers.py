# Find the build location and add that to the path
from __future__ import print_function, division
import sys
import os
from sysconfig import get_platform, get_python_version
distutilsname = 'lib.' + '-'.join([get_platform(),
                                   get_python_version()
                                   ])
fastnumberspath = os.path.join('build', distutilsname, 'fastnumbers')
sys.path.append(fastnumberspath)
import fastnumbers
from pytest import raises


def test_version():
    assert hasattr(fastnumbers, '__version__')


def test_safe_float():
    assert fastnumbers.safe_float('64') == float('64')
    assert fastnumbers.safe_float('-64.9e-24') == float('-64.9e-24')
    assert fastnumbers.safe_float('+64.9') == float('+64.9')
    assert fastnumbers.safe_float('not_a_number') == 'not_a_number'
    assert fastnumbers.safe_float(64) == float(64)
    assert isinstance(fastnumbers.safe_float(64), float)
    with raises(TypeError):
        fastnumbers.safe_float(['hey'])


def test_safe_int():
    assert fastnumbers.safe_int('64') == int('64')
    assert fastnumbers.safe_int('-64') == int('-64')
    assert fastnumbers.safe_int('not_a_number') == 'not_a_number'
    assert fastnumbers.safe_int(64) == 64
    assert fastnumbers.safe_int(64l) == 64l
    assert isinstance(fastnumbers.safe_int(64l), long)
    assert fastnumbers.safe_int(64.62) == 64
    assert fastnumbers.safe_int('65.4') == '65.4'
    assert isinstance(fastnumbers.safe_int(64.5), int)
    with raises(TypeError):
        fastnumbers.safe_int(['hey'])


def test_fast_float():
    assert fastnumbers.fast_float('-64.9e-24') == float('-64.9e-24')
    assert fastnumbers.fast_float('+64.9') == float('+64.9')
    assert fastnumbers.fast_float('not_a_number') == 'not_a_number'
    assert fastnumbers.fast_float(64) == 64.0
    assert fastnumbers.fast_float(64l) == 64.0
    assert fastnumbers.fast_float(64.46) == 64.46
    assert isinstance(fastnumbers.fast_float(64), float)
    with raises(TypeError):
        fastnumbers.safe_int(['hey'])
    assert fastnumbers.fast_float('23.7 lb') == '23.7 lb'


def test_fast_int():
    assert fastnumbers.fast_int('64') == int('64')
    assert fastnumbers.fast_int('-64') == int('-64')
    assert fastnumbers.fast_int('not_a_number') == 'not_a_number'
    assert fastnumbers.fast_int(64) == 64
    assert fastnumbers.fast_int(64.62) == 64
    assert fastnumbers.fast_int('65.4') == '65.4'
    assert isinstance(fastnumbers.fast_int(64.5), int)
    with raises(TypeError):
        fastnumbers.safe_int(['hey'])
    assert fastnumbers.fast_float('23 lb') == '23 lb'
