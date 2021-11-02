from builtins import float as pyfloat, int as pyint
from typing import Any, Callable, Optional, Sequence, Type, TypeVar, Union, overload

from typing_extensions import Protocol

__version__: str
max_int_len: pyint
dig: pyint
max_exp: pyint
min_exp: pyint

class HasIndex(Protocol):
    def __index__(self) -> pyint: ...

class HasInt(Protocol):
    def __int__(self) -> pyint: ...

class ItWillFloat(Protocol):
    def __float__(self) -> pyfloat: ...

InputType = Union[pyint, pyfloat, ItWillFloat, HasIndex, HasInt, str, bytes, bytearray]
FastInputType = TypeVar(
    "FastInputType",
    pyint,
    pyfloat,
    ItWillFloat,
    HasIndex,
    HasInt,
    str,
    bytes,
    bytearray,
)
QueryInputType = TypeVar("QueryInputType")
Default = TypeVar("Default")
Inf = TypeVar("Inf")
Nan = TypeVar("Nan")
TransformType = TypeVar("TransformType")

# Fast real
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    allow_underscores: bool = True,
) -> Union[FastInputType, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    coerce: bool = True,
    allow_underscores: bool = True,
) -> Union[FastInputType, Inf, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    nan: Nan,
    coerce: bool = True,
    allow_underscores: bool = True,
) -> Union[FastInputType, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    nan: Nan,
    coerce: bool = True,
    allow_underscores: bool = True,
) -> Union[FastInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    allow_underscores: bool = True,
) -> Union[Default, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    inf: Inf,
    allow_underscores: bool = True,
) -> Union[Default, Inf, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[Default, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = True,
) -> Union[TransformType, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = True,
    inf: Inf,
    allow_underscores: bool = True,
) -> Union[TransformType, Inf, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = True,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[TransformType, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = True,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: InputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, pyint, pyfloat]: ...
@overload
def fast_real(
    x: InputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    inf: Inf,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Inf, pyint, pyfloat]: ...
@overload
def fast_real(
    x: InputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    nan: Nan,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: InputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Inf, Nan, pyint, pyfloat]: ...

# Fast float
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
) -> Union[FastInputType, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    allow_underscores: bool = True,
) -> Union[FastInputType, Inf, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[FastInputType, Nan, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[FastInputType, Inf, Nan, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
) -> Union[Default, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    allow_underscores: bool = True,
) -> Union[Default, Inf, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[Default, Nan, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[Default, Inf, Nan, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    allow_underscores: bool = True,
) -> Union[TransformType, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    inf: Inf,
    allow_underscores: bool = True,
) -> Union[TransformType, Inf, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[TransformType, Nan, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = True,
) -> Union[TransformType, Inf, Nan, pyfloat]: ...
@overload
def fast_float(
    x: InputType,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Inf, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    nan: Nan,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Nan, pyfloat]: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Inf, Nan, pyfloat]: ...

# Fast int
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
) -> Union[FastInputType, pyint]: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    base: Union[pyint, HasIndex],
    allow_underscores: bool = True,
) -> Union[FastInputType, pyint]: ...
@overload
def fast_int(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
) -> Union[Default, pyint]: ...
@overload
def fast_int(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    base: Union[pyint, HasIndex],
    allow_underscores: bool = True,
) -> Union[Default, pyint]: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    allow_underscores: bool = True,
) -> Union[TransformType, pyint]: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    base: Union[pyint, HasIndex],
    allow_underscores: bool = True,
) -> Union[TransformType, pyint]: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, pyint]: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    base: Union[pyint, HasIndex],
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, pyint]: ...

# Fast forceint
@overload
def fast_forceint(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
) -> Union[FastInputType, pyint]: ...
@overload
def fast_forceint(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
) -> Union[Default, pyint]: ...
@overload
def fast_forceint(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    on_fail: Callable[[FastInputType], TransformType],
    allow_underscores: bool = True,
) -> Union[TransformType, pyint]: ...
@overload
def fast_forceint(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, pyint]: ...

# Checking

def isreal(
    x: Any,
    *,
    str_only: bool = False,
    num_only: bool = False,
    allow_inf: bool = False,
    allow_nan: bool = False,
    allow_underscores: bool = True,
) -> bool: ...
def isfloat(
    x: Any,
    *,
    str_only: bool = False,
    num_only: bool = False,
    allow_inf: bool = False,
    allow_nan: bool = False,
    allow_underscores: bool = True,
) -> bool: ...
def isint(
    x: Any,
    *,
    str_only: bool = False,
    num_only: bool = False,
    base: Union[pyint, HasIndex] = 0,
    allow_underscores: bool = True,
) -> bool: ...
def isintlike(
    x: Any,
    *,
    str_only: bool = False,
    num_only: bool = False,
    allow_underscores: bool = True,
) -> bool: ...

# Query type
@overload
def query_type(
    x: QueryInputType,
    *,
    allow_inf: bool = False,
    allow_nan: bool = False,
    coerce: bool = False,
    allow_underscores: bool = True,
) -> Union[Type[QueryInputType], Type[pyint], Type[pyfloat]]: ...
@overload
def query_type(
    x: QueryInputType,
    *,
    allow_inf: bool = False,
    allow_nan: bool = False,
    coerce: bool = False,
    allowed_types: Sequence[Type[Any]],
    allow_underscores: bool = True,
) -> Optional[Union[Type[QueryInputType], Type[pyint], Type[pyfloat]]]: ...

# Buitin replacements
@overload
def int(x: InputType = 0) -> pyint: ...
@overload
def int(x: InputType, base: Union[pyint, HasIndex]) -> pyint: ...
def float(x: InputType = 0.0) -> pyfloat: ...
def real(x: InputType = 0.0, *, coerce: bool = True) -> Union[pyint, pyfloat]: ...
