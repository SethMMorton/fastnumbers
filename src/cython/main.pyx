#
# C implementation of the python library fastnumbers.
#
# Author: Seth M. Morton, July 30, 2014
#
# distutils: language = c++
from cpython.getargs cimport (
    PyArg_ParseTuple,
    PyArg_ParseTupleAndKeywords,
)
from cpython.ref cimport PyObject
from cpython.version cimport PY_VERSION_HEX
from libc.limits cimport INT_MIN


cdef extern from "fastnumbers/parser.hpp":
    ctypedef enum ParserType:
        NUMERIC "ParserType::NUMERIC"
        UNICODE "ParserType::UNICODE"
        CHARACTER "ParserType::CHARACTER"
        UNKNOWN "ParserType::UNKNOWN"
    cdef cppclass Parser:
        @staticmethod
        bint float_is_intlike(double)


cdef extern from "fastnumbers/evaluator.hpp":
    ctypedef enum UserType:
        REAL "UserType::REAL"
        FLOAT "UserType::FLOAT"
        INT "UserType::INT"
        INTLIKE "UserType::INTLIKE"
        FORCEINT "UserType::FORCEINT"
    cdef cppclass Evaluator:
        Evaluator()
        void set_object(object) except +
        void set_coerce(bint)
        void set_nan_allowed(bint)
        void set_inf_allowed(bint)
        void set_base(int)
        void set_unicode_allowed(bint)
        void set_underscores_allowed(bint)
        object get_object()
        int get_base()
        bint is_default_base()
        ParserType parser_type()
        bint is_type(UserType) except +
        bint type_is_float() except +
        bint type_is_int() except +
        Payload as_type(UserType) except +


cdef extern from "fastnumbers/payload.hpp":
    ctypedef enum ActionType:
        NAN_ACTION "ActionType::NAN_ACTION"
        INF_ACTION "ActionType::INF_ACTION"
        NEG_NAN_ACTION "ActionType::NEG_NAN_ACTION"
        NEG_INF_ACTION "ActionType::NEG_INF_ACTION"
        ERROR_INVALID_INT "ActionType::ERROR_INVALID_INT"
        ERROR_INVALID_FLOAT "ActionType::ERROR_INVALID_FLOAT"
        ERROR_INVALID_BASE "ActionType::ERROR_INVALID_BASE"
        ERROR_INFINITY_TO_INT "ActionType::ERROR_INFINITY_TO_INT"
        ERROR_NAN_TO_INT "ActionType::ERROR_NAN_TO_INT"
        ERROR_BAD_TYPE_INT "ActionType::ERROR_BAD_TYPE_INT"
        ERROR_BAD_TYPE_FLOAT "ActionType::ERROR_BAD_TYPE_FLOAT"
        ERROR_ILLEGAL_EXPLICIT_BASE "ActionType::ERROR_ILLEGAL_EXPLICIT_BASE"

    ctypedef enum PayloadType:
        ACTION "PayloadType::ACTION"
        LONG "PayloadType::LONG"
        DOUBLE "PayloadType::DOUBLE"
        DOUBLE_TO_LONG "PayloadType::DOUBLE_TO_LONG"
        PYOBJECT "PayloadType::PYOBJECT"

    cdef cppclass Payload:
        Payload()
        PayloadType payload_type()
        ActionType get_action()
        double to_double()
        long to_long()
        object to_pyobject()


cdef extern from "fastnumbers/c_str_parsing.hpp":
    long FN_MAX_INT_LEN
    long FN_DBL_DIG
    long FN_MAX_EXP
    long FN_MIN_EXP


# Sentinel to use for arguments where None is a valid option
cdef object SENTINEL = object()


