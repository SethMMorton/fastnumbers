from builtins import float as pyfloat, int as pyint
from collections.abc import Callable, Iterable, Iterator, Sequence
from typing import (
    Any,
    Literal,
    NewType,
    TypeAlias,
    TypeVar,
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

ALLOWED_T = NewType("ALLOWED_T", object)
DISALLOWED_T = NewType("DISALLOWED_T", object)
INPUT_T = NewType("INPUT_T", object)
RAISE_T = NewType("RAISE_T", object)
STRING_ONLY_T = NewType("STRING_ONLY_T", object)
NUMBER_ONLY_T = NewType("NUMBER_ONLY_T", object)

ALLOWED: ALLOWED_T
DISALLOWED: DISALLOWED_T
INPUT: INPUT_T
RAISE: RAISE_T
STRING_ONLY: STRING_ONLY_T
NUMBER_ONLY: NUMBER_ONLY_T

InputType: TypeAlias = pyint | pyfloat | str | bytes | bytearray | memoryview[pyint]
IntInputType: TypeAlias = InputType | HasIndex | HasInt
FloatInputType: TypeAlias = InputType | HasIndex | ItWillFloat
IntBaseType: TypeAlias = pyint | HasIndex | HasInt
NumInputType: TypeAlias = pyint | pyfloat
StrInputType: TypeAlias = str | bytes | bytearray | memoryview[pyint]

AnyInputT = TypeVar("AnyInputT")
QueryInputT = TypeVar("QueryInputT")
NumInputT = TypeVar("NumInputT", bound=NumInputType)
StrInputT = TypeVar("StrInputT", bound=StrInputType)

ConsiderType: TypeAlias = STRING_ONLY_T | NUMBER_ONLY_T | None
InfNanCheckType: TypeAlias = STRING_ONLY_T | NUMBER_ONLY_T | ALLOWED_T | DISALLOWED_T
TrySelectorsType: TypeAlias = ALLOWED_T | INPUT_T | RAISE_T
FloatInt: TypeAlias = pyfloat | pyint

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
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
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
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyfloat: ...
@overload
def try_real(
    x: NumInputT,
    *,
    inf: TrySelectorsType | FloatInt | Callable[[NumInputT], FloatInt] = ...,
    nan: TrySelectorsType | FloatInt | Callable[[NumInputT], FloatInt] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt: ...
@overload
def try_real(
    x: StrInputT,
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputT], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputT], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[StrInputT], FloatInt],
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt: ...
@overload
def try_real(
    x: StrInputT,
    *,
    inf: TrySelectorsType
    | FloatInt
    | StrInputT
    | Callable[[StrInputT], FloatInt | StrInputT] = ...,
    nan: TrySelectorsType
    | FloatInt
    | StrInputT
    | Callable[[StrInputT], FloatInt | StrInputT] = ...,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt | StrInputT: ...
@overload
def try_real(
    x: AnyInputT,
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt],
    on_type_error: RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> FloatInt: ...
@overload
def try_real(
    x: Any,
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_real(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputT], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputT], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[StrInputT], FloatInt],
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[FloatInt]: ...
@overload
def try_real(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: TrySelectorsType
    | FloatInt
    | StrInputT
    | Callable[[StrInputT], FloatInt | StrInputT] = ...,
    nan: TrySelectorsType
    | FloatInt
    | StrInputT
    | Callable[[StrInputT], FloatInt | StrInputT] = ...,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[FloatInt | StrInputT]: ...
@overload
def try_real(
    x: Iterable[AnyInputT],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt],
    on_type_error: RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[FloatInt]: ...
@overload
def try_real(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[Any]: ...
@overload
def try_real(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputT], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[StrInputT], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[StrInputT], FloatInt],
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[FloatInt]: ...
@overload
def try_real(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: TrySelectorsType
    | FloatInt
    | StrInputT
    | Callable[[StrInputT], FloatInt | StrInputT] = ...,
    nan: TrySelectorsType
    | FloatInt
    | StrInputT
    | Callable[[StrInputT], FloatInt | StrInputT] = ...,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[FloatInt | StrInputT]: ...
@overload
def try_real(
    x: Iterable[AnyInputT],
    *,
    inf: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    nan: ALLOWED_T | RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    on_fail: RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt],
    on_type_error: RAISE_T | FloatInt | Callable[[AnyInputT], FloatInt] = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[FloatInt]: ...
@overload
def try_real(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    coerce: bool = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...

# Try float
@overload
def try_float(
    x: NumInputT,
    *,
    inf: TrySelectorsType | pyfloat | Callable[[NumInputT], pyfloat] = ...,
    nan: TrySelectorsType | pyfloat | Callable[[NumInputT], pyfloat] = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: StrInputT,
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputT], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputT], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[StrInputT], pyfloat],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: StrInputT,
    *,
    inf: TrySelectorsType
    | pyfloat
    | StrInputT
    | Callable[[StrInputT], pyfloat | StrInputT] = ...,
    nan: TrySelectorsType
    | pyfloat
    | StrInputT
    | Callable[[StrInputT], pyfloat | StrInputT] = ...,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyfloat | StrInputT: ...
@overload
def try_float(
    x: AnyInputT,
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat],
    on_type_error: RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyfloat: ...
@overload
def try_float(
    x: Any,
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_float(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputT], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputT], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[StrInputT], pyfloat],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyfloat]: ...
@overload
def try_float(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: TrySelectorsType
    | pyfloat
    | StrInputT
    | Callable[[StrInputT], pyfloat | StrInputT] = ...,
    nan: TrySelectorsType
    | pyfloat
    | StrInputT
    | Callable[[StrInputT], pyfloat | StrInputT] = ...,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyfloat | StrInputT]: ...
