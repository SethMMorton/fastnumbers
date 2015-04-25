fastnumbers
===========

.. image:: https://travis-ci.org/SethMMorton/fastnumbers.svg?branch=master
    :target: https://travis-ci.org/SethMMorton/fastnumbers

Convert strings to numbers quickly.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.python.org/pypi/fastnumbers
    - Documentation: http://pythonhosted.org//fastnumbers/

This module is a Python C extension that will convert strings to
numbers *much* faster than can be done using pure Python.  Additionally,
if the string cannot be converted, instead of a ``ValueError`` the return
value can be either the input as-is or a default value.

To achieve this, the module makes some assumptions about the input type
(input is ``int`` (or ``long``), ``float``, or ``str`` (or ``unicode``)),
and otherwise a ``TypeError`` is raised.

**NOTE:** The ``safe_real``, ``safe_float``, ``safe_int``, and
``safe_forceint`` functions are deprecated as of ``fastnumbers`` version
>= 0.3.0; ``fast_real``, ``fast_float``, ``fast_int``, and ``fast_forceint``
have each been reimplemented to fall back on the "safe" algorithm if
overflow or loss of precision is detect and so the separate "safe" functions
are no longer needed.

Examples
--------

``fastnumbers`` is essentially a fast C implementation of the following
Pure Python function:

.. code-block:: python

    def fast_float(input, raise_on_invalid=False, default=None):
        try:
            return float(input)
        except ValueError:
            if raise_on_invalid:
                raise
            return default if default is not None else input

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
    >>> # Integers are converted to floats
    >>> fast_float(54)
    54.0
    >>> # The default built-in float behavior can be triggered with
    >>> # "raise_on_invalid" set to True. 
    >>> fast_float('bad input', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
      ...
    ValueError: invalid literal for float(): bad input

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

The results will be similar to the below, by vary on the system you are on::

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

Full Suite of Functions
-----------------------

In addition to ``fast_float`` mentioned above, there are also

    - fast_real
    - fast_int
    - fast_forceint
    - isreal
    - isfloat
    - isint
    - isintlike

Please see the `API Documentation <http://pythonhosted.org//fastnumbers/api.html>`_
for full details.

Author
------

Seth M. Morton

History
-------

These are the last three entries of the changelog.  See the package documentation
for the complete `changelog <http://pythonhosted.org//fastnumbers/changelog.html>`_.

04-23-2015 v. 0.3.0
'''''''''''''''''''

    - Updated the ``fast_*`` functions to check if an overflow
      loss of precision has occurred, and if so fall back on the
      more accurate number conversion method.
    - In response to the above change, the ``safe_*`` functions
      are now deprecated, and internally now use the same code as
      the ``fast_*`` functions.
    - Updated all unit testing to use the ``hypothesis`` module,
      which results in better test coverage.

09-03-2014 v. 0.2.0
'''''''''''''''''''

    - Added a 'default' option to the conversion functions.

08-12-2014 v. 0.1.4
'''''''''''''''''''

    - Fixed bug where '.' was incorrectly identified as a valid
      float/int and converted to 0.  This bug only applied to the ``fast_*``
      and ``is*`` functions.
    - The method to catch corner-cases like '.', '+', 'e', etc. has been
      reworked to be more general... case-by-case patches should no longer
      be needed.
