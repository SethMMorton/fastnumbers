import array
from builtins import float as pyfloat, int as pyint
from collections.abc import Callable, Iterable
from typing import Any, TypeAlias, TypeVar, overload

import numpy as np
import numpy.typing as npt

from .fastnumbers import (
    ALLOWED as ALLOWED,
    ALLOWED_T,
    DISALLOWED as DISALLOWED,
    INPUT as INPUT,
    NUMBER_ONLY as NUMBER_ONLY,
    RAISE as RAISE,
    RAISE_T,
    STRING_ONLY as STRING_ONLY,
    check_float as check_float,
    check_int as check_int,
    check_intlike as check_intlike,
    check_real as check_real,
    fast_float as fast_float,
    fast_forceint as fast_forceint,
    fast_int as fast_int,
    fast_real as fast_real,
    float as float,  # noqa: A004
    int as int,  # noqa: A004
    isfloat as isfloat,
    isint as isint,
    isintlike as isintlike,
    isreal as isreal,
    query_type as query_type,
    real as real,
    try_float as try_float,
    try_forceint as try_forceint,
    try_int as try_int,
    try_real as try_real,
)

IntDType = TypeVar("IntDType", bound=np.integer)
FloatDType = TypeVar("FloatDType", bound=np.floating)
CallToInt: TypeAlias = Callable[[Any], pyint]
CallToFloat: TypeAlias = Callable[[Any], pyfloat]
has_numpy: bool

# Array conversion
@overload
def try_array(
    input: Iterable[Any],
    output: None = None,
    *,
    dtype: type[IntDType] | np.dtype[IntDType] | IntDType,
    inf: ALLOWED_T | pyint | CallToInt = ...,
    nan: ALLOWED_T | pyint | CallToInt = ...,
    on_fail: RAISE_T | pyint | CallToInt = ...,
    on_overflow: RAISE_T | pyint | CallToInt = ...,
    on_type_error: RAISE_T | pyint | CallToInt = ...,
    base: pyint = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> npt.NDArray[IntDType]: ...
@overload
def try_array(
    input: Iterable[Any],
    output: None = None,
    *,
    inf: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    nan: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    on_fail: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_overflow: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_type_error: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    base: pyint = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> npt.NDArray[np.float64]: ...
@overload
def try_array(
    input: Iterable[Any],
    output: None = None,
    *,
    dtype: type[FloatDType] | np.dtype[FloatDType] | FloatDType,
    inf: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    nan: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    on_fail: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_overflow: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_type_error: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    base: pyint = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> npt.NDArray[FloatDType]: ...
@overload
def try_array(
    input: Iterable[Any],
    output: npt.NDArray[IntDType],
    *,
    inf: ALLOWED_T | pyint | CallToInt = ...,
    nan: ALLOWED_T | pyint | CallToInt = ...,
    on_fail: RAISE_T | pyint | CallToInt = ...,
    on_overflow: RAISE_T | pyint | CallToInt = ...,
    on_type_error: RAISE_T | pyint | CallToInt = ...,
    base: pyint = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> None: ...
@overload
def try_array(
    input: Iterable[Any],
    output: npt.NDArray[FloatDType],
    *,
    inf: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    nan: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    on_fail: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_overflow: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_type_error: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    base: pyint = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> None: ...
@overload
def try_array(
    input: Iterable[Any],
    output: array.array[pyint],
    *,
    inf: ALLOWED_T | pyint | CallToInt = ...,
    nan: ALLOWED_T | pyint | CallToInt = ...,
    on_fail: RAISE_T | pyint | CallToInt = ...,
    on_overflow: RAISE_T | pyint | CallToInt = ...,
    on_type_error: RAISE_T | pyint | CallToInt = ...,
    base: pyint = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> None: ...
@overload
def try_array(
    input: Iterable[Any],
    output: array.array[pyfloat],
    *,
    inf: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    nan: ALLOWED_T | pyfloat | CallToInt | CallToFloat = ...,
    on_fail: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_overflow: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    on_type_error: RAISE_T | pyfloat | CallToInt | CallToFloat = ...,
    base: pyint = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> None: ...
