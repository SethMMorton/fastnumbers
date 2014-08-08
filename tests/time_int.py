from __future__ import print_function, division

# Std lib imports
import sys
import os
from timeit import repeat

# Local import
from time_base import time_conv

int_re = '''\
import re
int_regex = re.compile(r'[-+]?\d+$')
int_match = int_regex.match
def int_re(x):
    """Function to simulate safe_int but with regular expressions."""
    try:
        if int_match(x):
            return int(x)
        else:
            return x
    except TypeError:
        return int(x)
'''

int_try = '''\
def int_try(x):
    """Function to simulate safe_int but with try/except."""
    try:
        return int(x)
    except ValueError:
        return x
'''

time_conv(['int_re', int_re],
          ['int_try', int_try],
          ['safe_int', 'from fastnumbers import safe_int'],
          ['fast_int', 'from fastnumbers import fast_int'])
