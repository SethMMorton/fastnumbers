#
# C implementation of the python library fastnumbers.
#
# Author: Seth M. Morton, July 30, 2014
#
from cpython.getargs cimport PyArg_ParseTuple, PyArg_ParseTupleAndKeywords
from cpython.ref cimport PyObject
from libc.limits cimport INT_MIN

from options cimport Options, Options_Set_Return_Value, PyNumberType


cdef extern from "fastnumbers/objects.h":
    PyObject *PyObject_to_PyNumber(
        PyObject *obj, const PyNumberType type, const Options *options) except NULL
    PyObject *PyObject_is_number(
        PyObject *obj, const PyNumberType type, const Options *options) except NULL
    PyObject *PyObject_contains_type(PyObject *obj, const Options *options) except NULL


cdef extern from "fastnumbers/parsing.h":
    long FN_MAX_INT_LEN
    long FN_DBL_DIG
    long FN_MAX_EXP
    long FN_MIN_EXP


# Sentinel to use for arguments where None is a valid option
cdef object SENTINEL = object()


# Global exports
__version__ = "3.2.1"
max_int_len = FN_MAX_INT_LEN
dig = FN_DBL_DIG
max_exp = FN_MAX_EXP
min_exp = FN_MIN_EXP


def fast_real(
    x,
    default=SENTINEL,
    *,
    bint raise_on_invalid=False,
    on_fail=None,
    inf=SENTINEL,
    nan=SENTINEL,
    bint coerce=True,
    bint allow_underscores =True,
    key=None,
):
    """
    Quickly convert input to an `int` or `float` depending on value.

    Any input that is valid for the built-in `float` or `int` functions will
    be converted to either a `float` or `int`. An input of a single numeric
    unicode character is also valid. The return value is guaranteed
    to be of type `str`, `int`, or `float`.

    If the given input is a string and cannot be converted to a `float` or__pyx_n_s_x
    `int`, it will be returned as-is unless `default` or `raise_on_invalid`
    is given.

    Parameters
    ----------
    input : {str, float, int, long}
        The input you wish to convert to a real number.
    default : optional
        This value will be returned instead of the input
        when the input cannot be converted. Has no effect if
        *raise_on_invalid* is *True*.
    raise_on_invalid : bool, optional
        If *True*, a `ValueError` will be raised if string input cannot be
        converted to a `float` or `int`. If *False*, the string will be
        returned as-is. The default is *False*.
    on_fail : callable, optional
        If given and the *input* cannot be converted, the input will be
        passed to the callable object and its return value will be returned.
        The function expect only one positional argument.
        For backwards-compatability, you may call this option `key` instead
        of `on_fail`, but this is deprecated behavior.
    nan : optional
        If the input value is NAN or can be parsed as NAN, return this
        value instead of NAN.
    inf : optional
        If the input value is INF or can be parsed as INF, return this
        value instead of INF.
    coerce : bool, optional
        If the input can be converted to an `int` without loss of precision
        (even if the input was a `float` or float-containing `str`)
        coerce to an *int* rather than returning a `float`.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    out : {str, float, int}
        If the input could be converted to an `int`, the return type will be
        `int`.
        If the input could be converted to a `float` but not an `int`, the
        return type will be `float`.  Otherwise, the input `str` will be
        returned as-is (if `raise_on_invalid` is *False*) or whatever value
        is assigned to `default` if `default` is not *None*.

    Raises
    ------
    TypeError
        If the input is not one of `str`, `float`, or `int`.
    ValueError
        If `raise_on_invalid` is *True*, this will be raised if the input
        string cannot be converted to a `float` or `int`.

    See Also
    --------
    isreal
    real

    Examples
    --------

        >>> from fastnumbers import fast_real
        >>> fast_real('56')
        56
        >>> fast_real('56.0')
        56
        >>> fast_real('56.0', coerce=False)
        56.0
        >>> fast_real('56.07')
        56.07
        >>> fast_real('56.07 lb')
        '56.07 lb'
        >>> fast_real(56.07)
        56.07
        >>> fast_real(56.0)
        56
        >>> fast_real(56.0, coerce=False)
        56.0
        >>> fast_real(56)
        56
        >>> fast_real('invalid', default=50)
        50
        >>> fast_real('invalid', 50)  # 'default' is first optional positional arg
        50
        >>> fast_real('nan')
        nan
        >>> fast_real('nan', nan=0)
        0
        >>> fast_real('56.07', nan=0)
        56.07
        >>> fast_real('56.07 lb', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
        Traceback (most recent call last):
          ...
        ValueError: could not convert string to float: '56.07 lb'
        >>> fast_real('invalid', on_fail=len)
        7

    Notes
    -----
    It is roughly equivalent to (but much faster than)

        >>> def py_fast_real(input, default=None, raise_on_invalid=False,
        ...                  on_fail=None, nan=None, inf=None):
        ...     import math
        ...     try:
        ...         a = float(input)
        ...     except ValueError:
        ...         if raise_on_invalid:
        ...             raise
        ...         elif on_fail is not None:
        ...             return on_fail(input)
        ...         elif default is not None:
        ...             return default
        ...         else:
        ...             return input
        ...     else:
        ...         if nan is not None and math.isnan(a):
        ...             return nan
        ...         elif inf is not None and math.isinf(a):
        ...             return inf
        ...         else:
        ...             return int(a) if a.is_integer() else a
        ... 

    """
    cdef Options opts
    opts.retval = NULL
    opts.input = <PyObject *> x
    opts.handle_inf = NULL if inf is SENTINEL else <PyObject *> inf
    opts.handle_nan = NULL if nan is SENTINEL else <PyObject *> nan
    opts.coerce = coerce
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True
    opts.base = INT_MIN

    # Key function backwards compatibility
    if key is not None:
        if on_fail is not None:
            raise ValueError("Cannot set both on_fail and key")
        on_fail = key
    opts.on_fail = NULL if on_fail is None else <PyObject *> on_fail

    # Determine the return value
    cdef PyObject * c_default = NULL if default is SENTINEL else <PyObject *> default
    Options_Set_Return_Value(opts, <PyObject *> x, c_default , raise_on_invalid)

    return <object> PyObject_to_PyNumber(<PyObject *> x, PyNumberType.REAL, &opts)


