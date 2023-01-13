# -*- coding: utf-8 -*-
# Find the build location and add that to the path
import math
import random
import re
import unicodedata
from functools import partial
from typing import (
    Any,
    Callable,
    Dict,
    Iterable,
    List,
    NoReturn,
    Union,
)

from hypothesis import example, given
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
from pytest import mark, raises
from typing_extensions import Protocol

import fastnumbers

parametrize = mark.parametrize


# Assistance with type hints
def dummy(x: Any) -> Any:
    return x


FloatOrInt = Union[float, int]


class FastReal(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = None,
        *,
        raise_on_invalid: bool = False,
        inf: Any = None,
        nan: Any = None,
        on_fail: Callable[[Any], Any] = dummy,
        coerce: bool = True,
        allow_underscores: bool = True,
        key: Callable[[Any], Any] = dummy,
    ) -> Any:
        ...


class FastFloat(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = None,
        *,
        raise_on_invalid: bool = False,
        inf: Any = None,
        nan: Any = None,
        on_fail: Callable[[Any], Any] = dummy,
        allow_underscores: bool = True,
        key: Callable[[Any], Any] = dummy,
    ) -> Any:
        ...


class FastInt(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = None,
        *,
        raise_on_invalid: bool = False,
        base: int = 0,
        on_fail: Callable[[Any], Any] = dummy,
        allow_underscores: bool = True,
        key: Callable[[Any], Any] = dummy,
    ) -> Any:
        ...


class FastForceInt(Protocol):
    def __call__(
        self,
        x: Any,
        default: Any = None,
        *,
        raise_on_invalid: bool = False,
        on_fail: Callable[[Any], Any] = dummy,
        allow_underscores: bool = True,
        key: Callable[[Any], Any] = dummy,
    ) -> Any:
        ...


class IsReal(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = False,
        num_only: bool = False,
        allow_inf: bool = False,
        allow_nan: bool = False,
        allow_underscores: bool = True,
    ) -> bool:
        ...


class IsFloat(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = False,
        num_only: bool = False,
        allow_inf: bool = False,
        allow_nan: bool = False,
        allow_underscores: bool = True,
    ) -> bool:
        ...


class IsInt(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = False,
        num_only: bool = False,
        base: int = 0,
        allow_underscores: bool = True,
    ) -> bool:
        ...


class IsIntLike(Protocol):
    def __call__(
        self,
        x: Any,
        *,
        str_only: bool = False,
        num_only: bool = False,
        allow_underscores: bool = True,
    ) -> bool:
        ...


class Real(Protocol):
    def __call__(self, x: Any = 0.0, *, coerce: bool = True) -> Union[int, float]:
        ...


ConversionFuncs = Union[FastReal, FastFloat, FastInt, FastForceInt]
IdentificationFuncs = Union[IsReal, IsFloat, IsInt, IsIntLike]
NonBuiltinFuncs = Union[ConversionFuncs, IdentificationFuncs]

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


def a_number(s: Union[str, bytes]) -> bool:
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
    if re.match(r"\s*([-+]?\d+\.?\d*(?:[eE][-+]?\d+)?)\s*$", s, re.U):
        return True
    if re.match(r"\s*([-+]?\.\d+(?:[eE][-+]?\d+)?)\s*$", s, re.U):
        return True
    if s.strip().lstrip("[-+]") in numeric:
        return True
    return False


def space() -> str:
    """90% chance of ' ', 10% of unicode-only space."""
    return random.choice([" "] * 90 + ["\u2007"] * 10)


def pad(value: str) -> str:
    """Pad a string with whitespace at the front and back."""
    return random.randint(1, 100) * space() + value + random.randint(1, 100) * space()


def not_a_number(s: Union[str, bytes]) -> bool:
    return not a_number(s)


def an_integer(x: float) -> bool:
    return x.is_integer()


def not_an_integer(x: float) -> bool:
    return not x.is_integer()


