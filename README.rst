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
if the string cannot be converted, the string is returned as-is instead
of returning a ``ValueError`` (although this behavior is customizable).

To achieve this, the module makes some assumptions about the input type
(input is ``int`` (or ``long``), ``float``, or ``str`` (or ``unicode``)),
and otherwise a ``TypeError`` is raised.

Examples
--------

It is probably easiest to illustrate ``fastnumbers`` in use rather than
describe it:

.. code-block:: python

    >>> from fastnumbers import safe_float
    >>> def float_no_raise(input):
    ...     try:
    ...         return float(input)
    ...     except ValueError:
    ...         return input
    ... 
    >>> safe_float('56.07')
    56.07
    >>> float_no_raise('56.07') == safe_float('56.07')
    True
    >>> safe_float('bad input')
    'bad input'
    >>> float_no_raise('bad input') == safe_float('bad input')
    True
    >>> safe_float(54)
    54.0
    >>> float_no_raise(54) == safe_float(54)
    True

If you really need speed, there are fast versions of the conversion
functions:

.. code-block:: python

    >>> from fastnumbers import fast_float
    >>> fast_float('56.07')
    56.07
    >>> safe_float('56.07') == fast_float('56.07')
    True

The difference between ``safe_float`` and ``fast_float`` is that the fast
version uses an extremely fast implementation of ``atof`` under the hood
that does not do overflow or underflow checking, and also can lose precision
around the 12th decimal place for extreme exponents; for the majority of
cases, the results will be identical.

If you don't want to return the input as-is for invalid input, you can
either set ``raise_on_invalid`` or ``default`` to some value:

.. code-block:: python

    >>> from fastnumbers import safe_float
    >>> safe_float('bad input', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
      ...
    ValueError: invalid literal for float(): bad input
    >>> safe_float('bad input', default=0.0)
    0.0

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

    import re
    from timeit import timeit
    float_match = re.compile(r'[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$').match
    float_try = '''\
    def float_try(input):
        """Typical approach to this problem."""
        try:
            return float(input)
        except ValueError:
            return input
    '''
    
    float_re = '''\
    def float_re(input):
        """Alternate approach to this problem."""
        try:
            if float_match(x):
                return float(x)
            else:
                return x
        except TypeError:
            return float(x)
    '''
    
    print('Invalid input:')
    print(timeit('float_try("invalid")', float_try))
    print(timeit('float_re("invalid")', float_re))
    print(timeit('safe_float("invalid"), 'from fastnumbers import safe_float'))
    print(timeit('fast_float("invalid"), 'from fastnumbers import fast_float'))
    print()
    print('Valid input:')
    print(timeit('float_try("56.07")', float_try))
    print(timeit('float_re("56.07")', float_re))
    print(timeit('safe_float("56.07"), 'from fastnumbers import safe_float'))
    print(timeit('fast_float("56.07"), 'from fastnumbers import fast_float'))

The results will be similar to the below, by vary on the system you are on::

    Invalid input:
    2.28478188515
    0.601616001129
    0.543533372879
    0.185416555405

    Valid input:
    0.774985694885
    1.7571870327
    0.584108567238
    0.275424480438

As you can see, in all cases ``fastnumbers`` beats the pure python
implementations.

Full Suite of Functions
-----------------------

In addition to ``safe_float`` and ``fast_float`` mentioned above, there are
also

    - safe_real
    - safe_int
    - safe_forceint
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

08-12-2014 v. 0.1.3
'''''''''''''''''''

    - Fixed bug where 'e' and 'E' were incorrectly identified as a valid
      float/int and converted to 0.  This bug only applied to the ``fast_*``
      and ``is*`` functions.
