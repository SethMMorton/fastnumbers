from __future__ import annotations

import decimal
import math
import random
import re
import unicodedata
from functools import partial
from itertools import combinations
from typing import (
    Any,
    Callable,
    Iterable,
    Iterator,
    NoReturn,
    Union,
    cast,
)

import pytest
from conftest import base_n
from hypothesis import example, given, settings
from hypothesis.strategies import (
    binary,
    dictionaries,
    floats,
    integers,
    iterables,
    lists,
    sampled_from,
    sets,
    text,
    tuples,
)
from typing_extensions import Protocol

import fastnumbers

parametrize = pytest.mark.parametrize


FloatOrInt = Union[float, int]


class TryReal(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        inf: Any = ...,
        nan: Any = ...,
        on_fail: Any = ...,
        on_type_error: Any = ...,
        coerce: bool = ...,
        denoise: bool = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class TryFloat(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        inf: Any = ...,
        nan: Any = ...,
        on_fail: Any = ...,
        on_type_error: Any = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class TryInt(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        on_fail: Any = ...,
        on_type_error: Any = ...,
        base: int = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class TryForceInt(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        on_fail: Any = ...,
        on_type_error: Any = ...,
        denoise: bool = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class CheckReal(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        consider: Any = ...,
        inf: Any = ...,
        nan: Any = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


class CheckFloat(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        consider: Any = ...,
        inf: Any = ...,
        nan: Any = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


class CheckInt(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        consider: Any = ...,
        base: int = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


class CheckIntLike(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        consider: Any = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


class Real(Protocol):
    def __call__(self, x: Any = ..., *, coerce: bool = ...) -> int | float: ...


class FastReal(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = ...,
        *,
        on_fail: Callable[[Any], Any] = ...,
        raise_on_invalid: bool = ...,
        coerce: bool = ...,
        inf: Any = ...,
        nan: Any = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class FastFloat(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = ...,
        *,
        on_fail: Callable[[Any], Any] = ...,
        raise_on_invalid: bool = ...,
        inf: Any = ...,
        nan: Any = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class FastInt(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = ...,
        *,
        on_fail: Callable[[Any], Any] = ...,
        raise_on_invalid: bool = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class FastForceInt(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = ...,
        *,
        on_fail: Callable[[Any], Any] = ...,
        raise_on_invalid: bool = ...,
        allow_underscores: bool = ...,
    ) -> Any: ...


class IsReal(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = ...,
        num_only: bool = ...,
        allow_inf: bool = ...,
        allow_nan: bool = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


class IsFloat(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = ...,
        num_only: bool = ...,
        allow_inf: bool = ...,
        allow_nan: bool = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


class IsInt(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = ...,
        num_only: bool = ...,
        base: Any = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


class IsIntLike(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = ...,
        num_only: bool = ...,
        allow_underscores: bool = ...,
    ) -> bool: ...


ConversionFuncs = Union[TryReal, TryFloat, TryInt, TryForceInt]
OldConversionFuncs = Union[FastReal, FastFloat, FastInt, FastForceInt]
IdentificationFuncs = Union[CheckReal, CheckFloat, CheckInt, CheckIntLike]
OldIdentificationFuncs = Union[IsReal, IsFloat, IsInt, IsIntLike]

# Predefine Unicode digits, numbers, and not those.
digits = []
numeric = []
not_numeric = []
for x in range(0x1FFFFF):
    try:
        a = chr(x)
    except ValueError:
        break
    try:
        unicodedata.digit(a)
        digits.append(a)
    except ValueError:
        pass
    try:
        unicodedata.numeric(a)
        numeric.append(a)
    except ValueError:
        not_numeric.append(a)
numeric_not_digit = [x for x in numeric if x not in digits]
numeric_not_digit_not_int = [
    x for x in numeric_not_digit if not unicodedata.numeric(x).is_integer()
]


def a_number(s: str | bytes) -> bool:
    s = s.strip()
    try:
        int(s)
    except ValueError:
        try:
            float(s)
        except ValueError:
            pass
        else:
            return True
    else:
        return True
    if isinstance(s, bytes):
        return False
    if re.match(r"\s*([-+]?\d+\.?\d*(?:[eE][-+]?\d+)?)\s*$", s, re.UNICODE):
        return True
    if re.match(r"\s*([-+]?\.\d+(?:[eE][-+]?\d+)?)\s*$", s, re.UNICODE):
        return True
    return s.strip().lstrip("[-+]") in numeric


def space() -> str:
    """90% chance of ' ', 10% of unicode-only space."""
    return random.choice([" "] * 90 + ["\u2007"] * 10)


def pad(value: str) -> str:
    """Pad a string with whitespace at the front and back."""
    return random.randint(1, 100) * space() + value + random.randint(1, 100) * space()


def not_a_number(s: str | bytes) -> bool:
    return not a_number(s)


def an_integer(x: float) -> bool:
    return x.is_integer()


def not_an_integer(x: float) -> bool:
    return not x.is_integer()


def capture_result(  # type: ignore [no-untyped-def]
    func: ConversionFuncs | IdentificationFuncs, *args, **kwargs
) -> Any:
    """Execute a function, and either return the result or the exception message"""
    try:
        return func(*args, **kwargs)
    except Exception as e:  # noqa: BLE001
        return str(e)


def assert_integers_close(x: int, y: int) -> None:
    """Integers are the same except for the last non-zero digit"""
    x_str = str(x)
    y_str = str(y)

    # Get length with non-trailing zeros removed
    x_len = len(re.sub(r"0+$", "", x_str))
    y_len = len(re.sub(r"0+$", "", y_str))

    # Remove the non-trailing zeros, only keep 16 digits, then round the last digit.
    # Python's integer rounding seems to sometimes five up or down with no pattern
    # that I can detect - so if an integer ends in "5", it is forced to be both ending
    # in "4" and "6" to explore both rounding spaces.
    comp_len = min(max(x_len, y_len), 16)
    x_trimmed_1 = round(int(re.sub(r"5$", "4", x_str[:comp_len]) or 0), -1)
    y_trimmed_1 = round(int(re.sub(r"5$", "4", y_str[:comp_len]) or 0), -1)
    x_trimmed_2 = round(int(re.sub(r"5$", "6", x_str[:comp_len]) or 0), -1)
    y_trimmed_2 = round(int(re.sub(r"5$", "6", y_str[:comp_len]) or 0), -1)
    assert x_trimmed_2 in (y_trimmed_1, y_trimmed_2) or x_trimmed_1 in (
        y_trimmed_1,
        y_trimmed_2,
    )


class DumbFloatClass:
    def __float__(self) -> NoReturn:
        msg = "something here might go wrong"
        raise ValueError(msg)


class DumbIntClass:
    def __int__(self) -> NoReturn:
        msg = "something here might go wrong"
        raise ValueError(msg)


# Map function names to the actual functions,
# for dymamic declaration of which functions test below.
func_mapping: dict[str, Callable[..., Any]] = {
    "check_real": fastnumbers.check_real,
    "check_float": fastnumbers.check_float,
    "check_int": fastnumbers.check_int,
    "check_intlike": fastnumbers.check_intlike,
    "try_real": fastnumbers.try_real,
    "try_real_coerce_true": partial(fastnumbers.try_real, coerce=True),
    "try_real_coerce_false": partial(fastnumbers.try_real, coerce=False),
    "try_float": fastnumbers.try_float,
    "try_int": fastnumbers.try_int,
    "try_forceint": fastnumbers.try_forceint,
    "fast_real": fastnumbers.fast_real,
    "fast_real_coerce_true": partial(fastnumbers.fast_real, coerce=True),
    "fast_real_coerce_false": partial(fastnumbers.fast_real, coerce=False),
    "fast_float": fastnumbers.fast_float,
    "fast_int": fastnumbers.fast_int,
    "fast_forceint": fastnumbers.fast_forceint,
    "isreal": fastnumbers.isreal,
    "isfloat": fastnumbers.isfloat,
    "isint": fastnumbers.isint,
    "isintlike": fastnumbers.isintlike,
    "real": fastnumbers.real,
    "float": fastnumbers.float,
    "int": fastnumbers.int,
    "query_type": fastnumbers.query_type,
}


def get_funcs(function_names: Iterable[str]) -> list[Callable[..., Any]]:
    """Given a list of function names, return the associated functions"""
    return [func_mapping[x] for x in function_names]


# Common convenience functiom collections
conversion_funcs = ["try_real", "try_float", "try_int", "try_forceint"]
identification_funcs = ["check_real", "check_float", "check_int", "check_intlike"]
non_builtin_funcs = conversion_funcs + identification_funcs

# All ways to spell NaN, and most ways to spell infinity and negative infinity
all_nan = ["nan", "Nan", "nAn", "naN", "NAn", "NaN", "nAN", "NAN"]
all_nan += ["+" + x for x in all_nan] + ["-" + x for x in all_nan]
most_inf = ["inf", "Inf", "iNf", "inF", "INf", "InF", "iNF", "INF"]
most_inf += ["infinity", "INFINITY", "iNfInItY", "InFiNiTy", "inFINIty"]
neg_inf = ["-" + x for x in most_inf]
most_inf += ["+" + x for x in most_inf]

#################
# Sanity Checks #
#################


def test_version() -> None:
    assert hasattr(fastnumbers, "__version__")


@given(floats(allow_nan=False) | integers())
def test_real_returns_same_as_fast_real(x: FloatOrInt) -> None:
    assert fastnumbers.real(x) == fastnumbers.try_real(x)


class TestArguments:
    """Tests that high-level argument handling is as expected"""

    def test_real_no_arguments_returns_0(self) -> None:
        assert fastnumbers.real() == 0

    funcs = [
        *non_builtin_funcs,
        "query_type",
        "int",
        "real",
        "fast_real",
        "fast_float",
        "fast_int",
        "fast_forceint",
        "isreal",
        "isfloat",
        "isint",
        "isintlike",
    ]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_invalid_argument_raises_type_error(
        self, func: Callable[[Any], Any] | Real
    ) -> None:
        with pytest.raises(
            TypeError, match="got an unexpected keyword argument 'invalid'"
        ):
            func(5, invalid="dummy")  # type: ignore[call-arg]

    def test_invalid_argument_raises_type_error_no_kwargs(self) -> None:
        with pytest.raises(TypeError, match="takes no keyword arguments"):
            fastnumbers.float("5", invalid="dummy")  # type: ignore[call-arg]

    funcs = [
        *non_builtin_funcs,
        "query_type",
        "fast_real",
        "fast_float",
        "fast_int",
        "fast_forceint",
        "isreal",
        "isfloat",
        "isint",
        "isintlike",
    ]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_no_arguments_raises_type_error(self, func: Callable[[Any], Any]) -> None:
        with pytest.raises(TypeError, match="missing required argument 'x'"):
            func()  # type: ignore[call-arg]

    def test_invalid_argument_and_missing_positional(self) -> None:
        with pytest.raises(TypeError, match="missing required argument 'x'"):
            fastnumbers.try_float(on_fail=0.0)  # type: ignore[call-overload]

    def test_positional_as_kwarg_is_ok(self) -> None:
        assert fastnumbers.try_float(x="5") == 5.0

    def test_duplicate_argument(self) -> None:
        msg = r"argument for .* given by name \('x'\) and position"
        with pytest.raises(TypeError, match=msg):
            fastnumbers.try_float("5", x="4")  # type: ignore[call-overload]

    def test_too_many_positional_arguments1(self) -> None:
        msg = r"takes 1 positional arguments but 2 were given"
        with pytest.raises(TypeError, match=msg):
            fastnumbers.try_float("5", "4")  # type: ignore[call-overload]

    def test_too_many_positional_arguments2(self) -> None:
        msg = r"takes from 0 to 1 positional arguments but 2 were given"
        with pytest.raises(TypeError, match=msg):
            fastnumbers.float("5", "4")  # type: ignore[call-arg]


class TestSelectors:
    """Ensure that the mode selectors behave as expected"""

    selectors = [
        fastnumbers.ALLOWED,
        fastnumbers.DISALLOWED,
        fastnumbers.INPUT,
        fastnumbers.RAISE,
        fastnumbers.STRING_ONLY,
        fastnumbers.NUMBER_ONLY,
    ]

    @parametrize("x", selectors)
    def test_selectors_have_no_type(self, x: object) -> None:
        assert type(x) is object

    @parametrize("a, b", combinations(selectors, 2))
    def test_selectors_are_mutually_exclusive(self, a: object, b: object) -> None:
        assert a is not b

    funcs = ["try_real", "try_float"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize(
        "inf",
        [fastnumbers.DISALLOWED, fastnumbers.STRING_ONLY, fastnumbers.NUMBER_ONLY],
    )
    def test_selectors_are_rejected_when_invalid_inf_conv(
        self, func: TryReal | TryFloat, inf: object
    ) -> None:
        with pytest.raises(ValueError, match="'inf' and 'nan' cannot be"):
            func("5", inf=inf)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize(
        "nan",
        [fastnumbers.DISALLOWED, fastnumbers.STRING_ONLY, fastnumbers.NUMBER_ONLY],
    )
    def test_selectors_are_rejected_when_invalid_nan_conv(
        self, func: TryReal | TryFloat, nan: object
    ) -> None:
        with pytest.raises(ValueError, match="'inf' and 'nan' cannot be"):
            func("5", nan=nan)

    funcs = ["check_real", "check_float"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize(
        "inf",
        [fastnumbers.INPUT, fastnumbers.RAISE, str, float, int],
    )
    def test_selectors_are_rejected_when_invalid_inf_check(
        self, func: CheckReal | CheckFloat, inf: object
    ) -> None:
        with pytest.raises(ValueError, match="allowed values for 'inf' and 'nan'"):
            func("5", inf=inf)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize(
        "nan",
        [fastnumbers.INPUT, fastnumbers.RAISE, str, float, int],
    )
    def test_selectors_are_rejected_when_invalid_nan_check(
        self, func: CheckReal | CheckFloat, nan: object
    ) -> None:
        with pytest.raises(ValueError, match="allowed values for 'inf' and 'nan'"):
            func("5", nan=nan)

    funcs = identification_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize(
        "consider",
        [
            fastnumbers.DISALLOWED,
            fastnumbers.ALLOWED,
            fastnumbers.INPUT,
            fastnumbers.RAISE,
            float,
            int,
            str,
        ],
    )
    def test_selectors_are_rejected_when_invalid_for_consider(
        self, func: IdentificationFuncs, consider: object
    ) -> None:
        with pytest.raises(ValueError, match="allowed values for 'consider'"):
            func("5", consider=consider)

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize(
        "on_fail",
        [
            fastnumbers.DISALLOWED,
            fastnumbers.ALLOWED,
            fastnumbers.NUMBER_ONLY,
            fastnumbers.STRING_ONLY,
        ],
    )
    def test_selectors_are_rejected_when_invalid_for_on_fail(
        self, func: ConversionFuncs, on_fail: object
    ) -> None:
        with pytest.raises(
            ValueError, match="values for 'on_fail' and 'on_type_error'"
        ):
            func("5", on_fail=on_fail)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize(
        "on_type_error",
        [
            fastnumbers.DISALLOWED,
            fastnumbers.ALLOWED,
            fastnumbers.NUMBER_ONLY,
            fastnumbers.STRING_ONLY,
        ],
    )
    def test_selectors_are_rejected_when_invalid_for_on_type_error(
        self, func: ConversionFuncs, on_type_error: object
    ) -> None:
        with pytest.raises(
            ValueError, match="values for 'on_fail' and 'on_type_error'"
        ):
            func("5", on_type_error=on_type_error)


class TestBackwardsCompatibility:
    """Ensure "old" calling methods still match new methods"""

    funcs = ["fast_real", "fast_float", "fast_int", "fast_forceint"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_key(self, func: Any) -> None:
        with pytest.raises(ValueError, match=r"^Cannot set both on_fail and key$"):
            func("dummy", key=len, on_fail=len)
        assert func("dummy", key=len) == 5
        assert func("dummy", key=len) == func("dummy", on_fail=len)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_new_invalid_parings(self, func: Any) -> None:
        with pytest.raises(ValueError, match="Cannot set both on_fail and default"):
            func("5", default=0.0, on_fail=0.0)
        with pytest.raises(
            ValueError, match="Cannot set both on_fail and raise_on_invalid"
        ):
            func("5", raise_on_invalid=True, on_fail=0.0)

    old_to_new_conversion_pairing = []
    conv_pairs: list[tuple[OldConversionFuncs, ConversionFuncs]] = [
        (partial(fastnumbers.fast_real, allow_underscores=False), fastnumbers.try_real),
        (
            partial(fastnumbers.fast_float, allow_underscores=False),
            fastnumbers.try_float,
        ),
        (partial(fastnumbers.fast_int, allow_underscores=False), fastnumbers.try_int),
        (
            partial(fastnumbers.fast_forceint, allow_underscores=False),
            fastnumbers.try_forceint,
        ),
    ]
    for old1, new1 in conv_pairs:
        old_to_new_conversion_pairing += [
            (old1, new1),
            (
                partial(old1, raise_on_invalid=True),
                partial(new1, on_fail=fastnumbers.RAISE),
            ),
            (
                partial(old1, default="bananas"),
                partial(new1, on_fail="bananas"),
            ),
            (
                partial(old1),
                partial(new1, on_fail=fastnumbers.INPUT),  # default
            ),
        ]

    @given(floats() | integers() | text(max_size=50))
    @parametrize("old_func, new_func", old_to_new_conversion_pairing)
    def test_old_to_new_conversion_equivalence(
        self, old_func: ConversionFuncs, new_func: ConversionFuncs, x: Any
    ) -> None:
        for value in (x, (x, x)):
            old_result = capture_result(old_func, value)
            new_result = capture_result(new_func, value)
            if old_result != old_result and new_result != new_result:
                assert math.isnan(old_result)
                assert math.isnan(new_result)
            else:
                assert old_result == new_result

    old_to_new_checking_pairing = []
    check_pairs: list[tuple[OldIdentificationFuncs, IdentificationFuncs]] = [
        (partial(fastnumbers.isreal, allow_underscores=False), fastnumbers.check_real),
        (
            partial(fastnumbers.isfloat, allow_underscores=False),
            fastnumbers.check_float,
        ),
        (partial(fastnumbers.isint, allow_underscores=False), fastnumbers.check_int),
        (
            partial(fastnumbers.isintlike, allow_underscores=False),
            fastnumbers.check_intlike,
        ),
    ]
    for old2, new2 in check_pairs:
        old_to_new_checking_pairing += [
            (old2, new2),
            (
                partial(old2, str_only=False, num_only=False),
                partial(new2, consider=None),
            ),
            (
                partial(old2, str_only=True),
                partial(new2, consider=fastnumbers.STRING_ONLY),
            ),
            (
                partial(old2, num_only=True),
                partial(new2, consider=fastnumbers.NUMBER_ONLY),
            ),
        ]
    for old3, new3 in check_pairs[:2]:
        old = cast(Union[IsReal, IsFloat], old3)
        new = cast(Union[CheckReal, CheckFloat], new3)
        old_to_new_checking_pairing += [
            (partial(old, allow_inf=True), partial(new, inf=fastnumbers.ALLOWED)),
            (partial(old, allow_inf=False), partial(new, inf=fastnumbers.NUMBER_ONLY)),
            (partial(old, allow_nan=True), partial(new, nan=fastnumbers.ALLOWED)),
            (partial(old, allow_nan=False), partial(new, nan=fastnumbers.NUMBER_ONLY)),
        ]
    old_to_new_checking_pairing += [
        (
            partial(fastnumbers.isint, base=2),
            partial(fastnumbers.check_int, base=2),
        ),
    ]

    @given(floats() | integers() | text(max_size=50))
    @parametrize("old_func, new_func", old_to_new_checking_pairing)
    def test_old_to_new_checking_equivalence(
        self, old_func: IdentificationFuncs, new_func: IdentificationFuncs, x: Any
    ) -> None:
        old_result = old_func(x)
        new_result = new_func(x)
        assert old_result == new_result
        old_result = old_func((x, x))
        new_result = new_func((x, x))
        assert old_result == new_result


class TestUnderscores:
    """Tests to make sure underscores are well handled in >= 3.6."""

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_numbers_with_underscores_converted_toggled(
        self, func: ConversionFuncs
    ) -> None:
        x = "1_234_567"
        assert func(x) == x
        assert func(x, allow_underscores=True) in (float(x), int(x))
        assert func(x, allow_underscores=False) == x

    funcs = ["fast_real", "fast_float", "fast_int", "fast_forceint"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_numbers_with_underscores_converted_toggled_deprecated(
        self, func: ConversionFuncs
    ) -> None:
        x = "1_234_567"
        assert func(x) in (float(x), int(x))
        assert func(x, allow_underscores=True) in (float(x), int(x))
        assert func(x, allow_underscores=False) == x

    funcs = identification_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_numbers_with_underscores_identified_toggled(
        self, func: ConversionFuncs
    ) -> None:
        x = "1_234_567"
        assert not func(x)
        assert func(x, allow_underscores=True)
        assert not func(x, allow_underscores=False)

    funcs = ["isreal", "isfloat", "isint", "isintlike"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_numbers_with_underscores_identified_toggled_deprecated(
        self, func: ConversionFuncs
    ) -> None:
        x = "1_234_567"
        assert func(x)
        assert func(x, allow_underscores=True)
        assert not func(x, allow_underscores=False)

    def test_type_with_underscores_identified_toggled(self) -> None:
        x = "1_234_567"
        assert fastnumbers.query_type(x) is str
        assert fastnumbers.query_type(x, allow_underscores=True) is int
        assert fastnumbers.query_type(x, allow_underscores=False) is str


class TestErrorHandlingConversionFunctionsSuccessful:
    """
    Test the successful execution of the "error handling conversion" functions, e.g.:

    - try_real
    - try_float
    - try_int
    - try_forceint

    """

    # NaN and Infinity handling.
    # First float representation as input, then string.
    # All deal with try_real and try_float only.

    funcs = ["try_real", "try_float"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_nan_returns_nan(self, func: TryReal | TryFloat) -> None:
        assert math.isnan(func(float("nan")))
        assert math.isnan(func(float("nan"), nan=fastnumbers.ALLOWED))  # default

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", [*all_nan, pad("nan"), pad("-NAN")])
    def test_given_nan_string_returns_nan(
        self, func: TryReal | TryFloat, x: str
    ) -> None:
        assert math.isnan(func(x))
        assert math.isnan(func(x, nan=fastnumbers.ALLOWED))  # default

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_nan_returns_sub_value(self, func: TryReal | TryFloat) -> None:
        assert func(float("nan"), nan=0) == 0
        assert math.isnan(func(float("nan"), nan=fastnumbers.INPUT))
        with pytest.raises(ValueError):
            func(float("nan"), nan=fastnumbers.RAISE)
        assert func(float("nan"), nan=lambda _: "hello") == "hello"

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_with_nan_given_nan_string_returns_sub_value(
        self, func: TryReal | TryFloat
    ) -> None:
        assert func("nan", nan=0.0) == 0.0
        assert func("nan", nan=fastnumbers.INPUT) == "nan"
        with pytest.raises(ValueError):
            func("nan", nan=fastnumbers.RAISE)
        assert func("nan", nan=lambda _: "hello") == "hello"

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_inf_returns_inf(self, func: TryReal | TryFloat) -> None:
        assert math.isinf(func(float("inf")))
        assert math.isinf(func(float("inf"), inf=fastnumbers.ALLOWED))  # default

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", [*most_inf, pad("inf"), pad("+INFINITY")])
    def test_given_inf_string_returns_inf(
        self, func: TryReal | TryFloat, x: str
    ) -> None:
        assert math.isinf(func(x))
        assert math.isinf(func(float("inf"), inf=fastnumbers.ALLOWED))  # default

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", [*neg_inf, pad("-inf"), pad("-INFINITY")])
    def test_given_negative_inf_string_returns_negative_inf(
        self, func: TryReal | TryFloat, x: str
    ) -> None:
        assert func(x) == float("-inf")

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_inf_returns_sub_value(self, func: TryReal | TryFloat) -> None:
        assert func(float("inf"), inf=1000.0) == 1000.0
        assert math.isinf(func(float("inf"), inf=fastnumbers.INPUT))
        with pytest.raises(ValueError):
            func(float("inf"), inf=fastnumbers.RAISE)
        assert func(float("inf"), inf=lambda _: "hello") == "hello"

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_with_inf_given_inf_string_returns_sub_value(
        self, func: TryReal | TryFloat
    ) -> None:
        assert func("inf", inf=10000.0) == 10000.0
        assert func("-inf", inf=10000.0) == 10000.0
        assert func("inf", inf=fastnumbers.INPUT) == "inf"
        with pytest.raises(ValueError):
            func("inf", inf=fastnumbers.RAISE)
        assert func("inf", inf=lambda _: "hello") == "hello"

    # Float handling - both actual float input and strings containing floats.

    funcs = ["try_real_coerce_false", "try_float"]

    @given(floats(allow_nan=False))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_float_returns_float(
        self, func: TryReal | TryFloat, x: float
    ) -> None:
        result = func(x)
        assert result == x
        assert isinstance(result, float)

    @given(floats(allow_nan=False).map(repr))
    @example("5.675088586167575e-116")
    @example("10." + "0" * 1050)  # absurdly large number of zeros
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_float_string_returns_float(
        self, func: TryReal | TryFloat, x: str
    ) -> None:
        expected = float(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, float)
        assert func(pad(x)) == expected  # Accepts padding as well

    funcs = ["try_int", "try_forceint"]

    @given(floats(allow_nan=False, allow_infinity=False))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_float_returns_int(
        self, func: TryInt | TryForceInt, x: float
    ) -> None:
        expected = int(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, int)

    # Integer handling - both actual integer input and strings containing integers.

    funcs = [
        "try_real_coerce_true",
        "try_real_coerce_false",
        "try_int",
        "try_forceint",
    ]

    @given(integers())
    @example(int(10 * 300))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_int_returns_int(
        self, func: TryReal | TryInt | TryForceInt, x: int
    ) -> None:
        result = func(x)
        assert result == x
        assert isinstance(result, int)

    @given(integers().map(repr))
    @example("-128")
    @example("127")
    @example("-32768")
    @example("32767")
    @example("-2147483648")
    @example("2147483647")
    @example("-9223372036854775808")
    @example("9223372036854775807")
    @example("40992764608243448035")
    @example("-41538374848935286698640072416676709")
    @example("240278958776173358420034462324117625982")
    @example("1609422692302207451978552816956662956486")
    @example("-121799354242674784350540853922878239740762834")
    @example("32718704454132572934419741118153895444518280065843028297496525078")
    @example("33684944745210074227862907273261282807602986571245071790093633147269")
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_int_string_returns_int(
        self, func: TryReal | TryInt | TryForceInt, x: str
    ) -> None:
        expected = int(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, int)
        assert func(pad(x)) == expected  # Accepts padding as well

    # Special unicode character handling.

    funcs = [
        "try_real_coerce_true",
        "try_real_coerce_false",
        "try_int",
        "try_forceint",
    ]

    @given(sampled_from(digits))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_digit_returns_int(
        self, func: TryReal | TryInt | TryForceInt, x: str
    ) -> None:
        expected = unicodedata.digit(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, int)
        assert func(pad(x)) == expected  # Accepts padding as well

    funcs = ["try_real", "try_float"]

    @given(sampled_from(numeric_not_digit_not_int))
    @example("\u0f33")  # the only negative unicode character
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_numeral_returns_float(
        self, func: TryReal | TryInt | TryForceInt, x: str
    ) -> None:
        expected = unicodedata.numeric(x)
        result = func(x)
        assert result == expected
        assert isinstance(func(x), float)
        assert func(pad(x)) == expected  # Accepts padding as well

    # Tests to ensure correct evaluation is always the first priority.

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_evaluates_valid_even_when_on_fail_given(
        self, func: ConversionFuncs
    ) -> None:
        x = "7"
        expected = 7
        assert func(x, on_fail=len) == expected

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_evaluates_valid_even_when_raise_on_invalid_given(
        self, func: ConversionFuncs
    ) -> None:
        x = "7"
        expected = 7
        assert func(x, on_fail=fastnumbers.RAISE) == expected

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_evaluates_valid_even_when_default_given(
        self, func: ConversionFuncs
    ) -> None:
        x = "7"
        expected = 7
        assert func(x, on_fail=90) == expected

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_evaluates_valid_even_when_input_given(self, func: ConversionFuncs) -> None:
        x = "7"
        expected = 7
        assert func(x, on_fail=fastnumbers.INPUT) == expected


class TestErrorHandlingConversionFunctionsUnsucessful:
    """
    Test the unsuccessful execution of the "error handling conversion" functions, e.g.:

    - try_real
    - try_float
    - try_int
    - try_forceint

    """

    # Handle custom classes with weird behavior.

    funcs = ["try_real_coerce_true", "try_float"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_dumb_float_class_responds_to_internal_valueerror(
        self, func: TryReal | TryFloat
    ) -> None:
        x = DumbFloatClass()
        assert func(x) is x
        with pytest.raises(ValueError):
            func(x, on_fail=fastnumbers.RAISE)
        assert func(x, on_fail=5.0) == 5.0

    funcs = ["try_int", "try_forceint"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_dumb_int_class_responds_to_internal_valueerror(
        self, func: TryInt | TryForceInt
    ) -> None:
        x = DumbIntClass()
        assert func(x) is x
        with pytest.raises(ValueError):
            func(x, on_fail=fastnumbers.RAISE)
        assert func(x, on_fail=5) == 5

    # Handle invalid text input

    funcs = conversion_funcs

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_invalid_string_returns_string_as_is(
        self, func: ConversionFuncs, x: str
    ) -> None:
        assert func(x) is x

    # Handle invalid unicode character input

    funcs = conversion_funcs

    @given(sampled_from(random.sample(not_numeric, 1000)))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_non_numeral_returns_as_is(
        self, func: ConversionFuncs, x: str
    ) -> None:
        assert func(x) == x

    @given(text(min_size=2).filter(not_a_number))
    @example("   \u2007\u2007    ")
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_of_more_than_one_char_returns_as_is(
        self, func: ConversionFuncs, x: str
    ) -> None:
        assert func(x) == x

    # Handle other invalid input

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_invalid_type_raises_typeerror(self, func: ConversionFuncs) -> None:
        with pytest.raises(TypeError):
            func([1])
        with pytest.raises(TypeError):
            func([1], on_type_error=fastnumbers.RAISE)  # default

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_invalid_type_returns_sub_with_on_type_error(
        self, func: ConversionFuncs
    ) -> None:
        assert func([1], on_type_error=1000) == 1000
        assert func([1], on_type_error=fastnumbers.INPUT) == [1]
        assert func([1], on_type_error=len) == 1

    funcs = ["try_int", "try_forceint"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_nan_raises_valueerror_for_int_funcions(
        self, func: TryInt | TryForceInt
    ) -> None:
        with pytest.raises(ValueError):
            func(float("nan"), on_fail=fastnumbers.RAISE)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_inf_raises_overflowerror_for_int_funcions(
        self, func: TryInt | TryForceInt
    ) -> None:
        with pytest.raises(OverflowError):
            func(float("inf"), on_fail=fastnumbers.RAISE)

    # Demonstrate that the error handling options kick in on invalid input

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_invalid_raises_valueerror_if_raise_is_given(
        self, func: ConversionFuncs
    ) -> None:
        with pytest.raises(ValueError):
            func("this is invalid", on_fail=fastnumbers.RAISE)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_default_value_if_given_invalid_string(
        self, func: ConversionFuncs
    ) -> None:
        assert func("this is invalid", on_fail=90) == 90

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_transformed_input_if_invalid_and_on_fail_callable_is_given(
        self, func: ConversionFuncs
    ) -> None:
        x = "this is invalid"
        expected = len(x)
        result = func(x, on_fail=len)
        assert result == expected

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_input_if_invalid_and_on_fail_is_input(
        self, func: ConversionFuncs
    ) -> None:
        x = "this is invalid"
        assert func(x, on_fail=fastnumbers.INPUT) == func(x)  # default


class TestTryReal:
    """
    Tests for the try_real function that are too specific for the generalized tests.
    """

    @given(floats(allow_nan=False).filter(an_integer))
    def test_given_float_returns_int_if_intlike_with_coerce(self, x: float) -> None:
        expected = int(float(x))
        result = fastnumbers.try_real(x, coerce=True)
        assert result == expected
        assert isinstance(result, int)

    @given(floats(allow_nan=False).filter(an_integer))
    @settings(max_examples=500)
    def test_given_float_returns_int_matching_decimal_object_with_denoise(
        self, x: float
    ) -> None:
        expected = int(decimal.Decimal(str(x)))
        result = fastnumbers.try_real(x, coerce=True, denoise=True)
        # This is "best effort", and should be accurate to the final non-zero digit
        assert isinstance(result, int)
        assert_integers_close(result, expected)

    @given(floats(allow_nan=False).filter(an_integer).map(repr))
    @example("1234.56E56")
    @example("12345.60000E56")
    @example("1234560000E-3")
    @example("1234.56789012345678901234567890123456789e56")
    @example("1234.5678901234567890123456789012345678900000e56")
    @example("1234567890123456789012345678901234567890000000000000.")
    @example("123456789012345678901234567890123456789.0000000000000000000000000000")
    @example("1234567890123456789012345678901234567890000000000000e-5")
    def test_given_float_str_returns_int_matching_decimal_object_with_denoise(
        self, x: str
    ) -> None:
        expected = int(decimal.Decimal(x))
        result = fastnumbers.try_real(x, coerce=True, denoise=True)
        assert isinstance(result, int)
        assert result == expected

    @given(floats(allow_nan=False))
    def test_given_float_returns_float_or_int_with_coerce(self, x: float) -> None:
        expected = int(x) if x.is_integer() else x
        expected_type = int if x.is_integer() else float
        result = fastnumbers.try_real(x, coerce=True)
        assert result == expected
        assert isinstance(result, expected_type)

    @given(integers().map(float).map(repr))
    def test_given_float_string_returns_int_with_coerce_with_intlike(
        self, x: str
    ) -> None:
        expected = int(float(x))
        result = fastnumbers.try_real(x, coerce=True)
        assert result == expected
        assert isinstance(result, int)

    def test_underscores_with_denoise(self) -> None:
        given = "3.4_5_3e2_1"
        expected = 3453000000000000000000
        result = fastnumbers.try_real(given, denoise=True, allow_underscores=True)
        assert result == expected


class TestTryFloat:
    """
    Tests for the try_float function that are too specific for the generalized tests.
    """

    def test_with_range_of_exponents_correctly_parses(self) -> None:
        for x in range(-300, 300):
            val = f"1.0E{x:d}"
            assert fastnumbers.try_float(val) == float(val)
        for x in range(-300, 300):
            val = f"1.0000000000E{x:d}"
            assert fastnumbers.try_float(val) == float(val)

    @given(integers())
    def test_given_int_returns_float(self, x: int) -> None:
        expected = float(x)
        result = fastnumbers.try_float(x)
        assert result == expected
        assert isinstance(result, float)

    @given(integers().map(repr))
    def test_given_int_string_returns_float(self, x: str) -> None:
        expected = float(x)
        result = fastnumbers.try_float(x)
        assert result == expected
        assert isinstance(result, float)
        assert fastnumbers.try_float(pad(x)) == expected  # Accepts padding as well

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_float(self, x: str) -> None:
        expected = unicodedata.numeric(x)
        result = fastnumbers.try_float(x)
        assert result == expected
        assert isinstance(result, float)
        assert fastnumbers.try_float(pad(x)) == expected  # Accepts padding as well


class TestTryInt:
    """
    Tests for the try_int function that are too specific for the generalized tests.
    """

    @parametrize("base", [-1, 1, 37])
    def test_given_invalid_base_errors_with_valueerror(self, base: int) -> None:
        with pytest.raises(ValueError):
            fastnumbers.try_int("10", base=base)

    @given(floats(allow_nan=False))
    def test_given_float_string_returns_string_as_is(self, x: float) -> None:
        expected = repr(x)
        assert fastnumbers.try_int(expected) is expected  # includes int-like

    @given(floats().filter(not_an_integer).map(repr))
    @example("nan")
    @example("inf")  # float("inf") returns OverflowError, but "inf" is ValueError
    def test_given_float_string_raises_valueerror_if_raise_on_invalid_is_true(
        self, x: str
    ) -> None:
        with pytest.raises(ValueError):
            fastnumbers.try_int(x, on_fail=fastnumbers.RAISE)

    @given(integers())
    @example(-128)
    @example(127)
    @example(-32768)
    @example(32767)
    @example(-2147483648)
    @example(2147483647)
    @example(-9223372036854775808)
    @example(9223372036854775807)
    def test_given_int_string_returns_int_with_non_base_10(self, x: int) -> None:
        for base in range(2, 36 + 1):
            # Avoid recursion error because of overly simple baseN function.
            # Avoid cases where number ends up creating infinity
            actual = base_n(x, base)
            if len(repr(x)) < 30 and isinstance(actual, int):
                assert fastnumbers.try_int(base_n(x, base), base=base) == x
        assert fastnumbers.try_int(bin(x), base=2) == x
        assert fastnumbers.try_int(bin(x), base=0) == x
        assert fastnumbers.try_int(oct(x), base=8) == x
        assert fastnumbers.try_int(oct(x), base=0) == x
        assert fastnumbers.try_int(oct(x).replace("0o", "0"), base=8) == x
        assert fastnumbers.try_int(hex(x), base=16) == x
        assert fastnumbers.try_int(hex(x), base=0) == x
        # Force unicode path
        assert fastnumbers.try_int(hex(x).replace("0", "\uff10"), base=0) == x

    @parametrize("zero", ["0", "\uff10"])
    @parametrize("base", [0, 2, 8, 18])
    def test_given_multiple_zeros_with_base_returns_zero(
        self, zero: str, base: int
    ) -> None:
        assert fastnumbers.try_int(zero * 4, base=base) == 0

    @given(sampled_from(numeric_not_digit))
    def test_given_unicode_numeral_returns_as_is(self, x: str) -> None:
        assert fastnumbers.try_int(x) == x


class TestTryForceInt:
    """
    Tests for the try_forceint function that are too specific for the generalized tests.
    """

    @given(floats(allow_nan=False, allow_infinity=False))
    def test_given_float_string_returns_int(self, x: float) -> None:
        expected = int(x)
        result = fastnumbers.try_forceint(repr(x))
        assert result == expected
        assert isinstance(result, int)
        assert fastnumbers.try_forceint(pad(repr(x))) == expected  # Accepts padding

    @given(sampled_from(numeric))
    def test_given_unicode_numeral_returns_int(self, x: str) -> None:
        expected = int(unicodedata.numeric(x))
        result = fastnumbers.try_forceint(x)
        assert result == expected
        assert isinstance(result, int)
        assert fastnumbers.try_forceint(pad(x)) == expected  # Accepts padding

    @given(floats(allow_nan=False, allow_infinity=False))
    @settings(max_examples=500)
    def test_given_float_returns_int_matching_decimal_object_with_denoise(
        self, x: float
    ) -> None:
        expected = int(decimal.Decimal(str(x)))
        result = fastnumbers.try_forceint(x, denoise=True)
        # This is "best effort", and should be accurate to the final non-zero digit
        assert isinstance(result, int)
        assert_integers_close(result, expected)

    @given(floats(allow_nan=False, allow_infinity=False).map(repr))
    @example("1234.56E56")
    @example("12345.60000E56")
    @example("1234560000E-3")
    @example("1234.56789012345678901234567890123456789e56")
    @example("1234.5678901234567890123456789012345678900000e56")
    @example("1234567890123456789012345678901234567890000000000000.")
    @example("123456789012345678901234567890123456789.0000000000000000000000000000")
    @example("1234567890123456789012345678901234567890000000000000e-5")
    def test_given_float_str_returns_int_matching_decimal_object_with_denoise(
        self, x: str
    ) -> None:
        expected = int(decimal.Decimal(x))
        result = fastnumbers.try_forceint(x, denoise=True)
        assert isinstance(result, int)
        assert result == expected

    def test_underscores_with_denoise(self) -> None:
        given = "3.4_5_3e2_1"
        expected = 3453000000000000000000
        result = fastnumbers.try_forceint(given, denoise=True, allow_underscores=True)
        assert result == expected


class TestCheckingFunctions:
    """
    Test the successful execution of the "checking" functions, e.g.:

    - check_real
    - check_float
    - check_int
    - check_intlike

    """

    # Handling of NaN and infinity

    funcs = ["check_real", "check_float"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", [float("nan"), float("inf"), float("-inf")])
    def test_returns_true_for_nan_and_inf(
        self, func: CheckReal | CheckFloat, x: float
    ) -> None:
        assert func(x)
        assert func(x, nan=fastnumbers.NUMBER_ONLY, inf=fastnumbers.NUMBER_ONLY)
        assert not func(x, nan=fastnumbers.STRING_ONLY, inf=fastnumbers.STRING_ONLY)
        assert not func(x, nan=fastnumbers.DISALLOWED, inf=fastnumbers.DISALLOWED)
        assert func(x, nan=fastnumbers.ALLOWED, inf=fastnumbers.ALLOWED)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", [*all_nan, pad("nan"), pad("-NAN")])
    def test_returns_false_for_nan_string_unless_allow_nan_is_true(
        self, func: CheckReal | CheckFloat, x: str
    ) -> None:
        assert not func(x)
        assert not func(x, nan=fastnumbers.NUMBER_ONLY)  # default
        assert not func(x, nan=fastnumbers.DISALLOWED)
        assert func(x, nan=fastnumbers.ALLOWED)
        assert func(x, nan=fastnumbers.STRING_ONLY)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", most_inf + neg_inf + [pad("-inf"), pad("+INFINITY")])
    def test_returns_false_for_inf_string_unless_allow_infinity_is_true(
        self, func: CheckReal | CheckFloat, x: str
    ) -> None:
        assert not func(x)
        assert not func(x, nan=fastnumbers.NUMBER_ONLY)  # default
        assert not func(x, nan=fastnumbers.DISALLOWED)
        assert func(x, inf=fastnumbers.ALLOWED)
        assert func(x, inf=fastnumbers.STRING_ONLY)

    # Handling of numeric objects as input

    funcs = ["check_real", "check_int", "check_intlike"]

    @given(integers())
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_true_if_given_int(
        self, func: CheckReal | CheckInt | CheckIntLike, x: int
    ) -> None:
        assert func(x)
        assert func(x, consider=None)  # default
        assert func(x, consider=fastnumbers.NUMBER_ONLY)

    funcs = ["check_real", "check_float"]

    @given(floats())
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_true_if_given_float(
        self, func: CheckReal | CheckFloat, x: float
    ) -> None:
        assert func(x)
        assert func(x, consider=None)  # default
        assert func(x, consider=fastnumbers.NUMBER_ONLY)

    funcs = identification_funcs

    @given(integers() | floats())
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_if_given_number_and_str_only_is_true(
        self, func: IdentificationFuncs, x: FloatOrInt
    ) -> None:
        assert not func(x, consider=fastnumbers.STRING_ONLY)

    # Handling of strings containing numbers as input

    funcs = ["check_real", "check_float"]

    @given(floats(allow_nan=False, allow_infinity=False).map(repr))
    @example("10." + "0" * 1050)
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_true_if_given_float_string(
        self, func: CheckReal | CheckFloat, x: str
    ) -> None:
        assert func(x)
        assert func(pad(x))  # Accepts padding
        assert func(x, consider=None)  # default
        assert func(x, consider=fastnumbers.STRING_ONLY)

    funcs = identification_funcs

    @given(integers().map(repr))
    @example("40992764608243448035")
    @example("-41538374848935286698640072416676709")
    @example("240278958776173358420034462324117625982")
    @example("1609422692302207451978552816956662956486")
    @example("-121799354242674784350540853922878239740762834")
    @example("32718704454132572934419741118153895444518280065843028297496525078")
    @example("33684944745210074227862907273261282807602986571245071790093633147269")
    @example("1" + "0" * 1050)
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_true_if_given_int_string(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert func(x)
        assert func(pad(x))  # Accepts padding
        assert func(x, consider=None)  # default
        assert func(x, consider=fastnumbers.STRING_ONLY)

    @given((integers() | floats(allow_nan=False, allow_infinity=False)).map(repr))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_if_given_string_and_num_only_is_true(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x, consider=fastnumbers.NUMBER_ONLY)

    @given(sampled_from(digits))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_digit_returns_true(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert func(x)
        assert func(pad(x))  # Accepts padding

    funcs = ["check_real", "check_float"]

    @given(sampled_from(numeric_not_digit_not_int))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_numeral_returns_true(
        self, func: CheckReal | CheckFloat, x: str
    ) -> None:
        assert func(x)
        assert func(pad(x))  # Accepts padding

    # Handling of invalid input

    funcs = identification_funcs

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example(b"0e\x00")
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_if_given_non_number_string(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x)

    @given(sampled_from(random.sample(not_numeric, 1000)))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_non_numeral_returns_false(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x)

    @given(text(min_size=2).filter(not_a_number))
    @example("   \u2007\u2007    ")
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_of_more_than_one_char_returns_false(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x)

    funcs = ["check_int", "check_intlike"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_for_nan_or_inf_string(
        self, func: CheckInt | CheckIntLike
    ) -> None:
        assert not func("nan")
        assert not func("inf")


class TestCheckFloat:
    """
    Tests for the check_float function that are too specific for the generalized tests.
    """

    @given(integers())
    def test_returns_false_if_given_int(self, x: int) -> None:
        assert not fastnumbers.check_float(x)

    @given(integers().map(repr))
    def test_returns_false_if_given_int_string_and_strict_true(self, x: str) -> None:
        assert not fastnumbers.check_float(x, strict=True)

    @given(integers().map(repr))
    def test_returns_true_if_given_int_string_and_strict_false(self, x: str) -> None:
        assert fastnumbers.check_float(x)
        assert fastnumbers.check_float(x, strict=False)  # default


class TestCheckInt:
    """
    Tests for the check_int function that are too specific for the generalized tests.
    """

    @given(floats())
    def test_returns_false_if_given_float(self, x: str) -> None:
        assert not fastnumbers.check_int(x)

    @given(integers())
    def test_returns_true_if_given_int_with_base(self, x: int) -> None:
        for base in range(2, 36 + 1):
            assert fastnumbers.check_int(x, base=base)

    @given(integers())
    def test_returns_true_if_given_int_string_with_non_base_10(self, x: int) -> None:
        for base in range(2, 36 + 1):
            # Avoid recursion error because of overly simple baseN function.
            # Avoid cases where number ends up creating infinity
            actual = base_n(x, base)
            if len(repr(x)) < 30 and isinstance(actual, int):
                assert fastnumbers.check_int(base_n(x, base), base=base)
        assert fastnumbers.check_int(bin(x), base=2)
        assert fastnumbers.check_int(bin(x), base=0)
        assert fastnumbers.check_int(oct(x), base=8)
        assert fastnumbers.check_int(oct(x), base=0)
        assert fastnumbers.check_int(oct(x).replace("0o", "0"), base=8)
        if x != 0:
            assert not fastnumbers.check_int(oct(x).replace("0o", "0"), base=0)
        assert fastnumbers.check_int(hex(x), base=16)
        assert fastnumbers.check_int(hex(x), base=0)
        # Force unicode path
        assert fastnumbers.check_int(hex(x).replace("0", "\uff10"), base=0)

    def test_underscores(self) -> None:
        assert fastnumbers.check_int("0_0_0", allow_underscores=True)
        assert fastnumbers.check_int("0_0_0", base=0, allow_underscores=True)
        assert fastnumbers.check_int("4_2", allow_underscores=True)
        assert fastnumbers.check_int("4_2", base=0, allow_underscores=True)
        assert fastnumbers.check_int("1_0000_0000", allow_underscores=True)
        assert fastnumbers.check_int("1_0000_0000", base=0, allow_underscores=True)
        assert fastnumbers.check_int("0b1001_0100", base=0, allow_underscores=True)
        assert fastnumbers.check_int("0xffff_ffff", base=0, allow_underscores=True)
        assert fastnumbers.check_int("0o5_7_7", base=0, allow_underscores=True)
        assert fastnumbers.check_int("0b_0", base=0, allow_underscores=True)
        assert fastnumbers.check_int("0x_f", base=0, allow_underscores=True)
        assert fastnumbers.check_int("0o_5", base=0, allow_underscores=True)

        # Underscores in the base selector:
        assert not fastnumbers.check_int("0_b0", allow_underscores=True)
        assert not fastnumbers.check_int("0_b0", base=0, allow_underscores=True)
        assert not fastnumbers.check_int("0_xf", allow_underscores=True)
        assert not fastnumbers.check_int("0_xf", base=0, allow_underscores=True)
        assert not fastnumbers.check_int("0_o5", allow_underscores=True)
        assert not fastnumbers.check_int("0_o5", base=0, allow_underscores=True)

        # Old-style octal, still disallowed if base guess is needed:
        assert not fastnumbers.check_int("0_7", base=0, allow_underscores=True)
        assert not fastnumbers.check_int("09_99", base=0, allow_underscores=True)

        # Two underscores:
        assert not fastnumbers.check_int("0b1001__0100", base=0, allow_underscores=True)
        assert not fastnumbers.check_int("0xffff__ffff", base=0, allow_underscores=True)

    @given(floats(allow_nan=False, allow_infinity=False).map(repr))
    def test_returns_false_if_given_float_string(self, x: str) -> None:
        assert not fastnumbers.check_int(x)
        assert not fastnumbers.check_int(pad(x))
        for base in range(2, 36 + 1):
            if len(x) < 30:
                assert not fastnumbers.check_int(x, base=base)

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_false(self, x: str) -> None:
        assert not fastnumbers.check_int(x)


class TestCheckIntLike:
    """
    Tests for the check_intlike function that are too specific
    for the generalized tests.
    """

    @given(floats().filter(not_an_integer))
    def test_returns_false_if_given_non_integer_float(self, x: float) -> None:
        assert not fastnumbers.check_intlike(x)

    @given(floats().filter(an_integer))
    def test_returns_true_if_given_integer_float(self, x: float) -> None:
        assert fastnumbers.check_intlike(x)

    @given(
        floats(allow_nan=False, allow_infinity=False).filter(not_an_integer).map(repr)
    )
    def test_returns_false_if_given_non_integer_float_string(self, x: str) -> None:
        assert not fastnumbers.check_intlike(x)
        assert not fastnumbers.check_intlike(pad(x))  # Accepts padding

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_non_digit_numeral_returns_false(self, x: str) -> None:
        assert not fastnumbers.check_intlike(x)

    @given(
        sampled_from(numeric_not_digit).filter(
            lambda x: an_integer(unicodedata.numeric(x))
        )
    )
    def test_given_unicode_digit_numeral_returns_true(self, x: str) -> None:
        assert fastnumbers.check_intlike(x)
        assert fastnumbers.check_intlike(pad(x))  # Accepts padding


class TestQueryType:
    """Tests for the query_type function."""

    def test_allowed_type_must_be_a_sequence(self) -> None:
        with pytest.raises(TypeError, match="allowed_type is not a sequence type"):
            fastnumbers.query_type("5", allowed_types={str: float})  # type: ignore[call-overload]

    def test_allowed_type_must_non_empty(self) -> None:
        with pytest.raises(
            ValueError, match="allowed_type must not be an empty sequence"
        ):
            fastnumbers.query_type("5", allowed_types=[])

    @given(integers())
    def test_returns_int_if_given_int(self, x: int) -> None:
        assert fastnumbers.query_type(x) is int

    @given(floats())
    def test_returns_float_if_given_float(self, x: float) -> None:
        assert fastnumbers.query_type(x) is float

    @given(integers())
    def test_returns_none_if_given_int_and_int_is_not_allowed(self, x: int) -> None:
        assert fastnumbers.query_type(x, allowed_types=(float,)) is None

    @given(floats())
    def test_returns_none_if_given_float_and_float_is_not_allowed(
        self, x: float
    ) -> None:
        assert fastnumbers.query_type(x, allowed_types=(int,)) is None

    @given(integers().map(repr))
    def test_returns_int_if_given_int_string(self, x: str) -> None:
        assert fastnumbers.query_type(x) is int
        assert fastnumbers.query_type(pad(x)) is int  # Accepts padding

    @given(floats(allow_nan=False, allow_infinity=False).map(repr))
    def test_returns_float_if_given_float_string_padded_or_not(self, x: str) -> None:
        assert fastnumbers.query_type(x) is float
        assert fastnumbers.query_type(pad(x)) is float  # Accpets padding

    @given(integers().map(repr))
    def test_returns_none_if_given_int_string_and_int_is_not_allowed(
        self, x: str
    ) -> None:
        assert fastnumbers.query_type(x, allowed_types=(float, str)) is None

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_int(self, x: str) -> None:
        assert fastnumbers.query_type(x) is int
        assert fastnumbers.query_type(pad(x)) is int  # Accepts padding

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_float(self, x: str) -> None:
        assert fastnumbers.query_type(x) is float
        assert fastnumbers.query_type(pad(x)) is float  # Accepts padding

    @given(sampled_from(random.sample(not_numeric, 1000)))
    def test_given_unicode_non_numeral_returns_str_or_none_if_str_not_allowed(
        self, x: str
    ) -> None:
        assert fastnumbers.query_type(x) is str
        assert fastnumbers.query_type(x, allowed_types=(int, float)) is None

    @given(text(min_size=2).filter(not_a_number))
    @example("   \u2007\u2007    ")
    def test_given_unicode_of_more_than_one_char_returns_str(self, x: str) -> None:
        assert fastnumbers.query_type(x) is str

    @given(text().filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    def test_returns_str_if_given_non_number_string(self, x: str) -> None:
        assert fastnumbers.query_type(x) is str

    @given(text().filter(not_a_number))
    def test_returns_none_if_given_non_number_string_and_str_is_not_allowed(
        self, x: str
    ) -> None:
        assert fastnumbers.query_type(x, allowed_types=(int, float)) is None

    @given(binary().filter(not_a_number))
    def test_returns_bytes_if_given_non_number_string(self, x: bytes) -> None:
        assert fastnumbers.query_type(x) is bytes

    @given(binary().filter(not_a_number))
    def test_returns_none_if_given_non_number_bytes_and_bytes_is_not_allowed(
        self, x: bytes
    ) -> None:
        assert fastnumbers.query_type(x, allowed_types=(int, float)) is None

    @parametrize("x", [*all_nan, pad("+nan"), pad("-NAN")])
    def test_returns_str_for_nan_string_unless_allow_nan_is_true(self, x: str) -> None:
        assert fastnumbers.query_type(x) is str
        assert fastnumbers.query_type(x, allow_nan=True) is float

    @parametrize("x", most_inf + neg_inf + [pad("+inf"), pad("-INFINITY")])
    def test_returns_str_for_inf_string_unless_allow_infinity_is_true(
        self, x: str
    ) -> None:
        assert fastnumbers.query_type(x) is str
        assert fastnumbers.query_type(x, allow_inf=True) is float

    def test_given_nan_returns_float(self) -> None:
        assert fastnumbers.query_type(float("nan")) is float

    def test_given_inf_returns_float(self) -> None:
        assert fastnumbers.query_type(float("inf")) is float

    @given(floats(allow_nan=False).filter(an_integer))
    def test_given_float_returns_int_if_intlike_with_coerce(self, x: float) -> None:
        assert fastnumbers.query_type(x, coerce=True) is int

    @given(floats(allow_nan=False))
    def test_given_float_returns_float_or_int_with_coerce(self, x: float) -> None:
        assert (
            fastnumbers.query_type(x, coerce=True) is int if x.is_integer() else float
        )

    @given(
        lists(floats())
        | tuples(floats())
        | dictionaries(floats(), floats())
        | sets(floats())
        | iterables(floats())
    )
    def test_containers_returns_container_type(
        self, x: dict[float, float] | Iterable[float]
    ) -> None:
        assert fastnumbers.query_type(x) is type(x)
        assert fastnumbers.query_type(x, allowed_types=(float, int, str)) is None


class TestMappingFunctions:
    """Ensure that mapping functions operate on iterables"""

    @given(lists(floats() | integers() | text(max_size=50), min_size=1, max_size=50))
    @parametrize(
        "nomapper, mapper",
        [
            (fastnumbers.try_real, partial(fastnumbers.try_real, map=list)),
            (fastnumbers.try_float, partial(fastnumbers.try_float, map=list)),
            (fastnumbers.try_int, partial(fastnumbers.try_int, map=list)),
            (fastnumbers.try_forceint, partial(fastnumbers.try_forceint, map=list)),
        ],
    )
    @parametrize(
        "kwargs",
        [
            {},
            {"inf": fastnumbers.RAISE},
            {"nan": fastnumbers.RAISE},
            {"on_fail": fastnumbers.RAISE},
            {"base": 16},
            {"coerce": True},
        ],
    )
    def test_mapping_non_mapping_behave_the_same(
        self,
        nomapper: ConversionFuncs,
        mapper: ConversionFuncs,
        kwargs: dict[str, Any],
        x: list[float | int | str],
    ) -> None:
        nomapper = partial(nomapper, **kwargs)
        mapper = partial(mapper, **kwargs)
        nonmapping = capture_result(lambda y: list(map(nomapper, y)), x)
        mapping = capture_result(mapper, x)
        if nonmapping != nonmapping and mapping != mapping:
            assert math.isnan(nonmapping)
            assert math.isnan(mapping)
        else:
            assert nonmapping == mapping

    @parametrize(
        "nomapper, mapper",
        [
            (fastnumbers.try_real, partial(fastnumbers.try_real, map=list)),
            (fastnumbers.try_float, partial(fastnumbers.try_float, map=list)),
            (fastnumbers.try_int, partial(fastnumbers.try_int, map=list)),
            (fastnumbers.try_forceint, partial(fastnumbers.try_forceint, map=list)),
        ],
    )
    def test_mapping_non_mapping_behave_the_same_with_invalid_types(
        self, nomapper: ConversionFuncs, mapper: ConversionFuncs
    ) -> None:
        x = ["7", "5", None]
        nonmapping = capture_result(lambda y: list(map(nomapper, y)), x)
        mapping = capture_result(mapper, x)
        assert nonmapping == mapping

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=list),
            partial(fastnumbers.try_float, map=list),
            partial(fastnumbers.try_int, map=list),
            partial(fastnumbers.try_forceint, map=list),
            partial(fastnumbers.try_real, map=True),
            partial(fastnumbers.try_float, map=True),
            partial(fastnumbers.try_int, map=True),
            partial(fastnumbers.try_forceint, map=True),
        ],
    )
    @parametrize(
        "iterable_gen",
        [
            lambda: ["6", "4", "590"],
            lambda: ("6", "4", "590"),
            lambda: {"6", "4", "590"},
            lambda: iter(["6", "4", "590"]),
            lambda: (x for x in ["6", "4", "590"]),
        ],
    )
    def test_mapping_handles_any_iterable(
        self, func: ConversionFuncs, iterable_gen: Callable[[], Iterable[Any]]
    ) -> None:
        expected = [4, 6, 590]
        result = sorted(func(iterable_gen()))  # sorted needed b/c of set
        assert result == expected

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=list),
            partial(fastnumbers.try_float, map=list),
            partial(fastnumbers.try_int, map=list),
            partial(fastnumbers.try_forceint, map=list),
        ],
    )
    def test_mapping_handles_range(self, func: ConversionFuncs) -> None:
        """Range is a sequence but is not a 'fast sequence'"""
        expected = [0, 1, 2, 3]
        result = func(range(4))
        assert result == expected

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=True),
            partial(fastnumbers.try_float, map=True),
            partial(fastnumbers.try_int, map=True),
            partial(fastnumbers.try_forceint, map=True),
        ],
    )
    def test_mapping_iterator_handles_range(self, func: ConversionFuncs) -> None:
        """Range is a sequence but is not a 'fast sequence'"""
        result = func(range(4))
        assert next(result) == 0
        assert next(result) == 1
        assert next(result) == 2
        assert next(result) == 3
        assert next(result, None) is None

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=list),
            partial(fastnumbers.try_float, map=list),
            partial(fastnumbers.try_int, map=list),
            partial(fastnumbers.try_forceint, map=list),
            partial(fastnumbers.try_real, map=True),
            partial(fastnumbers.try_float, map=True),
            partial(fastnumbers.try_int, map=True),
            partial(fastnumbers.try_forceint, map=True),
        ],
    )
    def test_mapping_handles_broken_generator(self, func: ConversionFuncs) -> None:
        """A generator's exception should be returned"""

        def broken() -> Iterator[str]:
            """Not a good generator"""
            yield "5"
            yield "6"
            msg = "Fëanor"
            raise ValueError(msg)

        with pytest.raises(ValueError, match="Fëanor"):  # noqa: PT012
            for _ in func(broken()):
                pass

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=list),
            partial(fastnumbers.try_float, map=list),
            partial(fastnumbers.try_int, map=list),
            partial(fastnumbers.try_forceint, map=list),
        ],
    )
    @parametrize(
        "iterable_gen",
        [
            list,
            lambda: (),
            lambda: set(),
            lambda: iter([]),
            lambda: (x for x in []),  # type: ignore [var-annotated]
        ],
    )
    def test_mapping_handles_empty_iterable(
        self, func: ConversionFuncs, iterable_gen: Callable[[], Iterable[Any]]
    ) -> None:
        expected: list[Any] = []
        result = func(iterable_gen())
        assert result == expected

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=True),
            partial(fastnumbers.try_float, map=True),
            partial(fastnumbers.try_int, map=True),
            partial(fastnumbers.try_forceint, map=True),
        ],
    )
    @parametrize(
        "iterable_gen",
        [
            list,
            lambda: (),
            lambda: set(),
            lambda: iter([]),
            lambda: (x for x in []),  # type: ignore [var-annotated]
        ],
    )
    def test_mapping_iterable_handles_empty_iterable(
        self, func: ConversionFuncs, iterable_gen: Callable[[], Iterable[Any]]
    ) -> None:
        with pytest.raises(StopIteration):
            next(func(iterable_gen()))

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=list),
            partial(fastnumbers.try_float, map=list),
            partial(fastnumbers.try_int, map=list),
            partial(fastnumbers.try_forceint, map=list),
            partial(fastnumbers.try_real, map=True),
            partial(fastnumbers.try_float, map=True),
            partial(fastnumbers.try_int, map=True),
            partial(fastnumbers.try_forceint, map=True),
        ],
    )
    def test_mapping_raises_type_error_on_non_iterable(
        self, func: ConversionFuncs
    ) -> None:
        with pytest.raises(TypeError, match="'int' object is not iterable"):
            func(5)

    @parametrize(
        "func",
        [
            partial(fastnumbers.try_real, map=list),
            partial(fastnumbers.try_float, map=list),
            partial(fastnumbers.try_int, map=list),
            partial(fastnumbers.try_forceint, map=list),
            partial(fastnumbers.try_real, map=True),
            partial(fastnumbers.try_float, map=True),
            partial(fastnumbers.try_int, map=True),
            partial(fastnumbers.try_forceint, map=True),
        ],
    )
    @parametrize("style", [list, iter])
    def test_invalid_types_behave_as_expected(
        self, func: ConversionFuncs, style: Callable[[Any], Any]
    ) -> None:
        with pytest.raises(TypeError, match="not 'tuple'"):  # noqa: PT012
            for _ in func(style([("Fëanor",)])):
                pass
        expected = [5]
        result = list(func(style([("Fëanor",)]), on_type_error=5))
        assert result == expected