@overload
def try_float(
    x: Iterable[AnyInputT],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat],
    on_type_error: RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyfloat]: ...
@overload
def try_float(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[Any]: ...
@overload
def try_float(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputT], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[StrInputT], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[StrInputT], pyfloat],
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat]: ...
@overload
def try_float(
    x: Iterable[StrInputT | NumInputType],
    *,
    inf: TrySelectorsType
    | pyfloat
    | StrInputT
    | Callable[[StrInputT], pyfloat | StrInputT] = ...,
    nan: TrySelectorsType
    | pyfloat
    | StrInputT
    | Callable[[StrInputT], pyfloat | StrInputT] = ...,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat | StrInputT]: ...
@overload
def try_float(
    x: Iterable[AnyInputT],
    *,
    inf: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    nan: ALLOWED_T | RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    on_fail: RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat],
    on_type_error: RAISE_T | pyfloat | Callable[[AnyInputT], pyfloat] = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyfloat]: ...
@overload
def try_float(
    x: Iterable[Any],
    *,
    inf: Any = ...,
    nan: Any = ...,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
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
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_int(
    x: StrInputT,
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputT], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_int(
    x: StrInputT,
    *,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint | StrInputT: ...
@overload
def try_int(
    x: AnyInputT,
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputT], pyint],
    on_type_error: RAISE_T | pyint | Callable[[AnyInputT], pyint] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_int(
    x: Any,
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_int(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputT], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyint]: ...
@overload
def try_int(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyint | StrInputT]: ...
@overload
def try_int(
    x: Iterable[AnyInputT],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputT], pyint],
    on_type_error: RAISE_T | pyint | Callable[[AnyInputT], pyint] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyint]: ...
@overload
def try_int(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[Any]: ...
@overload
def try_int(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputT], pyint],
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_int(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyint | StrInputT]: ...
@overload
def try_int(
    x: Iterable[AnyInputT],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputT], pyint],
    on_type_error: RAISE_T | pyint | Callable[[AnyInputT], pyint] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_int(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[Any]: ...

# Try forceint
@overload
def try_forceint(
    x: NumInputType,
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: StrInputT,
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputT], pyint],
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: StrInputT,
    *,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint | StrInputT: ...
