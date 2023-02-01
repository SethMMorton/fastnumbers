fastnumbers
===========

.. image:: https://img.shields.io/pypi/v/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/pyversions/fastnumbers.svg
    :target: https://pypi.org/project/fastnumbers/

.. image:: https://img.shields.io/pypi/l/fastnumbers.svg
    :target: https://github.com/SethMMorton/fastnumbers/blob/main/LICENSE

.. image:: https://github.com/SethMMorton/fastnumbers/workflows/Tests/badge.svg
    :target: https://github.com/SethMMorton/fastnumbers/actions

.. image:: https://codecov.io/gh/SethMMorton/fastnumbers/branch/main/graph/badge.svg
    :target: https://codecov.io/gh/SethMMorton/fastnumbers

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

    #. Provide a set of convenience functions that wrap calls to
       ``int`` and ``float`` and provides easy, concise, powerful, fast
       and flexible error handling.
    #. Provide a set of functions that can be used to rapidly identify if
       an input *could* be converted to *int* or *float*.
    #. Provide drop-in replacements for the Python built-in ``int`` and
       ``float`` that are on par or faster with the Python equivalents
       (see the `Timing_` section for details). These functions
       should behave *identically* to the Python built-ins except for a few
       specific corner-cases as mentioned in the
       `API documentation for those functions <https://fastnumbers.readthedocs.io/en/stable/api.html#the-built-in-replacement-functions>`_.

       - **PLEASE** read the quick start for these functions to fully
         understand the caveats before using them.

**NOTICE**: As of ``fastnumbers`` version 4.0.0, only Python >= 3.7 is
supported.

**NOTICE**: As of ``fastnumbers`` version 4.0.0, the functions ``fast_real``,
``fast_float``, ``fast_int``, ``fast_forceint``, ``isreal``, ``isfloat``,
``isint``, and ``isintlike`` have been deprecated and are replaced with
``try_real``, ``try_float``, ``try_int``, ``try_forceint``, ``check_real``,
``check_float``, ``check_int``, and ``check_intlike``, respectively. These
new functions have more flexible APIs and have names that better reflect
the intent of the functions. The old functions can still be used (they will
*never* be removed from ``fastnumbers``), but the new ones should be
preferred for new development.

**NOTICE**: As of ``fastnumbers`` version 4.0.0, ``query_type`` now sets
``allow_underscores`` to ``False`` by default instead of ``True``.

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

- `Error-handling function API <https://fastnumbers.readthedocs.io/en/stable/api.html#the-error-handling-functions>`_

``try_float`` will be used to demonstrate the functionality of the
``try_*`` functions.

.. code-block:: python

    >>> from fastnumbers import RAISE, try_float
    >>> # Convert string to a float
    >>> try_float('56.07')
    56.07
    >>> # Integers are converted to floats
    >>> try_float(54)
    54.0
    >>>
    >>> # Unconvertable string returned as-is by default
    >>> try_float('bad input')
    'bad input'
    >>> # Unconvertable strings can trigger a default value
    >>> try_float('bad input', on_fail=0)
    0
    >>>
    >>> # One can ask inf or nan to be substituted with another value
    >>> try_float('nan')
    nan
    >>> try_float('nan', nan=0.0)
    0.0
    >>> try_float(float('nan'), nan=0.0)
    0.0
    >>> try_float('56.07', nan=0.0)
    56.07
    >>>
    >>> # The default built-in float behavior can be triggered with
    >>> # RAISE given to "on_fail".
    >>> try_float('bad input', on_fail=RAISE) #doctest: +IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
      ...
    ValueError: invalid literal for float(): bad input
    >>>
    >>> # A function can be used to return an alternate value for invalid input
    >>> try_float('bad input', on_fail=len)
    9
    >>> try_float(54, on_fail=len)
    54.0
    >>>
    >>> # Single unicode characters can be converted.
    >>> try_float('\u2164')  # Roman numeral 5 (V)
    5.0
    >>> try_float('\u2466')  # 7 enclosed in a circle
    7.0

``try_int`` behaves the same as ``try_float``, but for integers.

.. code-block:: python

    >>> from fastnumbers import try_int
    >>> try_int('1234')
    1234
    >>> try_int('\u2466')
    7

``try_real`` is like ``try_float`` or ``try_int`` depending
on if there is any fractional component of thi return value.