def fast_float(
    x,
    default=SENTINEL,
    *,
    bint raise_on_invalid=False,
    on_fail=None,
    inf=SENTINEL,
    nan=SENTINEL,
    bint allow_underscores=True,
    key=None,
):
    """
    Quickly convert input to a `float`.
    Any input that is valid for the built-in `float` function will
    be converted to a `float`. An input of a single numeric
    unicode character is also valid. The return value is guaranteed
    to be of type `str` or `float`.

    If the given input is a string and cannot be converted to a `float`
    it will be returned as-is unless `default` or `raise_on_invalid`
    is given.

    Parameters
    ----------
    input : {str, float, int, long}
        The input you wish to convert to a `float`.
    default : optional
        This value will be returned instead of the input
        when the input cannot be converted. Has no effect if
        *raise_on_invalid* is *True*.
    raise_on_invalid : bool, optional
        If *True*, a `ValueError` will be raised if string input cannot be
        converted to a `float`.  If *False*, the string will be
        returned as-is.  The default is *False*.
    on_fail : callable, optional
        If given and the *input* cannot be converted, the input will be
        passed to the callable object and its return value will be returned.
        The function expect only one positional argument.
        For backwards-compatability, you may call this option `key` instead
        of `on_fail`, but this is deprecated behavior.
    nan : optional
        If the input value is NAN or can be parsed as NAN, return this
        value instead of NAN.
    inf : optional
        If the input value is INF or can be parsed as INF, return this
        value instead of INF.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    out : {str, float}
        If the input could be converted to a `float` the return type will
        be `float`. Otherwise, the input `str` will be returned as-is
        (if `raise_on_invalid` is *False*) or whatever value
        is assigned to *default* if *default* is not *None*.

    Raises
    ------
    TypeError
        If the input is not one of `str`, `float`, or `int`.
    ValueError
        If `raise_on_invalid` is *True*, this will be raised if the input
        string cannot be converted to a `float`.

    See Also
    --------
    isfloat
    float

    Examples
    --------

        >>> from fastnumbers import fast_float
        >>> fast_float('56')
        56.0
        >>> fast_float('56.0')
        56.0
        >>> fast_float('56.07')
        56.07
        >>> fast_float('56.07 lb')
        '56.07 lb'
        >>> fast_float(56.07)
        56.07
        >>> fast_float(56)
        56.0
        >>> fast_float('invalid', default=50)
        50
        >>> fast_float('invalid', 50)  # 'default' is first optional positional arg
        50
        >>> fast_float('nan')
        nan
        >>> fast_float('nan', nan=0.0)
        0.0
        >>> fast_float('56.07', nan=0.0)
        56.07
        >>> fast_float('56.07 lb', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
        Traceback (most recent call last):
          ...
        ValueError: could not convert string to float: '56.07 lb'
        >>> fast_float('invalid', on_fail=len)
        7

    Notes
    -----
    It is roughly equivalent to (but much faster than)
    
        >>> def py_fast_float(input, default=None, raise_on_invalid=False,
        ...                   on_fail=None, nan=None, inf=None):
        ...     try:
        ...         x = float(input)
        ...     except ValueError:
        ...         if raise_on_invalid:
        ...             raise
        ...         elif on_fail is not None:
        ...             return on_fail(input)
        ...         elif default is not None:
        ...             return default
        ...         else:
        ...             return input
        ...     else:
        ...         if nan is not None and math.isnan(x):
        ...             return nan
        ...         elif inf is not None and math.isinf(x):
        ...             return inf
        ...         else:
        ...             return x
        ... 

    """
    cdef Options opts
    opts.retval = NULL
    opts.input = <PyObject *> x
    opts.handle_inf = NULL if inf is SENTINEL else <PyObject *> inf
    opts.handle_nan = NULL if nan is SENTINEL else <PyObject *> nan
    opts.coerce = False  # Not used
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True
    opts.base = INT_MIN

    # Key function backwards compatibility
    if key is not None:
        if on_fail is not None:
            raise ValueError("Cannot set both on_fail and key")
        on_fail = key
    opts.on_fail = NULL if on_fail is None else <PyObject *> on_fail

    # Determine the return value
    cdef PyObject * c_default = NULL if default is SENTINEL else <PyObject *> default
    Options_Set_Return_Value(opts, <PyObject *> x, c_default , raise_on_invalid)

    return <object> PyObject_to_PyNumber(<PyObject *> x, PyNumberType.FLOAT, &opts)


