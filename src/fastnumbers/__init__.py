from typing import TYPE_CHECKING

from .fastnumbers import (
    ALLOWED,
    DISALLOWED,
    INPUT,
    NUMBER_ONLY,
    RAISE,
    STRING_ONLY,
    __version__,
    array as _array,
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
    map_try_float,
    map_try_forceint,
    map_try_int,
    map_try_real,
    query_type,
    real,
    try_float,
    try_forceint,
    try_int,
    try_real,
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
    from typing import Any, Callable, Iterable, NewType, TypeVar, Union, overload

    IntT = TypeVar("IntT", np.int_)
    FloatT = TypeVar("FloatT", np.float_)
    CallToInt = Callable[[Any], int]
    CallToFloat = Callable[[Any], float]
    ALLOWED_T = NewType("ALLOWED_T", object)
    RAISE_T = NewType("RAISE_T", object)

    # Selectors
    ALLOWED: ALLOWED_T
    RAISE: RAISE_T

    @overload
    def try_array(
        input: Iterable[Any],
        output: None = None,
        *,
        dtype: IntT,
        inf: Union[ALLOWED_T, int, CallToInt] = ALLOWED,
        nan: Union[ALLOWED_T, int, CallToInt] = ALLOWED,
        on_fail: Union[RAISE_T, int, CallToInt] = RAISE,
        on_overflow: Union[RAISE_T, int, CallToInt] = RAISE,
        on_type_error: Union[RAISE_T, int, CallToInt] = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> np.ndarray[IntT]:
        ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: None = None,
        *,
        dtype: FloatT = np.float64,
        inf: Union[ALLOWED_T, int, float, CallToInt, CallToFloat] = ALLOWED,
        nan: Union[ALLOWED_T, int, float, CallToInt, CallToFloat] = ALLOWED,
        on_fail: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        on_overflow: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        on_type_error: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> np.ndarray[FloatT]:
        ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: np.ndarray[IntT],
        *,
        inf: Union[ALLOWED_T, int, CallToInt] = ALLOWED,
        nan: Union[ALLOWED_T, int, CallToInt] = ALLOWED,
        on_fail: Union[RAISE_T, int, CallToInt] = RAISE,
        on_overflow: Union[RAISE_T, int, CallToInt] = RAISE,
        on_type_error: Union[RAISE_T, int, CallToInt] = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None:
        ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: np.ndarray[FloatT],
        *,
        inf: Union[ALLOWED_T, int, float, CallToInt, CallToFloat] = ALLOWED,
        nan: Union[ALLOWED_T, int, float, CallToInt, CallToFloat] = ALLOWED,
        on_fail: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        on_overflow: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        on_type_error: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None:
        ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: array.array[int],
        *,
        inf: Union[ALLOWED_T, int, CallToInt] = ALLOWED,
        nan: Union[ALLOWED_T, int, CallToInt] = ALLOWED,
        on_fail: Union[RAISE_T, int, CallToInt] = RAISE,
        on_overflow: Union[RAISE_T, int, CallToInt] = RAISE,
        on_type_error: Union[RAISE_T, int, CallToInt] = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None:
        ...

    @overload
    def try_array(
        input: Iterable[Any],
        output: array.array[float],
        *,
        inf: Union[ALLOWED_T, int, float, CallToInt, CallToFloat] = ALLOWED,
        nan: Union[ALLOWED_T, int, float, CallToInt, CallToFloat] = ALLOWED,
        on_fail: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        on_overflow: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        on_type_error: Union[RAISE_T, int, float, CallToInt, CallToFloat] = RAISE,
        base: int = 10,
        allow_underscores: bool = False,
    ) -> None:
        ...


def try_array(input, output=None, *, dtype=None, **kwargs):
    """ """
    # If output is not provided, we construct a numpy array of the same length
    # as the input into which the C++ function can populate the output.
    if output is None:
        return_output = True

        # Construct a numpy ndarray of the appropriate length and
        # dtype to contain the output.
        if not has_numpy:
            raise RuntimeError(
                "To use fastnumbers.try_array without an explict "
                "output requires numpy to also be installed"
            )
        try:
            length = len(input)
        except TypeError:
            input = list(input)
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
                raise TypeError(
                    "Only numpy ndarray and array.array types for output are "
                    f"supported, not {type(output)}"
                )

    # Call the C++ extension
    _array(input, output, **kwargs)

    # If no output value was given on calling, we return the output as a return value.
    if return_output:
        return output


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
    "map_try_float",
    "map_try_forceint",
    "map_try_int",
    "map_try_real",
    "query_type",
    "real",
    "try_array",
    "try_float",
    "try_forceint",
    "try_int",
    "try_real",
]