.. code-block:: python

    >>> from fastnumbers import try_real
    >>> try_real('56')
    56
    >>> try_real('56.0')
    56
    >>> try_real('56.0', coerce=False)
    56.0
    >>> try_real('56.07')
    56.07
    >>> try_real(56.07)
    56.07
    >>> try_real(56.0)
    56
    >>> try_real(56.0, coerce=False)
    56.0

``try_forceint`` always returns an integer.

.. code-block:: python

    >>> from fastnumbers import try_forceint
    >>> try_forceint('56')
    56
    >>> try_forceint('56.0')
    56
    >>> try_forceint('56.07')
    56
    >>> try_forceint(56.07)
    56

About the ``on_fail`` option
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``on_fail`` option is a way for you to do *anything* in the event that
the given input cannot be converted to a number. It can

* return given object as-is if set to ``fastnumbers.INPUT`` (this is the default)
* raise a ``ValueError`` if set to ``fastnumbers.RAISE``
* return a default value if given any non-callable object
* call a function with the given object if given a single-argument callable

Below are a couple of ideas to get you thinking.

**NOTE**:: There is also an ``on_type_error`` option that behaves the same as
``on_fail`` except that a) it is triggered when the given object is of an
invalid type and b) the default value is ``fastnumbers.RAISE``, not
``fastnumbers.INPUT``.

.. code-block:: python

    >>> from fastnumbers import INPUT, RAISE, try_float
    >>> # You want to convert strings that can be converted to numbers, but
    >>> # leave the rest as strings. Use fastnumbers.INPUT (the default)
    >>> try_float('45.6')
    45.6
    >>> try_float('invalid input')
    'invalid input'
    >>> try_float('invalid input', on_fail=INPUT)
    'invalid input'
    >>>
    >>>
    >>>
    >>> # You want to convert any invalid string to NaN
    >>> try_float('45.6', on_fail=float('nan'))
    45.6
    >>> try_float('invalid input', on_fail=float('nan'))
    nan
    >>>
    >>>
    >>>
    >>> # Simple callable case, send the input through some function to generate a number.
    >>> try_float('invalid input', on_fail=lambda x: float(x.count('i')))  # count the 'i's
    3.0
    >>>
    >>>
    >>>
    >>> # Suppose we know that our input could either be a number, or if not
    >>> # then we know we just have to strip off parens to get to the number
    >>> # e.g. the input could be '45' or '(45)'. Also, suppose that if it
    >>> # still cannot be converted to a number we want to raise an exception.
    >>> def strip_parens_and_try_again(x):
    ...     return try_float(x.strip('()'), on_fail=RAISE)
    ...
    >>> try_float('45', on_fail=strip_parens_and_try_again)
    45.0
    >>> try_float('(45)', on_fail=strip_parens_and_try_again)
    45.0
    >>> try_float('invalid input', on_fail=strip_parens_and_try_again) #doctest: +IGNORE_EXCEPTION_DETAIL
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
    >>> try_float('45', on_fail=log_and_default)
    45.0
    >>> try_float('invalid input', on_fail=log_and_default)
    The input 'invalid input' is not valid!
    0.0
    >>> try_float('invalid input', on_fail=lambda x: log_and_default(x, default=float('nan')))
    The input 'invalid input' is not valid!
    nan

Checking Functions
++++++++++++++++++

- `Checking function API <https://fastnumbers.readthedocs.io/en/stable/api.html#the-checking-functions>`_

``check_float`` will be used to demonstrate the functionality of the
``check_*`` functions, as well as the ``query_type`` function.

.. code-block:: python

    >>> from fastnumbers import check_float
    >>> from fastnumbers import ALLOWED, DISALLOWED, NUMBER_ONLY, STRING_ONLY
    >>> # Check that a string can be converted to a float
    >>> check_float('56')
    True
    >>> check_float('56', strict=True)
    False
    >>> check_float('56.07')
    True
    >>> check_float('56.07 lb')
    False
    >>>
    >>> # Check if a given number is a float
    >>> check_float(56.07)
    True
    >>> check_float(56)
    False
    >>>
    >>> # Specify if only strings or only numbers are allowed
    >>> check_float(56.07, consider=STRING_ONLY)
    False
    >>> check_float('56.07', consider=NUMBER_ONLY)
    False
    >>>
    >>> # Customize handling for nan or inf (see API for more details)
    >>> check_float('nan')
    False
    >>> check_float('nan', nan=ALLOWED)
    True
    >>> check_float(float('nan'))
    True
    >>> check_float(float('nan'), nan=DISALLOWED)
    False

