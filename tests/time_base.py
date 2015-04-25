from __future__ import print_function, division

# Std lib imports
from timeit import repeat


def mean(x):
    return sum(x) / len(x)


def time_conv(regex, try_, fast):
    """\
    Run timing tests on multiple types of input,
    for multiple types of functions.
    """

    print('All timing results are the average of 10 runs.')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Non-number String,', x, mean(repeat('{}("not_a_number")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Int String,', x, mean(repeat('{}("-41053")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Large Int String,', x, mean(repeat('{}("35892482945872302493947939485729")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Float String,', x, mean(repeat('{}("-41053.543034e34")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Large Float String,', x, mean(repeat('{}("-41053.543028758302e256")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Float,', x, mean(repeat('{}(-41053.543028758302e100)'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Int,', x, mean(repeat('{}(-41053)'.format(y[0]), y[1], repeat=10)), 'seconds')


def time_test(regex, try_, fast):
    """\
    Run timing tests on multiple types of input,
    for multiple types of functions.
    """

    print('All timing results are the average of 10 runs.')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Non-number String,', x, mean(repeat('{}("not_a_number")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Int String,', x, mean(repeat('{}("-41053")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Large Int String,', x, mean(repeat('{}("35892482945872302493947939485729")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Float String,', x, mean(repeat('{}("-41053.543034e34")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Large Float String,', x, mean(repeat('{}("-41053.543028758302e256")'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Float,', x, mean(repeat('{}(-41053.543028758302e100)'.format(y[0]), y[1], repeat=10)), 'seconds')
    print()
    for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
        print('Int,', x, mean(repeat('{}(-41053)'.format(y[0]), y[1], repeat=10)), 'seconds')
