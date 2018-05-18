.. default-domain:: py
.. module:: fastnumbers

The :mod:`fastnumbers` module
=============================

Super-fast and clean conversions to numbers.

    - Source Code: https://github.com/SethMMorton/fastnumbers
    - Downloads: https://pypi.org/project/fastnumbers/
    - Documentation: http://fastnumbers.readthedocs.io/

Please see the :ref:`Timing Documentation <timing>` for timing details.
Check out the :ref:`API <api>`.

Quick Description
-----------------

The below examples showcase the :func:`fast_float` function, which is
a fast conversion functions with error-handling.
Please see the :ref:`API Documentation <api>`
for other functions that are available from :mod:`fastnumbers`.

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

**NOTE**: If you need locale-dependent conversions, supply the :mod:`fastnumbers`
function of your choice to :func:`locale.atof`.

.. code-block:: python

    import locale
    locale.setlocale(locale.LC_ALL, 'de_DE.UTF-8')
    print(atof('468,5', func=fast_float))  # Prints 468.5

Installation
------------

Installation of :mod:`fastnumbers` is ultra-easy.  Simply execute from the
command line:

.. code-block:: sh

    $ pip install fastnumbers

You can also download the source from https://pypi.org/project/fastnumbers/,
or browse the git repository at https://github.com/SethMMorton/fastnumbers.

If you choose to install from source (will need a C compiler and the Python headers),
you can unzip the source archive and enter the directory, and type:

.. code-block:: sh

    $ python setup.py install

If you want to build this documentation, enter:

.. code-block:: sh

    $ python setup.py build_sphinx

:mod:`fastnumbers` requires python version 2.7 or greater
(this includes python 3.x). Unit tests are only run on 2.7 and >= 3.4.

How to Run Tests
----------------

Please note that ``fastnumbers`` is NOT set-up to support ``python setup.py test``.

The recommended way to run tests with with `tox <https://tox.readthedocs.io/en/latest/>`_.
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

If you do not wish to use ``tox``, you can install the testing dependencies and run the
tests manually using `pytest <https://docs.pytest.org/en/latest/>`_ - ``fastnumbers``
contains a ``Pipfile`` for use with `pipenv <https://github.com/pypa/pipenv>`_ that
makes it easy for you to install the testing dependencies:

.. code-block:: sh

    $ pipenv install --skip-lock --dev
    $ pipenv install --skip-lock -e .
    $ pipenv run pytest

:mod:`fastnumbers` uses `pytest <https://docs.pytest.org/en/latest/>`_ to run its tests.
