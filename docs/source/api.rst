.. default-domain:: py
.. currentmodule:: fastnumbers

.. _api:

fastnumbers API
===============

.. contents::
    :local:

The "Built-In Replacement" Functions
------------------------------------

Each of these functions acts as a (potentially) faster drop-in replacement
for the equivalent Python built-in function. Please perform timing tests on
your platform with your data to determine if these functions will actually
provide you with a speed-up.

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

:func:`~fastnumbers.try_real`
+++++++++++++++++++++++++++++

.. autofunction:: try_real

:func:`~fastnumbers.try_float`
++++++++++++++++++++++++++++++

.. autofunction:: try_float

:func:`~fastnumbers.try_int`
++++++++++++++++++++++++++++

.. autofunction:: try_int

:func:`~fastnumbers.try_forceint`
+++++++++++++++++++++++++++++++++

.. autofunction:: try_forceint

The "Checking" Functions
------------------------

These functions return a Boolean value that indicates if the input
can return a certain number type or not.

:func:`~fastnumbers.check_real`
+++++++++++++++++++++++++++++++

.. autofunction:: check_real

:func:`~fastnumbers.check_float`
++++++++++++++++++++++++++++++++

.. autofunction:: check_float

:func:`~fastnumbers.check_int`
++++++++++++++++++++++++++++++

.. autofunction:: check_int

:func:`~fastnumbers.check_intlike`
++++++++++++++++++++++++++++++++++

.. autofunction:: check_intlike

:func:`~fastnumbers.query_type`
+++++++++++++++++++++++++++++++

.. autofunction:: query_type

Deprecated "Error-Handling" Functions
--------------------------------------

These functions have a less flexible interface than ``try_*`` functions,
but otherwise do the same thing. New code should prefer the ``try_*``
functions instead, but the below functions will never be removed.

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

Deprecated "Checking" Functions
-------------------------------

These functions have a less flexible interface than ``check_*`` functions,
but otherwise do the same thing. New code should prefer the ``check_*``
functions instead, but the below functions will never be removed.

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