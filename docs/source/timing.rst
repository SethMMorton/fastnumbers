.. default-domain:: py
.. currentmodule:: fastnumbers

.. _timing:

Timing 
======

In order for you to see the benefit of ``fastnumbers``, some timings
are collected below for comparison to equivalent python implementations.
The numbers may change depending on the machine you are on. Feel free
to download the source code to run all timing tests.

Timing Runner
-------------

The timing tests presented below use the following function to run the timings:

.. code-block:: python

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

Converting Floats
-----------------

The code to perform the `float` conversion timings is given below:

.. code-block:: python

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
              ['fast_float', 'from fastnumbers import fast_float'])

The following are the results::

    All timing results are the average of 10 runs.

    Non-number String, re: 0.573668313026 seconds
    Non-number String, try: 2.18123717308 seconds
    Non-number String, fast: 0.169103908539 seconds

    Int String, re: 1.14506702423 seconds
    Int String, try: 0.376295471191 seconds
    Int String, fast: 0.198381090164 seconds

    Large Int String, re: 1.92678444386 seconds
    Large Int String, try: 0.756599807739 seconds
    Large Int String, fast: 0.650611639023 seconds

    Float String, re: 1.82797636986 seconds
    Float String, try: 0.687785792351 seconds
    Float String, fast: 0.242390632629 seconds

    Large Float String, re: 2.31674897671 seconds
    Large Float String, try: 1.07994887829 seconds
    Large Float String, fast: 0.970883083344 seconds

    Float, re: 1.93790380955 seconds
    Float, try: 0.333303022385 seconds
    Float, fast: 0.133689498901 seconds

    Int, re: 2.0137783289 seconds
    Int, try: 0.326768898964 seconds
    Int, fast: 0.140902447701 seconds

Converting Ints
---------------

The code to perform the `int` conversion timings is given below:

.. code-block:: python

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
              ['fast_int', 'from fastnumbers import fast_int'])

The following are the results::
    
    All timing results are the average of 10 runs.

    Non-number String, re: 0.513276076317 seconds
    Non-number String, try: 3.59187278748 seconds
    Non-number String, fast: 0.154757094383 seconds

    Int String, re: 1.62476005554 seconds
    Int String, try: 0.857665300369 seconds
    Int String, fast: 0.188158082962 seconds

    Large Int String, re: 2.24101734161 seconds
    Large Int String, try: 1.12569539547 seconds
    Large Int String, fast: 0.643681025505 seconds

    Float String, re: 0.732820224762 seconds
    Float String, try: 3.60975520611 seconds
    Float String, fast: 0.163253188133 seconds

    Large Float String, re: 0.729602479935 seconds
    Large Float String, try: 3.65623159409 seconds
    Large Float String, fast: 0.163104772568 seconds

    Float, re: 2.32226393223 seconds
    Float, try: 0.539329576492 seconds
    Float, fast: 0.290531635284 seconds

    Int, re: 2.01958138943 seconds
    Int, try: 0.326728582382 seconds
    Int, fast: 0.130790877342 seconds

Checking Floats
---------------

The code to perform the `float` checking timings is given below:

.. code-block:: python

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

The following are the results::

    All timing results are the average of 10 runs.

    Non-number String, re: 1.00798327923 seconds
    Non-number String, try: 2.1671749115 seconds
    Non-number String, fast: 0.161317801476 seconds

    Int String, re: 1.25955090523 seconds
    Int String, try: 0.568465399742 seconds
    Int String, fast: 0.174383997917 seconds

    Large Int String, re: 1.58316028118 seconds
    Large Int String, try: 0.962632489204 seconds
    Large Int String, fast: 0.204193854332 seconds

    Float String, re: 1.43826227188 seconds
    Float String, try: 0.947708177567 seconds
    Float String, fast: 0.176113319397 seconds

    Large Float String, re: 1.50998635292 seconds
    Large Float String, try: 1.2951467514 seconds
    Large Float String, fast: 0.182495999336 seconds

    Float, re: 0.384200811386 seconds
    Float, try: 0.526714110374 seconds
    Float, fast: 0.132399153709 seconds

    Int, re: 0.385046958923 seconds
    Int, try: 0.525576925278 seconds
    Int, fast: 0.138587331772 seconds

Checking Ints
-------------

The code to perform the `int` checking timings is given below:

.. code-block:: python

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

The following are the results::

    All timing results are the average of 10 runs.

    Non-number String, re: 0.967393517494 seconds
    Non-number String, try: 3.5071721077 seconds
    Non-number String, fast: 0.142234826088 seconds

    Int String, re: 1.07628540993 seconds
    Int String, try: 1.07323606014 seconds
    Int String, fast: 0.146803045273 seconds

    Large Int String, re: 1.39286680222 seconds
    Large Int String, try: 1.29413485527 seconds
    Large Int String, fast: 0.190896821022 seconds

    Float String, re: 1.15165970325 seconds
    Float String, try: 3.55873417854 seconds
    Float String, fast: 0.148036885262 seconds

    Large Float String, re: 1.15159604549 seconds
    Large Float String, try: 3.60109534264 seconds
    Large Float String, fast: 0.145488548279 seconds

    Float, re: 0.400713467598 seconds
    Float, try: 0.685648226738 seconds
    Float, fast: 0.129911446571 seconds

    Int, re: 0.405667829514 seconds
    Int, try: 0.530900597572 seconds
    Int, fast: 0.127823591232 seconds
