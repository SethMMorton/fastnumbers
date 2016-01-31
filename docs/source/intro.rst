.. default-domain:: py
.. module:: fastnumbers

The :mod:`fastnumbers` module
=============================

Convert strings to numbers quickly.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.python.org/pypi/fastnumbers
    - Documentation: http://pythonhosted.org//fastnumbers/

Please see the
`Timing Documentation <http://pythonhosted.org//fastnumbers/timing.html>`_
for timing details.
Check out the `API <http://pythonhosted.org//fastnumbers/api.html>`_.

Quick Description
-----------------

:mod:`fastnumbers` contains functions that are fast C implementations similar
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

::

    import locale
    locale.setlocale(locale.LC_ALL, 'de_DE.UTF-8')
    print(atof('468,5', func=fast_float))  # Prints 468.5

Installation
------------

Installation of :mod:`fastnumbers` is ultra-easy.  Simply execute from the
command line::

    easy_install fastnumbers

or, if you have ``pip`` (preferred over ``easy_install``)::

    pip install fastnumbers

Both of the above commands will download the source for you.

You can also download the source from http://pypi.python.org/pypi/fastnumbers,
or browse the git repository at https://github.com/SethMMorton/fastnumbers.

If you choose to install from source (will need a C compiler and the Python headers),
you can unzip the source archive and enter the directory, and type::

    python setup.py install

If you wish to run the unit tests, enter::

    python setup.py test

If you want to build this documentation, enter::

    python setup.py build_sphinx

:mod:`fastnumbers` requires python version 2.6 or greater
(this includes python 3.x). Unit tests are only run on 2.7 and >= 3.3.