def fast_int(
    x,
    default=SENTINEL,
    *,
    bint raise_on_invalid=False,
    on_fail=None,
    base=INT_MIN,
    bint allow_underscores=True,
    key=None,
):
    """
    Quickly convert input to an `int`.

    Any input that is valid for the built-in `int`
    function will be converted to a `int`. An input
    of a single digit unicode character is also valid. The return value
    is guaranteed to be of type `str` or `int`.

    If the given input is a string and cannot be converted to an `int`
    it will be returned as-is unless `default` or `raise_on_invalid`
    is given.

    Parameters
    ----------
    input : {str, float, int, long}
        The input you wish to convert to an `int`.
    default : optional
        This value will be returned instead of the input
        when the input cannot be converted. Has no effect if
        *raise_on_invalid* is *True*.
    raise_on_invalid : bool, optional
        If *True*, a `ValueError` will be raised if string input cannot be
        converted to an `int`. If *False*, the string will be
        returned as-is. The default is *False*.
    on_fail : callable, optional
        If given and the *input* cannot be converted, the input will be
        passed to the callable object and its return value will be returned.
        The function expect only one positional argument.
        For backwards-compatability, you may call this option `key` instead
        of `on_fail`, but this is deprecated behavior.
    base : int, optional
        Follows the rules of Python's built-in :func:`int`; see it's
        documentation for your Python version. If given, the input
        **must** be of type `str`.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    out : {str, int}
        If the input could be converted to an `int`, the return type will be
        `int`.
        Otherwise, the input `str` will be returned as-is
        (if `raise_on_invalid` is *False*) or whatever value
        is assigned to *default* if *default* is not *None*.

    Raises
    ------
    TypeError
        If the input is not one of `str`, `float`, or `int`.
    ValueError
        If `raise_on_invalid` is *True*, this will be raised if the input
        string cannot be converted to an `int`.

    See Also
    --------
    fast_forceint
    isint
    int

    Examples
    --------

        >>> from fastnumbers import fast_int
        >>> fast_int('56')
        56
        >>> fast_int('56.0')
        '56.0'
        >>> fast_int('56.07 lb')
        '56.07 lb'
        >>> fast_int(56.07)
        56
        >>> fast_int(56)
        56
        >>> fast_int('invalid', default=50)
        50
        >>> fast_int('invalid', 50)  # 'default' is first optional positional arg
        50
        >>> fast_int('56.07 lb', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
        Traceback (most recent call last):
          ...
        ValueError: could not convert string to int: '56.07 lb'
        >>> fast_int('invalid', on_fail=len)
        7

    Notes
    -----
    It is roughly equivalent to (but much faster than)

        >>> def py_fast_int(input, default=None, raise_on_invalid=False, on_fail=None):
        ...     try:
        ...         return int(input)
        ...     except ValueError:
        ...         if raise_on_invalid:
        ...             raise
        ...         elif on_fail is not None:
        ...             return on_fail(input)
        ...         elif default is not None:
        ...             return default
        ...         else:
        ...             return input
        ... 

    """
    cdef Options opts
    opts.retval = NULL
    opts.input = <PyObject *> x
    opts.handle_inf = NULL
    opts.handle_nan = NULL
    opts.coerce = False  # Not used
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True

    # Key function backwards compatibility
    if key is not None:
        if on_fail is not None:
            raise ValueError("Cannot set both on_fail and key")
        on_fail = key
    opts.on_fail = NULL if on_fail is None else <PyObject *> on_fail

    # Determine the return value
    cdef PyObject * c_default = NULL if default is SENTINEL else <PyObject *> default
    Options_Set_Return_Value(opts, <PyObject *> x, c_default , raise_on_invalid)

    # Validate the integer base is in the accepted range
    cdef Py_ssize_t longbase
    try:
        longbase = <Py_ssize_t> base
    except OverflowError:
        raise ValueError("int() base must be >= 2 and <= 36")
    if (longbase != INT_MIN and longbase != 0 and longbase < 2) or longbase > 36:
        raise ValueError("int() base must be >= 2 and <= 36")
    opts.base = <int> longbase

    return <object> PyObject_to_PyNumber(<PyObject *> x, PyNumberType.INT, &opts)


