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

real_re = '''\
import re
real_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
real_match = real_regex.match
int_regex = re.compile(r'[-+]\d+$')
int_match = int_regex.match
def real_re(x):
    """Function to simulate safe_real but with regular expressions."""
    if int_match(x):
        return int(x)
    elif real_match(x):
        return float(x)
    else:
        return x
'''

real_try = '''\
def real_try(x):
    """Function to simulate safe_real but with try/except."""
    try:
        a = float(x)
    except ValueError:
        return x
    else:
        b = int(a)
        return b if a == b else b
'''

def mean(x):
    return sum(x) / len(x)


print('Non-number String,', 're:', mean(repeat('real_re("not_a_number")', real_re)))
print('Non-number String,', 'try:', mean(repeat('real_try("not_a_number")', real_try)))
print('Non-number String,', 'safe:', mean(repeat('safe_real("not_a_number")', 'from fastnumbers import safe_real')))
print('Non-number String,', 'fast:', mean(repeat('fast_real("not_a_number")', 'from fastnumbers import fast_real')))
print()
print('Int String,', 're:', mean(repeat('real_re("-41053")', real_re)))
print('Int String,', 'try:', mean(repeat('real_try("-41053")', real_try)))
print('Int String,', 'safe:', mean(repeat('safe_real("-41053")', 'from fastnumbers import safe_real')))
print('Int String,', 'fast:', mean(repeat('fast_real("-41053")', 'from fastnumbers import fast_real')))
print()
print('Large Int String,', 're:', mean(repeat('real_re("35892482945872302493")', real_re)))
print('Large Int String,', 'try:', mean(repeat('real_try("35892482945872302493")', real_try)))
print('Large Int String,', 'safe:', mean(repeat('safe_real("35892482945872302493")', 'from fastnumbers import safe_real')))
print('Large Int String,', 'fast:', mean(repeat('fast_real("35892482945872302493")', 'from fastnumbers import fast_real')))
print()
print('Float String,', 're:', mean(repeat('real_re("-41053.543028758302")', real_re)))
print('Float String,', 'try:', mean(repeat('real_try("-41053.543028758302")', real_try)))
print('Float String,', 'safe:', mean(repeat('safe_real("-41053.543028758302")', 'from fastnumbers import safe_real')))
print('Float String,', 'fast:', mean(repeat('fast_real("-41053.543028758302")', 'from fastnumbers import fast_real')))
print()
print('Float String with Exp,', 're:', mean(repeat('real_re("-41053.543028758302e100")', real_re)))
print('Float String with Exp,', 'try:', mean(repeat('real_try("-41053.543028758302e100")', real_try)))
print('Float String with Exp,', 'safe:', mean(repeat('safe_real("-41053.543028758302e100")', 'from fastnumbers import safe_real')))
print('Float String with Exp,', 'fast:', mean(repeat('fast_real("-41053.543028758302e100")', 'from fastnumbers import fast_real')))
print()
print('Float,', 'try:', mean(repeat('real_try(-41053.543028758302e100)', real_try)))
print('Float,', 'safe:', mean(repeat('safe_real(-41053.543028758302e100)', 'from fastnumbers import safe_real')))
print('Float,', 'fast:', mean(repeat('fast_real(-41053.543028758302e100)', 'from fastnumbers import fast_real')))
print()
print('Int,', 'try:', mean(repeat('real_try(-41053)', real_try)))
print('Int,', 'safe:', mean(repeat('safe_real(-41053)', 'from fastnumbers import safe_real')))
print('Int,', 'fast:', mean(repeat('fast_real(-41053)', 'from fastnumbers import fast_real')))

