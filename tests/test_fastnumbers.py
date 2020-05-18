# -*- coding: utf-8 -*-
# Find the build location and add that to the path
import math
import random
import re
import sys
import unicodedata
from itertools import repeat
from platform import python_version_tuple

from hypothesis import example, given
from hypothesis.strategies import binary, floats, integers, lists, sampled_from, text
from pytest import mark, raises

import fastnumbers

skipif = mark.skipif
parametrize = mark.parametrize

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


def a_number(s):
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


def space():
    """90% chance of ' ', 10% of unicode-only space."""
    return random.choice([" "] * 90 + [u"\u2007"] * 10)


def not_a_number(s):
    return not a_number(s)


def an_integer(x):
    return x.is_integer()


def not_an_integer(x):
    return not x.is_integer()


def base_n(num, b, numerals="0123456789abcdefghijklmnopqrstuvwxyz"):
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
    def __float__(self):
        raise ValueError("something here might go wrong")


class DumbIntClass(object):
    def __int__(self):
        raise ValueError("something here might go wrong")


def test_version():
    assert hasattr(fastnumbers, "__version__")


#################
# Sanity Checks #
#################


non_builtin_func_ids = [
    "fast_real",
    "fast_float",
    "fast_int",
    "fast_forceint",
    "isreal",
    "isfloat",
    "isint",
    "isintlike",
]
non_builtin_funcs = [getattr(fastnumbers, x) for x in non_builtin_func_ids]
conversion_func_ids = non_builtin_func_ids[:4]
conversion_funcs = non_builtin_funcs[:4]
identification_func_ids = non_builtin_func_ids[4:]
identification_funcs = non_builtin_funcs[4:]


@parametrize(
    "func", non_builtin_funcs + [fastnumbers.real], ids=non_builtin_func_ids + ["real"]
)
def test_invalid_argument_raises_type_error(func):
    with raises(TypeError):
        func(5, invalid="dummy")


@parametrize("func", non_builtin_funcs, ids=non_builtin_func_ids)
def test_no_arguments_raises_type_error(func):
    with raises(TypeError):
        func()


@parametrize("func", conversion_funcs, ids=conversion_func_ids)
def test_key_backwards_compatibility(func):
    with raises(ValueError, match=r"^Cannot set both on_fail and key$"):
        func("dummy", key=len, on_fail=len)
    assert func("dummy", key=len) == 5
    assert func("dummy", key=len) == func("dummy", on_fail=len)


def test_real_no_arguments_returns_0():
    assert fastnumbers.real() == 0


@given(floats(allow_nan=False) | integers())
def test_real_returns_same_as_fast_real(x):
    assert fastnumbers.real(x) == fastnumbers.fast_real(x)


@skipif(
    sys.version_info < (3, 6), reason="Underscore handling introduced in Python 3.6"
)
@parametrize("func", conversion_funcs, ids=conversion_func_ids)
def test_numbers_with_underscores_converted_according_to_allow_underscores_option(func):
    x = "1_234_567"
    assert func(x) in (float(x), int(x))
    assert func(x, allow_underscores=True) in (float(x), int(x))
    assert func(x, allow_underscores=False) == x


@skipif(
    sys.version_info < (3, 6), reason="Underscore handling introduced in Python 3.6"
)
@parametrize("func", identification_funcs, ids=identification_func_ids)
def test_numbers_with_underscores_identified_according_to_allow_underscores_option(
    func,
):
    x = "1_234_567"
    assert func(x)
    assert func(x, allow_underscores=True)
    assert not func(x, allow_underscores=False)


@skipif(
    sys.version_info < (3, 6), reason="Underscore handling introduced in Python 3.6"
)
def test_type_with_underscores_identified_according_to_allow_underscores_option():
    x = "1_234_567"
    assert fastnumbers.query_type(x) is int
    assert fastnumbers.query_type(x, allow_underscores=True) is int
    assert fastnumbers.query_type(x, allow_underscores=False) is str


