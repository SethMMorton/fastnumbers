from builtins import float as pyfloat, int as pyint
from typing import (
    Any,
    Callable,
    Iterable,
    Literal,
    Sequence,
    Type,
    TypeVar,
    overload,
)

from typing_extensions import Protocol

from . import ALLOWED_T, DISALLOWED_T, INPUT_T, RAISE_T, STRING_ONLY_T, NUMBER_ONLY_T

__version__: str

class HasIndex(Protocol):
    def __index__(self) -> pyint: ...

class HasInt(Protocol):
    def __int__(self) -> pyint: ...

class ItWillFloat(Protocol):
    def __float__(self) -> pyfloat: ...

InputType = pyint | pyfloat | ItWillFloat | HasIndex | HasInt | str | bytes | bytearray
ValidInputType = TypeVar(
    "ValidInputType",
    pyint,
    pyfloat,
    ItWillFloat,
    HasIndex,
    HasInt,
    str,
    bytes,
    bytearray,
)
AnyInputType = TypeVar("AnyInputType")
QueryInputType = TypeVar("QueryInputType")
Default = TypeVar("Default")
Inf = TypeVar("Inf")
Nan = TypeVar("Nan")
TransformType = TypeVar("TransformType")

NumInputType = TypeVar("NumInputType", pyint, pyfloat, ItWillFloat, HasIndex, HasInt)
StrInputType = TypeVar("StrInputType", str, bytes, bytearray)
IntBaseType = TypeVar("IntBaseType", pyint, HasIndex)

ConsiderType = STRING_ONLY_T | NUMBER_ONLY_T | None
InfNanCheckType = STRING_ONLY_T | NUMBER_ONLY_T | ALLOWED_T | DISALLOWED_T

