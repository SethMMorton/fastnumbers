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

The timing tests presented below use the following function to run the timings::

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
        for x, y in zip(('re:', 'try:', 'safe:', 'fast:'), (regex, try_, safe, fast)):
            print('Float,', x, mean(repeat('{}(-41053.543028758302e100)'.format(y[0]), y[1], repeat=10)), 'seconds')
        print()
        for x, y in zip(('re:', 'try:', 'safe:', 'fast:'), (regex, try_, safe, fast)):
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
            print('Large Int String,', x, mean(repeat('{}("35892482945872302493")'.format(y[0]), y[1], repeat=10)), 'seconds')
        print()
        for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
            print('Float String,', x, mean(repeat('{}("-41053.543028758302")'.format(y[0]), y[1], repeat=10)), 'seconds')
        print()
        for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
            print('Float String with Exp,', x, mean(repeat('{}("-41053.543028758302e100")'.format(y[0]), y[1], repeat=10)), 'seconds')
        print()
        for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
            print('Float,', x, mean(repeat('{}(-41053.543028758302e100)'.format(y[0]), y[1], repeat=10)), 'seconds')
        print()
        for x, y in zip(('re:', 'try:', 'fast:'), (regex, try_, fast)):
            print('Int,', x, mean(repeat('{}(-41053)'.format(y[0]), y[1], repeat=10)), 'seconds')

Converting Floats
-----------------

The code to perform the `float` conversion timings is given below::

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

The following are the results::

    All timing results are the average of 10 runs.

    Non-number String, re: 0.616039919853 seconds
    Non-number String, try: 2.36470468044 seconds
    Non-number String, safe: 0.541815161705 seconds
    Non-number String, fast: 0.166932344437 seconds

    Int String, re: 1.41083431244 seconds
    Int String, try: 0.40917494297 seconds
    Int String, safe: 0.221786832809 seconds
    Int String, fast: 0.215631604195 seconds

    Large Int String, re: 1.98383982182 seconds
    Large Int String, try: 0.734673571587 seconds
    Large Int String, safe: 0.53988802433 seconds
    Large Int String, fast: 0.249987316132 seconds

    Float String, re: 1.82425656319 seconds
    Float String, try: 0.73917388916 seconds
    Float String, safe: 0.54944832325 seconds
    Float String, fast: 0.278426790237 seconds

    Float String with Exp, re: 2.08345327377 seconds
    Float String with Exp, try: 0.813772559166 seconds
    Float String with Exp, safe: 0.625898575783 seconds
    Float String with Exp, fast: 0.284848928452 seconds

    Float, re: 2.13188829422 seconds
    Float, try: 0.343280601501 seconds
    Float, safe: 0.14397521019 seconds
    Float, fast: 0.141120290756 seconds

    Int, re: 2.15410194397 seconds
    Int, try: 0.330206418037 seconds
    Int, safe: 0.150358200073 seconds
    Int, fast: 0.145005345345 seconds

Converting Ints
---------------

The code to perform the `int` conversion timings is given below::

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

The following are the results::

    All timing results are the average of 10 runs.

    Non-number String, re: 0.583729720116 seconds
    Non-number String, try: 3.65640561581 seconds
    Non-number String, safe: 1.07036044598 seconds
    Non-number String, fast: 0.164116621017 seconds

    Int String, re: 1.76469781399 seconds
    Int String, try: 0.917023706436 seconds
    Int String, safe: 0.264699578285 seconds
    Int String, fast: 0.199232268333 seconds

    Large Int String, re: 2.11528892517 seconds
    Large Int String, try: 1.0588845253 seconds
    Large Int String, safe: 0.409788441658 seconds
    Large Int String, fast: 0.211229872704 seconds

    Float String, re: 0.763830590248 seconds
    Float String, try: 3.63326253891 seconds
    Float String, safe: 1.07677755356 seconds
    Float String, fast: 0.169603705406 seconds

    Float String with Exp, re: 0.758796548843 seconds
    Float String with Exp, try: 3.66965932846 seconds
    Float String with Exp, safe: 1.1057393074 seconds
    Float String with Exp, fast: 0.166244912148 seconds

    Float, re: 2.19709262848 seconds
    Float, try: 0.483218431473 seconds
    Float, safe: 0.299236702919 seconds
    Float, fast: 0.296409606934 seconds

    Int, re: 1.99162778854 seconds
    Int, try: 0.328531575203 seconds
    Int, safe: 0.138215708733 seconds
    Int, fast: 0.13524055481 seconds

Checking Floats
---------------

The code to perform the `float` checking timings is given below::

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

    Non-number String, re: 1.09915692806 seconds
    Non-number String, try: 2.4588334322 seconds
    Non-number String, fast: 0.168157553673 seconds

    Int String, re: 1.43935678005 seconds
    Int String, try: 0.684446048737 seconds
    Int String, fast: 0.177501416206 seconds

    Large Int String, re: 1.64392886162 seconds
    Large Int String, try: 0.969433355331 seconds
    Large Int String, fast: 0.185720419884 seconds

    Float String, re: 1.5123660326 seconds
    Float String, try: 0.983824372292 seconds
    Float String, fast: 0.178205919266 seconds

    Float String with Exp, re: 1.61418132782 seconds
    Float String with Exp, try: 1.04610798359 seconds
    Float String with Exp, fast: 0.182296299934 seconds

    Float, re: 0.382601308823 seconds
    Float, try: 0.546687793732 seconds
    Float, fast: 0.151167201996 seconds

    Int, re: 0.394346261024 seconds
    Int, try: 0.569098591805 seconds
    Int, fast: 0.156042075157 seconds

Checking Ints
-------------

The code to perform the `int` checking timings is given below::

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

    Non-number String, re: 0.968136119843 seconds
    Non-number String, try: 3.47836313248 seconds
    Non-number String, fast: 0.153612947464 seconds

    Int String, re: 1.14891810417 seconds
    Int String, try: 1.10013346672 seconds
    Int String, fast: 0.154205584526 seconds

    Large Int String, re: 1.32925269604 seconds
    Large Int String, try: 1.29976191521 seconds
    Large Int String, fast: 0.169347310066 seconds

    Float String, re: 1.16640629768 seconds
    Float String, try: 3.48899214268 seconds
    Float String, fast: 0.155058121681 seconds

    Float String with Exp, re: 1.18829126358 seconds
    Float String with Exp, try: 3.52492365837 seconds
    Float String with Exp, fast: 0.157090616226 seconds

    Float, re: 0.39362449646 seconds
    Float, try: 0.649575018883 seconds
    Float, fast: 0.134729671478 seconds

    Int, re: 0.405252981186 seconds
    Int, try: 0.49396905899 seconds
    Int, fast: 0.133502268791 seconds
