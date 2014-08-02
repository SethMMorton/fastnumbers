from __future__ import print_function, division

# Std lib imports
from timeit import repeat

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
float_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
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
    except ValueError:
        return x
'''

def mean(x):
    return sum(x) / len(x)


print('Non-number String,', 're:', mean(repeat('float_re("not_a_number")', float_re)))
print('Non-number String,', 'try:', mean(repeat('float_try("not_a_number")', float_try)))
print('Non-number String,', 'safe:', mean(repeat('safe_float("not_a_number")', 'from fastnumbers import safe_float')))
print('Non-number String,', 'fast:', mean(repeat('fast_float("not_a_number")', 'from fastnumbers import fast_float')))
print()
print('Int String,', 're:', mean(repeat('float_re("-41053")', float_re)))
print('Int String,', 'try:', mean(repeat('float_try("-41053")', float_try)))
print('Int String,', 'safe:', mean(repeat('safe_float("-41053")', 'from fastnumbers import safe_float')))
print('Int String,', 'fast:', mean(repeat('fast_float("-41053")', 'from fastnumbers import fast_float')))
print()
print('Float String,', 're:', mean(repeat('float_re("-41053.543028758302")', float_re)))
print('Float String,', 'try:', mean(repeat('float_try("-41053.543028758302")', float_try)))
print('Float String,', 'safe:', mean(repeat('safe_float("-41053.543028758302")', 'from fastnumbers import safe_float')))
print('Float String,', 'fast:', mean(repeat('fast_float("-41053.543028758302")', 'from fastnumbers import fast_float')))
print()
print('Float String with Exp,', 're:', mean(repeat('float_re("-41053.543028758302e100")', float_re)))
print('Float String with Exp,', 'try:', mean(repeat('float_try("-41053.543028758302e100")', float_try)))
print('Float String with Exp,', 'safe:', mean(repeat('safe_float("-41053.543028758302e100")', 'from fastnumbers import safe_float')))
print('Float String with Exp,', 'fast:', mean(repeat('fast_float("-41053.543028758302e100")', 'from fastnumbers import fast_float')))
print()
print('Float,', 'try:', mean(repeat('float_try(-41053.543028758302e100)', float_try)))
print('Float,', 'safe:', mean(repeat('safe_float(-41053.543028758302e100)', 'from fastnumbers import safe_float')))
print('Float,', 'fast:', mean(repeat('fast_float(-41053.543028758302e100)', 'from fastnumbers import fast_float')))
print()
print('Int,', 'try:', mean(repeat('float_try(-41053)', float_try)))
print('Int,', 'safe:', mean(repeat('safe_float(-41053)', 'from fastnumbers import safe_float')))
print('Int,', 'fast:', mean(repeat('fast_float(-41053)', 'from fastnumbers import fast_float')))

