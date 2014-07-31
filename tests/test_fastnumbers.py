# Find the build location and add that to the path
import sys
import os
from sysconfig import get_platform, get_python_version
distutilsname = 'lib.' + '-'.join([get_platform(),
                                   get_python_version()
                                   ])
fastnumberspath = os.path.join('build', distutilsname, 'fastnumbers')
sys.path.append(fastnumberspath)
import fastnumbers


def test_version():
    assert hasattr(fastnumbers, '__version__')


def test_asfloat():
    assert fastnumbers.asfloat('64') == float('64')
    assert fastnumbers.asfloat('-64.9e-24') == float('-64.9e-24')
    assert fastnumbers.asfloat('+64.9') == float('+64.9')
    assert fastnumbers.asfloat('not_a_number') == 'not_a_number'
    assert fastnumbers.asfloat(64) == float(64)
