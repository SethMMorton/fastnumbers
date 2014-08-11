.. default-domain:: py
.. module:: fastnumbers

The :mod:`fastnumbers` module
=============================

Convert strings to numbers quickly.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.python.org/pypi/fastnumbers
    - Documentation: http://pythonhosted.org//fastnumbers/

Quick Description
-----------------

::

    >>> from fastnumbers import fast_float
    >>> def float_no_raise(input):
    ...     try:
    ...         return float(input)
    ...     except ValueError:
    ...         return input
    ... 
    >>> fast_float('56.07')
    56.07
    >>> float_no_raise('56.07') == fast_float('56.07')
    True
    >>> fast_float('bad input')
    'bad input'
    >>> float_no_raise('bad input') == fast_float('bad input')
    True
    >>> fast_float(54)
    54.0
    >>> float_no_raise(54) == fast_float(54)
    True

This module is a Python C extension that will convert strings to
numbers *much* faster than can be done using pure Python.  Additionally,
if the string cannot be converted, the string is returned as-is instead
of returning a ``ValueError``.

To achieve this, the module makes some assumptions about the input type
(input is ``int`` (or ``long``), ``float``, or ``str`` (or ``unicode``)),
and otherwise a ``TypeError`` is raised.

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
(this includes python 3.x). 