def base_n(
    num: int, b: int, numerals: str = "0123456789abcdefghijklmnopqrstuvwxyz"
) -> str:
    """
    Convert any integer to a Base-N string representation.
    Shamelessly stolen from http://stackoverflow.com/a/2267428/1399279
    """
    neg = num < 0
    num = abs(num)
    val = ((num == 0) and numerals[0]) or (
        base_n(num // b, b, numerals).lstrip(numerals[0]) + numerals[num % b]
    )
    return "-" + val if neg else val


class DumbFloatClass(object):
    def __float__(self) -> NoReturn:
        raise ValueError("something here might go wrong")


class DumbIntClass(object):
    def __int__(self) -> NoReturn:
        raise ValueError("something here might go wrong")


# Map function names to the actual functions,
# for dymamic declaration of which functions test below.
func_mapping: Dict[str, Callable[..., Any]] = {
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
}


def get_funcs(function_names: Iterable[str]) -> List[Callable[..., Any]]:
    """Given a list of function names, return the associated functions"""
    return [func_mapping[x] for x in function_names]


# Common convenience functiom collections
conversion_funcs = ["fast_real", "fast_float", "fast_int", "fast_forceint"]
identification_funcs = ["isreal", "isfloat", "isint", "isintlike"]
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
    assert fastnumbers.real(x) == fastnumbers.fast_real(x)


class TestArguments:
    """Tests that high-level argument handling is as expected"""

    def test_real_no_arguments_returns_0(self) -> None:
        assert fastnumbers.real() == 0

    funcs = non_builtin_funcs + ["real"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_invalid_argument_raises_type_error(
        self, func: Union[NonBuiltinFuncs, Real]
    ) -> None:
        with raises(TypeError):
            func(5, invalid="dummy")  # type: ignore

    funcs = non_builtin_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_no_arguments_raises_type_error(self, func: NonBuiltinFuncs) -> None:
        with raises(TypeError):
            func()  # type: ignore


class TestBackwardsCompatibility:

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_key(self, func: ConversionFuncs) -> None:
        with raises(ValueError, match=r"^Cannot set both on_fail and key$"):
            func("dummy", key=len, on_fail=len)
        assert func("dummy", key=len) == 5
        assert func("dummy", key=len) == func("dummy", on_fail=len)


class TestUnderscores:
    """Tests to make sure underscores are well handled in >= 3.6."""

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_numbers_with_underscores_converted_toggled(
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
        assert func(x)
        assert func(x, allow_underscores=True)
        assert not func(x, allow_underscores=False)

    def test_type_with_underscores_identified_toggled(self) -> None:
        x = "1_234_567"
        assert fastnumbers.query_type(x) is int
        assert fastnumbers.query_type(x, allow_underscores=True) is int
        assert fastnumbers.query_type(x, allow_underscores=False) is str


class TestErrorHandlingConversionFunctionsSuccessful:
    """
    Test the successful execution of the "error handling conversion" functions, e.g.:

    - fast_real
    - fast_float
    - fast_int
    - fast_forceint

    """

    # NaN and Infinity handling.
    # First float representation as input, then string.
    # All deal with fast_real and fast_float only.

    funcs = ["fast_real", "fast_float"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_nan_returns_nan(self, func: Union[FastReal, FastFloat]) -> None:
        assert math.isnan(func(float("nan")))

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", all_nan + [pad("nan"), pad("-NAN")])
    def test_given_nan_string_returns_nan(
        self, func: Union[FastReal, FastFloat], x: str
    ) -> None:
        assert math.isnan(func(x))

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_nan_returns_sub_value(
        self, func: Union[FastReal, FastFloat]
    ) -> None:
        assert func(float("nan"), nan=0) == 0

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_with_nan_given_nan_string_returns_sub_value(
        self, func: Union[FastReal, FastFloat]
    ) -> None:
        assert func("nan", nan=0.0) == 0.0

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_inf_returns_inf(self, func: Union[FastReal, FastFloat]) -> None:
        assert math.isinf(func(float("inf")))

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", most_inf + [pad("inf"), pad("+INFINITY")])
    def test_given_inf_string_returns_inf(
        self, func: Union[FastReal, FastFloat], x: str
    ) -> None:
        assert func(x) == float("inf")

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", neg_inf + [pad("-inf"), pad("-INFINITY")])
    def test_given_negative_inf_string_returns_negative_inf(
        self, func: Union[FastReal, FastFloat], x: str
    ) -> None:
        assert func(x) == float("-inf")

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_inf_returns_sub_value(
        self, func: Union[FastReal, FastFloat]
    ) -> None:
        assert func(float("inf"), inf=1000.0) == 1000.0

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_with_inf_given_inf_string_returns_sub_value(
        self, func: Union[FastReal, FastFloat]
    ) -> None:
        assert func("inf", inf=10000.0) == 10000.0
        assert func("-inf", inf=10000.0) == 10000.0

    # Float handling - both actual float input and strings containing floats.

    funcs = ["fast_real_coerce_false", "fast_float"]

    @given(floats(allow_nan=False))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_float_returns_float(
        self, func: Union[FastReal, FastFloat], x: float
    ) -> None:
        result = func(x)
        assert result == x
        assert isinstance(result, float)

    @given(floats(allow_nan=False).map(repr))
    @example("5.675088586167575e-116")
    @example("10." + "0" * 1050)  # absurdly large number of zeros
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_float_string_returns_float(
        self, func: Union[FastReal, FastFloat], x: str
    ) -> None:
        expected = float(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, float)
        assert func(pad(x)) == expected  # Accepts padding as well

    funcs = ["fast_int", "fast_forceint"]

    @given(floats(allow_nan=False, allow_infinity=False))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_float_returns_int(
        self, func: Union[FastInt, FastForceInt], x: float
    ) -> None:
        expected = int(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, int)

    # Integer handling - both actual integer input and strings containing integers.

    funcs = [
        "fast_real_coerce_true",
        "fast_real_coerce_false",
        "fast_int",
        "fast_forceint",
    ]

    @given(integers())
    @example(int(10 * 300))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_int_returns_int(
        self, func: Union[FastReal, FastInt, FastForceInt], x: int
    ) -> None:
        result = func(x)
        assert result == x
        assert isinstance(result, int)

    @given(integers().map(repr))
    @example("40992764608243448035")
    @example("-41538374848935286698640072416676709")
    @example("240278958776173358420034462324117625982")
    @example("1609422692302207451978552816956662956486")
    @example("-121799354242674784350540853922878239740762834")
    @example("32718704454132572934419741118153895444518280065843028297496525078")
    @example("33684944745210074227862907273261282807602986571245071790093633147269")
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_int_string_returns_int(
        self, func: Union[FastReal, FastInt, FastForceInt], x: str
    ) -> None:
        expected = int(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, int)
        assert func(pad(x)) == expected  # Accepts padding as well

    # Special unicode character handling.

    funcs = [
        "fast_real_coerce_true",
        "fast_real_coerce_false",
        "fast_int",
        "fast_forceint",
    ]

    @given(sampled_from(digits))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_digit_returns_int(
        self, func: Union[FastReal, FastInt, FastForceInt], x: str
    ) -> None:
        expected = unicodedata.digit(x)
        result = func(x)
        assert result == expected
        assert isinstance(result, int)
        assert func(pad(x)) == expected  # Accepts padding as well

    funcs = ["fast_real", "fast_float"]

    @given(sampled_from(numeric_not_digit_not_int))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_numeral_returns_float(
        self, func: Union[FastReal, FastInt, FastForceInt], x: str
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
        assert func(x, raise_on_invalid=True) == expected

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_evaluates_valid_even_when_default_given(
        self, func: ConversionFuncs
    ) -> None:
        x = "7"
        expected = 7
        assert func(x, default=90) == expected


class TestErrorHandlingConversionFunctionsUnsucessful:
    """
    Test the unsuccessful execution of the "error handling conversion" functions, e.g.:

    - fast_real
    - fast_float
    - fast_int
    - fast_forceint

    """

    # Handle custom classes with weird behavior.

    funcs = ["fast_real_coerce_true", "fast_float"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_dumb_float_class_responds_to_internal_valueerror(
        self, func: Union[FastReal, FastFloat]
    ) -> None:
        x = DumbFloatClass()
        assert func(x) is x
        with raises(ValueError):
            func(x, raise_on_invalid=True)
        assert func(x, default=5.0) == 5.0

    funcs = ["fast_int", "fast_forceint"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_dumb_int_class_responds_to_internal_valueerror(
        self, func: Union[FastInt, FastForceInt]
    ) -> None:
        x = DumbIntClass()
        assert func(x) is x
        with raises(ValueError):
            func(x, raise_on_invalid=True)
        assert func(x, default=5) == 5

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

    @given(sampled_from(not_numeric))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_non_numeral_returns_as_is(
        self, func: ConversionFuncs, x: str
    ) -> None:
        assert func(x) == x

    @given(text(min_size=2).filter(not_a_number))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_of_more_than_one_char_returns_as_is(
        self, func: ConversionFuncs, x: str
    ) -> None:
        assert func(x) == x

    # Handle other invalid input

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_invalid_type_raises_typeerror(self, func: ConversionFuncs) -> None:
        with raises(TypeError):
            func([1])

    funcs = ["fast_int", "fast_forceint"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_nan_raises_valueerror_for_int_funcions(
        self, func: Union[FastInt, FastForceInt]
    ) -> None:
        with raises(ValueError):
            func(float("nan"), raise_on_invalid=True)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_inf_raises_overflowerror_for_int_funcions(
        self, func: Union[FastInt, FastForceInt]
    ) -> None:
        with raises(OverflowError):
            func(float("inf"), raise_on_invalid=True)

    # Demonstrate that the error handling options kick in on invalid input

    funcs = conversion_funcs

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_invalid_raises_valueerror_if_raise_on_invalid_is_true(
        self, func: ConversionFuncs
    ) -> None:
        with raises(ValueError):
            func("this is invalid", raise_on_invalid=True)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_default_value_if_given_invalid_string(
        self, func: ConversionFuncs
    ) -> None:
        assert func("this is invalid", default=90) == 90

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_raise_on_invalid_takes_precedence_over_default(
        self, func: ConversionFuncs
    ) -> None:
        with raises(ValueError):
            func("this is invalid", default=90, raise_on_invalid=True)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_transformed_input_if_invalid_and_on_fail_is_given(
        self, func: ConversionFuncs
    ) -> None:
        x = "this is invalid"
        expected = len(x)
        result = func(x, on_fail=len)
        assert result == expected


class TestFastReal:
    """
    Tests for the fast_real function that are too specific for the generalized tests.
    """

    @given(floats(allow_nan=False).filter(an_integer))
    def test_given_float_returns_int_if_intlike_with_coerce(self, x: float) -> None:
        expected = int(float(x))
        result = fastnumbers.fast_real(x, coerce=True)
        assert result == expected
        assert isinstance(result, int)

    @given(floats(allow_nan=False))
    def test_given_float_returns_float_or_int_with_coerce(self, x: float) -> None:
        expected = int(x) if x.is_integer() else x
        expected_type = int if x.is_integer() else float
        result = fastnumbers.fast_real(x, coerce=True)
        assert result == expected
        assert isinstance(result, expected_type)

    @given(integers().map(float).map(repr))
    def test_given_float_string_returns_int_with_coerce_with_intlike(
        self, x: str
    ) -> None:
        expected = int(float(x))
        result = fastnumbers.fast_real(x, coerce=True)
        assert result == expected
        assert isinstance(result, int)


class TestFastFloat:
    """
    Tests for the fast_float function that are too specific for the generalized tests.
    """

    def test_with_range_of_exponents_correctly_parses(self) -> None:
        for x in range(-300, 300):
            val = "1.0E{0:d}".format(x)
            assert fastnumbers.fast_float(val) == float(val)
        for x in range(-300, 300):
            val = "1.0000000000E{0:d}".format(x)
            assert fastnumbers.fast_float(val) == float(val)

    @given(integers())
    def test_given_int_returns_float(self, x: int) -> None:
        expected = float(x)
        result = fastnumbers.fast_float(x)
        assert result == expected
        assert isinstance(result, float)

    @given(integers().map(repr))
    def test_given_int_string_returns_float(self, x: str) -> None:
        expected = float(x)
        result = fastnumbers.fast_float(x)
        assert result == expected
        assert isinstance(result, float)
        assert fastnumbers.fast_float(pad(x)) == expected  # Accepts padding as well

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_float(self, x: str) -> None:
        expected = unicodedata.numeric(x)
        result = fastnumbers.fast_float(x)
        assert result == expected
        assert isinstance(result, float)
        assert fastnumbers.fast_float(pad(x)) == expected  # Accepts padding as well


class TestFastInt:
    """
    Tests for the fast_int function that are too specific for the generalized tests.
    """

    @parametrize("base", [-1, 1, 37])
    def test_given_invalid_base_errors_with_valueerror(self, base: int) -> None:
        with raises(ValueError):
            fastnumbers.fast_int("10", base=base)

    @given(floats(allow_nan=False))
    def test_given_float_string_returns_string_as_is(self, x: float) -> None:
        expected = repr(x)
        assert fastnumbers.fast_int(expected) is expected  # includes int-like

    @given(floats().filter(not_an_integer).map(repr))
    @example("nan")
    @example("inf")  # float("inf") returns OverflowError, but "inf" is ValueError
    def test_given_float_string_raises_valueerror_if_raise_on_invalid_is_true(
        self, x: str
    ) -> None:
        with raises(ValueError):
            fastnumbers.fast_int(x, raise_on_invalid=True)

    @given(integers())
    def test_given_int_string_returns_int_with_non_base_10(self, x: int) -> None:
        for base in range(2, 36 + 1):
            # Avoid recursion error because of overly simple baseN function.
            # Avoid cases where number ends up creating infinity
            actual = base_n(x, base)
            if len(repr(x)) < 30 and isinstance(actual, int):
                assert fastnumbers.fast_int(base_n(x, base), base=base) == x
        assert fastnumbers.fast_int(bin(x), base=2) == x
        assert fastnumbers.fast_int(bin(x), base=0) == x
        assert fastnumbers.fast_int(oct(x), base=8) == x
        assert fastnumbers.fast_int(oct(x), base=0) == x
        assert fastnumbers.fast_int(oct(x).replace("0o", "0"), base=8) == x
        assert fastnumbers.fast_int(hex(x), base=16) == x
        assert fastnumbers.fast_int(hex(x), base=0) == x
        # Force unicode path
        assert fastnumbers.fast_int(hex(x).replace("0", "\uFF10"), base=0) == x

    @parametrize("zero", ["0", "\uFF10"])
    @parametrize("base", [0, 2, 8, 18])
    def test_given_multiple_zeros_with_base_returns_zero(
        self, zero: str, base: int
    ) -> None:
        assert fastnumbers.fast_int(zero * 4, base=base) == 0

    @given(sampled_from(numeric_not_digit))
    def test_given_unicode_numeral_returns_as_is(self, x: str) -> None:
        assert fastnumbers.fast_int(x) == x


class TestFastForceInt:
    """
    Tests for the fast_forceint function that are too specific for the generalized tests.
    """

    @given(floats(allow_nan=False, allow_infinity=False))
    def test_given_float_string_returns_int(self, x: float) -> None:
        expected = int(x)
        result = fastnumbers.fast_forceint(repr(x))
        assert result == expected
        assert isinstance(result, int)
        assert fastnumbers.fast_forceint(pad(repr(x))) == expected  # Accepts padding

    @given(sampled_from(numeric))
    def test_given_unicode_numeral_returns_int(self, x: str) -> None:
        expected = int(unicodedata.numeric(x))
        result = fastnumbers.fast_forceint(x)
        assert result == expected
        assert isinstance(result, int)
        assert fastnumbers.fast_forceint(pad(x)) == expected  # Accepts padding


class TestCheckingFunctions:
    """
    Test the successful execution of the "checking" functions, e.g.:

    - isreal
    - isfloat
    - isint
    - isintlike

    """

    # Handling of NaN and infinity

    funcs = ["isreal", "isfloat"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", [float("nan"), float("inf"), float("-inf")])
    def test_returns_true_for_nan_and_inf(
        self, func: Union[IsReal, IsFloat], x: float
    ) -> None:
        assert func(x)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", all_nan + [pad("nan"), pad("-NAN")])
    def test_returns_false_for_nan_string_unless_allow_nan_is_true(
        self, func: Union[IsReal, IsFloat], x: str
    ) -> None:
        assert not func(x)
        assert func(x, allow_nan=True)

    @parametrize("func", get_funcs(funcs), ids=funcs)
    @parametrize("x", most_inf + neg_inf + [pad("-inf"), pad("+INFINITY")])
    def test_returns_false_for_inf_string_unless_allow_infinity_is_true(
        self, func: Union[IsReal, IsFloat], x: str
    ) -> None:
        assert not func(x)
        assert func(x, allow_inf=True)

    # Handling of numeric objects as input

    funcs = ["isreal", "isint", "isintlike"]

    @given(integers())
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_true_if_given_int(
        self, func: Union[IsReal, IsInt, IsIntLike], x: int
    ) -> None:
        assert func(x)
        assert func(x, num_only=True)

    funcs = ["isreal", "isfloat"]

    @given(floats())
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_true_if_given_float(
        self, func: Union[IsReal, IsFloat], x: float
    ) -> None:
        assert func(x)
        assert func(x, num_only=True)

    funcs = identification_funcs

    @given(integers() | floats())
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_if_given_number_and_str_only_is_true(
        self, func: IdentificationFuncs, x: FloatOrInt
    ) -> None:
        assert not func(x, str_only=True)

    # Handling of strings containing numbers as input

    funcs = ["isreal", "isfloat"]

    @given(floats(allow_nan=False, allow_infinity=False).map(repr))
    @example("10." + "0" * 1050)
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_true_if_given_float_string(
        self, func: Union[IsReal, IsFloat], x: str
    ) -> None:
        assert func(x)
        assert func(pad(x))  # Accepts padding

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

    @given((integers() | floats(allow_nan=False, allow_infinity=False)).map(repr))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_if_given_string_and_num_only_is_true(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x, num_only=True)

    @given(sampled_from(digits))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_digit_returns_true(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert func(x)
        assert func(pad(x))  # Accepts padding

    funcs = ["isreal", "isfloat"]

    @given(sampled_from(numeric_not_digit_not_int))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_numeral_returns_true(
        self, func: Union[IsReal, IsFloat], x: str
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
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_if_given_non_number_string(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x)

    @given(sampled_from(not_numeric))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_non_numeral_returns_false(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x)

    @given(text(min_size=2).filter(not_a_number))
    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_given_unicode_of_more_than_one_char_returns_false(
        self, func: IdentificationFuncs, x: str
    ) -> None:
        assert not func(x)

    funcs = ["isint", "isintlike"]

    @parametrize("func", get_funcs(funcs), ids=funcs)
    def test_returns_false_for_nan_or_inf_string(
        self, func: Union[IsInt, IsIntLike]
    ) -> None:
        assert not func("nan")
        assert not func("inf")


class TestIsFloat:
    """Tests for the isfloat function that are too specific for the generalized tests."""

    @given(integers())
    def test_returns_false_if_given_int(self, x: int) -> None:
        assert not fastnumbers.isfloat(x)


class TestIsInt:
    """Tests for the isint function that are too specific for the generalized tests."""

    @given(floats())
    def test_returns_false_if_given_float(self, x: str) -> None:
        assert not fastnumbers.isint(x)

    @given(integers())
    def test_returns_true_if_given_int_string_with_non_base_10(self, x: int) -> None:
        for base in range(2, 36 + 1):
            # Avoid recursion error because of overly simple baseN function.
            # Avoid cases where number ends up creating infinity
            actual = base_n(x, base)
            if len(repr(x)) < 30 and isinstance(actual, int):
                assert fastnumbers.isint(base_n(x, base), base=base)
        assert fastnumbers.isint(bin(x), base=2)
        assert fastnumbers.isint(bin(x), base=0)
        assert fastnumbers.isint(oct(x), base=8)
        assert fastnumbers.isint(oct(x), base=0)
        assert fastnumbers.isint(oct(x).replace("0o", "0"), base=8)
        if x != 0:
            assert not fastnumbers.isint(oct(x).replace("0o", "0"), base=0)
        assert fastnumbers.isint(hex(x), base=16)
        assert fastnumbers.isint(hex(x), base=0)
        # Force unicode path
        assert fastnumbers.isint(hex(x).replace("0", "\uFF10"), base=0)

    def test_underscores(self) -> None:
        assert fastnumbers.isint("0_0_0")
        assert fastnumbers.isint("0_0_0", base=0)
        assert fastnumbers.isint("4_2")
        assert fastnumbers.isint("4_2", base=0)
        assert fastnumbers.isint("1_0000_0000")
        assert fastnumbers.isint("1_0000_0000", base=0)
        assert fastnumbers.isint("0b1001_0100", base=0)
        assert fastnumbers.isint("0xffff_ffff", base=0)
        assert fastnumbers.isint("0o5_7_7", base=0)
        assert fastnumbers.isint("0b_0", base=0)
        assert fastnumbers.isint("0x_f", base=0)
        assert fastnumbers.isint("0o_5", base=0)

        # Underscores in the base selector:
        assert not fastnumbers.isint("0_b0")
        assert not fastnumbers.isint("0_b0", base=0)
        assert not fastnumbers.isint("0_xf")
        assert not fastnumbers.isint("0_xf", base=0)
        assert not fastnumbers.isint("0_o5")
        assert not fastnumbers.isint("0_o5", base=0)

        # Old-style octal, still disallowed if base guess is needed:
        assert not fastnumbers.isint("0_7", base=0)
        assert not fastnumbers.isint("09_99", base=0)

        # Two underscores:
        assert not fastnumbers.isint("0b1001__0100", base=0)
        assert not fastnumbers.isint("0xffff__ffff", base=0)

    @given(floats(allow_nan=False, allow_infinity=False).map(repr))
    def test_returns_false_if_given_float_string(self, x: str) -> None:
        assert not fastnumbers.isint(x)
        assert not fastnumbers.isint(pad(x))
        for base in range(2, 36 + 1):
            if len(x) < 30:
                assert not fastnumbers.isint(x, base=base)

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_false(self, x: str) -> None:
        assert not fastnumbers.isint(x)


class TestIsIntLike:
    """
    Tests for the isintlike function that are too specific for the generalized tests.
    """

    @given(floats().filter(not_an_integer))
    def test_returns_false_if_given_non_integer_float(self, x: float) -> None:
        assert not fastnumbers.isintlike(x)

    @given(floats().filter(an_integer))
    def test_returns_true_if_given_integer_float(self, x: float) -> None:
        assert fastnumbers.isintlike(x)

    @given(
        floats(allow_nan=False, allow_infinity=False).filter(not_an_integer).map(repr)
    )
    def test_returns_false_if_given_non_integer_float_string(self, x: str) -> None:
        assert not fastnumbers.isintlike(x)
        assert not fastnumbers.isintlike(pad(x))  # Accepts padding

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_non_digit_numeral_returns_false(self, x: str) -> None:
        assert not fastnumbers.isintlike(x)

    @given(
        sampled_from(numeric_not_digit).filter(
            lambda x: an_integer(unicodedata.numeric(x))
        )
    )
    def test_given_unicode_digit_numeral_returns_true(self, x: str) -> None:
        assert fastnumbers.isintlike(x)
        assert fastnumbers.isintlike(pad(x))  # Accepts padding


class TestQueryType:
    """Tests for the query_type function."""

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

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_str_or_none_if_str_not_allowed(
        self, x: str
    ) -> None:
        assert fastnumbers.query_type(x) is str
        assert fastnumbers.query_type(x, allowed_types=(int, float)) is None

    @given(text(min_size=2).filter(not_a_number))
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

    @parametrize("x", all_nan + [pad("+nan"), pad("-NAN")])
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
        assert fastnumbers.query_type(x, coerce=True) == int

    @given(floats(allow_nan=False))
    def test_given_float_returns_float_or_int_with_coerce(self, x: float) -> None:
        assert (
            fastnumbers.query_type(x, coerce=True) == int if x.is_integer() else float
        )

    @given(
        lists(floats())
        | tuples(floats())
        | dictionaries(floats(), floats())
        | sets(floats())
        | iterables(floats())
    )
    def test_containers_returns_container_type(
        self, x: Union[Dict[float, float], Iterable[float]]
    ) -> None:
        assert fastnumbers.query_type(x) is type(x)
        assert fastnumbers.query_type(x, allowed_types=(float, int, str)) is None
