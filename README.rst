fastnumbers
===========

.. image:: https://img.shields.io/travis/SethMMorton/fastnumbers/master.svg?label=travis-ci
    :target: https://travis-ci.org/SethMMorton/fastnumbers

.. image:: https://ci.appveyor.com/api/projects/status/5ahtcvmt3aoui3mw/branch/master?svg=true
    :target: https://ci.appveyor.com/project/SethMMorton/fastnumbers/branch/master

.. image:: https://codecov.io/gh/SethMMorton/fastnumbers/branch/master/graph/badge.svg
    :target: https://codecov.io/gh/SethMMorton/fastnumbers

.. image:: https://api.codacy.com/project/badge/Grade/7221f3d2be3147e9a975d604f1770cfb
    :target: https://www.codacy.com/app/SethMMorton/fastnumbers

.. image:: https://img.shields.io/pypi/pyversions/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/format/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/l/fastnumbers.svg
    :target: https://github.com/SethMMorton/fastnumbers/blob/master/LICENSE

Super-fast and clean conversions to numbers.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.org/project/fastnumbers/
    - Documentation: http://fastnumbers.readthedocs.io/

``fastnumbers`` is a module with the following three objectives:

    #. Provide drop-in replacements for the Python built-in ``int`` and
       ``float`` that on average are up to 2x faster. These functions
       should behave *identically* to the Python built-ins except for a few
       specific corner-cases as mentioned in the
       `API documentation <http://fastnumbers.readthedocs.io/en/master/api.html>`_.
    #. Provide a set of convenience functions that wrap the above
       ``int`` and ``float`` replacements and provides easy, concise,
       powerful, fast and flexible error handling.
    #. Provide a set of functions that can be used to rapidly identify if
       an input *could* be converted to *int* or *float*.

Examples
--------

The below examples showcase the ``fast_float`` function, which is
a fast conversion function with error-handling.
Please see the
`API Documentation <http://fastnumbers.readthedocs.io/en/master/api.html>`_
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
Please see the following Jupyter notebooks for timing information on various Python versions.

    - https://github.com/SethMMorton/fastnumbers/blob/master/TIMING_27.ipynb
    - https://github.com/SethMMorton/fastnumbers/blob/master/TIMING_35.ipynb
    - https://github.com/SethMMorton/fastnumbers/blob/master/TIMING_36.ipynb

Author
------

Seth M. Morton

History
-------

These are the last three entries of the changelog. See the package documentation
for the complete `changelog <http://fastnumbers.readthedocs.io/en/master/changelog.html>`_.

11-11-2017 v. 2.0.2
'''''''''''''''''''

    - Updated docstrings.
    - Improved timing documentation.
    - Added testing for Python 3.7.

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
