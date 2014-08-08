from __future__ import print_function, division

# Std lib imports
import sys
import os
from timeit import repeat

# Local import
from time_base import time_test

isintlike_re = '''\
import re
float_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
float_match = float_regex.match
int_regex = re.compile(r'[-+]?\d+$')
int_match = int_regex.match
def isintlike_re(x):
    """Function to simulate isintlike but with regular expressions."""
    try:
        if int_match(x):
            return True
        elif float_match(x):
            return float(x).is_integer()
        else:
            return False
    except TypeError:
        return int(x) == x
'''

isintlike_try = '''\
def isintlike_try(x):
    """Function to simulate isintlike but with try/except."""
    try:
        a = int(x)
    except ValueError:
        try:
            a = float(x)
        except ValueError:
            return False
        else:
            return a.is_integer()
    else:
        return a == float(x)
'''

time_test(['isintlike_re', isintlike_re],
          ['isintlike_try', isintlike_try],
          ['isintlike', 'from fastnumbers import isintlike'])