def fast_forceint(
    x,
    default=SENTINEL,
    *,
    bint raise_on_invalid=False,
    on_fail=None,
    bint allow_underscores=True,
    key=None,
):
    """
    Quickly convert input to an `int`, truncating if is a `float`.

    Any input that is valid for the built-in `int`
    function will be converted to a `int`. An input
    of a single numeric unicode character is also valid. The return value
    is guaranteed to be of type `str` or `int`.

    In addition to the above, any input valid for the built-in `float` will
    be parsed and the truncated to the nearest integer; for example, '56.07'
    will be converted to `56`.

    If the given input is a string and cannot be converted to an `int`
    it will be returned as-is unless `default` or `raise_on_invalid`
    is given.

    Parameters
    ----------
    input : {str, float, int, long}
        The input you wish to convert to an `int`.
    default : optional
        This value will be returned instead of the input
        when the input cannot be converted. Has no effect if
        *raise_on_invalid* is *True*
    raise_on_invalid : bool, optional
        If *True*, a `ValueError` will be raised if string input cannot be
        converted to an `int`.  If *False*, the string will be
        returned as-is.  The default is *False*.
    on_fail : callable, optional
        If given and the *input* cannot be converted, the input will be
        passed to the callable object and its return value will be returned.
        The function expect only one positional argument.
        For backwards-compatability, you may call this option `key` instead
        of `on_fail`, but this is deprecated behavior.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    out : {str, int}
        If the input could be converted to an `int`, the return type will be
        `int`.
        Otherwise, the input `str` will be returned as-is
        (if `raise_on_invalid` is *False*) or whatever value
        is assigned to *default* if *default* is not *None*.

    Raises
    ------
    TypeError
        If the input is not one of `str`, `float`, or `int`.
    ValueError
        If `raise_on_invalid` is *True*, this will be raised if the input
        string cannot be converted to an `int`.

    See Also
    --------
    fast_int
    isintlike

    Examples
    --------

        >>> from fastnumbers import fast_forceint
        >>> fast_forceint('56')
        56
        >>> fast_forceint('56.0')
        56
        >>> fast_forceint('56.07')
        56
        >>> fast_forceint('56.07 lb')
        '56.07 lb'
        >>> fast_forceint(56.07)
        56
        >>> fast_forceint(56)
        56
        >>> fast_forceint('invalid', default=50)
        50
        >>> fast_forceint('invalid', 50)  # 'default' is first optional positional arg
        50
        >>> fast_forceint('56.07 lb', raise_on_invalid=True) #doctest: +IGNORE_EXCEPTION_DETAIL
        Traceback (most recent call last):
          ...
        ValueError: could not convert string to float: '56.07 lb'
        >>> fast_forceint('invalid', on_fail=len)
        7

    Notes
    -----
    It is roughly equivalent to (but much faster than)

        >>> def py_fast_forceint(input, default=None, raise_on_invalid=False, on_fail=None):
        ...     try:
        ...         return int(input)
        ...     except ValueError:
        ...         try:
        ...             return int(float(input))
        ...         except ValueError:
        ...             if raise_on_invalid:
        ...                 raise
        ...             elif on_fail is not None:
        ...                 return on_fail(input)
        ...             elif default is not None:
        ...                 return default
        ...             else:
        ...                 return input
        ... 

    """
    cdef Options opts
    opts.retval = NULL
    opts.input =  <PyObject *> x
    opts.handle_inf = NULL
    opts.handle_nan = NULL
    opts.coerce = False  # Not used
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True
    opts.base = INT_MIN

    # Key function backwards compatibility
    if key is not None:
        if on_fail is not None:
            raise ValueError("Cannot set both on_fail and key")
        on_fail = key
    opts.on_fail = NULL if on_fail is None else <PyObject *> on_fail

    # Determine the return value
    cdef PyObject * c_default = NULL if default is SENTINEL else <PyObject *> default
    Options_Set_Return_Value(opts, <PyObject *> x, c_default , raise_on_invalid)

    return <object> PyObject_to_PyNumber(<PyObject *> x, PyNumberType.FORCEINT, &opts)