# Infinity and NaN can be cached at startup
cdef object INFINITY = float("inf")
cdef object NEG_INFINITY = float("-inf")
cdef object NAN = float("nan")
cdef object NEG_NAN = float("-nan")


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
    cdef Evaluator evaluator
    evaluator.set_object(x)
    evaluator.set_coerce(coerce)
    evaluator.set_underscores_allowed(allow_underscores)
    on_fail = on_fail_backwards_compatibility(on_fail, key)
    on_invalid_return = determine_failure_return_value(x, raise_on_invalid, default)
    return convert_evaluator_payload(
        x, evaluator, UserType.REAL, inf, nan, on_invalid_return, on_fail
    )


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
    cdef Evaluator evaluator
    evaluator.set_object(x)
    evaluator.set_underscores_allowed(allow_underscores)
    on_fail = on_fail_backwards_compatibility(on_fail, key)
    on_invalid_return = determine_failure_return_value(x, raise_on_invalid, default)
    return convert_evaluator_payload(
        x, evaluator, UserType.FLOAT, inf, nan, on_invalid_return, on_fail
    )


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
    cdef Evaluator evaluator
    evaluator.set_object(x)
    evaluator.set_base(validate_integer_base(base))
    evaluator.set_unicode_allowed(evaluator.is_default_base())
    evaluator.set_underscores_allowed(allow_underscores)
    on_fail = on_fail_backwards_compatibility(on_fail, key)
    on_invalid_return = determine_failure_return_value(x, raise_on_invalid, default)
    return convert_evaluator_payload(
        x, evaluator, UserType.INT, None, None, on_invalid_return, on_fail
    )


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
    cdef Evaluator evaluator
    evaluator.set_object(x)
    evaluator.set_underscores_allowed(allow_underscores)
    on_fail = on_fail_backwards_compatibility(on_fail, key)
    on_invalid_return = determine_failure_return_value(x, raise_on_invalid, default)
    return convert_evaluator_payload(
        x, evaluator, UserType.FORCEINT, None, None, on_invalid_return, on_fail
    )


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
    return object_is_number(
        x,
        UserType.REAL,
        INT_MIN,
        allow_nan,
        allow_inf,
        str_only,
        num_only,
        allow_underscores,
    )


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
    return object_is_number(
        x,
        UserType.FLOAT,
        INT_MIN,
        allow_nan,
        allow_inf,
        str_only,
        num_only,
        allow_underscores,
    )


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
    return object_is_number(
        x,
        UserType.INT,
        validate_integer_base(base),
        False,
        False,
        str_only,
        num_only,
        allow_underscores,
    )


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
    return object_is_number(
        x,
        UserType.INTLIKE,
        INT_MIN,
        False,
        False,
        str_only,
        num_only,
        allow_underscores,
    )


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
    if allowed_types is not None:
        if not isinstance(allowed_types, (list, tuple, set)):
            raise TypeError(f"allowed_type is not a sequence type: {allowed_types!r}")
        if len(allowed_types) < 1:
            raise ValueError("allowed_type must not be an empty sequence")
    
    # Create the evaluator and populate with the appropriate options
    cdef Evaluator evaluator
    evaluator.set_object(x)
    evaluator.set_coerce(coerce)
    evaluator.set_underscores_allowed(allow_underscores)
    if evaluator.parser_type() == ParserType.NUMERIC:
        evaluator.set_nan_allowed(True)
        evaluator.set_inf_allowed(True)
    else:
        evaluator.set_nan_allowed(allow_nan)
        evaluator.set_inf_allowed(allow_inf)

    if evaluator.type_is_int():
        return validate_query_type(int, allowed_types)
    elif evaluator.type_is_float():
        return validate_query_type(float, allowed_types)
    else:
        return validate_query_type(type(x), allowed_types)


def fn_int(*args, **kwargs):
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
    cdef PyObject *base_ = NULL
    if PY_VERSION_HEX >= 0x030700A0:
        PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:int", ["", "base", NULL], &x, &base_)
    else:
        PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:int", ["x", "base", NULL], &x, &base_)

    # When converting base to integer, note that in the way this function
    # is parsing arguments a NULL is allowed so that is handled specially
    cdef int base = INT_MIN if base_ == NULL else validate_integer_base(<object> base_)
    if x == NULL:
        if base != INT_MIN:
            raise TypeError("int() missing string argument")
        return 0

    cdef Evaluator evaluator
    evaluator.set_object(<object> x)
    evaluator.set_unicode_allowed(False)
    evaluator.set_base(base)
    return convert_evaluator_payload(
        <object> x, evaluator, UserType.INT, None, None, SENTINEL, None
    )


