fastnumbers
===========

.. image:: https://img.shields.io/pypi/v/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/pyversions/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/l/fastnumbers.svg
    :target: https://github.com/SethMMorton/fastnumbers/blob/master/LICENSE

.. image:: https://img.shields.io/travis/SethMMorton/fastnumbers/master.svg?label=travis-ci
    :target: https://travis-ci.com/SethMMorton/fastnumbers

.. image:: https://codecov.io/gh/SethMMorton/fastnumbers/branch/master/graph/badge.svg
    :target: https://codecov.io/gh/SethMMorton/fastnumbers

.. image:: https://api.codacy.com/project/badge/Grade/7221f3d2be3147e9a975d604f1770cfb
    :target: https://www.codacy.com/app/SethMMorton/fastnumbers

Super-fast and clean conversions to numbers.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.org/project/fastnumbers/
    - Documentation: https://fastnumbers.readthedocs.io/
    - `Quick Start`_
    - `Timing`_
    - `High-level Algorithm`_
    - `How To Run Tests`_
    - `History`_

``fastnumbers`` is a module with the following three objectives (in order
of decreasing importance as to why the module was created):

    #. Provide a set of convenience functions that wrap the above
       ``int`` and ``float`` replacements and provides easy, concise,
       powerful, fast and flexible error handling.
    #. Provide a set of functions that can be used to rapidly identify if
       an input *could* be converted to *int* or *float*.
    #. Provide drop-in replacements for the Python built-in ``int`` and
       ``float`` that on average are *up to* 2x faster. These functions
       should behave *identically* to the Python built-ins except for a few
       specific corner-cases as mentioned in the
       `API documentation for those functions <https://fastnumbers.readthedocs.io/en/master/api.html#the-built-in-replacement-functions>`_.

       - **PLEASE** read the quick start for these functions to fully
         understand the caveats before using them.

**NOTICE**: As of ``fastnumbers`` version 3.0.0, only Python >= 3.5 is
supported.

Quick Start
-----------

- `Error-handling Functions`_
- `Checking Functions`_
- `Drop-in Replacement Functions`_

There are three broad categories of functions exposed by ``fastnumbers``.
The below quick start will demonstrate each of these categories. The
quick start is "by example", and will show a sample interactive session
using the ``fastnumbers`` API.

Error-Handling Functions
++++++++++++++++++++++++

- `Error-handling function API <https://fastnumbers.readthedocs.io/en/master/api.html#the-error-handling-functions>`_

``fast_float`` will be used to demonstrate the functionality of the
``fast_*`` functions.