# Try real
@overload
def try_real(
    x: pyint,
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_real(
    x: pyfloat,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: Literal[False],
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def try_real(
    x: NumInputType,
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[NumInputType], pyfloat | pyint] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[NumInputType], pyfloat | pyint] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> pyfloat | pyint: ...
@overload
def try_real(
    x: StrInputType,
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | StrInputType
    | Callable[[StrInputType], pyfloat | pyint | StrInputType] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | StrInputType
    | Callable[[StrInputType], pyfloat | pyint | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> pyfloat | pyint | StrInputType: ...
@overload
def try_real(
    x: StrInputType,
    *,
    inf: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[StrInputType], pyfloat | pyint] = ...,
    nan: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[StrInputType], pyfloat | pyint] = ...,
    on_fail: RAISE_T | pyfloat | pyint | Callable[[StrInputType], pyfloat | pyint],
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> pyfloat | pyint: ...
@overload
def try_real(
    x: NumInputType,
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Any: ...
@overload
def try_real(
    x: StrInputType,
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Any: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[AnyInputType], pyfloat | pyint] = ...,
    nan: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[AnyInputType], pyfloat | pyint] = ...,
    on_fail: RAISE_T | pyfloat | pyint | Callable[[AnyInputType], pyfloat | pyint],
    on_type_error: pyfloat | pyint | Callable[[AnyInputType], pyfloat | pyint],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> pyfloat | pyint: ...
@overload
def try_real(
    x: Any,
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Any: ...

# Try float
@overload
def try_float(
    x: NumInputType,
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | Callable[[NumInputType], pyfloat] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | Callable[[NumInputType], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: StrInputType,
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> pyfloat | StrInputType: ...
@overload
def try_float(
    x: StrInputType,
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[StrInputType], pyfloat],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: NumInputType,
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> Any: ...
@overload
def try_float(
    x: StrInputType,
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> Any: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat],
    on_type_error: pyfloat | Callable[[AnyInputType], pyfloat],
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: Any,
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
) -> Any: ...

# Try int
@overload
def try_int(
    x: NumInputType,
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_int(
    x: StrInputType,
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputType: ...
@overload
def try_int(
    x: StrInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_int(
    x: StrInputType,
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> Any: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_int(
    x: Any,
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> Any: ...

# Try forceint
@overload
def try_forceint(
    x: NumInputType,
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: StrInputType,
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputType: ...
@overload
def try_forceint(
    x: StrInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: StrInputType,
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> Any: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: Any,
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
) -> Any: ...

# Try real mapping
@overload
def map_try_real(
    x: Iterable[pyint],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_real(
    x: Iterable[pyfloat],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: Literal[False],
    allow_underscores: bool = ...,
) -> list[pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[NumInputType],
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[NumInputType], pyfloat | pyint] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[NumInputType], pyfloat | pyint] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat | pyint]: ...
@overload
def map_try_real(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | StrInputType
    | Callable[[StrInputType], pyfloat | pyint | StrInputType] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | pyint
    | StrInputType
    | Callable[[StrInputType], pyfloat | pyint | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat | pyint | StrInputType]: ...
@overload
def map_try_real(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[StrInputType], pyfloat | pyint] = ...,
    nan: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[StrInputType], pyfloat | pyint] = ...,
    on_fail: RAISE_T | pyfloat | pyint | Callable[[StrInputType], pyfloat | pyint],
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat | pyint]: ...
@overload
def map_try_real(
    x: Iterable[NumInputType],
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...
@overload
def map_try_real(
    x: Iterable[StrInputType],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[AnyInputType], pyfloat | pyint] = ...,
    nan: ALLOWED_T
    | RAISE_T
    | pyfloat
    | pyint
    | Callable[[AnyInputType], pyfloat | pyint] = ...,
    on_fail: RAISE_T | pyfloat | pyint | Callable[[AnyInputType], pyfloat | pyint],
    on_type_error: pyfloat | pyint | Callable[[AnyInputType], pyfloat | pyint],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat | pyint]: ...
@overload
def map_try_real(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...

# Try float mapping
@overload
def map_try_float(
    x: Iterable[NumInputType],
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | Callable[[NumInputType], pyfloat] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | Callable[[NumInputType], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    nan: ALLOWED_T
    | INPUT_T
    | RAISE_T
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat | StrInputType]: ...
@overload
def map_try_float(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[StrInputType], pyfloat],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[NumInputType],
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...
@overload
def map_try_float(
    x: Iterable[StrInputType],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat],
    on_type_error: pyfloat | Callable[[AnyInputType], pyfloat],
    allow_underscores: bool = ...,
) -> list[pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
) -> list[Any]: ...

# Try int mapping
@overload
def map_try_int(
    x: Iterable[NumInputType],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> list[pyint | StrInputType]: ...
@overload
def map_try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_int(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...

# Try forceint mapping
@overload
def map_try_forceint(
    x: Iterable[NumInputType],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[pyint | StrInputType]: ...
@overload
def map_try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
) -> list[Any]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
) -> list[Any]: ...

# Fast real
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> ValidInputType | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> ValidInputType | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> ValidInputType | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> ValidInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> Default | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> Default | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    coerce: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    coerce: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    coerce: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | Inf | Nan | pyint | pyfloat: ...

# Fast float
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> ValidInputType | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> ValidInputType | Inf | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> ValidInputType | Nan | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> ValidInputType | Inf | Nan | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> Default | Inf | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> Default | Nan | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    allow_underscores: bool = ...,
) -> TransformType | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    inf: Inf,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | Inf | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | Nan | pyfloat: ...
@overload
def fast_float(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | Inf | Nan | pyfloat: ...

# Fast int
@overload
def fast_int(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> ValidInputType | pyint: ...
@overload
def fast_int(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    base: IntBaseType,
    allow_underscores: bool = ...,
) -> ValidInputType | pyint: ...
@overload
def fast_int(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def fast_int(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    base: IntBaseType,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def fast_int(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def fast_int(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    base: IntBaseType,
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def fast_int(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | pyint: ...
@overload
def fast_int(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    base: IntBaseType,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | pyint: ...

# Fast forceint
@overload
def fast_forceint(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> ValidInputType | pyint: ...
@overload
def fast_forceint(
    x: ValidInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def fast_forceint(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[ValidInputType], TransformType],
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def fast_forceint(
    x: ValidInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[ValidInputType], TransformType],
) -> TransformType | pyint: ...

# Checking

def check_real(
    x: Any,
    *,
    consider: ConsiderType = ...,
    inf: InfNanCheckType = ...,
    nan: InfNanCheckType = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def check_float(
    x: Any,
    *,
    consider: ConsiderType = ...,
    inf: InfNanCheckType = ...,
    nan: InfNanCheckType = ...,
    strict: bool = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def check_int(
    x: Any,
    *,
    consider: ConsiderType = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def check_intlike(
    x: Any,
    *,
    consider: ConsiderType = ...,
    allow_underscores: bool = ...,
) -> bool: ...

# Deprecated checking

def isreal(
    x: Any,
    *,
    str_only: bool = ...,
    num_only: bool = ...,
    allow_inf: bool = ...,
    allow_nan: bool = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def isfloat(
    x: Any,
    *,
    str_only: bool = ...,
    num_only: bool = ...,
    allow_inf: bool = ...,
    allow_nan: bool = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def isint(
    x: Any,
    *,
    str_only: bool = ...,
    num_only: bool = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def isintlike(
    x: Any,
    *,
    str_only: bool = ...,
    num_only: bool = ...,
    allow_underscores: bool = ...,
) -> bool: ...

# Query type
@overload
def query_type(
    x: QueryInputType,
    *,
    allow_inf: bool = ...,
    allow_nan: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Type[QueryInputType] | Type[pyint] | Type[pyfloat]: ...
@overload
def query_type(
    x: QueryInputType,
    *,
    allow_inf: bool = ...,
    allow_nan: bool = ...,
    coerce: bool = ...,
    allowed_types: Sequence[Type[Any]],
    allow_underscores: bool = ...,
) -> Type[QueryInputType] | Type[pyint] | Type[pyfloat] | None: ...

# Buitin replacements
@overload
def int(x: InputType = ...) -> pyint: ...
@overload
def int(x: InputType, base: IntBaseType) -> pyint: ...
def float(x: InputType = ...) -> pyfloat: ...
def real(x: InputType = ..., *, coerce: bool = ...) -> pyint | pyfloat: ...
