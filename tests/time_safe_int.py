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

int_re = '''\
import re
int_regex = re.compile(r'[-+]\d+$')
int_match = int_regex.match
def int_re(x):
    """Function to simulate safe_int but with regular expressions."""
    if int_match(x):
        return int(x)
    else:
        return x
'''

int_try = '''\
def int_try(x):
    """Function to simulate safe_int but with try/except."""
    try:
        return int(x)
    except:
        return x
'''

print('Non-number String,', 're:', timeit('int_re("not_a_number")', int_re))
print('Non-number String,', 'try:', timeit('int_try("not_a_number")', int_try))
print('Non-number String,', 'C:', timeit('safe_int("not_a_number")', 'from fastnumbers import safe_int'))
print()
print('Int String,', 're:', timeit('int_re("-41053")', int_re))
print('Int String,', 'try:', timeit('int_try("-41053")', int_try))
print('Int String,', 'C:', timeit('safe_int("-41053")', 'from fastnumbers import safe_int'))
print()
print('Float,', 'try:', timeit('int_try(-41053.565305)', int_try))
print('Float,', 'C:', timeit('safe_int(-41053.565305)', 'from fastnumbers import safe_int'))
print()
print('Int,', 'try:', timeit('int_try(-41053)', int_try))
print('Int,', 'C:', timeit('safe_int(-41053)', 'from fastnumbers import safe_int'))