``check_int`` works the same as ``check_float``, but for integers.

.. code-block:: python

    >>> from fastnumbers import check_int
    >>> check_int('56')
    True
    >>> check_int(56)
    True
    >>> check_int('56.0')
    False
    >>> check_int(56.0)
    False

``check_real`` is very permissive - any float or integer is accepted.

.. code-block:: python

    >>> from fastnumbers import check_real
    >>> check_real('56.0')
    True
    >>> check_real('56')
    True
    >>> check_real(56.0)
    True
    >>> check_real(56)
    True

``check_intlike`` checks if a number is "int-like", if it has no
fractional component.

.. code-block:: python

    >>> from fastnumbers import check_intlike
    >>> check_intlike('56.0')
    True
    >>> check_intlike('56.7')
    False
    >>> check_intlike(56.0)
    True
    >>> check_intlike(56.7)
    False

The ``query_type`` function can be used if you need to determine if
a value is one of many types, rather than whether or not it is one specific
type.

.. code-block:: python

    >>> from fastnumbers import query_type
    >>> query_type('56.0')
    <class 'float'>
    >>> query_type('56')
    <class 'int'>
    >>> query_type(56.0)
    <class 'float'>
    >>> query_type(56)
    <class 'int'>
    >>> query_type(56.0, coerce=True)
    <class 'int'>
    >>> query_type('56.0', allowed_types=(float, int))
    <class 'float'>
    >>> query_type('hey')
    <class 'str'>
    >>> query_type('hey', allowed_types=(float, int))  # returns None

Drop-in Replacement Functions
+++++++++++++++++++++++++++++

- `Drop-in replacement function API <https://fastnumbers.readthedocs.io/en/stable/api.html#the-built-in-replacement-functions>`_

**PLEASE** do not take it for granted that these functions will provide you
with a speedup - they may not. Every platform, compiler, and data-set is
different, and you should perform a timing test on your system with your data
to evaluate if you will see a benefit. As you can see from the data linked in
the `Timing`_ section, the amount of speedup you will get is particularly
data-dependent. *In general* you will see a performance boost for floats (and
this boost increases as the size of the float increases), but for integers it
is largely dependent on the length of the integer. You will likely *not* see
a performance boost if the input are already numbers instead of strings.

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

``real`` is provided to give a float or int depending
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
Please look https://github.com/SethMMorton/fastnumbers/tree/main/profiling.

High-Level Algorithm
--------------------

For integers, CPython goes to great lengths to ensure that your string input
is converted to a number *correctly* and *losslessly* (you can prove this to
yourself by examining the source code for
`integer conversions <https://github.com/python/cpython/blob/e349bf23584eef20e0d1e1b2989d9b1430f15507/Objects/longobject.c#L2213>`_).
This extra effort is only needed for integers that cannot fit into a 64-bit
integer data type - for those that can, a naive algorithm of < 10 lines
of C code is sufficient and significantly faster. ``fastnumbers`` uses a
heuristic to determine if the input can be safely converted with the much
faster naive algorithm, and if so it does so, falling back on
the CPython implementation for longer input strings.
Most real-world numbers pass the heuristic and so you should generally see
improved performance with ``fastnumbers`` for integers.

For floats, ``fastnumbers`` utilizes the ultra-fast
`fast_float::from_chars <https://github.com/fastfloat/fast_float>`_ function
to convert strings representing floats into a C ``double`` both quickly *and
safely* - the conversion provides the same accuracy as the CPython
`float conversion function <https://github.com/python/cpython/blob/e349bf23584eef20e0d1e1b2989d9b1430f15507/Python/dtoa.c#L1434>`_
but instead of scaling linearly with length of the input string it seems
to have roughly constant performance. By completely bypassing the CPython
converter we get significant performance gains with no penalty, so you
should always see improved performance with ``fastnumbers`` for floats.

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
Suppose you want to run tests for Python 3.8 - you can run tests by simply
executing the following:

.. code-block:: sh

    $ tox -e py38

``tox`` will create virtual a virtual environment for your tests and install
all the needed testing requirements for you.

If you want to run testing on all supported Python versions you can simply execute

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

Please visit the changelog `on GitHub <https://github.com/SethMMorton/fastnumbers/blob/main/CHANGELOG.md>`_
or `in the documentation <https://fastnumbers.readthedocs.io/en/stable/changelog.html>`_.
