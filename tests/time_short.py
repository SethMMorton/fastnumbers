from __future__ import print_function, division

from timeit import timeit
float_try = '''\
def float_try(input):
    """Typical approach to this problem."""
    try:
        return float(input)
    except ValueError:
        return input
'''

float_re = '''\
import re
float_match = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$').match
def float_re(input):
    """Alternate approach to this problem."""
    try:
        if float_match(input):
            return float(input)
        else:
            return input
    except TypeError:
        return float(input)
'''

print('Invalid input:')
print("Try:", timeit('float_try("invalid")', float_try))
print("re:", timeit('float_re("invalid")', float_re))
print("fast", timeit('fast_float("invalid")', 'from fastnumbers import fast_float'))
print()
print('Valid input:')
print("try:", timeit('float_try("56.07")', float_try))
print("re:", timeit('float_re("56.07")', float_re))
print("fast", timeit('fast_float("56.07")', 'from fastnumbers import fast_float'))