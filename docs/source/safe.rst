.. default-domain:: py
.. currentmodule:: fastnumbers

The "Safe" Functions
====================

Each of these functions will quickly convert strings to numbers
(and also numbers to numbers) faster than a pure Python implementation,
but still does some bounds and overflow checking on the conversion.

:func:`~fastnumbers.safe_real`
------------------------------

.. autofunction:: safe_real

:func:`~fastnumbers.safe_float`
-------------------------------

.. autofunction:: safe_float

:func:`~fastnumbers.safe_int`
-----------------------------

.. autofunction:: safe_int

:func:`~fastnumbers.safe_forceint`
----------------------------------

.. autofunction:: safe_forceint