.. code-block:: python

    >>> from fastnumbers import fast_float
    >>> # Convert string to a float
    >>> fast_float('56.07')
    56.07
    >>> # Integers are converted to floats
    >>> fast_float(54)
    54.0
    >>>
    >>> # Unconvertable string returned as-is by default
    >>> fast_float('bad input')
    'bad input'
    >>> # Unconvertable strings can trigger a default value
    >>> fast_float('bad input', default=0)
    0
    >>> # 'default' is also the first optional positional arg
    >>> fast_float('bad input', 0)
    0
    >>>
    >>> # One can ask inf or nan to be substituted with another value
    >>> fast_float('nan')
    nan
    >>> fast_float('nan', nan=0.0)
    0.0
    >>> fast_float(float('nan'), nan=0.0)
    0.0
    >>> fast_float('56.07', nan=0.0)
    56.07
    >>>
    >>> # The default built-in float behavior can be triggered with
    >>> # "raise_on_invalid" set to True.
    >>> fast_float('bad input', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
      ...
    ValueError: invalid literal for float(): bad input
    >>>
    >>> # A function can be used to return an alternate value for invalid input
    >>> fast_float('bad input', on_fail=len)
    9
    >>> fast_float(54, on_fail=len)
    54.0
    >>>
    >>> # Single unicode characters can be converted.
    >>> fast_float('\u2164')  # Roman numeral 5 (V)
    5.0
    >>> fast_float('\u2466')  # 7 enclosed in a circle
    7.0

``fast_int`` behaves the same as ``fast_float``, but for integers.

.. code-block:: python

    >>> from fastnumbers import fast_int
    >>> fast_int('1234')
    1234
    >>> fast_int('\u2466')
    7

``fast_real`` is like ``fast_float`` or ``fast_int`` depending
on if there is any fractional component of thi return value.

.. code-block:: python

    >>> from fastnumbers import fast_real
    >>> fast_real('56')
    56
    >>> fast_real('56.0')
    56
    >>> fast_real('56.0', coerce=False)
    56.0
    >>> fast_real('56.07')
    56.07
    >>> fast_real(56.07)
    56.07
    >>> fast_real(56.0)
    56
    >>> fast_real(56.0, coerce=False)
    56.0
    >>>
    >>>

``fast_forceint`` always returns an integer.

.. code-block:: python

    >>> from fastnumbers import fast_forceint
    >>> fast_forceint('56')
    56
    >>> fast_forceint('56.0')
    56
    >>> fast_forceint('56.07')
    56
    >>> fast_forceint(56.07)
    56

About the ``on_fail`` option
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``on_fail`` option is a way for you to do *anything* in the event that
the given input cannot be converted to a number. Here are a couple of ideas
to get you thinking.

.. code-block:: python

    >>> from fastnumbers import fast_float
    >>> # Simple case, send the input through some function to generate a number.
    >>> fast_float('invalid input', on_fail=lambda x: float(x.count('i')))  # count the 'i's
    3.0
    >>>
    >>>
    >>>
    >>> # Suppose we know that our input could either be a number, or if not
    >>> # then we know we just have to strip off parens to get to the number
    >>> # e.g. the input could be '45' or '(45)'. Also, suppose that if it
    >>> # still cannot be converted to a number we want to raise an exception.
    >>> def strip_parens_and_try_again(x):
    ...     return fast_float(x.strip('()'), raise_on_invalid=True)
    ...
    >>> fast_float('45', on_fail=strip_parens_and_try_again)
    45.0
    >>> fast_float('(45)', on_fail=strip_parens_and_try_again)
    45.0
    >>> fast_float('invalid input', on_fail=strip_parens_and_try_again) #doctest: +IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
      ...
    ValueError: invalid literal for float(): invalid input
    >>>
    >>>
    >>>
    >>> # Suppose that whenever an invalid input is given, it needs to be
    >>> # logged and then a default value is returned.
    >>> def log_and_default(x, log_method=print, default=0.0):
    ...     log_method("The input {!r} is not valid!".format(x))
    ...     return default
    ...
    >>> fast_float('45', on_fail=log_and_default)
    45.0
    >>> fast_float('invalid input', on_fail=log_and_default)
    The input 'invalid input' is not valid!
    0.0
    >>> fast_float('invalid input', on_fail=lambda x: log_and_default(x, default=float('nan')))
    The input 'invalid input' is not valid!
    nan

Checking Functions
++++++++++++++++++

- `Checking function API <https://fastnumbers.readthedocs.io/en/master/api.html#the-checking-functions>`_

``isfloat`` will be used to demonstrate the functionality of the
``is*`` functions.

.. code-block:: python

    >>> from fastnumbers import isfloat
    >>> # Check that a string can be converted to a float
    >>> isfloat('56')
    True
    >>> isfloat('56.07')
    True
    >>> isfloat('56.07 lb')
    False
    >>>
    >>> # Check if a given number is a float
    >>> isfloat(56.07)
    True
    >>> isfloat(56)
    False
    >>>
    >>> # Specify if only strings or only numbers are allowed
    >>> isfloat(56.07, str_only=True)
    False
    >>> isfloat('56.07', num_only=True)
    False
    >>>
    >>> # Customize handling for nan or inf
    >>> isfloat('nan')
    False
    >>> isfloat('nan', allow_nan=True)
    True

``isint`` works the same as ``isfloat``, but for integers.

.. code-block:: python

    >>> from fastnumbers import isint
    >>> isint('56')
    True
    >>> isint(56)
    True
    >>> isint('56.0')
    False
    >>> isint(56.0)
    False

``isreal`` is very permissive - any float or integer is accepted.

.. code-block:: python

    >>> from fastnumbers import isreal
    >>> isreal('56.0')
    True
    >>> isreal('56')
    True
    >>> isreal(56.0)
    True
    >>> isreal(56)
    True

``isintlike`` checks if a number is "int-like", if it has no
fractional component.

.. code-block::

    >>> from fastnumbers import isintlike
    >>> isintlike('56.0')
    True
    >>> isintlike('56.7')
    False
    >>> isintlike(56.0)
    True
    >>> isintlike(56.7)
    False

Drop-in Replacement Functions
+++++++++++++++++++++++++++++

- `Drop-in replacement function API <https://fastnumbers.readthedocs.io/en/master/api.html#the-built-in-replacement-functions>`_

**PLEASE** do not take it for granted that these functions will provide you
with a speedup - they may not. Every platform, compiler, and data-set is
different, and you should perform a timing test on your system with your data
to evaluate if you will see a benefit. As you can see from the data linked in
the `Timing`_ section, the amount of speedup you will get is particularly
data-dependent.

**NOTE**: in the below examples, we use ``from fastnumbers import int`` instead
of ``import fastnumbers``. This is because calling ``fastnumbers.int()`` is a
bit slower than just ``int()`` because Python has to first find ``fastnumbers``
in your namespace, then find ``int`` in the ``fastnumbers`` namespace, instead
of just finding ``int`` in your namespace - this will slow down the function
call and defeat the purpose of using ``fastnumbers``. If you do not want to
actually shadow the built-in ``int`` function, you can do
``from fastnumbers import int as fn_int`` or something like that.

.. code-block:: python

    >>> # Use is identical to the built-in functions
    >>> from fastnumbers import float, int
    >>> float('10')
    10.0
    >>> int('10')
    10
    >>> float('bad input') #doctest: +IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
      ...
    ValueError: invalid literal for float(): bad input

``real`` is is provided to give a float or int depending
on the fractional component of the input.

.. code-block:: python

    >>> from fastnumbers import real
    >>> real('56.0')
    56
    >>> real('56.7')
    56.7
    >>> real('56.0', coerce=False)
    56.0

Timing
------

Just how much faster is ``fastnumbers`` than a pure python implementation?
Please see the following Jupyter notebooks for timing information on various
Python versions.

    - https://nbviewer.jupyter.org/github/SethMMorton/fastnumbers/blob/master/TIMING_35.ipynb
    - https://nbviewer.jupyter.org/github/SethMMorton/fastnumbers/blob/master/TIMING_36.ipynb
    - https://nbviewer.jupyter.org/github/SethMMorton/fastnumbers/blob/master/TIMING_37.ipynb

High-Level Algorithm
--------------------

CPython goes to great lengths to ensure that your string input is converted to a
number *correctly* (you can prove this to yourself by examining the source code
for
`integer conversions <https://github.com/python/cpython/blob/e349bf23584eef20e0d1e1b2989d9b1430f15507/Objects/longobject.c#L2213>`_
and for
`float conversions <https://github.com/python/cpython/blob/e349bf23584eef20e0d1e1b2989d9b1430f15507/Python/dtoa.c#L1434>`_),
but this extra effort is only needed for very large
integers or for floats with many digits or large exponents. For integers, if
the result could fit into a C ``long`` then a naive algorithm of < 10 lines
of C code is sufficient. For floats, if the number does not require high
precision or does not have a large exponent (such as "-123.45e6") then a
short naive algorithm is also possible.

These naive algorithms are quite fast, but the performance improvement comes
at the expense of being unsafe (no protection against overflow or round-off
errors). ``fastnumbers`` uses a heuristic to determine if the input can be
safely converted with the much faster naive algorithm. These heuristics are
extremely conservative - if there is *any* chance that the naive result would
not give *exactly* the same result as the built-in functions then it will fall
back on CPython's conversionfunction. For this reason, ``fastnumbers`` is aways
*at least as fast* as CPython's built-in ``float`` and ``int`` functions, and
oftentimes is significantly faster because most real-world numbers pass the
heuristic.

Installation
------------

Use ``pip``!

.. code-block::

    $ pip install fastnumbers

How to Run Tests
----------------

Please note that ``fastnumbers`` is NOT set-up to support
``python setup.py test``.

The recommended way to run tests is with
`tox <https://tox.readthedocs.io/en/latest/>`_.
Suppose you want to run tests for Python 3.6 - you can run tests by simply
executing the following:

.. code-block:: sh

    $ tox -e py36

``tox`` will create virtual a virtual environment for your tests and install
all the needed testing requirements for you.

If you want to run testing on all of Python 3.5, 3.6, 3.7, and 3.8 you can
simply execute

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

Please visit the changelog `on GitHub <https://github.com/SethMMorton/fastnumbers/blob/master/CHANGELOG.md>`_
or `in the documentation <https://fastnumbers.readthedocs.io/en/master/changelog.html>`_.
