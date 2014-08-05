from __future__ import print_function, division

# Std lib imports
import sys
import os
from timeit import repeat

# Local import
from time_base import time_test

isfloat_re = '''\
import re
float_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
float_match = float_regex.match
nums = set([float, int])
def isfloat_re(x):
    """Function to simulate isfloat but with regular expressions."""
    t = type(x)
    return t == float if t in nums else bool(float_match(x))
'''

isfloat_try = '''\
def isfloat_try(x):
    """Function to simulate isfloat but with try/except."""
    try:
        float(x)
    except ValueError:
        return False
    else:
        return type(x) != int
'''

time_test(['isfloat_re', isfloat_re],
          ['isfloat_try', isfloat_try],
          ['isfloat', 'from fastnumbers import isfloat'])