def isreal(
    x,
    *,
    bint str_only=False,
    bint num_only=False,
    allow_inf=False,
    allow_nan=False,
    bint allow_underscores=True
):
    """
    Quickly determine if a string is a real number.

    Returns *True* if the input is valid input for the built-in `float` or
    `int` functions, or is a single valid numeric unicode character.

    The input may be whitespace-padded.

    Parameters
    ----------
    input :
        The input you wish to test if it is a real number.
    str_only : bool, optional
        If *True*, then any non-`str` input will cause this function to return
        *False*. The default is *False*.
    num_only : bool, optional
        If *True*, then any `str` input will cause this function to return
        *False*. The default is *False*.
    allow_inf : bool, optional
        If *True*, then the strings 'inf' and 'infinity' will also return *True*.
        This check is case-insensitive, and the string may be signed (i.e. '+/-').
        The default is *False*.
    allow_nan : bool, optional
        If *True*, then the string 'nan' will also return *True*.
        This check is case-insensitive, and the string may be signed (i.e. '+/-').
        The default is *False*.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    result : bool
        Whether or not the input is a real number.

    See Also
    --------
    fast_real

    Examples
    --------

        >>> from fastnumbers import isreal
        >>> isreal('56')
        True
        >>> isreal('56.07')
        True
        >>> isreal('56.07', num_only=True)
        False
        >>> isreal('56.07 lb')
        False
        >>> isreal(56.07)
        True
        >>> isreal(56.07, str_only=True)
        False
        >>> isreal(56)
        True
        >>> isreal('nan')
        False
        >>> isreal('nan', allow_nan=True)
        True

    Notes
    -----
    It is roughly equivalent to (but much faster than)

        >>> import re
        >>> def py_isreal(input, str_only=False, num_only=False,
        ...               allow_nan=False, allow_inf=False):
        ...     if str_only and type(input) != str:
        ...         return False
        ...     if num_only and type(input) not in (float, int):
        ...         return False
        ...     try:
        ...         x = bool(re.match(r'[-+]?\\d*\\.?\\d+(?:[eE][-+]?\\d+)?$', input))
        ...     except TypeError:
        ...         return type(input) in (float, int)
        ...     else:
        ...         if x:
        ...             return True
        ...         elif allow_inf and input.lower().strip().lstrip('-+') in ('inf', 'infinity'):
        ...             return True
        ...         elif allow_nan and input.lower().strip().lstrip('-+') == 'nan':
        ...             return True
        ...         else:
        ...             return False
        ... 

    """
    cdef Options opts
    opts.retval = <PyObject *> None
    opts.input = NULL
    opts.on_fail = NULL
    opts.handle_inf = <PyObject *> allow_inf
    opts.handle_nan = <PyObject *> allow_nan
    opts.coerce = True  # Not used
    opts.num_only = num_only
    opts.str_only = str_only
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True
    opts.base = INT_MIN
    return <object> PyObject_is_number(<PyObject *> x, PyNumberType.REAL, &opts)


def isfloat(
    x,
    *,
    bint str_only=False,
    bint num_only=False,
    allow_inf=False,
    allow_nan=False,
    bint allow_underscores=True
):
    """
    Quickly determine if a string is a `float`.

    Returns *True* if the input is valid input for the built-in `float`
    function, is already a valid `float`, or is a single valid numeric unicode
    character. It differs from `isreal` in that an `int` input will return
    *False*.

    The input may be whitespace-padded.

    Parameters
    ----------
    input :
        The input you wish to test if it is a `float`.
    str_only : bool, optional
        If *True*, then any non-`str` input will cause this function to return
        *False*. The default is *False*.
    num_only : bool, optional
        If *True*, then any `str` input will cause this function to return
        *False*. The default is *False*.
    allow_inf : bool, optional
        If *True*, then the strings 'inf' and 'infinity' will also return *True*.
        This check is case-insensitive, and the string may be signed (i.e. '+/-').
        The default is *False*.
    allow_nan : bool, optional
        If *True*, then the string 'nan' will also return *True*.
        This check is case-insensitive, and the string may be signed (i.e. '+/-').
        The default is *False*.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    result : bool
        Whether or not the input is a `float`.

    See Also
    --------
    fast_float, isreal

    Examples
    --------

        >>> from fastnumbers import isfloat
        >>> isfloat('56')
        True
        >>> isfloat('56.07')
        True
        >>> isfloat('56.07', num_only=True)
        False
        >>> isfloat('56.07 lb')
        False
        >>> isfloat(56.07)
        True
        >>> isfloat(56.07, str_only=True)
        False
        >>> isfloat(56)
        False
        >>> isfloat('nan')
        False
        >>> isfloat('nan', allow_nan=True)
        True

    Notes
    -----
    It is roughly equivalent to (but much faster than)

        >>> import re
        >>> def py_isfloat(input, str_only=False, num_only=False,
        ...                allow_nan=False, allow_inf=False):
        ...     if str_only and type(input) != str:
        ...         return False
        ...     if num_only and type(input) != float:
        ...         return False
        ...     try:
        ...         x = bool(re.match(r'[-+]?\\d*\\.?\\d+(?:[eE][-+]?\\d+)?$', input))
        ...     except TypeError:
        ...         return type(input) == float
        ...     else:
        ...         if x:
        ...             return True
        ...         elif allow_inf and input.lower().strip().lstrip('-+') in ('inf', 'infinity'):
        ...             return True
        ...         elif allow_nan and input.lower().strip().lstrip('-+') == 'nan':
        ...             return True
        ...         else:
        ...             return False

    """
    cdef Options opts
    opts.retval = <PyObject *> None
    opts.input = NULL
    opts.on_fail = NULL
    opts.handle_inf = <PyObject *> allow_inf
    opts.handle_nan = <PyObject *> allow_nan
    opts.coerce = True  # Not used
    opts.num_only = num_only
    opts.str_only = str_only
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True
    opts.base = INT_MIN
    return <object> PyObject_is_number(<PyObject *> x, PyNumberType.FLOAT, &opts)