class TestFastReal:
    """Tests for the fast_real function."""

    def test_with_coerce_given_dumb_class_responds_to_internal_valueerror(self):
        x = DumbFloatClass()
        assert fastnumbers.fast_real(x, coerce=True) is x
        with raises(ValueError):
            fastnumbers.fast_real(x, coerce=True, raise_on_invalid=True)
        assert fastnumbers.fast_real(x, coerce=True, default=5) == 5

    @given(floats(allow_nan=False))
    def test_given_float_returns_float(self, x):
        assert fastnumbers.fast_real(x, coerce=False) == x
        assert fastnumbers.fast_real(x, raise_on_invalid=True, coerce=False) == x
        assert isinstance(fastnumbers.fast_real(x, coerce=False), float)

    @given(floats(allow_nan=False).filter(an_integer))
    def test_given_float_returns_int_if_intlike_with_coerce(self, x):
        assert fastnumbers.fast_real(x, coerce=True) == int(float(x))
        assert isinstance(fastnumbers.fast_real(x, coerce=True), int)

    @given(floats(allow_nan=False))
    def test_given_float_returns_float_or_int_with_coerce(self, x):
        assert fastnumbers.fast_real(x, coerce=True) == int(x) if x.is_integer() else x
        assert isinstance(
            fastnumbers.fast_real(x, coerce=True), int if x.is_integer() else float
        )

    def test_given_nan_returns_nan(self):
        assert math.isnan(fastnumbers.fast_real(float("nan")))

    def test_given_nan_returns_sub_value(self):
        assert fastnumbers.fast_real(float("nan"), nan=0) == 0

    def test_given_inf_returns_inf(self):
        assert math.isinf(fastnumbers.fast_real(float("inf")))

    def test_given_inf_returns_sub_value(self):
        assert fastnumbers.fast_real(float("inf"), inf=1000.0) == 1000.0

    @given(floats(allow_nan=False))
    @example(5.675088586167575e-116)
    def test_given_float_string_returns_float(self, x):
        y = repr(x)
        assert fastnumbers.fast_real(y, coerce=False) == x
        assert fastnumbers.fast_real(y, raise_on_invalid=True, coerce=False) == x
        assert isinstance(fastnumbers.fast_real(y, coerce=False), float)

    @given(integers())
    def test_given_float_string_returns_int_with_coerce_with_intlike(self, x):
        y = repr(float(x))
        assert fastnumbers.fast_real(y, coerce=True) == int(float(x))
        assert isinstance(fastnumbers.fast_real(y, coerce=True), int)

    def test_given_nan_string_returns_nan(self):
        assert math.isnan(fastnumbers.fast_real("nan"))
        assert math.isnan(fastnumbers.fast_real("NaN"))

    def test_with_nan_given_nan_string_returns_sub_value(self):
        assert fastnumbers.fast_real("nan", nan=0) == 0

    def test_given_inf_string_returns_inf(self):
        assert fastnumbers.fast_real("inf") == float("inf")
        assert fastnumbers.fast_real("-INF") == float("-inf")
        assert fastnumbers.fast_real("infinity") == float("inf")
        assert fastnumbers.fast_real("-infINIty") == float("-inf")

    def test_given_very_large_float_returns_inf_with_coerce_on_or_off(self):
        assert fastnumbers.fast_real("3e106196") == float("inf")
        assert fastnumbers.fast_real("3e106196", coerce=False) == float("inf")
        assert fastnumbers.fast_real("3e106196", raise_on_invalid=True) == float("inf")

    def test_with_inf_given_inf_string_returns_sub_value(self):
        assert fastnumbers.fast_real("inf", inf=10000.0) == 10000.0

    @given(floats(allow_nan=False), integers(0, 100), integers(0, 100))
    def test_given_padded_float_strings_returns_float(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.fast_real(y, coerce=False) == x
        assert isinstance(fastnumbers.fast_real(y, coerce=False), float)

    def test_given_padded_nan_returns_nan(self):
        assert math.isnan(fastnumbers.fast_real("     nan "))

    def test_given_padded_inf_string_returns_inf(self):
        assert fastnumbers.fast_real("   inf        ") == float("inf")
        assert fastnumbers.fast_real("  -infINIty  ") == float("-inf")

    @given(integers())
    @example(int(10 * 300))
    def test_given_int_returns_int(self, x):
        assert fastnumbers.fast_real(x) == x
        assert isinstance(fastnumbers.fast_real(x), int)

    @given(integers())
    @example(int(10 * 300))
    def test_given_int_returns_int_with_coerce(self, x):
        assert fastnumbers.fast_real(x, coerce=True) == x
        assert isinstance(fastnumbers.fast_real(x, coerce=True), int)

    @given(integers())
    @example(40992764608243448035)
    @example(-41538374848935286698640072416676709)
    @example(240278958776173358420034462324117625982)
    @example(1609422692302207451978552816956662956486)
    @example(-121799354242674784350540853922878239740762834)
    @example(32718704454132572934419741118153895444518280065843028297496525078)
    @example(33684944745210074227862907273261282807602986571245071790093633147269)
    def test_given_int_string_returns_int(self, x):
        y = repr(x)
        assert fastnumbers.fast_real(y) == x
        assert isinstance(fastnumbers.fast_real(y), int)

    @given(integers(), integers(0, 100), integers(0, 100))
    @example(40992764608243448035, 1, 1)
    @example(-41538374848935286698640072416676709, 1, 1)
    @example(240278958776173358420034462324117625982, 1, 1)
    @example(1609422692302207451978552816956662956486, 1, 1)
    @example(-121799354242674784350540853922878239740762834, 1, 1)
    @example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
    @example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
    def test_given_padded_int_string_returns_int(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.fast_real(y) == x
        assert isinstance(fastnumbers.fast_real(y), int)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_int(self, x):
        assert fastnumbers.fast_real(x) == unicodedata.digit(x)
        assert isinstance(fastnumbers.fast_real(x), int)
        # Try padded as well
        assert fastnumbers.fast_real(u"   " + x + u"   ") == unicodedata.digit(x)

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_float(self, x):
        assert fastnumbers.fast_real(x) == unicodedata.numeric(x)
        assert isinstance(fastnumbers.fast_real(x), float)
        # Try padded as well
        assert fastnumbers.fast_real(u"   " + x + u"   ") == unicodedata.numeric(x)

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_as_is(self, x):
        assert fastnumbers.fast_real(x) == x

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_as_is(self, x):
        assert fastnumbers.fast_real(x) == x

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    def test_given_invalid_string_returns_string_as_is(self, x):
        assert fastnumbers.fast_real(x) is x

    @given((text() | binary()).filter(not_a_number))
    def test_given_invalid_string_raises_valueerror_if_raise_on_invalid_is_true(
        self, x
    ):
        with raises(ValueError):
            fastnumbers.fast_real(x, raise_on_invalid=True)

    @given(lists(integers()))
    def test_given_invalid_type_raises_typeerror(self, x):
        with raises(TypeError):
            fastnumbers.fast_real(x)

    @given((text() | binary()).filter(not_a_number))
    def test_returns_default_value_if_given_invalid_string(self, x):
        assert fastnumbers.fast_real(x, default=90) == 90
        assert fastnumbers.fast_real(x, 90) == 90

    @given((text() | binary()).filter(not_a_number))
    def test_raises_valueerror_if_raise_on_invalid_is_true_with_default(self, x):
        with raises(ValueError):
            fastnumbers.fast_real(x, default=90, raise_on_invalid=True)
            fastnumbers.fast_real(x, 90, True)

    @given(integers() | floats(allow_nan=False))
    def test_returns_input_as_is_if_valid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_real(x, on_fail=len) == x
        assert fastnumbers.fast_real(repr(x), on_fail=len) == x

    @given((text() | binary()).filter(not_a_number))
    def test_returns_transformed_input_if_invalid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_real(x, on_fail=len) == len(x)


class TestFastFloat:
    """Tests for the fast_float function."""

    def test_given_dumb_class_responds_to_internal_valueerror(self):
        x = DumbFloatClass()
        assert fastnumbers.fast_float(x) is x
        with raises(ValueError):
            fastnumbers.fast_float(x, raise_on_invalid=True)
        assert fastnumbers.fast_float(x, default=5) == 5

    @given(floats(allow_nan=False))
    def test_given_float_returns_float(self, x):
        assert fastnumbers.fast_float(x) == x
        assert fastnumbers.fast_float(x, raise_on_invalid=True) == x
        assert isinstance(fastnumbers.fast_float(x), float)

    def test_given_nan_returns_nan(self):
        assert math.isnan(fastnumbers.fast_float(float("nan")))

    def test_given_nan_returns_sub_value(self):
        assert fastnumbers.fast_float(float("nan"), nan=0) == 0

    def test_given_inf_returns_inf(self):
        assert math.isinf(fastnumbers.fast_float(float("inf")))

    def test_given_inf_returns_sub_value(self):
        assert fastnumbers.fast_float(float("inf"), inf=1000.0) == 1000.0

    def test_with_range_of_exponents_correctly_parses(self):
        for x in range(-300, 300):
            val = "1.0E{0:d}".format(x)
            assert fastnumbers.fast_float(val) == float(val)
        for x in range(-300, 300):
            val = "1.0000000000E{0:d}".format(x)
            assert fastnumbers.fast_float(val) == float(val)

    @given(floats(allow_nan=False).filter(not_an_integer))
    @example(5.675088586167575e-116)
    @example("10." + "0" * 1050)
    def test_given_float_string_returns_float(self, x):
        if isinstance(x, str):
            y = x
            x = float(x)
        else:
            y = repr(x)
        assert fastnumbers.fast_float(y) == x
        assert fastnumbers.fast_float(y, raise_on_invalid=True) == x
        assert isinstance(fastnumbers.fast_float(y), float)

    def test_given_nan_string_returns_nan(self):
        assert math.isnan(fastnumbers.fast_float("nan"))
        assert math.isnan(fastnumbers.fast_float("NaN"))

    def test_given_inf_string_returns_inf(self):
        assert fastnumbers.fast_float("inf") == float("inf")
        assert fastnumbers.fast_float("-INF") == float("-inf")
        assert fastnumbers.fast_float("infinity") == float("inf")
        assert fastnumbers.fast_float("-infINIty") == float("-inf")

    @given(
        floats(allow_nan=False).filter(not_an_integer),
        integers(0, 100),
        integers(0, 100),
    )
    def test_given_padded_float_strings_returns_float(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.fast_float(y) == x
        assert isinstance(fastnumbers.fast_float(y), float)

    def test_given_padded_nan_returns_nan(self):
        assert math.isnan(fastnumbers.fast_float("     nan "))
        assert math.isnan(fastnumbers.fast_float(u"     nan\u2007"))

    def test_given_padded_inf_string_returns_inf(self):
        assert fastnumbers.fast_float("   inf        ") == float("inf")
        assert fastnumbers.fast_float("  -infINIty  ") == float("-inf")
        assert fastnumbers.fast_float(u" \u2007 inf        ") == float("inf")

    @given(integers())
    def test_given_int_returns_float(self, x):
        assert fastnumbers.fast_float(x) == float(x)
        assert isinstance(fastnumbers.fast_float(x), float)

    @given(integers())
    @example(40992764608243448035)
    @example(-41538374848935286698640072416676709)
    @example(240278958776173358420034462324117625982)
    @example(1609422692302207451978552816956662956486)
    @example(-121799354242674784350540853922878239740762834)
    @example(32718704454132572934419741118153895444518280065843028297496525078)
    @example(33684944745210074227862907273261282807602986571245071790093633147269)
    def test_given_int_string_returns_float(self, x):
        y = repr(x)
        assert fastnumbers.fast_float(y) == float(x)
        assert isinstance(fastnumbers.fast_float(y), float)

    @given(integers(), integers(0, 100), integers(0, 100))
    @example(40992764608243448035, 1, 1)
    @example(-41538374848935286698640072416676709, 1, 1)
    @example(240278958776173358420034462324117625982, 1, 1)
    @example(1609422692302207451978552816956662956486, 1, 1)
    @example(-121799354242674784350540853922878239740762834, 1, 1)
    @example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
    @example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
    def test_given_padded_int_string_returns_float(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.fast_float(y) == float(x)
        assert isinstance(fastnumbers.fast_float(y), float)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_float(self, x):
        assert fastnumbers.fast_float(x) == unicodedata.numeric(x)
        assert isinstance(fastnumbers.fast_float(x), float)
        # Try padded as well
        assert fastnumbers.fast_float(u"   " + x + u"   ") == unicodedata.numeric(x)

    @given(sampled_from(numeric))
    def test_given_unicode_numeral_returns_float(self, x):
        assert fastnumbers.fast_float(x) == unicodedata.numeric(x)
        assert isinstance(fastnumbers.fast_float(x), float)
        # Try padded as well
        assert fastnumbers.fast_float(u"   " + x + u"   ") == unicodedata.numeric(x)

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_as_is(self, x):
        assert fastnumbers.fast_float(x) == x

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_as_is(self, x):
        assert fastnumbers.fast_float(x) == x

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    def test_given_invalid_string_returns_string_as_is(self, x):
        assert fastnumbers.fast_float(x) is x

    @given((text() | binary()).filter(not_a_number))
    def test_given_invalid_string_raises_valueerror_if_raise_on_invalid_is_true(
        self, x
    ):
        with raises(ValueError):
            fastnumbers.fast_float(x, raise_on_invalid=True)

    @given(lists(integers()))
    def test_given_invalid_type_raises_typeerror(self, x):
        with raises(TypeError):
            fastnumbers.fast_float(x)

    @given((text() | binary()).filter(not_a_number))
    def test_returns_default_value_if_given_invalid_string(self, x):
        assert fastnumbers.fast_float(x, default=90.0) == 90.0
        assert fastnumbers.fast_float(x, 90.0) == 90.0

    @given((text() | binary()).filter(not_a_number))
    def test_raises_valueerror_if_raise_on_invalid_is_true_with_default(self, x):
        with raises(ValueError):
            assert fastnumbers.fast_float(x, default=90.0, raise_on_invalid=True)
            assert fastnumbers.fast_float(x, 90.0, True)

    @given(floats(allow_nan=False))
    def test_returns_input_as_is_if_valid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_float(x, on_fail=len) == x
        assert fastnumbers.fast_float(repr(x), on_fail=len) == x

    @given((text() | binary()).filter(not_a_number))
    def test_returns_transformed_input_if_invalid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_float(x, on_fail=len) == len(x)


class TestFastInt:
    """Tests for the fast_int function."""

    def test_given_dumb_class_responds_to_internal_valueerror(self):
        x = DumbIntClass()
        assert fastnumbers.fast_int(x) is x
        with raises(ValueError):
            fastnumbers.fast_int(x, raise_on_invalid=True)
        assert fastnumbers.fast_int(x, default=5) == 5

    def test_given_invalid_base_errors_with_valueerror(self):
        with raises(ValueError):
            fastnumbers.fast_int("10", base=-1)
        with raises(ValueError):
            fastnumbers.fast_int("10", base=1)
        with raises(ValueError):
            fastnumbers.fast_int("10", base=37)

    @given(floats(allow_nan=False, allow_infinity=False))
    def test_given_float_returns_int(self, x):
        assert fastnumbers.fast_int(x) == int(x)
        assert fastnumbers.fast_int(x, raise_on_invalid=True) == int(x)
        assert isinstance(fastnumbers.fast_int(x), int)

    def test_given_nan_raises_valueerror_or_returns_as_is_or_returns_default(self):
        with raises(ValueError):
            fastnumbers.fast_int(float("nan"), raise_on_invalid=True)
        assert math.isnan(fastnumbers.fast_int(float("nan")))
        assert fastnumbers.fast_int(float("nan"), "Sample") == "Sample"

    def test_given_inf_raises_overflowerror(self):
        with raises(OverflowError):
            fastnumbers.fast_int(float("inf"), raise_on_invalid=True)
        assert math.isinf(fastnumbers.fast_int(float("inf")))
        assert fastnumbers.fast_int(float("inf"), "Sample") == "Sample"

    @given(floats(allow_nan=False).filter(not_an_integer))
    def test_given_float_string_returns_string_as_is(self, x):
        y = repr(x)
        assert fastnumbers.fast_int(y) is y

    @given(floats(allow_nan=False).filter(not_an_integer))
    def test_given_float_intlike_string_returns_string_as_is(self, x):
        y = repr(x)
        assert fastnumbers.fast_int(y) is y

    @given(floats().filter(not_an_integer))
    @example(float("nan"))
    @example(float("inf"))
    def test_given_float_string_raises_valueerror_if_raise_on_invalid_is_true(self, x):
        y = repr(x)
        with raises(ValueError):
            fastnumbers.fast_int(y, raise_on_invalid=True)

    @given(integers())
    @example(int(10 * 300))
    def test_given_int_returns_int(self, x):
        assert fastnumbers.fast_int(x) == x
        assert isinstance(fastnumbers.fast_int(x), int)

    @given(integers())
    @example(40992764608243448035)
    @example(-41538374848935286698640072416676709)
    @example(240278958776173358420034462324117625982)
    @example(1609422692302207451978552816956662956486)
    @example(-121799354242674784350540853922878239740762834)
    @example(32718704454132572934419741118153895444518280065843028297496525078)
    @example(33684944745210074227862907273261282807602986571245071790093633147269)
    @example(int("1" + "0" * 1050))
    def test_given_int_string_returns_int(self, x):
        y = repr(x)
        assert fastnumbers.fast_int(y) == x
        assert isinstance(fastnumbers.fast_int(y), int)
        for base in range(2, 36 + 1):
            if (
                len(y) < 30
            ):  # Avoid recursion error because of overly simple baseN function.
                assert fastnumbers.fast_int(base_n(x, base), base=base) == x
        assert fastnumbers.fast_int(bin(x), base=2) == x
        assert fastnumbers.fast_int(bin(x), base=0) == x
        assert fastnumbers.fast_int(oct(x), base=8) == x
        assert fastnumbers.fast_int(oct(x), base=0) == x
        assert fastnumbers.fast_int(hex(x), base=16) == x
        assert fastnumbers.fast_int(hex(x), base=0) == x
        assert (
            fastnumbers.fast_int(hex(x).replace("0", u"\uFF10"), base=0) == x
        )  # To force unicode path

    def test_given_multiple_zeros_with_base_returns_zero(self):
        assert fastnumbers.fast_int("0" * 4, base=0) == 0
        assert fastnumbers.fast_int("0" * 4, base=2) == 0
        assert fastnumbers.fast_int("0" * 4, base=8) == 0
        assert fastnumbers.fast_int("0" * 4, base=16) == 0
        assert fastnumbers.fast_int(u"\uFF10" * 4, base=0) == 0
        assert fastnumbers.fast_int(u"\uFF10" * 4, base=2) == 0
        assert fastnumbers.fast_int(u"\uFF10" * 4, base=8) == 0
        assert fastnumbers.fast_int(u"\uFF10" * 4, base=16) == 0

    @given(integers(), integers(0, 100), integers(0, 100))
    @example(40992764608243448035, 1, 1)
    @example(-41538374848935286698640072416676709, 1, 1)
    @example(240278958776173358420034462324117625982, 1, 1)
    @example(1609422692302207451978552816956662956486, 1, 1)
    @example(-121799354242674784350540853922878239740762834, 1, 1)
    @example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
    @example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
    def test_given_padded_int_string_returns_int(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.fast_int(y) == x
        assert isinstance(fastnumbers.fast_int(y), int)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_int(self, x):
        assert fastnumbers.fast_int(x) == unicodedata.digit(x)
        assert isinstance(fastnumbers.fast_int(x), int)
        # Try padded as well
        assert fastnumbers.fast_int(u"   " + x + u"   ") == unicodedata.digit(x)

    @given(sampled_from(numeric_not_digit))
    def test_given_unicode_numeral_returns_as_is(self, x):
        assert fastnumbers.fast_int(x) == x

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_as_is(self, x):
        assert fastnumbers.fast_int(x) == x

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_as_is(self, x):
        assert fastnumbers.fast_int(x) == x

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    def test_given_invalid_string_returns_string_as_is(self, x):
        assert fastnumbers.fast_int(x) is x
        assert fastnumbers.fast_int(x, base=10) is x

    @given((text() | binary()).filter(not_a_number))
    def test_given_invalid_string_raises_valueerror_if_raise_on_invalid_is_true(
        self, x
    ):
        with raises(ValueError):
            fastnumbers.fast_int(x, raise_on_invalid=True)
            fastnumbers.fast_int(x, None, True)

    @given(lists(integers()))
    def test_given_invalid_type_raises_typeerror(self, x):
        with raises(TypeError):
            fastnumbers.fast_int(x)

    @given((text() | binary()).filter(not_a_number))
    def test_returns_default_value_if_given_invalid_string(self, x):
        assert fastnumbers.fast_int(x, default=90) == 90
        assert fastnumbers.fast_int(x, 90) == 90

    @given((text() | binary()).filter(not_a_number))
    def test_raises_valueerror_if_raise_on_invalid_is_true_with_default(self, x):
        with raises(ValueError):
            assert fastnumbers.fast_int(x, default=90, raise_on_invalid=True)

    @given(integers())
    def test_returns_input_as_is_if_valid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_int(x, on_fail=len) == x
        assert fastnumbers.fast_int(repr(x), on_fail=len) == x

    @given((text() | binary()).filter(not_a_number))
    def test_returns_transformed_input_if_invalid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_int(x, on_fail=len) == len(x)


class TestFastForceInt:
    """Tests for the fast_forceint function."""

    def test_given_dumb_class_responds_to_internal_valueerror(self):
        x = DumbIntClass()
        assert fastnumbers.fast_forceint(x) is x
        with raises(ValueError):
            fastnumbers.fast_forceint(x, raise_on_invalid=True)
        assert fastnumbers.fast_forceint(x, default=5) == 5

    @given(floats(allow_nan=False, allow_infinity=False))
    def test_given_float_returns_int(self, x):
        assert fastnumbers.fast_forceint(x) == int(x)
        assert fastnumbers.fast_forceint(x, raise_on_invalid=True) == int(x)
        assert isinstance(fastnumbers.fast_forceint(x), int)

    def test_given_nan_raises_valueerror_or_returns_as_is_or_returns_default(self):
        with raises(ValueError):
            fastnumbers.fast_forceint(float("nan"), raise_on_invalid=True)
        assert math.isnan(fastnumbers.fast_forceint(float("nan")))
        assert fastnumbers.fast_forceint(float("nan"), "Sample") == "Sample"

    def test_given_inf_raises_overflowerror(self):
        with raises(OverflowError):
            fastnumbers.fast_forceint(float("inf"), raise_on_invalid=True)
        assert math.isinf(fastnumbers.fast_forceint(float("inf")))
        assert fastnumbers.fast_forceint(float("inf"), "Sample") == "Sample"

    @given(floats(allow_nan=False, allow_infinity=False))
    def test_given_float_string_returns_int(self, x):
        y = repr(x)
        assert fastnumbers.fast_forceint(y) == int(x)
        assert fastnumbers.fast_forceint(y, raise_on_invalid=True) == int(x)
        assert isinstance(fastnumbers.fast_forceint(y), int)

    def test_given_nan_string_raises_valueerror_with_raise_on_invalid_as_true(self):
        with raises(ValueError):
            fastnumbers.fast_forceint("nan", raise_on_invalid=True)

    def test_given_inf_string_raises_overflowerror_with_raise_on_invalid_as_true(self):
        with raises(OverflowError):
            fastnumbers.fast_forceint("inf", raise_on_invalid=True)
            fastnumbers.fast_forceint("-infinity", raise_on_invalid=True)

    @given(
        floats(allow_nan=False, allow_infinity=False),
        integers(0, 100),
        integers(0, 100),
    )
    def test_given_padded_float_strings_returns_int(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.fast_forceint(y) == int(x)
        assert isinstance(fastnumbers.fast_forceint(y), int)

    @given(integers())
    @example(int(10 * 300))
    def test_given_int_returns_int(self, x):
        assert fastnumbers.fast_forceint(x) == x
        assert isinstance(fastnumbers.fast_forceint(x), int)

    @given(integers())
    @example(40992764608243448035)
    @example(-41538374848935286698640072416676709)
    @example(240278958776173358420034462324117625982)
    @example(1609422692302207451978552816956662956486)
    @example(-121799354242674784350540853922878239740762834)
    @example(32718704454132572934419741118153895444518280065843028297496525078)
    @example(33684944745210074227862907273261282807602986571245071790093633147269)
    def test_given_int_string_returns_int(self, x):
        y = repr(x)
        assert fastnumbers.fast_forceint(y) == x
        assert isinstance(fastnumbers.fast_forceint(y), int)

    @given(integers(), integers(0, 100), integers(0, 100))
    @example(40992764608243448035, 1, 1)
    @example(-41538374848935286698640072416676709, 1, 1)
    @example(240278958776173358420034462324117625982, 1, 1)
    @example(1609422692302207451978552816956662956486, 1, 1)
    @example(-121799354242674784350540853922878239740762834, 1, 1)
    @example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
    @example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
    def test_given_padded_int_string_returns_int(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.fast_forceint(y) == x
        assert isinstance(fastnumbers.fast_forceint(y), int)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_int(self, x):
        assert fastnumbers.fast_forceint(x) == unicodedata.digit(x)
        assert isinstance(fastnumbers.fast_forceint(x), int)
        # Try padded as well
        assert fastnumbers.fast_forceint(u"   " + x + u"   ") == unicodedata.digit(x)

    @given(sampled_from(numeric))
    def test_given_unicode_numeral_returns_int(self, x):
        assert fastnumbers.fast_forceint(x) == int(unicodedata.numeric(x))
        assert isinstance(fastnumbers.fast_forceint(x), int)
        # Try padded as well
        assert fastnumbers.fast_forceint(u"   " + x + u"   ") == int(
            unicodedata.numeric(x)
        )

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_as_is(self, x):
        assert fastnumbers.fast_forceint(x) == x

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_as_is(self, x):
        assert fastnumbers.fast_forceint(x) == x

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    def test_given_invalid_string_returns_string_as_is(self, x):
        assert fastnumbers.fast_forceint(x) is x

    @given((text() | binary()).filter(not_a_number))
    def test_given_invalid_string_raises_valueerror_if_raise_on_invalid_is_true(
        self, x
    ):
        with raises(ValueError):
            fastnumbers.fast_forceint(x, raise_on_invalid=True)

    @given(lists(integers()))
    def test_given_invalid_type_raises_typeerror(self, x):
        with raises(TypeError):
            fastnumbers.fast_forceint(x)

    @given((text() | binary()).filter(not_a_number))
    def test_returns_default_value_if_given_invalid_string(self, x):
        assert fastnumbers.fast_forceint(x, default=90.0) == 90.0
        assert fastnumbers.fast_forceint(x, 90.0) == 90.0

    @given((text() | binary()).filter(not_a_number))
    def test_raises_valueerror_if_raise_on_invalid_is_true_with_default(self, x):
        with raises(ValueError):
            assert fastnumbers.fast_forceint(x, default=90.0, raise_on_invalid=True)

    @given(integers() | floats(allow_nan=False, allow_infinity=False))
    def test_returns_input_as_is_if_valid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_forceint(x, on_fail=len) == int(x)
        assert fastnumbers.fast_forceint(repr(x), on_fail=len) == int(x)

    @given((text() | binary()).filter(not_a_number))
    def test_returns_transformed_input_if_invalid_and_on_fail_is_given(self, x):
        assert fastnumbers.fast_forceint(x, on_fail=len) == len(x)


class TestIsReal:
    """Tests for the isreal function."""

    @given(integers())
    def test_returns_true_if_given_int(self, x):
        assert fastnumbers.isreal(x)
        assert fastnumbers.isreal(x, num_only=True)

    @given(floats())
    def test_returns_true_if_given_float(self, x):
        assert fastnumbers.isreal(x)
        assert fastnumbers.isreal(x, num_only=True)

    @given(integers())
    def test_returns_false_if_given_int_and_str_only_is_true(self, x):
        assert not fastnumbers.isreal(x, str_only=True)

    @given(floats())
    def test_returns_false_if_given_float_and_str_only_is_true(self, x):
        assert not fastnumbers.isreal(x, str_only=True)

    @given(integers(), integers(0, 100), integers(0, 100))
    def test_returns_true_if_given_int_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.isreal(repr(x))
        assert fastnumbers.isreal(repr(x), str_only=True)
        assert fastnumbers.isreal(y)

    @given(
        floats(allow_nan=False, allow_infinity=False),
        integers(0, 100),
        integers(0, 100),
    )
    def test_returns_true_if_given_float_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.isreal(repr(x))
        assert fastnumbers.isreal(repr(x), str_only=True)
        assert fastnumbers.isreal(y)

    @given(integers())
    def test_returns_false_if_given_string_and_num_only_is_true(self, x):
        assert not fastnumbers.isreal(repr(x), num_only=True)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_true(self, x):
        assert fastnumbers.isreal(x)
        # Try padded as well
        assert fastnumbers.isreal(u"   " + x + u"   ")

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_true(self, x):
        assert fastnumbers.isreal(x)
        # Try padded as well
        assert fastnumbers.isreal(u"   " + x + u"   ")

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_false(self, x):
        assert not fastnumbers.isreal(x)

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_false(self, x):
        assert not fastnumbers.isreal(x)

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    def test_returns_false_if_given_non_number_string(self, x):
        assert not fastnumbers.isreal(x)

    def test_returns_false_for_nan_string_unless_allow_nan_is_true(self):
        assert not fastnumbers.isreal("nan")
        assert fastnumbers.isreal("nan", allow_nan=True)
        assert fastnumbers.isreal("-NaN", allow_nan=True)

    def test_returns_false_for_inf_string_unless_allow_infinity_is_true(self):
        assert not fastnumbers.isreal("inf")
        assert fastnumbers.isreal("inf", allow_inf=True)
        assert fastnumbers.isreal("-INFINITY", allow_inf=True)


class TestIsFloat:
    """Tests for the isfloat function."""

    @given(integers())
    def test_returns_false_if_given_int(self, x):
        assert not fastnumbers.isfloat(x)

    @given(floats())
    def test_returns_true_if_given_float(self, x):
        assert fastnumbers.isfloat(x)
        assert fastnumbers.isfloat(x, num_only=True)

    @given(floats())
    def test_returns_false_if_given_float_and_str_only_is_true(self, x):
        assert not fastnumbers.isfloat(x, str_only=True)

    @given(integers(), integers(0, 100), integers(0, 100))
    @example(40992764608243448035, 1, 1)
    @example(-41538374848935286698640072416676709, 1, 1)
    @example(240278958776173358420034462324117625982, 1, 1)
    @example(1609422692302207451978552816956662956486, 1, 1)
    @example(-121799354242674784350540853922878239740762834, 1, 1)
    @example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
    @example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
    def test_returns_true_if_given_int_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.isfloat(repr(x))
        assert fastnumbers.isfloat(repr(x), str_only=True)
        assert fastnumbers.isfloat(y)

    @given(
        floats(allow_nan=False, allow_infinity=False),
        integers(0, 100),
        integers(0, 100),
    )
    @example("10." + "0" * 1050, 1, 1)
    def test_returns_true_if_given_float_string_padded_or_not(self, x, y, z):
        if isinstance(x, str):
            y = x
            x = float(x)
        else:
            y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.isfloat(repr(x))
        assert fastnumbers.isfloat(repr(x), str_only=True)
        assert fastnumbers.isfloat(y)

    @given(floats(allow_nan=False, allow_infinity=False))
    def test_returns_false_if_given_string_and_num_only_is_true(self, x):
        assert not fastnumbers.isfloat(repr(x), num_only=True)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_true(self, x):
        assert fastnumbers.isfloat(x)
        # Try padded as well
        assert fastnumbers.isfloat(u"   " + x + u"   ")

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_true(self, x):
        assert fastnumbers.isfloat(x)
        # Try padded as well
        assert fastnumbers.isfloat(u"   " + x + u"   ")

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_false(self, x):
        assert not fastnumbers.isfloat(x)

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_false(self, x):
        assert not fastnumbers.isfloat(x)

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    def test_returns_false_if_given_non_number_string(self, x):
        assert not fastnumbers.isfloat(x)

    def test_returns_false_for_nan_string_unless_allow_nan_is_true(self):
        assert not fastnumbers.isfloat("nan")
        assert fastnumbers.isfloat("nan", allow_nan=True)
        assert fastnumbers.isfloat("-NaN", allow_nan=True)

    def test_returns_false_for_inf_string_unless_allow_infinity_is_true(self):
        assert not fastnumbers.isfloat("inf")
        assert fastnumbers.isfloat("inf", allow_inf=True)
        assert fastnumbers.isfloat("-INFINITY", allow_inf=True)


class TestIsInt:
    """Tests for the isint function."""

    @given(integers())
    def test_returns_true_if_given_int(self, x):
        assert fastnumbers.isint(x, num_only=True)

    @given(floats())
    def test_returns_false_if_given_float(self, x):
        assert not fastnumbers.isint(x)

    @given(integers())
    def test_returns_false_if_given_int_and_str_only_is_true(self, x):
        assert not fastnumbers.isint(x, str_only=True)

    @given(integers(), integers(0, 100), integers(0, 100))
    @example(40992764608243448035, 1, 1)
    @example(-41538374848935286698640072416676709, 1, 1)
    @example(240278958776173358420034462324117625982, 1, 1)
    @example(1609422692302207451978552816956662956486, 1, 1)
    @example(-121799354242674784350540853922878239740762834, 1, 1)
    @example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
    @example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
    @example(int("1" + "0" * 1050), 1, 1)
    def test_returns_true_if_given_int_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.isint(repr(x)) is True
        assert fastnumbers.isint(repr(x), str_only=True)
        assert fastnumbers.isint(y)
        for base in range(2, 36 + 1):
            if (
                len(repr(x)) < 30
            ):  # Avoid recursion error because of overly simple baseN function.
                assert fastnumbers.isint(base_n(x, base), base=base)
        assert fastnumbers.isint(bin(x), base=2)
        assert fastnumbers.isint(bin(x), base=0)
        assert fastnumbers.isint(oct(x), base=8)
        assert fastnumbers.isint(oct(x), base=0)
        assert fastnumbers.isint(oct(x).replace("0o", "0"), base=8)
        if python_version_tuple()[0] == "2" or x == 0:
            assert fastnumbers.isint(oct(x).replace("0o", "0"), base=0)
        else:
            assert not fastnumbers.isint(oct(x).replace("0o", "0"), base=0)
        assert fastnumbers.isint(hex(x), base=16)
        assert fastnumbers.isint(hex(x), base=0)

    def test_underscores(self):
        if int(python_version_tuple()[0]) > 3 or (
            int(python_version_tuple()[0]) == 3 and int(python_version_tuple()[0]) >= 6
        ):
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
            # Old-style octal, still disallowed:
            assert not fastnumbers.isint("0_7")
            assert not fastnumbers.isint("0_7", base=0)
            assert not fastnumbers.isint("09_99")
            assert not fastnumbers.isint("09_99", base=0)
            assert not fastnumbers.isint("0b1001__0100", base=0)
            assert not fastnumbers.isint("0xffff__ffff", base=0)

    @given(
        floats(allow_nan=False, allow_infinity=False),
        integers(0, 100),
        integers(0, 100),
    )
    def test_returns_false_if_given_float_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert not fastnumbers.isint(repr(x))
        assert not fastnumbers.isint(y)
        for base in range(2, 36 + 1):
            if (
                len(y) < 30
            ):  # Avoid recursion error because of overly simple baseN function.
                assert not fastnumbers.isint(y, base=base)

    @given(integers())
    def test_returns_false_if_given_string_and_num_only_is_true(self, x):
        assert not fastnumbers.isint(repr(x), num_only=True)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_true(self, x):
        assert fastnumbers.isint(x)
        # Try padded as well
        assert fastnumbers.isint(u"   " + x + u"   ")

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_false(self, x):
        assert not fastnumbers.isint(x)

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_false(self, x):
        assert not fastnumbers.isint(x)

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_false(self, x):
        assert not fastnumbers.isint(x)

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    def test_returns_false_if_given_non_number_string(self, x):
        assert not fastnumbers.isint(x)

    def test_returns_false_for_nan_or_inf_string(self):
        assert not fastnumbers.isint("nan")
        assert not fastnumbers.isint("inf")


class TestIsIntLike:
    """Tests for the isintlike function."""

    @given(integers())
    def test_returns_true_if_given_int(self, x):
        assert fastnumbers.isintlike(x)
        assert fastnumbers.isintlike(x, num_only=True)

    @given(floats().filter(not_an_integer))
    def test_returns_false_if_given_non_integer_float(self, x):
        assert not fastnumbers.isintlike(x)

    @given(floats().filter(an_integer))
    def test_returns_true_if_given_integer_float(self, x):
        assert fastnumbers.isintlike(x)

    @given(integers())
    def test_returns_false_if_given_int_and_str_only_is_true(self, x):
        assert not fastnumbers.isintlike(x, str_only=True)

    @given(floats().filter(an_integer))
    def test_returns_false_if_given_integer_float_and_str_only_is_true(self, x):
        assert not fastnumbers.isintlike(x, str_only=True)

    @given(integers(), integers(0, 100), integers(0, 100))
    @example(40992764608243448035, 1, 1)
    @example(-41538374848935286698640072416676709, 1, 1)
    @example(240278958776173358420034462324117625982, 1, 1)
    @example(1609422692302207451978552816956662956486, 1, 1)
    @example(-121799354242674784350540853922878239740762834, 1, 1)
    @example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
    @example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
    @example(int("1" + "0" * 1050), 1, 1)
    def test_returns_true_if_given_int_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.isintlike(repr(x))
        assert fastnumbers.isintlike(repr(x), str_only=True)
        assert fastnumbers.isintlike(y)

    @given(
        floats(allow_nan=False, allow_infinity=False).filter(an_integer),
        integers(0, 100),
        integers(0, 100),
    )
    @example("10." + "0" * 1050, 1, 1)
    def test_returns_true_if_given_integer_float_string_padded_or_not(self, x, y, z):
        if isinstance(x, str):
            x = float(x)
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.isintlike(repr(x))
        assert fastnumbers.isintlike(y)

    @given(
        floats(allow_nan=False, allow_infinity=False).filter(not_an_integer),
        integers(0, 100),
        integers(0, 100),
    )
    def test_returns_false_if_given_non_integer_float_string_padded_or_not(
        self, x, y, z
    ):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert not fastnumbers.isintlike(repr(x))
        assert not fastnumbers.isintlike(y)

    @given(integers())
    def test_returns_false_if_given_string_and_num_only_is_true(self, x):
        assert not fastnumbers.isintlike(repr(x), num_only=True)

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_true(self, x):
        assert fastnumbers.isintlike(x)
        # Try padded as well
        assert fastnumbers.isintlike(u"   " + x + u"   ")

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_non_digit_numeral_returns_false(self, x):
        assert not fastnumbers.isintlike(x)

    @given(
        sampled_from(numeric_not_digit).filter(
            lambda x: an_integer(unicodedata.numeric(x))
        )
    )
    def test_given_unicode_digit_numeral_returns_false(self, x):
        assert fastnumbers.isintlike(x)
        # Try padded as well
        assert fastnumbers.isintlike(u"   " + x + u"   ")

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_false(self, x):
        assert not fastnumbers.isintlike(x)

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_false(self, x):
        assert not fastnumbers.isintlike(x)

    @given((text() | binary()).filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    @example("a" * 1050)
    def test_returns_false_if_given_non_number_string(self, x):
        assert not fastnumbers.isintlike(x)

    def test_returns_false_for_nan_or_inf_string(self):
        assert not fastnumbers.isintlike("nan")
        assert not fastnumbers.isintlike("inf")


class TestQueryType:
    """Tests for the query_type function."""

    @given(integers())
    def test_returns_int_if_given_int(self, x):
        assert fastnumbers.query_type(x) is int

    @given(floats())
    def test_returns_float_if_given_float(self, x):
        assert fastnumbers.query_type(x) is float

    @given(integers())
    def test_returns_none_if_given_int_and_int_is_not_allowed(self, x):
        assert fastnumbers.query_type(x, allowed_types=(float,)) is None

    @given(floats())
    def test_returns_none_if_given_float_and_float_is_not_allowed(self, x):
        assert fastnumbers.query_type(x, allowed_types=(int,)) is None

    @given(integers(), integers(0, 100), integers(0, 100))
    def test_returns_int_if_given_int_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.query_type(repr(x)) is int
        assert fastnumbers.query_type(y) is int

    @given(
        floats(allow_nan=False, allow_infinity=False),
        integers(0, 100),
        integers(0, 100),
    )
    def test_returns_float_if_given_float_string_padded_or_not(self, x, y, z):
        y = "".join(repeat(space(), y)) + repr(x) + "".join(repeat(space(), z))
        assert fastnumbers.query_type(repr(x)) is float
        assert fastnumbers.query_type(y) is float

    @given(integers())
    def test_returns_none_if_given_int_string_and_int_is_not_allowed(self, x):
        assert fastnumbers.query_type(repr(x), allowed_types=(float, str)) is None

    @given(sampled_from(digits))
    def test_given_unicode_digit_returns_int(self, x):
        assert fastnumbers.query_type(x) is int
        # Try padded as well
        assert fastnumbers.query_type(u"   " + x + u"   ") is int

    @given(sampled_from(numeric_not_digit_not_int))
    def test_given_unicode_numeral_returns_float(self, x):
        assert fastnumbers.query_type(x) is float
        # Try padded as well
        assert fastnumbers.query_type(u"   " + x + u"   ") is float

    @given(sampled_from(not_numeric))
    def test_given_unicode_non_numeral_returns_str_or_none_if_str_not_allowed(self, x):
        assert fastnumbers.query_type(x) is str
        assert fastnumbers.query_type(x, allowed_types=(int, float)) is None

    @given(text(min_size=2).filter(not_a_number))
    def test_given_unicode_of_more_than_one_char_returns_str(self, x):
        assert fastnumbers.query_type(x) is str

    @given(text().filter(not_a_number))
    @example("+")
    @example("-")
    @example("e")
    @example("e8")
    @example(".")
    def test_returns_str_if_given_non_number_string(self, x):
        assert fastnumbers.query_type(x) is str

    @given(text().filter(not_a_number))
    def test_returns_none_if_given_non_number_string_and_str_is_not_allowed(self, x):
        assert fastnumbers.query_type(x, allowed_types=(int, float)) is None

    @given(binary().filter(not_a_number))
    def test_returns_bytes_if_given_non_number_string(self, x):
        assert fastnumbers.query_type(x) is bytes

    @given(binary().filter(not_a_number))
    def test_returns_none_if_given_non_number_bytes_and_bytes_is_not_allowed(self, x):
        assert fastnumbers.query_type(x, allowed_types=(int, float)) is None

    def test_returns_str_for_nan_string_unless_allow_nan_is_true(self):
        assert fastnumbers.query_type("nan") is str
        assert fastnumbers.query_type("nan", allow_nan=True) is float
        assert fastnumbers.query_type("-NaN", allow_nan=True) is float

    def test_returns_str_for_inf_string_unless_allow_infinity_is_true(self):
        assert fastnumbers.query_type("inf") is str
        assert fastnumbers.query_type("inf", allow_inf=True) is float
        assert fastnumbers.query_type("-INFINITY", allow_inf=True) is float

    def test_given_nan_returns_float(self):
        assert fastnumbers.query_type(float("nan")) is float

    def test_given_inf_returns_float(self):
        assert fastnumbers.query_type(float("inf")) is float

    @given(floats(allow_nan=False).filter(an_integer))
    def test_given_float_returns_int_if_intlike_with_coerce(self, x):
        assert fastnumbers.query_type(x, coerce=True) == int

    @given(floats(allow_nan=False))
    def test_given_float_returns_float_or_int_with_coerce(self, x):
        assert (
            fastnumbers.query_type(x, coerce=True) == int if x.is_integer() else float
        )
