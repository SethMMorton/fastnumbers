from builtins import float as pyfloat, int as pyint
from typing import (
    Any,
    Callable,
    Iterable,
    NewType,
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
AnyInputType = TypeVar("AnyInputType")
QueryInputType = TypeVar("QueryInputType")
Default = TypeVar("Default")
Default2 = TypeVar("Default2")
Inf = TypeVar("Inf")
Nan = TypeVar("Nan")
TransformType = TypeVar("TransformType")
TransformType2 = TypeVar("TransformType2")

ConsiderType = STRING_ONLY_T | NUMBER_ONLY_T | None
InfNanCheckType = STRING_ONLY_T | NUMBER_ONLY_T | ALLOWED_T | DISALLOWED_T

# Try real
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T = ...,
    nan: ALLOWED_T = ...,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyint | pyfloat: ...

# Try float
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T = ...,
    nan: ALLOWED_T = ...,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | Inf | Nan | pyfloat: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | Inf | Nan | pyfloat: ...

# Try int
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyint: ...
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: Default,
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Default | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: INPUT_T,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Default2,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> Default | Default2 | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> Default | TransformType | pyint: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | pyint: ...

# Try forceint
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: RAISE_T = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyint: ...
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> FastInputType | Default | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> FastInputType | TransformType | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | Default | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> Default | Default2 | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> Default | TransformType | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> AnyInputType | TransformType | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> Default | TransformType | pyint: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> TransformType | TransformType2 | pyint: ...

# Try real mapping
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T = ...,
    nan: ALLOWED_T = ...,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | pyint | pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyint, pyfloat]: ...
@overload
def map_try_real(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyint, pyfloat]: ...

# Try float mapping
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T = ...,
    nan: ALLOWED_T = ...,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2, pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: ALLOWED_T | INPUT_T | RAISE_T = ...,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | INPUT_T | RAISE_T = ...,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[FastInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | Inf | Nan | pyfloat]: ...
@overload
def map_try_float(
    x: Iterable[AnyInputType],
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | Inf | Nan | pyfloat]: ...

# Try int mapping
@overload
def map_try_int(
    x: Iterable[FastInputType],
    *,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_int(
    x: Iterable[FastInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | pyint]: ...
@overload
def map_try_int(
    x: Iterable[FastInputType],
    *,
    on_fail: Default,
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[Default | pyint]: ...
@overload
def map_try_int(
    x: Iterable[FastInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: RAISE_T = ...,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: Default,
    on_type_error: INPUT_T,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: Default,
    on_type_error: Default2,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyint]: ...
@overload
def map_try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    base: HasIndex | pyint = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | pyint]: ...

# Try forceint mapping
@overload
def map_try_forceint(
    x: Iterable[FastInputType],
    *,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[FastInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[FastInputType | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[FastInputType],
    *,
    on_fail: Default,
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[Default | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[FastInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T | RAISE_T = ...,
    allow_underscores: bool = ...,
) -> list[TransformType | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[FastInputType | Default | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: INPUT_T | RAISE_T = ...,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[FastInputType | TransformType | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | Default | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = ...,
) -> list[Default | Default2 | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = ...,
) -> list[AnyInputType | TransformType | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = ...,
) -> list[Default | TransformType | pyint]: ...
@overload
def map_try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = ...,
) -> list[TransformType | TransformType2 | pyint]: ...

# Fast real
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> Default | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> Default | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
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
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = ...,
    allow_underscores: bool = ...,
) -> TransformType | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    coerce: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | Inf | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | Nan | pyint | pyfloat: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | Inf | Nan | pyint | pyfloat: ...

# Fast float
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> FastInputType | Nan | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> FastInputType | Inf | Nan | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
) -> Default | Inf | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
) -> Default | Nan | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> Default | Inf | Nan | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    allow_underscores: bool = ...,
) -> TransformType | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    inf: Inf,
    allow_underscores: bool = ...,
) -> TransformType | Inf | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Nan | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
) -> TransformType | Inf | Nan | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | Inf | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | Nan | pyfloat: ...
@overload
def fast_float(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    inf: Inf,
    nan: Nan,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | Inf | Nan | pyfloat: ...

# Fast int
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyint: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    base: pyint | HasIndex,
    allow_underscores: bool = ...,
) -> FastInputType | pyint: ...
@overload
def fast_int(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def fast_int(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    base: pyint | HasIndex,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    base: pyint | HasIndex,
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | pyint: ...
@overload
def fast_int(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    base: pyint | HasIndex,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
) -> TransformType | pyint: ...

# Fast forceint
@overload
def fast_forceint(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> FastInputType | pyint: ...
@overload
def fast_forceint(
    x: FastInputType,
    default: Default,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> Default | pyint: ...
@overload
def fast_forceint(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    on_fail: Callable[[FastInputType], TransformType],
    allow_underscores: bool = ...,
) -> TransformType | pyint: ...
@overload
def fast_forceint(
    x: FastInputType,
    *,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
    key: Callable[[FastInputType], TransformType],
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
    base: pyint | HasIndex = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def check_intlike(
    x: Any,
    *,
    consider: ConsiderType = ...,
    allow_underscores: bool = ...,
) -> bool: ...
def isreal(
    x: Any,
    *,
    str_only: bool = ...,
    num_only: bool = ...,
    allow_inf: bool = ...,
    allow_nan: bool = ...,
    allow_underscores: bool = ...,
) -> bool: ...

# Deprecated checking

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
    base: pyint | HasIndex = ...,
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
def int(x: InputType, base: pyint | HasIndex) -> pyint: ...
def float(x: InputType = ...) -> pyfloat: ...
def real(x: InputType = ..., *, coerce: bool = ...) -> pyint | pyfloat: ...