def isint(
    x,
    *,
    bint str_only=False,
    bint num_only=False,
    base=INT_MIN,
    bint allow_underscores=True
):
    """
    Quickly determine if a string is an `int`.

    Returns *True* if the input is valid input for the built-in `int`
    function, is already a valid `int`, or is a single valid digit unicode
    character. It differs from `isintlike` in that a `float` input will
    return *False* and that `int`-like strings (i.e. '45.0') will return
    *False*.

    The input may be whitespace-padded.

    Parameters
    ----------
    input :
        The input you wish to test if it is an `int`.
    str_only : bool, optional
        If *True*, then any non-`str` input will cause this function to return
        *False*. The default is *False*.
    num_only : bool, optional
        If *True*, then any `str` input will cause this function to return
        *False*. The default is *False*.
    base : int, optional
        Follows the rules of Python's built-in :func:`int`; see it's
        documentation for your Python version. If given, the input
        **must** be of type `str`.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    result : bool
        Whether or not the input is an `int`.

    See Also
    --------
    fast_int, isintlike

    Examples
    --------

        >>> from fastnumbers import isint
        >>> isint('56')
        True
        >>> isint('56', num_only=True)
        False
        >>> isint('56.07')
        False
        >>> isint('56.07 lb')
        False
        >>> isint(56.07)
        False
        >>> isint(56)
        True
        >>> isint(56, str_only=True)
        False

    Notes
    -----
    It is roughly equivalent to (but much faster than)

        >>> import re
        >>> def py_isint(input, str_only=False, num_only=False):
        ...     if str_only and type(input) != str:
        ...         return False
        ...     if num_only and type(input) != int:
        ...         return False
        ...     try:
        ...         return bool(re.match(r'[-+]?\\d+$', input))
        ...     except TypeError:
        ...         return False
        ... 

    """
    cdef Options opts
    opts.retval = <PyObject *> None
    opts.input = NULL
    opts.on_fail = NULL
    opts.handle_inf = <PyObject *> False
    opts.handle_nan = <PyObject *> False
    opts.coerce = True  # Not used
    opts.num_only = num_only
    opts.str_only = str_only
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True

    # Validate the integer base is in the accepted range
    cdef Py_ssize_t longbase
    try:
        longbase = <Py_ssize_t> base
    except OverflowError:
        raise ValueError("int() base must be >= 2 and <= 36")
    if (longbase != INT_MIN and longbase != 0 and longbase < 2) or longbase > 36:
        raise ValueError("int() base must be >= 2 and <= 36")
    opts.base = <int> longbase
    return <object> PyObject_is_number(<PyObject *> x, PyNumberType.INT, &opts)


