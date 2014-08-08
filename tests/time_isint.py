from __future__ import print_function, division

# Std lib imports
import sys
import os
from timeit import repeat

# Local import
from time_base import time_test

isint_re = '''\
import re
int_regex = re.compile(r'[-+]?\d+$')
int_match = int_regex.match
nums = set([float, int])
def isint_re(x):
    """Function to simulate isint but with regular expressions."""
    t = type(x)
    return t == int if t in nums else bool(int_match(x))
'''

isint_try = '''\
def isint_try(x):
    """Function to simulate isint but with try/except."""
    try:
        int(x)
    except ValueError:
        return False
    else:
        return type(x) != float
'''

time_test(['isint_re', isint_re],
          ['isint_try', isint_try],
          ['isint', 'from fastnumbers import isint'])
