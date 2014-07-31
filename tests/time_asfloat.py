from __future__ import print_function, division

# Std lib imports
from timeit import timeit

# Find the build location and add that to the path
import sys
import os
from sysconfig import get_platform, get_python_version
distutilsname = 'lib.' + '-'.join([get_platform(),
                                   get_python_version()
                                   ])
fastnumberspath = os.path.join('build', distutilsname, 'fastnumbers')
sys.path.append(fastnumberspath)

asfloat_re = '''\
import re
float_re = re.compile(r'([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)$')
float_match = float_re.match
def asfloat_re(x):
    """Function to simulate asfloat but with regular expressions."""
    if float_match(x):
        return float(x)
    else:
        return x
'''

asfloat_try = '''\
def asfloat_try(x):
    """Function to simulate asfloat but with try/except."""
    try:
        return float(x)
    except:
        return x
'''

print('Non-number String,', 're:', timeit('asfloat_re("not_a_number")', asfloat_re))
print('Non-number String,', 'try:', timeit('asfloat_try("not_a_number")', asfloat_try))
print('Non-number String,', 'C:', timeit('asfloat("not_a_number")', 'from fastnumbers import asfloat'))
print()
print('Int String,', 're:', timeit('asfloat_re("-41053")', asfloat_re))
print('Int String,', 'try:', timeit('asfloat_try("-41053")', asfloat_try))
print('Int String,', 'C:', timeit('asfloat("-41053")', 'from fastnumbers import asfloat'))
print()
print('Float String,', 're:', timeit('asfloat_re("-41053.543028758302")', asfloat_re))
print('Float String,', 'try:', timeit('asfloat_try("-41053.543028758302")', asfloat_try))
print('Float String,', 'C:', timeit('asfloat("-41053.543028758302")', 'from fastnumbers import asfloat'))
print()
print('Float String with Exp,', 're:', timeit('asfloat_re("-41053.543028758302e100")', asfloat_re))
print('Float String with Exp,', 'try:', timeit('asfloat_try("-41053.543028758302e100")', asfloat_try))
print('Float String with Exp,', 'C:', timeit('asfloat("-41053.543028758302e100")', 'from fastnumbers import asfloat'))

