from builtins import float as pyfloat, int as pyint
from typing import (
    Any,
    Callable,
    NewType,
    Optional,
    Sequence,
    Type,
    TypeVar,
    Union,
    overload,
)

from typing_extensions import Protocol

__version__: str

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
AnyInputType = TypeVar("AnyInputType")
QueryInputType = TypeVar("QueryInputType")
Default = TypeVar("Default")
Default2 = TypeVar("Default2")
Inf = TypeVar("Inf")
Nan = TypeVar("Nan")
TransformType = TypeVar("TransformType")
TransformType2 = TypeVar("TransformType2")

ALLOWED_T = NewType("ALLOWED_T", object)
DISALLOWED_T = NewType("DISALLOWED_T", object)
INPUT_T = NewType("INPUT_T", object)
RAISE_T = NewType("RAISE_T", object)
STRING_ONLY_T = NewType("STRING_ONLY_T", object)
NUMBER_ONLY_T = NewType("NUMBER_ONLY_T", object)
ConsiderType = Union[STRING_ONLY_T, NUMBER_ONLY_T, None]
InfNanCheckType = Union[STRING_ONLY_T, NUMBER_ONLY_T, ALLOWED_T, DISALLOWED_T]

# Selectors
ALLOWED: ALLOWED_T
DISALLOWED: DISALLOWED_T
INPUT: INPUT_T
RAISE: RAISE_T
STRING_ONLY: STRING_ONLY_T
NUMBER_ONLY: NUMBER_ONLY_T

# Try real
@overload
def try_real(
    x: FastInputType,
    *,
    inf: ALLOWED_T = ALLOWED,
    nan: ALLOWED_T = ALLOWED,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyint, pyfloat]: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    coerce: bool = True,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyint, pyfloat]: ...

# Try float
@overload
def try_float(
    x: FastInputType,
    *,
    inf: ALLOWED_T = ALLOWED,
    nan: ALLOWED_T = ALLOWED,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = RAISE,
    allow_underscores: bool = False,
) -> pyfloat: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Inf, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Inf, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Union[ALLOWED_T, INPUT_T, RAISE_T] = ALLOWED,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Nan,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Inf,
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: FastInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, Inf, Nan, pyfloat]: ...
@overload
def try_float(
    x: AnyInputType,
    *,
    inf: Callable[[FastInputType], Inf],
    nan: Callable[[FastInputType], Nan],
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, Inf, Nan, pyfloat]: ...

# Try int
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = RAISE,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> pyint: ...
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: RAISE_T = RAISE,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[FastInputType, pyint]: ...
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: Default,
    on_type_error: RAISE_T = RAISE,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[Default, pyint]: ...
@overload
def try_int(
    x: FastInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: RAISE_T = RAISE,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[TransformType, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[AnyInputType, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: INPUT_T,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Default2,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[Default, Default2, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyint]: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    base: Union[HasIndex, pyint] = 10,
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, pyint]: ...

# Try forceint
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: RAISE_T,
    on_type_error: RAISE_T = RAISE,
    allow_underscores: bool = False,
) -> pyint: ...
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: RAISE_T = RAISE,
    allow_underscores: bool = False,
) -> Union[FastInputType, pyint]: ...
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: Default,
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[Default, pyint]: ...
@overload
def try_forceint(
    x: FastInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Union[INPUT_T, RAISE_T] = RAISE,
    allow_underscores: bool = False,
) -> Union[TransformType, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[FastInputType, Default, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Union[INPUT_T, RAISE_T] = INPUT,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[FastInputType, TransformType, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, Default, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Default2,
    allow_underscores: bool = False,
) -> Union[Default, Default2, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Default,
    on_type_error: Callable[[AnyInputType], TransformType],
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: INPUT_T,
    allow_underscores: bool = False,
) -> Union[AnyInputType, TransformType, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Default,
    allow_underscores: bool = False,
) -> Union[Default, TransformType, pyint]: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: Callable[[FastInputType], TransformType],
    on_type_error: Callable[[AnyInputType], TransformType2],
    allow_underscores: bool = False,
) -> Union[TransformType, TransformType2, pyint]: ...

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
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    inf: Inf,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Inf, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
    *,
    raise_on_invalid: bool = False,
    coerce: bool = True,
    nan: Nan,
    allow_underscores: bool = True,
    key: Callable[[FastInputType], TransformType],
) -> Union[TransformType, Nan, pyint, pyfloat]: ...
@overload
def fast_real(
    x: FastInputType,
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
    x: FastInputType,
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

def check_real(
    x: Any,
    *,
    consider: ConsiderType = None,
    inf: InfNanCheckType = NUMBER_ONLY,
    nan: InfNanCheckType = NUMBER_ONLY,
    allow_underscores: bool = False,
) -> bool: ...
def check_float(
    x: Any,
    *,
    consider: ConsiderType = None,
    inf: InfNanCheckType = NUMBER_ONLY,
    nan: InfNanCheckType = NUMBER_ONLY,
    strict: bool = False,
    allow_underscores: bool = False,
) -> bool: ...
def check_int(
    x: Any,
    *,
    consider: ConsiderType = None,
    base: Union[pyint, HasIndex] = 0,
    allow_underscores: bool = False,
) -> bool: ...
def check_intlike(
    x: Any,
    *,
    consider: ConsiderType = None,
    allow_underscores: bool = False,
) -> bool: ...
def isreal(
    x: Any,
    *,
    str_only: bool = False,
    num_only: bool = False,
    allow_inf: bool = False,
    allow_nan: bool = False,
    allow_underscores: bool = True,
) -> bool: ...

# Deprecated checking

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
    allow_underscores: bool = False,
) -> Union[Type[QueryInputType], Type[pyint], Type[pyfloat]]: ...
@overload
def query_type(
    x: QueryInputType,
    *,
    allow_inf: bool = False,
    allow_nan: bool = False,
    coerce: bool = False,
    allowed_types: Sequence[Type[Any]],
    allow_underscores: bool = False,
) -> Optional[Union[Type[QueryInputType], Type[pyint], Type[pyfloat]]]: ...

# Buitin replacements
@overload
def int(x: InputType = 0) -> pyint: ...
@overload
def int(x: InputType, base: Union[pyint, HasIndex]) -> pyint: ...
def float(x: InputType = 0.0) -> pyfloat: ...
def real(x: InputType = 0.0, *, coerce: bool = True) -> Union[pyint, pyfloat]: ...
