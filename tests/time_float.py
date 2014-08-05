from __future__ import print_function, division

# Std lib imports
import sys
import os
from timeit import repeat

# Local import
from time_base import time_conv

float_re = '''\
import re
float_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
float_match = float_regex.match
def float_re(x):
    """Function to simulate safe_float but with regular expressions."""
    try:
        if float_match(x):
            return float(x)
        else:
            return x
    except TypeError:
        return float(x)
'''

float_try = '''\
def float_try(x):
    """Function to simulate safe_float but with try/except."""
    try:
        return float(x)
    except ValueError:
        return x
'''

time_conv(['float_re', float_re],
          ['float_try', float_try],
          ['safe_float', 'from fastnumbers import safe_float'],
          ['fast_float', 'from fastnumbers import fast_float'])