def isintlike(
    x,
    *,
    bint str_only=False,
    bint num_only=False,
    bint allow_underscores=True
):
    """
    Quickly determine if a string (or object) is an `int` or `int`-like.

    Returns *True* if the input is valid input for the built-in `int`
    function, is already a valid `int` or `float`, or is a single valid
    numeric unicode character. It differs from `isintlike` in that `int`-like
    floats or strings (i.e. '45.0') will return *True*.

    The input may be whitespace-padded.

    Parameters
    ----------
    input :
        The input you wish to test if it is a `int`-like.

    Returns
    -------
    result : bool
        Whether or not the input is an `int`.
    str_only : bool, optional
        If *True*, then any non-`str` input will cause this function to return
        *False*. The default is *False*.
    num_only : bool, optional
        If *True*, then any `str` input will cause this function to return
        *False*. The default is *False*.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    See Also
    --------
    fast_forceint

    Examples
    --------

        >>> from fastnumbers import isintlike
        >>> isintlike('56')
        True
        >>> isintlike('56', num_only=True)
        False
        >>> isintlike('56.07')
        False
        >>> isintlike('56.0')
        True
        >>> isintlike('56.07 lb')
        False
        >>> isintlike(56.07)
        False
        >>> isintlike(56.0)
        True
        >>> isintlike(56.0, str_only=True)
        False
        >>> isintlike(56)
        True

    Notes
    -----
    It is roughly equivalent to (but much faster than)

        >>> import re
        >>> def py_isintlike(input, str_only=False, num_only=False):
        ...     if str_only and type(input) != str:
        ...         return False
        ...     if num_only and type(input) not in (int, float):
        ...         return False
        ...     try:
        ...         if re.match(r'[-+]?\\d+$', input):
        ...             return True
        ...         elif re.match(r'[-+]?\\d*\\.?\\d+(?:[eE][-+]?\\d+)?$', input):
        ...             return float(input).is_integer()
        ...         else:
        ...             return False
        ...     except TypeError:
        ...         if type(input) == float:
        ...             return input.is_integer()
        ...         elif type(input) == int:
        ...             return True
        ...         else:
        ...             return False
        ... 

    """
    cdef Options opts
    opts.retval = <PyObject *> None
    opts.input = NULL
    opts.on_fail = NULL
    opts.handle_inf = <PyObject *> False
    opts.handle_nan = <PyObject *> False
    opts.coerce = True  # Not used
    opts.num_only = num_only
    opts.str_only = str_only
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True
    opts.base = INT_MIN
    return <object> PyObject_is_number(<PyObject *> x, PyNumberType.INTLIKE, &opts)


def query_type(
    x,
    *,
    allow_inf=False,
    allow_nan=False,
    bint coerce=False,
    allowed_types=None,
    bint allow_underscores=True
):
    """
    Quickly determine the type that fastnumbers would return for a given input.

    For string or bytes-like input, the contents of the string will be examined and
    the type *int* or *float* will be returned if the object contains a representation
    of an *int* or *float*, respectively. For all other cases, the type of the input
    object is returned, just like the built-in function *type*.

    The input may be whitespace-padded.

    Parameters
    ----------
    input :
        The input of which you wish to query the type fastnumbers might return.
    allow_inf : bool, optional
        If *True*, then the strings 'inf' and 'infinity' will also return *float*.
        This check is case-insensitive, and the string may be signed (i.e. '+/-').
        The default is *False*.
    allow_nan : bool, optional
        If *True*, then the string 'nan' will also return *float*.
        This check is case-insensitive, and the string may be signed (i.e. '+/-').
        The default is *False*.
    coerce : bool, optional
        If *True*, then numbers that are given as *float* but could be converted to
        an *int* without loss of precision will return type *int* instead of *float*.
    allowed_types : sequence of types, optional
        If given, then only the given types may be returned, and anything else will
        return *None*.
    allow_underscores : bool, optional
        Starting with Python 3.6, underscores are allowed in numeric literals
        and in strings passed to `int` or `float` (see PEP 515 for details on
        what is and is not allowed). You can disable that behavior by setting
        this option to *False* - the default is *True*.

    Returns
    -------
    result : type
        The type that fastnumbers might return for the given input.

    See Also
    --------
    isreal
    isfloat
    isint
    isintlike

    Examples
    --------

        >>> from fastnumbers import query_type
        >>> query_type('56')
        <class 'int'>
        >>> query_type('56.07')
        <class 'float'>
        >>> query_type('56.07 lb')
        <class 'str'>
        >>> query_type('56.07 lb', allowed_types=(float, int))  # returns None
        >>> query_type('56.0')
        <class 'float'>
        >>> query_type('56.0', coerce=True)
        <class 'int'>
        >>> query_type(56.07)
        <class 'float'>
        >>> query_type(56)
        <class 'int'>
        >>> query_type('nan')
        <class 'str'>
        >>> query_type('nan', allow_nan=True)
        <class 'float'>

    """
    cdef Options opts
    opts.retval = <PyObject *> None
    opts.input = NULL
    opts.on_fail = NULL
    opts.handle_inf = <PyObject *> allow_inf
    opts.handle_nan = <PyObject *> allow_nan
    opts.coerce = coerce
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = allow_underscores
    opts.allow_uni = True
    opts.base = INT_MIN

    if allowed_types is not None:
        if not isinstance(allowed_types, (list, tuple, set)):
            raise TypeError(f"allowed_type is not a sequence type: {allowed_types!r}")
        if len(allowed_types) < 1:
            raise ValueError("allowed_type must not be an empty sequence")
    
    result = PyObject_contains_type(<PyObject *> x, &opts)

    if result != NULL and allowed_types is not None and <object> result not in allowed_types:
        return None
    else:
        return <object> result


