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

forceint_re = '''\
import re
float_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
float_match = float_regex.match
int_regex = re.compile(r'[-+]\d+$')
int_match = int_regex.match
def forceint_re(x):
    """Function to simulate safe_forceint but with regular expressions."""
    if int_match(x):
        return int(x)
    elif float_match(x):
        return int(float(x))
    else:
        return x
'''

forceint_try = '''\
def forceint_try(x):
    """Function to simulate safe_forceint but with try/except."""
    try:
        return int(x)
    except ValueError:
        try:
            return int(float(x))
        except ValueError:
            return x
'''

def mean(x):
    return sum(x) / len(x)


print('Non-number String,', 're:', mean(repeat('forceint_re("not_a_number")', forceint_re)))
print('Non-number String,', 'try:', mean(repeat('forceint_try("not_a_number")', forceint_try)))
print('Non-number String,', 'safe:', mean(repeat('safe_forceint("not_a_number")', 'from fastnumbers import safe_forceint')))
print('Non-number String,', 'fast:', mean(repeat('fast_forceint("not_a_number")', 'from fastnumbers import fast_forceint')))
print()
print('Int String,', 're:', mean(repeat('forceint_re("-41053")', forceint_re)))
print('Int String,', 'try:', mean(repeat('forceint_try("-41053")', forceint_try)))
print('Int String,', 'safe:', mean(repeat('safe_forceint("-41053")', 'from fastnumbers import safe_forceint')))
print('Int String,', 'fast:', mean(repeat('fast_forceint("-41053")', 'from fastnumbers import fast_forceint')))
print()
print('Large Int String,', 're:', mean(repeat('forceint_re("35892482945872302493")', forceint_re)))
print('Large Int String,', 'try:', mean(repeat('forceint_try("35892482945872302493")', forceint_try)))
print('Large Int String,', 'safe:', mean(repeat('safe_forceint("35892482945872302493")', 'from fastnumbers import safe_forceint')))
print('Large Int String,', 'fast:', mean(repeat('fast_forceint("35892482945872302493")', 'from fastnumbers import fast_forceint')))
print()
print('Float String,', 're:', mean(repeat('forceint_re("-41053.543028758302")', forceint_re)))
print('Float String,', 'try:', mean(repeat('forceint_try("-41053.543028758302")', forceint_try)))
print('Float String,', 'safe:', mean(repeat('safe_forceint("-41053.543028758302")', 'from fastnumbers import safe_forceint')))
print('Float String,', 'fast:', mean(repeat('fast_forceint("-41053.543028758302")', 'from fastnumbers import fast_forceint')))
print()
print('Float String with Exp,', 're:', mean(repeat('forceint_re("-41053.543028758302e100")', forceint_re)))
print('Float String with Exp,', 'try:', mean(repeat('forceint_try("-41053.543028758302e100")', forceint_try)))
print('Float String with Exp,', 'safe:', mean(repeat('safe_forceint("-41053.543028758302e100")', 'from fastnumbers import safe_forceint')))
print('Float String with Exp,', 'fast:', mean(repeat('fast_forceint("-41053.543028758302e100")', 'from fastnumbers import fast_forceint')))
print()
print('Float,', 'try:', mean(repeat('forceint_try(-41053.543028758302e100)', forceint_try)))
print('Float,', 'safe:', mean(repeat('safe_forceint(-41053.543028758302e100)', 'from fastnumbers import safe_forceint')))
print('Float,', 'fast:', mean(repeat('fast_forceint(-41053.543028758302e100)', 'from fastnumbers import fast_forceint')))
print()
print('Int,', 'try:', mean(repeat('forceint_try(-41053)', forceint_try)))
print('Int,', 'safe:', mean(repeat('safe_forceint(-41053)', 'from fastnumbers import safe_forceint')))
print('Int,', 'fast:', mean(repeat('fast_forceint(-41053)', 'from fastnumbers import fast_forceint')))

