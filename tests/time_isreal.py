from __future__ import print_function, division

# Local import
from time_base import comparison_timer

isreal_re = '''\
import re
real_regex = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$')
real_match = real_regex.match
nums = set([float, int])
def isreal_re(x):
    """Function to simulate isreal but with regular expressions."""
    return type(x) in nums or bool(real_match(x))
'''

isreal_try = '''\
def isreal_try(x):
    """Function to simulate isreal but with try/except."""
    try:
        float(x)
    except ValueError:
        return False
    else:
        return True
'''

comparison_timer(['isreal_re', isreal_re],
                 ['isreal_try', isreal_try],
                 ['isreal', 'from fastnumbers import isreal'])
