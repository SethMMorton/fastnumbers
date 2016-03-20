fastnumbers
===========

.. image:: https://travis-ci.org/SethMMorton/fastnumbers.svg?branch=master
    :target: https://travis-ci.org/SethMMorton/fastnumbers

Convert strings to numbers quickly.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.python.org/pypi/fastnumbers
    - Documentation: http://pythonhosted.org//fastnumbers/

This module is a Python C extension that will convert strings to
numbers *much* faster than can be done using pure Python; numeric types
can also be converted to other numeric types.

Additionally, the user has control over what happens in the event that the
input string cannot be converted to a number:

    - the input can be returned as-is (this is the default behavior)
    - the input can be passed to a user-given key function then returned
    - a ``ValueError`` can be raised (like the built-in ``float`` or ``int``)
    - a default value can be returned

Examples
--------

``fastnumbers`` contains functions that are fast C implementations similar
to the following Pure Python function:

.. code-block:: python

    def fast_float(input, default=None, raise_on_invalid=False, key=None, inf=None, nan=None):
        import math
        try:
            x = float(input)
        except ValueError:
            if raise_on_invalid:
                raise
            elif key is not None:
                return key(input)
            return default if default is not None else input
        else:
            if inf is not None and math.isinf(x):
                return inf
            elif nan is not None and math.isnan(x):
                return nan
            else:
                return x

In addition to ``fast_float``, there are also ``fast_real``,
``fast_int``, ``fast_forceint``, ``isreal``, ``isfloat``, ``isint``, 
and ``isintlike`` - please see the
`API Documentation <http://pythonhosted.org//fastnumbers/api.html>`_
for full details.

Some example usage:

.. code-block:: python

    >>> from fastnumbers import fast_float
    >>> # Convert string to a float
    >>> fast_float('56.07')
    56.07
    >>> # Unconvertable string returned as-is by default
    >>> fast_float('bad input')
    'bad input'
    >>> # Unconvertable strings can trigger a default value
    >>> fast_float('bad input', default=0)
    0
    >>> # 'default' is also the first optional positional arg
    >>> fast_float('bad input', 0)
    0
    >>> # Integers are converted to floats
    >>> fast_float(54)
    54.0
    >>> # One can ask inf or nan to be substituted with another value
    >>> fast_float('nan')
    nan
    >>> fast_float('nan', nan=0.0)
    0.0
    >>> fast_float(float('nan'), nan=0.0)
    0.0
    >>> fast_float('56.07', nan=0.0)
    56.07
    >>> # The default built-in float behavior can be triggered with
    >>> # "raise_on_invalid" set to True. 
    >>> fast_float('bad input', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
      ...
    ValueError: invalid literal for float(): bad input
    >>> # A key function can be used to return an alternate value for invalid input
    >>> fast_float('bad input', key=len)
    9
    >>> fast_float(54, key=len)
    54.0
    >>> # Single unicode characters can be converted.
    >>> fast_float(u'\u2164')  # Roman numeral 5 (V)
    5.0
    >>> fast_float(u'\u2466')  # 7 enclosed in a circle
    7.0

**NOTE**: If you need locale-dependent conversions, supply the ``fastnumbers``
function of your choice to ``locale.atof``.

.. code-block:: python

    import locale
    locale.setlocale(locale.LC_ALL, 'de_DE.UTF-8')
    print(atof('468,5', func=fast_float))  # Prints 468.5

Timing
------

Just how much faster is ``fastnumbers`` than a pure python implementation?
Below are the timing results for the ``*_float`` functions; please see the
`Timing Documentation <http://pythonhosted.org//fastnumbers/timing.html>`_
for details into all timing results.

.. code-block:: python

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

The results will be similar to below, but vary based on your system::

    Invalid input:
    Try: 2.27156710625
    re: 0.570491075516
    fast 0.173984050751

    Valid input:
    try: 0.378665924072
    re: 1.08740401268
    fast 0.204708099365

As you can see, in all cases ``fastnumbers`` beats the pure python
implementations.

Author
------

Seth M. Morton

History
-------

These are the last three entries of the changelog.  See the package documentation
for the complete `changelog <http://pythonhosted.org//fastnumbers/changelog.html>`_.

03-19-2016 v. 0.7.4
'''''''''''''''''''

    - Added the "coerce" option to fast_real.

03-08-2016 v. 0.7.3
'''''''''''''''''''

    - Newline is now considered to be whitespace (for consistency with
      the builtin float and int).

03-07-2016 v. 0.7.2
'''''''''''''''''''

    - Fixed overflow bug in exponential parts of floats.
