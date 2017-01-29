from __future__ import print_function, division

from time_base import comparison_timer

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

comparison_timer(['float_re', float_re],
                 ['float_try', float_try],
                 ['fast_float', 'from fastnumbers import fast_float'])