def int(*args, **kwargs):
    """
    Drop-in but faster replacement for the built-in *int*.

    Behaves identically to the built-in *int* except for the following:

        - Cannot convert from the ``__trunc__`` special method of an object.
        - Is implemented as a function, not a class, which means it cannot be
        sub-classed, and has no *from_bytes* classmethod.
        - You cannot use this function in ``isinstance``.

    If you need any of the above functionality you can still access the original
    *int* class through ``builtins``.

        >>> from fastnumbers import int
        >>> isinstance(9, int) # doctest: +ELLIPSIS
        Traceback (most recent call last):
            ...
        TypeError: ...
        >>> import builtins
        >>> isinstance(9, builtins.int)
        True

    """
    # At the time of this writing, Cython does not natively support arguments
    # that are positional only but also optional. In order to be consistent with
    # the python built-in this is required, so we use the Python C-API function
    # to parse the arguments.
    # TODO: If/when cython supports the positional-only syntax `/` investigate
    #       if that can be used to support this use case.
    cdef PyObject *x = NULL
    cdef PyObject *base = NULL
    PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:int", ["", "base", NULL], &x, &base)

    cdef Options opts
    opts.retval = NULL
    opts.input = <PyObject *> x
    opts.handle_inf = NULL
    opts.handle_nan = NULL
    opts.coerce = False  # Not used
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = True
    opts.allow_uni = False

    # Validate the integer base is in the accepted range
    cdef Py_ssize_t longbase
    try:
        if base == NULL:
            longbase = <Py_ssize_t> INT_MIN
        else:
            longbase = <Py_ssize_t> <object> base
    except OverflowError:
        raise ValueError("int() base must be >= 2 and <= 36")
    if (longbase != INT_MIN and longbase != 0 and longbase < 2) or longbase > 36:
        raise ValueError("int() base must be >= 2 and <= 36")
    opts.base = <int> longbase

    if x == NULL:
        if opts.base != INT_MIN:
            raise TypeError("int() missing string argument")
        return 0

    return <object> PyObject_to_PyNumber(<PyObject *> x, PyNumberType.INT, &opts)

def float(*args):
    """
    Drop-in but faster replacement for the built-in *float*.

    Behaves identically to the built-in *float* except for the following:

        - Is implemented as a function, not a class, which means it cannot be
        sub-classed, and has no *fromhex* classmethod.
        - A *ValueError* will be raised instead of a *UnicodeEncodeError*
        if a partial surrogate is given as input.
        - You cannot use this function in ``isinstance``.

    If you need any of the above functionality you can still access the original
    *float* class through ``builtins``.

        >>> from fastnumbers import float
        >>> isinstance(9.4, float)  # doctest: +ELLIPSIS
        Traceback (most recent call last):
            ...
        TypeError: ...
        >>> import builtins
        >>> isinstance(9.4, builtins.float)
        True

    """
    # At the time of this writing, Cython does not natively support arguments
    # that are positional only but also optional. In order to be consistent with
    # the python built-in this is required, so we use the Python C-API function
    # to parse the arguments.
    # TODO: If/when cython supports the positional-only syntax `/` investigate
    #       if that can be used to support this use case.
    cdef PyObject *x = NULL
    PyArg_ParseTuple(args, "|O:float", &x)

    cdef Options opts
    opts.retval = NULL
    opts.input = x
    opts.handle_inf = NULL
    opts.handle_nan = NULL
    opts.coerce = False  # Not used
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = True
    opts.allow_uni = False
    opts.base = INT_MIN

    if x == NULL:
        return 0.0

    return <object> PyObject_to_PyNumber(x, PyNumberType.FLOAT, &opts)

def real(*args, **kwargs):
    """
    Convert to *float* or *int*, whichever is most appropriate.

    If an *int* literal or string containing an *int* is provided,
    then an *int* will be returned. If a *float* literal or a string
    containing a non-*int* and non-*complex* number is provided,
    a *float* will be returned.

    If *coerce* is *True* (the default), then if a *float* is given
    that has no decimal places after conversion or only zeros after
    the decimal point, it will be returned as an *int* instead of a
    *float*.

    """
    # At the time of this writing, Cython does not natively support arguments
    # that are positional only but also optional. In order to be consistent with
    # the python built-in this is required, so we use the Python C-API function
    # to parse the arguments.
    # TODO: If/when cython supports the positional-only syntax `/` investigate
    #       if that can be used to support this use case.
    cdef PyObject *x = NULL
    cdef bint coerce = True
    PyArg_ParseTupleAndKeywords(args, kwargs, "|O$p:real", ["", "coerce", NULL], &x, &coerce)

    cdef Options opts
    opts.retval = NULL
    opts.input = x
    opts.handle_inf = NULL
    opts.handle_nan = NULL
    opts.coerce = coerce
    opts.num_only = True
    opts.str_only = True
    opts.allow_underscores = True
    opts.allow_uni = False
    opts.base = INT_MIN

    if x == NULL:
        return 0 if coerce else 0.0

    return <object> PyObject_to_PyNumber(x, PyNumberType.REAL, &opts)
