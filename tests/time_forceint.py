from __future__ import print_function, division

# Std lib imports
import sys
import os
from timeit import repeat

# Local import
from time_base import time_conv

forceint_re = '''\
import re
float_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
float_match = float_regex.match
int_regex = re.compile(r'[-+]\d+$')
int_match = int_regex.match
def forceint_re(x):
    """Function to simulate safe_forceint but with regular expressions."""
    try:
        if int_match(x):
            return int(x)
        elif float_match(x):
            return int(float(x))
        else:
            return x
    except TypeError:
        return int(x)
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

time_conv(['forceint_re', forceint_re],
          ['forceint_try', forceint_try],
          ['safe_forceint', 'from fastnumbers import safe_forceint'],
          ['fast_forceint', 'from fastnumbers import fast_forceint'])
