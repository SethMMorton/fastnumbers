from builtins import float as pyfloat, int as pyint
from typing import (
    Any,
    Callable,
    Iterable,
    Iterator,
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
AnyInputType = TypeVar("AnyInputType")
QueryInputType = TypeVar("QueryInputType")

NumInputType = TypeVar("NumInputType", pyint, pyfloat, ItWillFloat, HasIndex, HasInt)
StrInputType = TypeVar("StrInputType", str, bytes, bytearray)
IntBaseType = TypeVar("IntBaseType", pyint, HasIndex)

ConsiderType = STRING_ONLY_T | NUMBER_ONLY_T | None
InfNanCheckType = STRING_ONLY_T | NUMBER_ONLY_T | ALLOWED_T | DISALLOWED_T
TrySelectorsType = ALLOWED_T | INPUT_T | RAISE_T
FloatInt = pyfloat | pyint

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
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_real(
    x: pyfloat,
    *,
    inf: TrySelectorsType | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    nan: TrySelectorsType | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: Literal[False],
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyfloat: ...
@overload
def try_real(
    x: NumInputType,
    *,
    inf: TrySelectorsType | FloatInt | Callable[[NumInputType], FloatInt] = ...,
    nan: TrySelectorsType | FloatInt | Callable[[NumInputType], FloatInt] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt: ...
@overload
def try_real(
    x: StrInputType,
    *,
    inf: TrySelectorsType
    | FloatInt
    | StrInputType
    | Callable[[StrInputType], FloatInt | StrInputType] = ...,
    nan: TrySelectorsType
    | FloatInt
    | StrInputType
    | Callable[[StrInputType], FloatInt | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt | StrInputType: ...
@overload
def try_real(
    x: StrInputType,
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputType], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputType], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[StrInputType], FloatInt],
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt: ...
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
    map: Literal[False] = ...,
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
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_real(
    x: AnyInputType,
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt],
    on_type_error: FloatInt | Callable[[AnyInputType], FloatInt],
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt: ...
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
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_real(
    x: Iterable[pyint],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint]: ...
@overload
def try_real(
    x: Iterable[pyfloat],
    *,
    inf: TrySelectorsType | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    nan: TrySelectorsType | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: Literal[False],
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyfloat]: ...
@overload
def try_real(
    x: Iterable[NumInputType],
    *,
    inf: TrySelectorsType | FloatInt | Callable[[NumInputType], FloatInt] = ...,
    nan: TrySelectorsType | FloatInt | Callable[[NumInputType], FloatInt] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[FloatInt]: ...
@overload
def try_real(
    x: Iterable[StrInputType],
    *,
    inf: TrySelectorsType
    | FloatInt
    | StrInputType
    | Callable[[StrInputType], FloatInt | StrInputType] = ...,
    nan: TrySelectorsType
    | FloatInt
    | StrInputType
    | Callable[[StrInputType], FloatInt | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[FloatInt | StrInputType]: ...
@overload
def try_real(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputType], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputType], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[StrInputType], FloatInt],
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[FloatInt]: ...
@overload
def try_real(
    x: Iterable[NumInputType],
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_real(
    x: Iterable[StrInputType],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt],
    on_type_error: FloatInt | Callable[[AnyInputType], FloatInt],
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[FloatInt]: ...
@overload
def try_real(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_real(
    x: Iterable[pyint],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_real(
    x: Iterable[pyfloat],
    *,
    inf: TrySelectorsType | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    nan: TrySelectorsType | pyfloat | Callable[[pyfloat], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: Literal[False],
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat]: ...
@overload
def try_real(
    x: Iterable[NumInputType],
    *,
    inf: TrySelectorsType | FloatInt | Callable[[NumInputType], FloatInt] = ...,
    nan: TrySelectorsType | FloatInt | Callable[[NumInputType], FloatInt] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[FloatInt]: ...
@overload
def try_real(
    x: Iterable[StrInputType],
    *,
    inf: TrySelectorsType
    | FloatInt
    | StrInputType
    | Callable[[StrInputType], FloatInt | StrInputType] = ...,
    nan: TrySelectorsType
    | FloatInt
    | StrInputType
    | Callable[[StrInputType], FloatInt | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[FloatInt | StrInputType]: ...
@overload
def try_real(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputType], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputType], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[StrInputType], FloatInt],
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[FloatInt]: ...
@overload
def try_real(
    x: Iterable[NumInputType],
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...
@overload
def try_real(
    x: Iterable[StrInputType],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...
@overload
def try_real(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[AnyInputType], FloatInt],
    on_type_error: FloatInt | Callable[[AnyInputType], FloatInt],
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[FloatInt]: ...
@overload
def try_real(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...

# Try float
@overload
def try_float(
    x: NumInputType,
    *,
    inf: TrySelectorsType | pyfloat | Callable[[NumInputType], pyfloat] = ...,
    nan: TrySelectorsType | pyfloat | Callable[[NumInputType], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: StrInputType,
    *,
    inf: TrySelectorsType
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    nan: TrySelectorsType
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
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
    map: Literal[False] = ...,
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
    map: Literal[False] = ...,
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
    map: Literal[False] = ...,
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
    map: Literal[False] = ...,
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
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_float(
    x: Iterable[NumInputType],
    *,
    inf: TrySelectorsType | pyfloat | Callable[[NumInputType], pyfloat] = ...,
    nan: TrySelectorsType | pyfloat | Callable[[NumInputType], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyfloat]: ...
@overload
def try_float(
    x: Iterable[StrInputType],
    *,
    inf: TrySelectorsType
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    nan: TrySelectorsType
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyfloat | StrInputType]: ...
@overload
def try_float(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[StrInputType], pyfloat],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyfloat]: ...
@overload
def try_float(
    x: Iterable[NumInputType],
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_float(
    x: Iterable[StrInputType],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat],
    on_type_error: pyfloat | Callable[[AnyInputType], pyfloat],
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyfloat]: ...
@overload
def try_float(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_float(
    x: Iterable[NumInputType],
    *,
    inf: TrySelectorsType | pyfloat | Callable[[NumInputType], pyfloat] = ...,
    nan: TrySelectorsType | pyfloat | Callable[[NumInputType], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat]: ...
@overload
def try_float(
    x: Iterable[StrInputType],
    *,
    inf: TrySelectorsType
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    nan: TrySelectorsType
    | pyfloat
    | StrInputType
    | Callable[[StrInputType], pyfloat | StrInputType] = ...,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat | StrInputType]: ...
@overload
def try_float(
    x: Iterable[StrInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[StrInputType], pyfloat],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat]: ...
@overload
def try_float(
    x: Iterable[NumInputType],
    *,
    inf: Any | Callable[[NumInputType], Any] = ...,
    nan: Any | Callable[[NumInputType], Any] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...
@overload
def try_float(
    x: Iterable[StrInputType],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...
@overload
def try_float(
    x: Iterable[AnyInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[AnyInputType], pyfloat],
    on_type_error: pyfloat | Callable[[AnyInputType], pyfloat],
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat]: ...
@overload
def try_float(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...

# Try int
@overload
def try_int(
    x: NumInputType,
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_int(
    x: StrInputType,
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint | StrInputType: ...
@overload
def try_int(
    x: StrInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_int(
    x: StrInputType,
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_int(
    x: AnyInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_int(
    x: Any,
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_int(
    x: Iterable[NumInputType],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint]: ...
@overload
def try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint | StrInputType]: ...
@overload
def try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint]: ...
@overload
def try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint]: ...
@overload
def try_int(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_int(
    x: Iterable[NumInputType],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint | StrInputType]: ...
@overload
def try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_int(
    x: Iterable[StrInputType],
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...
@overload
def try_int(
    x: Iterable[AnyInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_int(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...

# Try forceint
@overload
def try_forceint(
    x: NumInputType,
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: StrInputType,
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint | StrInputType: ...
@overload
def try_forceint(
    x: StrInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: StrInputType,
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_forceint(
    x: AnyInputType,
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: Any,
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_forceint(
    x: Iterable[NumInputType],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint]: ...
@overload
def try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint | StrInputType]: ...
@overload
def try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint]: ...
@overload
def try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[pyint]: ...
@overload
def try_forceint(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
    map: Type[list],
) -> list[Any]: ...
@overload
def try_forceint(
    x: Iterable[NumInputType],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: INPUT_T = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint | StrInputType]: ...
@overload
def try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputType], pyint],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_forceint(
    x: Iterable[StrInputType],
    *,
    on_fail: Any,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...
@overload
def try_forceint(
    x: Iterable[AnyInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputType], pyint],
    on_type_error: pyint | Callable[[AnyInputType], pyint],
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_forceint(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    allow_underscores: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...

# Fast real
@overload
def fast_real(
    x: pyint,
    default: Any = ...,
    *,
    on_fail: Callable[[Any], Any] = ...,
    raise_on_invalid: bool = ...,
    coerce: bool = ...,
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_real(
    x: pyfloat,
    default: Any = ...,
    *,
    on_fail: Callable[[Any], Any] = ...,
    raise_on_invalid: bool = ...,
    coerce: Literal[False],
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def fast_real(
    x: NumInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[Any], Any] = ...,
    raise_on_invalid: bool = ...,
    coerce: Literal[True] = ...,
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> FloatInt: ...
@overload
def fast_real(
    x: StrInputType,
    default: FloatInt,
    *,
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: FloatInt = ...,
    nan: FloatInt = ...,
    allow_underscores: bool = ...,
) -> FloatInt: ...
@overload
def fast_real(
    x: StrInputType,
    *,
    raise_on_invalid: Literal[True],
    coerce: bool = ...,
    inf: FloatInt = ...,
    nan: FloatInt = ...,
    allow_underscores: bool = ...,
) -> FloatInt: ...
@overload
def fast_real(
    x: StrInputType,
    *,
    on_fail: Callable[[StrInputType], FloatInt],
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: FloatInt = ...,
    nan: FloatInt = ...,
    allow_underscores: bool = ...,
) -> FloatInt: ...
@overload
def fast_real(
    x: StrInputType,
    default: StrInputType = ...,
    *,
    on_fail: Callable[[StrInputType], StrInputType] = ...,
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: FloatInt | StrInputType = ...,
    nan: FloatInt | StrInputType = ...,
    allow_underscores: bool = ...,
) -> FloatInt | StrInputType: ...
@overload
def fast_real(
    x: StrInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputType], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: Any = ...,
    nan: Any = ...,
    allow_underscores: bool = ...,
) -> FloatInt | Any: ...

# Fast float
@overload
def fast_float(
    x: NumInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[Any], Any] = ...,
    raise_on_invalid: bool = ...,
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def fast_float(
    x: StrInputType,
    default: pyfloat,
    *,
    raise_on_invalid: Literal[False] = ...,
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def fast_float(
    x: StrInputType,
    *,
    raise_on_invalid: Literal[True],
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def fast_float(
    x: StrInputType,
    *,
    on_fail: Callable[[StrInputType], pyfloat],
    raise_on_invalid: Literal[False] = ...,
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def fast_float(
    x: StrInputType,
    default: StrInputType = ...,
    *,
    on_fail: Callable[[StrInputType], StrInputType] = ...,
    raise_on_invalid: Literal[False] = ...,
    inf: pyfloat | StrInputType = ...,
    nan: pyfloat | StrInputType = ...,
    allow_underscores: bool = ...,
) -> pyfloat | StrInputType: ...
@overload
def fast_float(
    x: StrInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputType], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    inf: Any = ...,
    nan: Any = ...,
    allow_underscores: bool = ...,
) -> pyfloat | Any: ...

# Fast int
@overload
def fast_int(
    x: NumInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[Any], Any] = ...,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_int(
    x: StrInputType,
    default: pyint,
    *,
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_int(
    x: StrInputType,
    *,
    raise_on_invalid: Literal[True],
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_int(
    x: StrInputType,
    *,
    on_fail: Callable[[StrInputType], pyint],
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_int(
    x: StrInputType,
    default: StrInputType = ...,
    *,
    on_fail: Callable[[StrInputType], StrInputType] = ...,
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputType: ...
@overload
def fast_int(
    x: StrInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputType], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint | Any: ...

# Fast forceint
@overload
def fast_forceint(
    x: NumInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[Any], Any] = ...,
    raise_on_invalid: bool = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_forceint(
    x: StrInputType,
    default: pyint,
    *,
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_forceint(
    x: StrInputType,
    *,
    raise_on_invalid: Literal[True],
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_forceint(
    x: StrInputType,
    *,
    on_fail: Callable[[StrInputType], pyint],
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_forceint(
    x: StrInputType,
    default: StrInputType = ...,
    *,
    on_fail: Callable[[StrInputType], StrInputType] = ...,
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputType: ...
@overload
def fast_forceint(
    x: StrInputType,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputType], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint | Any: ...

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
@overload
def real(x: pyint = ..., *, coerce: bool = ...) -> pyint: ...
@overload
def real(x: pyfloat = ..., *, coerce: Literal[False]) -> pyfloat: ...
@overload
def real(x: InputType = ..., *, coerce: bool = ...) -> pyint | pyfloat: ...
