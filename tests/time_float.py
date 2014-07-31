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

float_re = '''\
import re
float_regex = re.compile(r'([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)$')
float_match = float_regex.match
def float_re(x):
    """Function to simulate safe_float but with regular expressions."""
    if float_match(x):
        return float(x)
    else:
        return x
'''

float_try = '''\
def float_try(x):
    """Function to simulate safe_float but with try/except."""
    try:
        return float(x)
    except:
        return x
'''

print('Non-number String,', 're:', timeit('float_re("not_a_number")', float_re))
print('Non-number String,', 'try:', timeit('float_try("not_a_number")', float_try))
print('Non-number String,', 'safe:', timeit('safe_float("not_a_number")', 'from fastnumbers import safe_float'))
print('Non-number String,', 'fast:', timeit('fast_float("not_a_number")', 'from fastnumbers import fast_float'))
print()
print('Int String,', 're:', timeit('float_re("-41053")', float_re))
print('Int String,', 'try:', timeit('float_try("-41053")', float_try))
print('Int String,', 'safe:', timeit('safe_float("-41053")', 'from fastnumbers import safe_float'))
print('Int String,', 'fast:', timeit('fast_float("-41053")', 'from fastnumbers import fast_float'))
print()
print('Float String,', 're:', timeit('float_re("-41053.543028758302")', float_re))
print('Float String,', 'try:', timeit('float_try("-41053.543028758302")', float_try))
print('Float String,', 'safe:', timeit('safe_float("-41053.543028758302")', 'from fastnumbers import safe_float'))
print('Float String,', 'fast:', timeit('fast_float("-41053.543028758302")', 'from fastnumbers import fast_float'))
print()
print('Float String with Exp,', 're:', timeit('float_re("-41053.543028758302e100")', float_re))
print('Float String with Exp,', 'try:', timeit('float_try("-41053.543028758302e100")', float_try))
print('Float String with Exp,', 'safe:', timeit('safe_float("-41053.543028758302e100")', 'from fastnumbers import safe_float'))
print('Float String with Exp,', 'fast:', timeit('fast_float("-41053.543028758302e100")', 'from fastnumbers import fast_float'))
print()
print('Float,', 'try:', timeit('float_try(-41053.543028758302e100)', float_try))
print('Float,', 'safe:', timeit('safe_float(-41053.543028758302e100)', 'from fastnumbers import safe_float'))
print('Float,', 'fast:', timeit('fast_float(-41053.543028758302e100)', 'from fastnumbers import fast_float'))
print()
print('Int,', 'try:', timeit('float_try(-41053)', float_try))
print('Int,', 'safe:', timeit('safe_float(-41053)', 'from fastnumbers import safe_float'))
print('Int,', 'fast:', timeit('fast_float(-41053)', 'from fastnumbers import fast_float'))

