.. default-domain:: py
.. currentmodule:: fastnumbers

.. _api:

fastnumbers API
===============

.. contents::
    :local:

The "Built-In Replacement" Functions
------------------------------------

Each of these functions acts as a faster drop-in replacement for
the equivalent Python built-in function.

:func:`~fastnumbers.float`
++++++++++++++++++++++++++

.. autofunction:: float

:func:`~fastnumbers.int`
++++++++++++++++++++++++

.. autofunction:: int

:func:`~fastnumbers.real`
+++++++++++++++++++++++++

.. autofunction:: real

The "Error-Handling" Functions
------------------------------

Each of these functions will quickly convert strings to numbers
(and also numbers to numbers) with fast and convenient error handling.
They are guaranteed to return results identical to the built-in
``float`` or ``int`` functions.

:func:`~fastnumbers.fast_real`
++++++++++++++++++++++++++++++

.. autofunction:: fast_real

:func:`~fastnumbers.fast_float`
+++++++++++++++++++++++++++++++

.. autofunction:: fast_float

:func:`~fastnumbers.fast_int`
+++++++++++++++++++++++++++++

.. autofunction:: fast_int

:func:`~fastnumbers.fast_forceint`
++++++++++++++++++++++++++++++++++

.. autofunction:: fast_forceint

The "Checking" Functions
------------------------

These functions return a Boolean value that indicates if the input
can return a certain number type or not.

:func:`~fastnumbers.isreal`
+++++++++++++++++++++++++++

.. autofunction:: isreal

:func:`~fastnumbers.isfloat`
++++++++++++++++++++++++++++

.. autofunction:: isfloat

:func:`~fastnumbers.isint`
++++++++++++++++++++++++++

.. autofunction:: isint

:func:`~fastnumbers.isintlike`
++++++++++++++++++++++++++++++

.. autofunction:: isintlike
