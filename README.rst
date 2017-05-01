fastnumbers
===========

.. image:: https://img.shields.io/travis/SethMMorton/fastnumbers/master.svg?label=travis-ci
    :target: https://travis-ci.org/SethMMorton/fastnumbers

.. image:: https://ci.appveyor.com/api/projects/status/5ahtcvmt3aoui3mw/branch/master?svg=true
    :target: https://ci.appveyor.com/project/SethMMorton/fastnumbers/branch/master)

.. image:: https://codecov.io/gh/SethMMorton/fastnumbers/branch/master/graph/badge.svg
    :target: https://codecov.io/gh/SethMMorton/fastnumbers

.. image:: https://api.codacy.com/project/badge/Grade/7221f3d2be3147e9a975d604f1770cfb
    :target: https://www.codacy.com/app/SethMMorton/fastnumbers

.. image:: https://img.shields.io/pypi/pyversions/fastnumbers.svg
    :target: https://pypi.python.org/pypi/fastnumbers

.. image:: https://img.shields.io/pypi/format/fastnumbers.svg
    :target: https://pypi.python.org/pypi/fastnumbers

.. image:: https://img.shields.io/pypi/l/fastnumbers.svg
    :target: https://github.com/SethMMorton/fastnumbers/blob/master/LICENSE

Super-fast and clean conversions to numbers.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.python.org/pypi/fastnumbers
    - Documentation: http://fastnumbers.readthedocs.io/

``fastnumbers`` is a module with the following three objectives:

    #. Provide drop-in replacements for the Python built-in ``int`` and
       ``float`` that on average is up to 2x faster. These functions
       should be *identically* to the Python built-ins except for a few
       specific corner-cases as mentioned in the
       `API documentation <http://fastnumbers.readthedocs.io/en/stable/api.html>`_.
    #. Provide a set of convenience functions that wraps the above
       ``int`` and ``float`` replacements and provides easy, concise,
       powerful, fast and flexible error handling.
    #. Provide a set of functions that can be used to rapidly identify if
       an input *could* be converted to *int* or *float*.

Examples
--------

The below examples showcase the ``fast_float`` function, which is
a fast conversion functions with error-handling.
Please see the
`API Documentation <http://fastnumbers.readthedocs.io/en/stable/api.html>`_
for other functions that are available from ``fastnumbers``.

.. code-block:: python

    >>> from fastnumbers import fast_float, float as fnfloat
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
Below are the timing results for the ``fast_float`` and ``float`` function
on Python 2.7; please see the
`Timing Documentation <http://fastnumbers.readthedocs.io/en/stable/timing.html>`_
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
    print("try:", timeit('float_try("invalid")', float_try))
    print("re:", timeit('float_re("invalid")', float_re))
    print("fast", timeit('fast_float("invalid")', 'from fastnumbers import fast_float'))
    print()
    print('Valid input:')
    print("try:", timeit('float_try("56.07e14")', float_try))
    print("re:", timeit('float_re("56.07e14")', float_re))
    print("fast", timeit('fast_float("56.07e14")', 'from fastnumbers import fast_float'))
    print()
    print('Built-in float compared to fastnumbers.float:')
    print("Built-in:", timeit('float("56.07e14")'))
    print("fastnumbers:", timeit('float("56.07e14")', 'from fastnumbers import float'))
    print()

The results will be similar to below, but vary based on your system::

    Invalid input:
    try: 2.09141492844
    re: 0.724852085114
    fast 0.181249141693

    Valid input:
    try: 0.365114927292
    re: 1.42145609856
    fast 0.228940963745

    Built-in float compared to fastnumbers.float:
    Built-in: 0.234441041946
    fastnumbers: 0.228511810303

As you can see, in all cases ``fastnumbers`` beats the pure python
implementations (although not *always* significant).

Author
------

Seth M. Morton

History
-------

These are the last three entries of the changelog.  See the package documentation
for the complete `changelog <http://fastnumbers.readthedocs.io/en/stable/changelog.html>`_.

04-30-2017 v. 2.0.1
'''''''''''''''''''

    - Fixed bug in decimal digit limit on GCC.

04-30-2017 v. 2.0.0
'''''''''''''''''''

    - Dropped support for Python 2.6.
    - Added support for Python 3.6 underscores.
    - Added drop-in replacements for the built-in ``int()`` and ``float()`` functions.
    - Incorporated unit tests from Python's testing library to ensure that any
      input that Python can handle will also be handled the same way by ``fastnumbers``.
    - Added Appveyor testing to ensure no surprises on Windows.
    - Revamped documentation.
    - Refactored internal mechanism for assessing overflow to be faster in the most
      common cases.

04-23-2016 v. 1.0.0
'''''''''''''''''''

    - "coerce" in ``fast_real`` now applies to any input, not just numeric;
      the default is now *True* instead of *False*.
    - Now all ASCII whitespace characters are stripped by fastnumbers
    - Typechecking is now more forgiving
    - fastnumbers now checks for errors when converting between numeric types
    - Fixed bug where very small numbers are not converted properly
    - Testing now includes Python 2.6.
    - Removed ``safe_*`` functions (which were deprecated since version 0.3.0)
    - Fixed unicode handling on Windows.
    - Fixed Python2.6 on Windows.

03-19-2016 v. 0.7.4
'''''''''''''''''''

    - Added the "coerce" option to fast_real.