def fn_float(*args):
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

    if x == NULL:
        return 0.0

    cdef Evaluator evaluator
    evaluator.set_object(<object> x)
    evaluator.set_unicode_allowed(False)
    return convert_evaluator_payload(
        <object> x, evaluator, UserType.FLOAT, SENTINEL, SENTINEL, SENTINEL, None
    )


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

    if x == NULL:
        return 0 if coerce else 0.0

    cdef Evaluator evaluator
    evaluator.set_object(<object> x)
    evaluator.set_coerce(coerce)
    evaluator.set_unicode_allowed(False)
    return convert_evaluator_payload(
        <object> x, evaluator, UserType.REAL, SENTINEL, SENTINEL, SENTINEL, None
    )

# PRIVATE FUNCTIONS


cdef validate_query_type(result, allowed_types):
    """Ensure the type is allowed, otherwise return None"""
    if allowed_types is not None and result not in allowed_types:
        return None
    else:
        return result


cdef object_is_number(
    obj,
    const UserType type,
    int base,
    bint allow_nan,
    bint allow_inf,
    bint str_only,
    bint num_only,
    bint allow_underscores,
):
    """Check if an arbitrary PyObject is a PyNumber."""

    # Create the evaluator and populate with the appropriate options
    cdef Evaluator evaluator
    evaluator.set_object(obj)
    evaluator.set_base(base)
    evaluator.set_nan_allowed(allow_nan)
    evaluator.set_inf_allowed(allow_inf)
    evaluator.set_underscores_allowed(allow_underscores)

    # If the user explictly asked to disallow some types, check that here.
    cdef ParserType ptype = evaluator.parser_type()
    if ptype == ParserType.NUMERIC:
        if str_only:
            return False
        evaluator.set_nan_allowed(True)
        evaluator.set_inf_allowed(True)
    elif ptype == ParserType.UNICODE or ptype == ParserType.CHARACTER:
        if num_only:
            return False
    else:
        return False

    # Evaluate the type!
    return evaluator.is_type(type)


cdef on_fail_backwards_compatibility(on_fail, key):
    """Ensure that both on_fail and key are not given together"""
    if key is not None:
        if on_fail is not None:
            raise ValueError("Cannot set both on_fail and key")
        return key
    return on_fail


cdef int validate_integer_base(base) except -1:
    """Ensure the given integer base is within range"""
    cdef Py_ssize_t base_
    try:
        base_ = <Py_ssize_t> base
    except OverflowError:
        raise ValueError("int() base must be >= 2 and <= 36")
    if (base_ != INT_MIN and base_ != 0 and base_ < 2) or base_ > 36:
        raise ValueError("int() base must be >= 2 and <= 36")
    return <int> base_


cdef determine_failure_return_value(obj, bint raise_on_invalid, default):
    """Return the best return value for when an error occurs"""
    if raise_on_invalid:
        return SENTINEL
    elif default is not SENTINEL:
        return default
    else:
        return obj