@overload
def try_forceint(
    x: AnyInputT,
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputT], pyint],
    on_type_error: RAISE_T | pyint | Callable[[AnyInputT], pyint] = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> pyint: ...
@overload
def try_forceint(
    x: Any,
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[False] = ...,
) -> Any: ...
@overload
def try_forceint(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputT], pyint],
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyint]: ...
@overload
def try_forceint(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyint | StrInputT]: ...
@overload
def try_forceint(
    x: Iterable[AnyInputT],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputT], pyint],
    on_type_error: RAISE_T | pyint | Callable[[AnyInputT], pyint] = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[pyint]: ...
@overload
def try_forceint(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: type[list],
) -> list[Any]: ...
@overload
def try_forceint(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: RAISE_T | pyint | Callable[[StrInputT], pyint],
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_forceint(
    x: Iterable[StrInputT | NumInputType],
    *,
    on_fail: INPUT_T | StrInputT | Callable[[StrInputT], StrInputT] = ...,
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyint | StrInputT]: ...
@overload
def try_forceint(
    x: Iterable[AnyInputT],
    *,
    on_fail: RAISE_T | pyint | Callable[[AnyInputT], pyint],
    on_type_error: RAISE_T | pyint | Callable[[AnyInputT], pyint] = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
    map: Literal[True],
) -> Iterator[pyint]: ...
@overload
def try_forceint(
    x: Iterable[Any],
    *,
    on_fail: Any = ...,
    on_type_error: Any = ...,
    denoise: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
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
    x: pyint,
    default: Any = ...,
    *,
    key: Callable[[Any], Any] = ...,
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
    x: pyfloat,
    default: Any = ...,
    *,
    key: Callable[[Any], Any] = ...,
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
    x: NumInputType,
    default: Any = ...,
    *,
    key: Callable[[Any], Any] = ...,
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
    x: StrInputT,
    *,
    on_fail: Callable[[StrInputT], FloatInt],
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: FloatInt = ...,
    nan: FloatInt = ...,
    allow_underscores: bool = ...,
) -> FloatInt: ...
@overload
def fast_real(
    x: StrInputT,
    *,
    key: Callable[[StrInputT], FloatInt],
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: FloatInt = ...,
    nan: FloatInt = ...,
    allow_underscores: bool = ...,
) -> FloatInt: ...
@overload
def fast_real(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    on_fail: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: FloatInt | StrInputT = ...,
    nan: FloatInt | StrInputT = ...,
    allow_underscores: bool = ...,
) -> FloatInt | StrInputT: ...
@overload
def fast_real(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    key: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: FloatInt | StrInputT = ...,
    nan: FloatInt | StrInputT = ...,
    allow_underscores: bool = ...,
) -> FloatInt | StrInputT: ...
@overload
def fast_real(
    x: StrInputT,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputT], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    coerce: bool = ...,
    inf: Any = ...,
    nan: Any = ...,
    allow_underscores: bool = ...,
) -> FloatInt | Any: ...
@overload
def fast_real(
    x: StrInputT,
    default: Any = ...,
    *,
    key: Callable[[StrInputT], Any] = ...,
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
    x: NumInputType,
    default: Any = ...,
    *,
    key: Callable[[Any], Any] = ...,
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
    x: StrInputT,
    *,
    on_fail: Callable[[StrInputT], pyfloat],
    raise_on_invalid: Literal[False] = ...,
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def fast_float(
    x: StrInputT,
    *,
    key: Callable[[StrInputT], pyfloat],
    raise_on_invalid: Literal[False] = ...,
    inf: pyfloat = ...,
    nan: pyfloat = ...,
    allow_underscores: bool = ...,
) -> pyfloat: ...
@overload
def fast_float(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    on_fail: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    inf: pyfloat | StrInputT = ...,
    nan: pyfloat | StrInputT = ...,
    allow_underscores: bool = ...,
) -> pyfloat | StrInputT: ...
@overload
def fast_float(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    key: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    inf: pyfloat | StrInputT = ...,
    nan: pyfloat | StrInputT = ...,
    allow_underscores: bool = ...,
) -> pyfloat | StrInputT: ...
@overload
def fast_float(
    x: StrInputT,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputT], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    inf: Any = ...,
    nan: Any = ...,
    allow_underscores: bool = ...,
) -> pyfloat | Any: ...
@overload
def fast_float(
    x: StrInputT,
    default: Any = ...,
    *,
    key: Callable[[StrInputT], Any] = ...,
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
    x: NumInputType,
    default: Any = ...,
    *,
    key: Callable[[Any], Any] = ...,
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
    x: StrInputT,
    *,
    on_fail: Callable[[StrInputT], pyint],
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_int(
    x: StrInputT,
    *,
    key: Callable[[StrInputT], pyint],
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_int(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    on_fail: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputT: ...
@overload
def fast_int(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    key: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputT: ...
@overload
def fast_int(
    x: StrInputT,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputT], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
) -> pyint | Any: ...
@overload
def fast_int(
    x: StrInputT,
    default: Any = ...,
    *,
    key: Callable[[StrInputT], Any] = ...,
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
    x: NumInputType,
    default: Any = ...,
    *,
    key: Callable[[Any], Any] = ...,
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
    x: StrInputT,
    *,
    on_fail: Callable[[StrInputT], pyint],
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_forceint(
    x: StrInputT,
    *,
    key: Callable[[StrInputT], pyint],
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint: ...
@overload
def fast_forceint(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    on_fail: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputT: ...
@overload
def fast_forceint(
    x: StrInputT,
    default: StrInputT = ...,
    *,
    key: Callable[[StrInputT], StrInputT] = ...,
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint | StrInputT: ...
@overload
def fast_forceint(
    x: StrInputT,
    default: Any = ...,
    *,
    on_fail: Callable[[StrInputT], Any] = ...,
    raise_on_invalid: Literal[False] = ...,
    allow_underscores: bool = ...,
) -> pyint | Any: ...
@overload
def fast_forceint(
    x: StrInputT,
    default: Any = ...,
    *,
    key: Callable[[StrInputT], Any] = ...,
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
    allow_unicode_chars: bool = ...,
) -> bool: ...
def check_float(
    x: Any,
    *,
    consider: ConsiderType = ...,
    inf: InfNanCheckType = ...,
    nan: InfNanCheckType = ...,
    strict: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> bool: ...
def check_int(
    x: Any,
    *,
    consider: ConsiderType = ...,
    base: IntBaseType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> bool: ...
def check_intlike(
    x: Any,
    *,
    consider: ConsiderType = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
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
    x: QueryInputT,
    *,
    allow_inf: bool = ...,
    allow_nan: bool = ...,
    coerce: bool = ...,
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> type[QueryInputT | pyint | pyfloat]: ...
@overload
def query_type(
    x: QueryInputT,
    *,
    allow_inf: bool = ...,
    allow_nan: bool = ...,
    coerce: bool = ...,
    allowed_types: Sequence[type[Any]],
    allow_underscores: bool = ...,
    allow_unicode_chars: bool = ...,
) -> type[QueryInputT | pyint | pyfloat] | None: ...

# Buitin replacements
@overload
def int(x: IntInputType = ...) -> pyint: ...
@overload
def int(x: IntInputType, base: IntBaseType) -> pyint: ...
def float(x: FloatInputType = ...) -> pyfloat: ...
@overload
def real(x: pyint = ..., *, coerce: bool = ...) -> pyint: ...
@overload
def real(x: pyfloat = ..., *, coerce: Literal[False]) -> pyfloat: ...
@overload
def real(
    x: IntInputType | FloatInputType = ..., *, coerce: bool = ...
) -> pyint | pyfloat: ...
