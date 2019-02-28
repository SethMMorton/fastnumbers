fastnumbers
===========

.. image:: https://img.shields.io/pypi/v/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/pyversions/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/l/fastnumbers.svg
    :target: https://github.com/SethMMorton/fastnumbers/blob/master/LICENSE

.. image:: https://img.shields.io/travis/SethMMorton/fastnumbers/master.svg?label=travis-ci
    :target: https://travis-ci.org/SethMMorton/fastnumbers

.. image:: https://ci.appveyor.com/api/projects/status/5ahtcvmt3aoui3mw/branch/master?svg=true
    :target: https://ci.appveyor.com/project/SethMMorton/fastnumbers/branch/master

.. image:: https://codecov.io/gh/SethMMorton/fastnumbers/branch/master/graph/badge.svg
    :target: https://codecov.io/gh/SethMMorton/fastnumbers

.. image:: https://api.codacy.com/project/badge/Grade/7221f3d2be3147e9a975d604f1770cfb
    :target: https://www.codacy.com/app/SethMMorton/fastnumbers

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

**NOTICE**: The first major version release after Jan 1, 2020 will drop support for Python 2.7.

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

    - https://nbviewer.jupyter.org/github/SethMMorton/fastnumbers/blob/master/TIMING_27.ipynb
    - https://nbviewer.jupyter.org/github/SethMMorton/fastnumbers/blob/master/TIMING_35.ipynb
    - https://nbviewer.jupyter.org/github/SethMMorton/fastnumbers/blob/master/TIMING_36.ipynb

How Is ``fastnumbers`` So Fast?
-------------------------------

CPython goes to great lengths to ensure that your string input is converted to a
number *correctly* (you can prove this to yourself by examining the source code for
`integer conversions <https://github.com/python/cpython/blob/e349bf23584eef20e0d1e1b2989d9b1430f15507/Objects/longobject.c#L2213>`_
and for
`float conversions <https://github.com/python/cpython/blob/e349bf23584eef20e0d1e1b2989d9b1430f15507/Python/dtoa.c#L1434>`_),
but this extra effort is only needed for very large
integers or for floats with many digits or large exponents. For integers, if the
result could fit into a C ``long`` then a naive algorithm of < 10 lines of C code
is sufficient. For floats, if the number does not require high precision or does not
have a large exponent (such as "-123.45e6") then a short naive algorithm is also
possible.

These naive algorithms are quite fast, but the performance improvement comes at the
expense of being unsafe (no protection against overflow or round-off errors).
``fastnumbers`` uses a heuristic to determine if the input can be safely converted
with the much faster naive algorithm. These heuristics are extremely conservative -
if there is *any* chance that the naive result would not give *exactly* the same
result as the built-in functions then it will fall back on CPython's conversion
function. For this reason, ``fastnumbers`` is aways *at least as fast* as CPython's
built-in ``float`` and ``int`` functions, and oftentimes is significantly faster
because most real-world numbers pass the heuristic.

Installation
------------

Use ``pip``!

.. code-block::

    $ pip install fastnumbers

How to Run Tests
----------------

Please note that ``fastnumbers`` is NOT set-up to support ``python setup.py test``.

The recommended way to run tests is with `tox <https://tox.readthedocs.io/en/latest/>`_.
Suppose you want to run tests for Python 3.6 - you can run tests by simply executing the
following:

.. code-block:: sh

    $ tox -e py36

``tox`` will create virtual a virtual environment for your tests and install all the
needed testing requirements for you.

If you want to run testing on all of Python 2.7, 3.4, 3.5, 3.6, and 3.7 you can simply
execute

.. code-block:: sh

    $ tox

If you do not wish to use ``tox``, you can install the testing dependencies with the
``dev-requirements.txt`` file and then run the tests manually using
`pytest <https://docs.pytest.org/en/latest/>`_.

.. code-block:: sh

    $ pip install -r dev/requirements.txt
    $ pytest

Author
------

Seth M. Morton

History
-------

Please visit the `changelog <http://fastnumbers.readthedocs.io/en/master/changelog.html>`_.
