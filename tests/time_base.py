from __future__ import print_function, division

# Std lib imports
from timeit import repeat


def mean(x):
    return sum(x) / len(x)


def time_conv(regex, try_, safe, fast):
    """\
    Run timing tests on multiple types of input,
    for multiple types of functions.
    """

    print('All timing results are the average of 10 runs.')
    print()
    for x, y in zip(('re:', 'try:', 'safe:', 'fast:'), (regex, try_, safe, fast)):
        print('Non-number String,', x, mean(repeat('{}("not_a_number")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'safe:', 'fast:'), (regex, try_, safe, fast)):
        print('Int String,', x, mean(repeat('{}("-41053")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'safe:', 'fast:'), (regex, try_, safe, fast)):
        print('Large Int String,', x, mean(repeat('{}("35892482945872302493")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'safe:', 'fast:'), (regex, try_, safe, fast)):
        print('Float String,', x, mean(repeat('{}("-41053.543028758302")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'safe:', 'fast:'), (regex, try_, safe, fast)):
        print('Float String with Exp,', x, mean(repeat('{}("-41053.543028758302e100")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('try:', 'safe:', 'fast:'), (try_, safe, fast)):
        print('Float,', x, mean(repeat('{}(-41053.543028758302e100)'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('try:', 'safe:', 'fast:'), (try_, safe, fast)):
        print('Int,', x, mean(repeat('{}(-41053)'.format(y[0]), y[1], repeat=10)), 'seconds')
