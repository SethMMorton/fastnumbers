"""Public interface for the fastnumbers package."""

from __future__ import annotations

from typing import TYPE_CHECKING

try:
    # The redundant "as" tells mypy to treat as explict import
    from fastnumbers._version import __version__, __version_tuple__
except ImportError:
    __version__ = "unknown version"
    __version_tuple__ = (0, 0, "unknown version")
from .fastnumbers import (
    ALLOWED,
    DISALLOWED,
    INPUT,
    NUMBER_ONLY,
    RAISE,
    STRING_ONLY,
    check_float,
    check_int,
    check_intlike,
    check_real,
    fast_float,
    fast_forceint,
    fast_int,
    fast_real,
    float,
    int,
    isfloat,
    isint,
    isintlike,
    isreal,
    query_type,
    real,
    try_float,
    try_forceint,
    try_int,
    try_real,
)
from .fastnumbers import (
    array as _array,
)

try:
    import numpy as np
except ImportError:
    has_numpy = False
else:
    has_numpy = True
    _allowed_dtypes = {
        np.int8,
        np.uint8,
        np.int16,
        np.uint16,
        np.int32,
        np.uint32,
        np.int64,
        np.uint64,
        np.float32,
        np.float64,
    }

# Hide all type checking code at runtime behind this gate
if TYPE_CHECKING:
    import array
    from typing import Any, Callable, Iterable, NewType, TypeVar, overload

    IntT = TypeVar("IntT", np.int_)
    FloatT = TypeVar("FloatT", np.float64)
    CallToInt = Callable[[Any], int]
    CallToFloat = Callable[[Any], float]
    ALLOWED_T = NewType("ALLOWED_T", object)
    RAISE_T = NewType("RAISE_T", object)

    ALLOWED_T = NewType("ALLOWED_T", object)
    DISALLOWED_T = NewType("DISALLOWED_T", object)
    INPUT_T = NewType("INPUT_T", object)
    RAISE_T = NewType("RAISE_T", object)
    STRING_ONLY_T = NewType("STRING_ONLY_T", object)
    NUMBER_ONLY_T = NewType("NUMBER_ONLY_T", object)

    # Selectors
    ALLOWED: ALLOWED_T
    DISALLOWED: DISALLOWED_T
    INPUT: INPUT_T
    RAISE: RAISE_T
    STRING_ONLY: STRING_ONLY_T
    NUMBER_ONLY: NUMBER_ONLY_T

    @overload
    def try_array(
        input: Iterable[Any],
        output: None = None,
        *,
        dtype: IntT,
        inf: ALLOWED_T | int | CallToInt = ALLOWED,
        nan: ALLOWED_T | int | CallToInt = ALLOWED,
        on_fail: RAISE_T | int | CallToInt = RAISE,
        on_overflow: RAISE_T | int | CallToInt = RAISE,
        on_type_error: RAISE_T | int | CallToInt = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> np.ndarray[IntT]: ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: None = None,
        *,
        dtype: FloatT = np.float64,
        inf: ALLOWED_T | int | float | CallToInt | CallToFloat = ALLOWED,
        nan: ALLOWED_T | int | float | CallToInt | CallToFloat = ALLOWED,
        on_fail: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        on_overflow: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        on_type_error: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> np.ndarray[FloatT]: ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: np.ndarray[IntT],
        *,
        inf: ALLOWED_T | int | CallToInt = ALLOWED,
        nan: ALLOWED_T | int | CallToInt = ALLOWED,
        on_fail: RAISE_T | int | CallToInt = RAISE,
        on_overflow: RAISE_T | int | CallToInt = RAISE,
        on_type_error: RAISE_T | int | CallToInt = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None: ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: np.ndarray[FloatT],
        *,
        inf: ALLOWED_T | int | float | CallToInt | CallToFloat = ALLOWED,
        nan: ALLOWED_T | int | float | CallToInt | CallToFloat = ALLOWED,
        on_fail: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        on_overflow: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        on_type_error: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None: ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: array.array[int],
        *,
        inf: ALLOWED_T | int | CallToInt = ALLOWED,
        nan: ALLOWED_T | int | CallToInt = ALLOWED,
        on_fail: RAISE_T | int | CallToInt = RAISE,
        on_overflow: RAISE_T | int | CallToInt = RAISE,
        on_type_error: RAISE_T | int | CallToInt = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None: ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: array.array[float],
        *,
        inf: ALLOWED_T | int | float | CallToInt | CallToFloat = ALLOWED,
        nan: ALLOWED_T | int | float | CallToInt | CallToFloat = ALLOWED,
        on_fail: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        on_overflow: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        on_type_error: RAISE_T | int | float | CallToInt | CallToFloat = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None: ...


def try_array(input, output=None, *, dtype=None, **kwargs):  # noqa: A002, D417
    r"""
    Quickly convert an iterable's contents into an array.

    Is basically a direct analogue to using the ``map`` option in :func:`try_float`
    and friends, except that it returns an array object instead of a list, and there
    are more restrictions of what can be returned (since the outputs must fit
    inside C data-types).

    Parameters
    ----------
    input
        The iterable of values to convert into an array.
    output : optional
        If specified, it is an already existing array object that will contain
        the converted data. It must be of the same length as the input, and
        must be one-dimensional (though a 1D slice of a multi-dimensional array
        is allowed). ``numpy.ndarray`` and ``array.array`` types are allowed.
        If *None*, a ``numpy.ndarray`` will be created for you and will be
        returned as the return value.
    dtype : optional
        If ``output`` is *None*, this specifies the *dtype* of the returned
        ``ndarray``. The default is ``np.float64``. The *dtype* must be of
        integral or float type. Ignored if ``output`` is not *None*.
    inf : optional
        Control how INF is interpreted/handled. The default is *ALLOWED*, which
        indicates that both the string \"inf\" or the float INF are accepted.
        Other valid values are a callable accepting a single argument that will be
        called with the input to return an alternate value, or a default value to
        be returned instead of INF. Ignored if the *dtype* is integral.
    nan : optional
        Control how NaN is interpreted/handled. Behavior matches that of
        `inf` except it is for the string \"nan\" and the value NaN.
        Ignored if the *dtype* is integral.
    on_fail : optional
        Control what happens when an input string cannot be converted to a *float*.
        The default is *RAISE* to indicate a *ValueError* should be raised, a
        callable accepting a single argument that will be called with the input to
        return an alternate value, or a default value to be returned instead of the
        input.
    on_overflow : optional
        Control what happens when the input does not fit in the desired output data
        type. Behavior matches that of ``on_fail`` except that a *OverflowError* is
        raised instead of *ValueError*.
    on_type_error : optional
        Control what happens when the input is neither numeric nor string. Behavior
        matches that of ``on_fail`` except that a *TypeError* is raised instead of
        *ValueError*.
    base : int, optional
        Follows the rules of Python's built-in :func:*int*; see it's
        documentation for your Python version. If given, the input
        **must** be of type *str*. Ignored if the *dtype* is not integral.
    allow_underscores : bool, optional
        Underscores are allowed in numeric literals and in strings passed to *int*
        or *float* (see PEP 515 for details on what is and is not allowed). You can
        enable that behavior by setting this option to *True* - the default is
        *False*.

    Returns
    -------
    ndarray
        If ``output`` was *None*, this function will return the result in a numpy
        ndarray of the specified *dtype*.
    None
        If ``output`` was not *None*

    Raises
    ------
    TypeError
        If the input is not one of *str*, *float*, or *int* and ``on_type_error`` is
        set to *RAISE*.
    OverflowError
        If the input cannot fit into the desired *dtype* and the *dtype* is of
        integral type and ``on_overflow`` is set to *RAISE*.
    ValueError
        If ``on_fail`` is set to *RAISE* and a triggering event is set.
    TypeError
        If ``output`` is given and it is of an invalid type (including data type).
    RuntimeError
        If ``output`` is not *None* but *numpy* is not installed.
    TypeError
        If the value (or return value of the callable) given to `inf,` `nan`,
        ``on_fail``, ``on_overflow``, or ``on_type_error`` is not a float or int.
    OverflowError
        If the *dtype* is integral and the value (or return value of the
        callable) given to ``on_fail``, ``on_overflow``, or ``on_type_error`` cannot
        fit into the data type specified.
    ValueError
        If the *dtype* is integral and the value (or return value of the
        callable) given to ``on_fail``, ``on_overflow``, or ``on_type_error`` is a
        float.

    Examples
    --------
        >>> from fastnumbers import try_array
        >>> import numpy as np
        >>> try_array(["5", "3", "8"])
        array([5., 3., 8.])
        >>> output = np.empty(3, dtype=np.int32)
        >>> try_array(["5", "3", "8"], output=output)
        >>> np.array_equal(output, np.array([5, 3, 8], dtype=np.int32))
        True

    """
    # If output is not provided, we construct a numpy array of the same length
    # as the input into which the C++ function can populate the output.
    if output is None:
        return_output = True

        # Construct a numpy ndarray of the appropriate length and
        # dtype to contain the output.
        if not has_numpy:
            msg = (
                "To use fastnumbers.try_array without an explict "
                "output requires numpy to also be installed"
            )
            raise RuntimeError(msg)
        try:
            length = len(input)
        except TypeError:
            input = list(input)  # noqa: A001
            length = len(input)
        output = np.empty(length, dtype=dtype or np.float64)
    else:
        return_output = False

        # Let's be conservative about what we feed to the C++ code.
        try:
            if output.dtype.type not in _allowed_dtypes:
                raise TypeError(
                    "The only supported numpy dtypes for output are: "
                    + ", ".join(sorted([x.__name__ for x in _allowed_dtypes]))
                    + f" not {output.dtype.name}"
                )
        except AttributeError:
            if not hasattr(output, "typecode"):
                msg = (
                    "Only numpy ndarray and array.array types for output are "
                    f"supported, not {type(output)}"
                )
                raise TypeError(msg) from None

    # Call the C++ extension
    _array(input, output, **kwargs)

    # If no output value was given on calling, we return the output as a return value.
    if return_output:
        return output
    return None


__all__ = [
    "ALLOWED",
    "DISALLOWED",
    "INPUT",
    "NUMBER_ONLY",
    "RAISE",
    "STRING_ONLY",
    "__version__",
    "check_float",
    "check_int",
    "check_intlike",
    "check_real",
    "fast_float",
    "fast_forceint",
    "fast_int",
    "fast_real",
    "float",
    "int",
    "isfloat",
    "isint",
    "isintlike",
    "isreal",
    "query_type",
    "real",
    "try_array",
    "try_float",
    "try_forceint",
    "try_int",
    "try_real",
]
