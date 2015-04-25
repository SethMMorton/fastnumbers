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

**NOTE:** The :func:`safe_real`, :func:`safe_float`, :func:`safe_int`, and
:func:`safe_forceint` functions are deprecated as of :mod:`fastnumbers` version
>= 0.3.0; :func:`fast_real`, :func:`fast_float`, :func:`fast_int`, and
:func:`fast_forceint` have each been reimplemented to fall back on the
"safe" algorithm if overflow or loss of precision is detect and so the
separate "safe" functions are no longer needed.

Quick Description
-----------------

:mod:`fastnumbers` is essentially a fast C implementation of the following
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

To achieve this, the module makes some assumptions about the input type
(input is ``int`` (or ``long``), ``float``, or ``str`` (or ``unicode``)),
and otherwise a ``TypeError`` is raised.

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