cdef convert_evaluator_payload(
    object obj,
    Evaluator & evaluator,
    const UserType ntype,
    object infinity,
    object nan,
    object return_object,
    object on_fail,
):
    """Convert the Payload of an Evaluator into a PyNumber"""
    cdef Payload payload = evaluator.as_type(ntype)
    cdef PayloadType ptype = payload.payload_type()
    cdef ActionType atype

    # I realize this chain of ifs looks ugly and "the wrong way", but
    # Cython will smartly convert this into a switch statement because
    # it can detect all of the statements in the if are from the same enum.

    # Level 1: If the payload contains an actual number,
    #          convert to PyObject directly
    if ptype == PayloadType.LONG:
        return payload.to_long()

    elif ptype == PayloadType.DOUBLE:
        return payload.to_double()

    elif ptype == PayloadType.DOUBLE_TO_LONG:
        return int(payload.to_double())

    elif ptype == PayloadType.PYOBJECT:
        if return_object is SENTINEL:
            return payload.to_pyobject()
        try:
            return payload.to_pyobject()
        except Exception:
            return on_fail(obj) if on_fail is not None else return_object
            
    # Level 2: We need to instruct Cython as to what action to take
    elif ptype == PayloadType.ACTION:
        atype = payload.get_action()

        # Return the appropriate value for when infinity is found
        if atype == ActionType.INF_ACTION:
            return INFINITY if infinity is SENTINEL else infinity

        # Return the appropriate value for when negative infinity is found
        elif atype == ActionType.NEG_INF_ACTION:
            return NEG_INFINITY if infinity is SENTINEL else infinity

        # Return the appropriate value for when NaN is found
        elif atype == ActionType.NAN_ACTION:
            return NAN if nan is SENTINEL else nan

        # Return the appropriate value for when negative NaN is found
        elif atype == ActionType.NEG_NAN_ACTION:
            return NEG_NAN if nan is SENTINEL else nan

        # Raise an exception due passing an invalid type to convert to
        # an integer or float, or if using an explicit integer base
        # where it shouldn't be used
        elif (atype == ActionType.ERROR_BAD_TYPE_INT
                or atype == ActionType.ERROR_BAD_TYPE_FLOAT
                or atype == ActionType.ERROR_ILLEGAL_EXPLICIT_BASE
        ):
            raise_appropriate_exception(obj, atype, evaluator)

        # Raise an exception if that is what the user has asked for
        elif return_object is SENTINEL:
            raise_appropriate_exception(obj, atype, evaluator)

        # Transform the input via a function
        elif on_fail is not None:
            return on_fail(obj)

        # Return the input as-is
        else:
            return return_object


cdef raise_appropriate_exception(obj, ActionType atype, Evaluator & evaluator):
    """Prepare and raise the appropriate exception given an action type"""
    cdef str obj_repr
    cdef str obj_name
    cdef str msg
    cdef type exception_type

    # Raise an exception due passing an invalid type to convert to an integer
    if atype == ActionType.ERROR_BAD_TYPE_INT:
        obj_name = type(obj).__name__
        msg = f"int() argument must be a string, a bytes-like object or a number, not '{obj_name}'"
        exception_type = TypeError

    # Raise an exception due passing an invalid type to convert to a float
    elif atype == ActionType.ERROR_BAD_TYPE_FLOAT:
        obj_name = type(obj).__name__
        msg = f"float() argument must be a string or a number, not '{obj_name}'"
        exception_type = TypeError

    # Raise an exception due to useing an explict integer base where it shouldn't
    elif atype == ActionType.ERROR_ILLEGAL_EXPLICIT_BASE:
        msg = "int() can't convert non-string with explicit base"
        exception_type = TypeError

    # Raise an exception due to an invalid integer
    elif atype == ActionType.ERROR_INVALID_INT:
        base = evaluator.get_base()
        obj_repr = repr(obj)
        msg = f"invalid literal for int() with base {base}: {obj_repr}"
        exception_type = ValueError

    # Raise an exception due to an invalid float
    elif atype == ActionType.ERROR_INVALID_FLOAT:
        obj_repr = repr(obj)
        msg = f"could not convert string to float: {obj_repr}"
        exception_type = ValueError

    # Raise an exception due to an invalid base for integer conversion
    elif atype == ActionType.ERROR_INVALID_BASE:
        msg = "int() can't convert non-string with explicit base"
        exception_type = TypeError

    # Raise an exception due to attempting to convert infininty to an integer
    elif atype == ActionType.ERROR_INFINITY_TO_INT:
        msg = "cannot convert float infinity to integer"
        exception_type = OverflowError

    # Raise an exception due to attempting to convert NaN to an integer
    elif atype == ActionType.ERROR_NAN_TO_INT:
        msg = "cannot convert float NaN to integer"
        exception_type = ValueError

    raise exception_type(msg)
