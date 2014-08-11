.. default-domain:: py
.. currentmodule:: fastnumbers

The "Fast" Functions
====================

Each of these functions will quickly convert strings to numbers
(and also numbers to numbers) faster than a pure Python implementation,
but without bounds and overflow checking on the conversion, so they
are much faster than the "safe" functions but may not give accurate
results for very large or very small numbers.

:func:`~fastnumbers.fast_real`
------------------------------

.. autofunction:: fast_real

:func:`~fastnumbers.fast_float`
-------------------------------

.. autofunction:: fast_float

:func:`~fastnumbers.fast_int`
-----------------------------

.. autofunction:: fast_int

:func:`~fastnumbers.fast_forceint`
----------------------------------

.. autofunction:: fast_forceint
