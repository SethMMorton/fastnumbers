# -*- coding: utf-8 -*-
# Find the build location and add that to the path
from __future__ import print_function, division
import re
import sys
import os
import math
import unicodedata
from itertools import repeat
from platform import python_version_tuple
from pytest import raises
from hypothesis import given, assume, example
from hypothesis.strategies import (
    sampled_from,
    floats,
    integers,
    text,
    binary,
    lists,
)
import fastnumbers

if python_version_tuple()[0] == '3':
    long = int
    unichr = chr


# Predefine Unicode digits, numbers, and not those.
digits = []
numeric = []
not_numeric = []
for x in range(0x1FFFFF):
    try:
        a = unichr(x)
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
not_numeric = sampled_from(not_numeric)
not_numeric = [not_numeric.example() for _ in range(1000)]  # This is too big otherwise
numeric_not_digit = [x for x in numeric if x not in digits]
numeric_not_digit_not_int = [x for x in numeric_not_digit if not unicodedata.numeric(x).is_integer()]


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
    if python_version_tuple()[0] == '3':
        if isinstance(s, bytes):
            return False
    if re.match(r'\s*([-+]?\d+\.?\d*(?:[eE][-+]?\d+)?)\s*$', s, re.U):
        return True
    if re.match(r'\s*([-+]?\d+[lL]?)\s*$', s, re.U):
        return True
    if re.match(r'\s*([-+]?\.\d+(?:[eE][-+]?\d+)?)\s*$', s, re.U):
        return True
    if int(python_version_tuple()[0]) >= 3 or not isinstance(s, str):
        if s in numeric:
            return True
    return False


def baseN(num, b, numerals="0123456789abcdefghijklmnopqrstuvwxyz"):
    """
    Convert any integer to a Base-N string representation.
    Shamelessly stolen from http://stackoverflow.com/a/2267428/1399279
    """
    neg = num < 0
    num = abs(num)
    val = ((num == 0) and numerals[0]) or (baseN(num // b, b, numerals).lstrip(numerals[0]) + numerals[num % b])
    return '-' + val if neg else val


class DumbFloatClass(object):
    def __float__(self):
        raise ValueError("something here might go wrong")


class DumbIntClass(object):
    def __int__(self):
        raise ValueError("something here might go wrong")


def test_version():
    assert hasattr(fastnumbers, '__version__')


#################
# Sanity Checks #
#################


def test_fast_real_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.fast_real(5, invalid='dummy')


def test_fast_float_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.fast_float(5, invalid='dummy')


def test_fast_int_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.fast_int(5, invalid='dummy')


def test_fast_forceint_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.fast_forceint(5, invalid='dummy')


def test_isreal_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.isreal(5, invalid='dummy')


def test_isfloat_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.isfloat(5, invalid='dummy')


def test_isint_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.isint(5, invalid='dummy')


def test_isintlike_with_no_arguments_fails():
    with raises(TypeError):
        fastnumbers.isintlike(5, invalid='dummy')


#############
# Fast Real #
#############


def test_fast_real_with_coerce_given_dumb_class_responds_to_internal_ValueError():
    x = DumbFloatClass()
    assert fastnumbers.fast_real(x, coerce=True) is x
    with raises(ValueError):
        fastnumbers.fast_real(x, coerce=True, raise_on_invalid=True)
    assert fastnumbers.fast_real(x, coerce=True, default=5) == 5


@given(floats())
def test_fast_real_given_float_returns_float(x):
    assume(not math.isnan(x))
    assert fastnumbers.fast_real(x, coerce=False) == x
    assert fastnumbers.fast_real(x, raise_on_invalid=True, coerce=False) == x
    assert fastnumbers.fast_real(x, None, True, coerce=False) == x
    assert isinstance(fastnumbers.fast_real(x, coerce=False), float)


@given(floats())
def test_fast_real_given_float_returns_int_if_intlike_with_coerce(x):
    assume(not math.isnan(x))
    assume(x.is_integer())
    assert fastnumbers.fast_real(x, coerce=True) == int(float(x))
    assert isinstance(fastnumbers.fast_real(x, coerce=True), (long, int))


@given(floats())
def test_fast_real_given_float_returns_float_or_int_with_coerce(x):
    assume(not math.isnan(x))
    assert fastnumbers.fast_real(x, coerce=True) == int(x) if x.is_integer() else x
    assert isinstance(fastnumbers.fast_real(x, coerce=True), (int, long) if x.is_integer() else float)


def test_fast_real_given_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_real(float('nan')))


def test_fast_real_given_nan_returns_sub_value():
    assert fastnumbers.fast_real(float('nan'), nan=0) == 0


def test_fast_real_given_inf_returns_inf():
    assert math.isinf(fastnumbers.fast_real(float('inf')))


def test_fast_real_given_inf_returns_sub_value():
    assert fastnumbers.fast_real(float('inf'), inf=1000.0) == 1000.0


@given(floats())
@example(5.675088586167575e-116)
def test_fast_real_given_float_string_returns_float(x):
    assume(not math.isnan(x))
    y = repr(x)
    assert fastnumbers.fast_real(y, coerce=False) == x
    assert fastnumbers.fast_real(y, raise_on_invalid=True, coerce=False) == x
    assert fastnumbers.fast_real(y, None, True, coerce=False) == x
    assert isinstance(fastnumbers.fast_real(y, coerce=False), float)


@given(integers())
def test_fast_real_given_float_string_returns_int_with_coerce_with_intlike(x):
    y = repr(float(x))
    assert fastnumbers.fast_real(y, coerce=True) == int(float(x))
    assert isinstance(fastnumbers.fast_real(y, coerce=True), (long, int))


def test_fast_real_given_nan_string_returns_nan():
    assert math.isnan(fastnumbers.fast_real('nan'))
    assert math.isnan(fastnumbers.fast_real('NaN'))


def test_fast_real_with_nan_given_nan_string_returns_sub_value():
    assert fastnumbers.fast_real('nan', nan=0) == 0


def test_fast_real_given_inf_string_returns_inf():
    assert fastnumbers.fast_real('inf') == float('inf')
    assert fastnumbers.fast_real('-INF') == float('-inf')
    assert fastnumbers.fast_real('infinity') == float('inf')
    assert fastnumbers.fast_real('-infINIty') == float('-inf')


def test_fast_real_with_inf_given_inf_string_returns_sub_value():
    assert fastnumbers.fast_real('inf', inf=10000.0) == 10000.0


@given(floats(), integers(0, 100), integers(0, 100))
def test_fast_real_given_padded_float_strings_returns_float(x, y, z):
    assume(not math.isnan(x))
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.fast_real(y, coerce=False) == x
    assert isinstance(fastnumbers.fast_real(y, coerce=False), float)


def test_fast_real_given_padded_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_real('     nan '))


def test_fast_real_given_padded_inf_string_returns_inf():
    assert fastnumbers.fast_real('   inf        ') == float('inf')
    assert fastnumbers.fast_real('  -infINIty  ') == float('-inf')


@given(integers())
@example(1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)
def test_fast_real_given_int_returns_int(x):
    assert fastnumbers.fast_real(x) == x
    assert isinstance(fastnumbers.fast_real(x), (int, long))

@given(integers())
@example(1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)
def test_fast_real_given_int_returns_int_with_coerce(x):
    assert fastnumbers.fast_real(x, coerce=True) == x
    assert isinstance(fastnumbers.fast_real(x, coerce=True), (int, long))


@given(integers())
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_fast_real_given_int_string_returns_int(x):
    y = repr(x)
    assert fastnumbers.fast_real(y) == x
    assert isinstance(fastnumbers.fast_real(y), (int, long))


@given(integers(), integers(0, 100), integers(0, 100))
@example(40992764608243448035, 1, 1)
@example(-41538374848935286698640072416676709, 1, 1)
@example(240278958776173358420034462324117625982, 1, 1)
@example(1609422692302207451978552816956662956486, 1, 1)
@example(-121799354242674784350540853922878239740762834, 1, 1)
@example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
@example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
def test_fast_real_given_padded_int_string_returns_int(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.fast_real(y) == x
    assert isinstance(fastnumbers.fast_real(y), (int, long))


@given(sampled_from(digits))
def test_fast_real_given_unicode_digit_returns_int(x):
    assert fastnumbers.fast_real(x) == unicodedata.digit(x)
    assert isinstance(fastnumbers.fast_real(x), (int, long))
    # Try padded as well
    assert fastnumbers.fast_real(u'   ' + x + u'   ') == unicodedata.digit(x)


@given(sampled_from(numeric_not_digit_not_int))
def test_fast_real_given_unicode_numeral_returns_float(x):
    assert fastnumbers.fast_real(x) == unicodedata.numeric(x)
    assert isinstance(fastnumbers.fast_real(x), float)
    # Try padded as well
    assert fastnumbers.fast_real(u'   ' + x + u'   ') == unicodedata.numeric(x)


@given(sampled_from(not_numeric))
def test_fast_real_given_unicode_non_numeral_returns_as_is(x):
    assert fastnumbers.fast_real(x) == x


@given(text(min_size=2))
def test_fast_real_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_real(x) == x


@given(text() | binary())
@example('+')
@example('-')
@example('e8')
@example('.')
def test_fast_real_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_real(x) is x


@given(text() | binary())
def test_fast_real_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_real(x, raise_on_invalid=True)


@given(lists(integers()))
def test_fast_real_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_real(x)


@given(text() | binary())
def test_fast_real_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_real(x, default=90) == 90
    assert fastnumbers.fast_real(x, 90) == 90


@given(text() | binary())
def test_fast_real_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_real(x, default=90, raise_on_invalid=True)
        fastnumbers.fast_real(x, 90, True)


@given(integers() | floats())
def test_fast_real_returns_input_as_is_if_valid_and_key_is_given(x):
    fastnumbers.fast_real(x, key=len) == x
    fastnumbers.fast_real(str(x), key=len) == x


@given(text() | binary())
def test_fast_real_returns_transformed_input_if_invalid_and_key_is_given(x):
    assume(not a_number(x))
    fastnumbers.fast_real(x, key=len) == len(x)


##############
# Fast Float #
##############


def test_fast_float_given_dumb_class_responds_to_internal_ValueError():
    x = DumbFloatClass()
    assert fastnumbers.fast_float(x) is x
    with raises(ValueError):
        fastnumbers.fast_float(x, raise_on_invalid=True)
    assert fastnumbers.fast_float(x, default=5) == 5


@given(floats())
def test_fast_float_given_float_returns_float(x):
    assume(not math.isnan(x))
    assert fastnumbers.fast_float(x) == x
    assert fastnumbers.fast_float(x, None, True) == x
    assert fastnumbers.fast_float(x, raise_on_invalid=True) == x
    assert isinstance(fastnumbers.fast_float(x), float)


def test_fast_float_given_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_float(float('nan')))


def test_fast_float_given_nan_returns_sub_value():
    assert fastnumbers.fast_float(float('nan'), nan=0) == 0


def test_fast_float_given_inf_returns_inf():
    assert math.isinf(fastnumbers.fast_float(float('inf')))


def test_fast_float_given_inf_returns_sub_value():
    assert fastnumbers.fast_float(float('inf'), inf=1000.0) == 1000.0


def test_fast_float_with_range_of_exponents_correctly_parses():
    for x in range(-300, 300):
        val = '1.0E{0:d}'.format(x)
        assert fastnumbers.fast_float(val) == float(val)
    for x in range(-300, 300):
        val = '1.0000000000E{0:d}'.format(x)
        assert fastnumbers.fast_float(val) == float(val)


@given(floats())
@example(5.675088586167575e-116)
def test_fast_float_given_float_string_returns_float(x):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = repr(x)
    assert fastnumbers.fast_float(y) == x
    assert fastnumbers.fast_float(y, None, True) == x
    assert fastnumbers.fast_float(y, raise_on_invalid=True) == x
    assert isinstance(fastnumbers.fast_float(y), float)


def test_fast_float_given_nan_string_returns_nan():
    assert math.isnan(fastnumbers.fast_float('nan'))
    assert math.isnan(fastnumbers.fast_float('NaN'))


def test_fast_float_given_inf_string_returns_inf():
    assert fastnumbers.fast_float('inf') == float('inf')
    assert fastnumbers.fast_float('-INF') == float('-inf')
    assert fastnumbers.fast_float('infinity') == float('inf')
    assert fastnumbers.fast_float('-infINIty') == float('-inf')


@given(floats(), integers(0, 100), integers(0, 100))
def test_fast_float_given_padded_float_strings_returns_float(x, y, z):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.fast_float(y) == x
    assert isinstance(fastnumbers.fast_float(y), float)


def test_fast_float_given_padded_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_float('     nan '))


def test_fast_float_given_padded_inf_string_returns_inf():
    assert fastnumbers.fast_float('   inf        ') == float('inf')
    assert fastnumbers.fast_float('  -infINIty  ') == float('-inf')


@given(integers())
def test_fast_float_given_int_returns_float(x):
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
def test_fast_float_given_int_string_returns_float(x):
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
def test_fast_float_given_padded_int_string_returns_float(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.fast_float(y) == float(x)
    assert isinstance(fastnumbers.fast_float(y), float)


@given(sampled_from(digits))
def test_fast_float_given_unicode_digit_returns_float(x):
    assert fastnumbers.fast_float(x) == unicodedata.numeric(x)
    assert isinstance(fastnumbers.fast_float(x), float)
    # Try padded as well
    assert fastnumbers.fast_float(u'   ' + x + u'   ') == unicodedata.numeric(x)


@given(sampled_from(numeric))
def test_fast_float_given_unicode_numeral_returns_float(x):
    assert fastnumbers.fast_float(x) == unicodedata.numeric(x)
    assert isinstance(fastnumbers.fast_float(x), float)
    # Try padded as well
    assert fastnumbers.fast_float(u'   ' + x + u'   ') == unicodedata.numeric(x)


@given(sampled_from(not_numeric))
def test_fast_float_given_unicode_non_numeral_returns_as_is(x):
    assert fastnumbers.fast_float(x) == x


@given(text(min_size=2))
def test_fast_float_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_float(x) == x


@given(text() | binary())
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_fast_float_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_float(x) is x


@given(text() | binary())
def test_fast_float_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_float(x, None, True)
        fastnumbers.fast_float(x, raise_on_invalid=True)


@given(lists(integers()))
def test_fast_float_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_float(x)


@given(text() | binary())
def test_fast_float_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_float(x, default=90.0) == 90.0
    assert fastnumbers.fast_float(x, 90.0) == 90.0


@given(text() | binary())
def test_fast_float_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        assert fastnumbers.fast_float(x, default=90.0, raise_on_invalid=True)
        assert fastnumbers.fast_float(x, 90.0, True)


@given(integers() | floats())
def test_fast_float_returns_input_as_is_if_valid_and_key_is_given(x):
    fastnumbers.fast_float(x, key=len) == x
    fastnumbers.fast_float(str(x), key=len) == x


@given(text() | binary())
def test_fast_float_returns_transformed_input_if_invalid_and_key_is_given(x):
    assume(not a_number(x))
    fastnumbers.fast_float(x, key=len) == len(x)


############
# Fast Int #
############


def test_fast_int_given_dumb_class_responds_to_internal_ValueError():
    x = DumbIntClass()
    assert fastnumbers.fast_int(x) is x
    with raises(ValueError):
        fastnumbers.fast_int(x, raise_on_invalid=True)
    assert fastnumbers.fast_int(x, default=5) == 5


def test_fast_int_given_invalid_base_errors_with_ValueError():
    with raises(ValueError):
        fastnumbers.fast_int('10', base=-1)
    with raises(ValueError):
        fastnumbers.fast_int('10', base=1)
    with raises(ValueError):
        fastnumbers.fast_int('10', base=37)


@given(floats())
def test_fast_int_given_float_returns_int(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assert fastnumbers.fast_int(x) == int(x)
    assert fastnumbers.fast_int(x, raise_on_invalid=True) == int(x)
    assert fastnumbers.fast_int(x, None, True) == int(x)
    assert isinstance(fastnumbers.fast_int(x), (int, long))


def test_fast_int_given_nan_raises_ValueError_or_returns_as_is_or_returns_default():
    with raises(ValueError):
        fastnumbers.fast_int(float('nan'), raise_on_invalid=True)
    assert math.isnan(fastnumbers.fast_int(float('nan')))
    assert fastnumbers.fast_int(float('nan'), 'Sample') == 'Sample'


def test_fast_int_given_inf_raises_OverflowError():
    with raises(OverflowError):
        fastnumbers.fast_int(float('inf'), raise_on_invalid=True)
    assert math.isinf(fastnumbers.fast_int(float('inf')))
    assert fastnumbers.fast_int(float('inf'), 'Sample') == 'Sample'


@given(floats())
def test_fast_int_given_float_string_returns_string_as_is(x):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = repr(x)
    assert fastnumbers.fast_int(y) is y


@given(floats())
def test_fast_int_given_float_intlike_string_returns_string_as_is(x):
    assume(not math.isnan(x))
    assume(x.is_integer())
    y = repr(x)
    assert fastnumbers.fast_int(y) is y


@given(floats())
@example(float('nan'))
@example(float('inf'))
def test_fast_int_given_float_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not x.is_integer())
    y = repr(x)
    with raises(ValueError):
        fastnumbers.fast_int(y, None, True)
        fastnumbers.fast_int(y, raise_on_invalid=True)


@given(integers())
@example(1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)
def test_fast_int_given_int_returns_int(x):
    assert fastnumbers.fast_int(x) == x
    assert isinstance(fastnumbers.fast_int(x), (int, long))


@given(integers())
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_fast_int_given_int_string_returns_int(x):
    y = repr(x)
    assert fastnumbers.fast_int(y) == x
    assert isinstance(fastnumbers.fast_int(y), (int, long))
    for base in range(2, 36+1):
        if len(y) < 30:  # Avoid recursion error because of overly simple baseN function.
            assert fastnumbers.fast_int(baseN(x, base), base=base) == x
    assert fastnumbers.fast_int(bin(x), base=2) == x
    assert fastnumbers.fast_int(bin(x), base=0) == x
    assert fastnumbers.fast_int(oct(x), base=8) == x
    assert fastnumbers.fast_int(oct(x), base=0) == x
    assert fastnumbers.fast_int(hex(x), base=16) == x
    assert fastnumbers.fast_int(hex(x), base=0) == x


@given(integers(), integers(0, 100), integers(0, 100))
@example(40992764608243448035, 1, 1)
@example(-41538374848935286698640072416676709, 1, 1)
@example(240278958776173358420034462324117625982, 1, 1)
@example(1609422692302207451978552816956662956486, 1, 1)
@example(-121799354242674784350540853922878239740762834, 1, 1)
@example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
@example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
def test_fast_int_given_padded_int_string_returns_int(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.fast_int(y) == x
    assert isinstance(fastnumbers.fast_int(y), (int, long))


@given(sampled_from(digits))
def test_fast_int_given_unicode_digit_returns_int(x):
    assert fastnumbers.fast_int(x) == unicodedata.digit(x)
    assert isinstance(fastnumbers.fast_int(x), (int, long))
    # Try padded as well
    assert fastnumbers.fast_int(u'   ' + x + u'   ') == unicodedata.digit(x)


@given(sampled_from(numeric_not_digit))
def test_fast_int_given_unicode_numeral_returns_as_is(x):
    assert fastnumbers.fast_int(x) == x


@given(sampled_from(not_numeric))
def test_fast_int_given_unicode_non_numeral_returns_as_is(x):
    assert fastnumbers.fast_int(x) == x


@given(text(min_size=2))
def test_fast_int_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_int(x) == x


@given(text() | binary())
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_fast_int_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_int(x) is x
    assert fastnumbers.fast_int(x, base=10) is x


@given(text() | binary())
def test_fast_int_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_int(x, raise_on_invalid=True)
        fastnumbers.fast_int(x, None, True)


@given(lists(integers()))
def test_fast_int_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_int(x)


@given(text() | binary())
def test_fast_int_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_int(x, default=90) == 90
    assert fastnumbers.fast_int(x, 90) == 90


@given(text() | binary())
def test_fast_int_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        assert fastnumbers.fast_int(x, default=90, raise_on_invalid=True)
        assert fastnumbers.fast_int(x, 90, True)


@given(integers())
def test_fast_int_returns_input_as_is_if_valid_and_key_is_given(x):
    fastnumbers.fast_int(x, key=len) == x
    fastnumbers.fast_int(str(x), key=len) == x


@given(text() | binary())
def test_fast_int_returns_transformed_input_if_invalid_and_key_is_given(x):
    assume(not a_number(x))
    fastnumbers.fast_int(x, key=len) == len(x)


#################
# Fast Forceint #
#################


def test_fast_forceint_given_dumb_class_responds_to_internal_ValueError():
    x = DumbIntClass()
    assert fastnumbers.fast_forceint(x) is x
    with raises(ValueError):
        fastnumbers.fast_forceint(x, raise_on_invalid=True)
    assert fastnumbers.fast_forceint(x, default=5) == 5


@given(floats())
def test_fast_forceint_given_float_returns_int(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assert fastnumbers.fast_forceint(x) == int(x)
    assert fastnumbers.fast_forceint(x, raise_on_invalid=True) == int(x)
    assert fastnumbers.fast_forceint(x, None, True) == int(x)
    assert isinstance(fastnumbers.fast_forceint(x), (int, long))


def test_fast_forceint_given_nan_raises_ValueError_or_returns_as_is_or_returns_default():
    with raises(ValueError):
        fastnumbers.fast_forceint(float('nan'), raise_on_invalid=True)
    assert math.isnan(fastnumbers.fast_forceint(float('nan')))
    assert fastnumbers.fast_forceint(float('nan'), 'Sample') == 'Sample'


def test_fast_forceint_given_inf_raises_OverflowError():
    with raises(OverflowError):
        fastnumbers.fast_forceint(float('inf'), raise_on_invalid=True)
    assert math.isinf(fastnumbers.fast_forceint(float('inf')))
    assert fastnumbers.fast_forceint(float('inf'), 'Sample') == 'Sample'


@given(floats())
def test_fast_forceint_given_float_string_returns_int(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = repr(x)
    assert fastnumbers.fast_forceint(y) == int(x)
    assert fastnumbers.fast_forceint(y, None, True) == int(x)
    assert fastnumbers.fast_forceint(y, raise_on_invalid=True) == int(x)
    assert isinstance(fastnumbers.fast_forceint(y), (int, long))


def test_fast_forceint_given_nan_string_raises_ValueError_with_raise_on_invalid_as_True():
    with raises(ValueError): 
        fastnumbers.fast_forceint('nan', raise_on_invalid=True)
        fastnumbers.fast_forceint('nan', None, True)


def test_fast_forceint_given_inf_string_raises_OverflowError_with_raise_on_invalid_as_True():
    with raises(OverflowError):
        fastnumbers.fast_forceint('inf', raise_on_invalid=True)
        fastnumbers.fast_forceint('-infinity', raise_on_invalid=True)
        fastnumbers.fast_forceint('inf', None, True)
        fastnumbers.fast_forceint('-infinity', None, True)


@given(floats(), integers(0, 100), integers(0, 100))
def test_fast_forceint_given_padded_float_strings_returns_int(x, y, z):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.fast_forceint(y) == int(x)
    assert isinstance(fastnumbers.fast_forceint(y), (int, long))


@given(integers())
@example(1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)
def test_fast_forceint_given_int_returns_int(x):
    assert fastnumbers.fast_forceint(x) == x
    assert isinstance(fastnumbers.fast_forceint(x), (int, long))


@given(integers())
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_fast_forceint_given_int_string_returns_int(x):
    y = repr(x)
    assert fastnumbers.fast_forceint(y) == x
    assert isinstance(fastnumbers.fast_forceint(y), (int, long))


@given(integers(), integers(0, 100), integers(0, 100))
@example(40992764608243448035, 1, 1)
@example(-41538374848935286698640072416676709, 1, 1)
@example(240278958776173358420034462324117625982, 1, 1)
@example(1609422692302207451978552816956662956486, 1, 1)
@example(-121799354242674784350540853922878239740762834, 1, 1)
@example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
@example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
def test_fast_forceint_given_padded_int_string_returns_int(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.fast_forceint(y) == x
    assert isinstance(fastnumbers.fast_forceint(y), (int, long))


@given(sampled_from(digits))
def test_fast_forceint_given_unicode_digit_returns_int(x):
    assert fastnumbers.fast_forceint(x) == unicodedata.digit(x)
    assert isinstance(fastnumbers.fast_forceint(x), (int, long))
    # Try padded as well
    assert fastnumbers.fast_forceint(u'   ' + x + u'   ') == unicodedata.digit(x)


@given(sampled_from(numeric))
def test_fast_forceint_given_unicode_numeral_returns_int(x):
    assert fastnumbers.fast_forceint(x) == int(unicodedata.numeric(x))
    assert isinstance(fastnumbers.fast_forceint(x), (int, long))
    # Try padded as well
    assert fastnumbers.fast_forceint(u'   ' + x + u'   ') == int(unicodedata.numeric(x))


@given(sampled_from(not_numeric))
def test_fast_forceint_given_unicode_non_numeral_returns_as_is(x):
    assert fastnumbers.fast_forceint(x) == x


@given(text(min_size=2))
def test_fast_forceint_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_int(x) == x


@given(text() | binary())
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_fast_forceint_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_forceint(x) is x


@given(text() | binary())
def test_fast_forceint_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_forceint(x, raise_on_invalid=True)
        fastnumbers.fast_forceint(x, None, True)


@given(lists(integers()))
def test_fast_forceint_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_forceint(x)


@given(text() | binary())
def test_fast_forceint_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_forceint(x, default=90.0) == 90.0
    assert fastnumbers.fast_forceint(x, 90.0) == 90.0


@given(text() | binary())
def test_fast_forceint_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        assert fastnumbers.fast_forceint(x, default=90.0, raise_on_invalid=True)
        assert fastnumbers.fast_forceint(x, 90.0, True)


@given(integers() | floats())
def test_fast_forceint_returns_input_as_is_if_valid_and_key_is_given(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    fastnumbers.fast_forceint(x, key=len) == int(x)
    fastnumbers.fast_forceint(str(x), key=len) == int(x)


@given(text() | binary())
def test_fast_forceint_returns_transformed_input_if_invalid_and_key_is_given(x):
    assume(not a_number(x))
    fastnumbers.fast_forceint(x, key=len) == len(x)


###########
# Is Real #
###########


@given(integers())
def test_isreal_returns_True_if_given_int(x):
    assert fastnumbers.isreal(x)
    assert fastnumbers.isreal(x, num_only=True)


@given(floats())
def test_isreal_returns_True_if_given_float(x):
    assert fastnumbers.isreal(x)
    assert fastnumbers.isreal(x, num_only=True)


@given(integers())
def test_isreal_returns_False_if_given_int_and_str_only_is_True(x):
    assert not fastnumbers.isreal(x, str_only=True)


@given(floats())
def test_isreal_returns_False_if_given_float_and_str_only_is_True(x):
    assert not fastnumbers.isreal(x, str_only=True)


@given(integers(), integers(0, 100), integers(0, 100))
def test_isreal_returns_True_if_given_int_string_padded_or_not(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.isreal(repr(x))
    assert fastnumbers.isreal(repr(x), str_only=True)
    assert fastnumbers.isreal(y)


@given(floats(), integers(0, 100), integers(0, 100))
def test_isreal_returns_True_if_given_float_string_padded_or_not(x, y, z):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.isreal(repr(x))
    assert fastnumbers.isreal(repr(x), str_only=True)
    assert fastnumbers.isreal(y)


@given(integers())
def test_isreal_returns_False_if_given_string_and_num_only_is_True(x):
    assert not fastnumbers.isreal(repr(x), num_only=True)


@given(sampled_from(digits))
def test_isreal_given_unicode_digit_returns_True(x):
    assert fastnumbers.isreal(x)
    # Try padded as well
    assert fastnumbers.isreal(u'   ' + x + u'   ')


@given(sampled_from(numeric_not_digit_not_int))
def test_isreal_given_unicode_numeral_returns_True(x):
    assert fastnumbers.isreal(x)
    # Try padded as well
    assert fastnumbers.isreal(u'   ' + x + u'   ')


@given(sampled_from(not_numeric))
def test_isreal_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isreal(x)


@given(text(min_size=2))
def test_isreal_given_unicode_of_more_than_one_char_returns_False(x):
    assume(not a_number(x))
    assert not fastnumbers.isreal(x)


@given(text() | binary())
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_isreal_returns_False_if_given_non_number_string(x):
    assume(not a_number(x))
    assert not fastnumbers.isreal(x)


def test_isreal_returns_False_for_nan_string_unless_allow_nan_is_True():
    assert not fastnumbers.isreal('nan')
    assert fastnumbers.isreal('nan', allow_nan=True)
    assert fastnumbers.isreal('-NaN', allow_nan=True)


def test_isreal_returns_False_for_inf_string_unless_allow_inf_is_True():
    assert not fastnumbers.isreal('inf')
    assert fastnumbers.isreal('inf', allow_inf=True)
    assert fastnumbers.isreal('-INFINITY', allow_inf=True)


############
# Is Float #
############


@given(integers())
def test_isfloat_returns_False_if_given_int(x):
    assert not fastnumbers.isfloat(x)


@given(floats())
def test_isfloat_returns_True_if_given_float(x):
    assert fastnumbers.isfloat(x)
    assert fastnumbers.isfloat(x, num_only=True)


@given(floats())
def test_isfloat_returns_False_if_given_float_and_str_only_is_True(x):
    assert not fastnumbers.isfloat(x, str_only=True)


@given(integers(), integers(0, 100), integers(0, 100))
@example(40992764608243448035, 1, 1)
@example(-41538374848935286698640072416676709, 1, 1)
@example(240278958776173358420034462324117625982, 1, 1)
@example(1609422692302207451978552816956662956486, 1, 1)
@example(-121799354242674784350540853922878239740762834, 1, 1)
@example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
@example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
def test_isfloat_returns_True_if_given_int_string_padded_or_not(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.isfloat(repr(x))
    assert fastnumbers.isfloat(repr(x), str_only=True)
    assert fastnumbers.isfloat(y)


@given(floats(), integers(0, 100), integers(0, 100))
def test_isfloat_returns_True_if_given_float_string_padded_or_not(x, y, z):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.isfloat(repr(x))
    assert fastnumbers.isfloat(repr(x), str_only=True)
    assert fastnumbers.isfloat(y)


@given(floats())
def test_isfloat_returns_False_if_given_string_and_num_only_is_True(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assert not fastnumbers.isfloat(repr(x), num_only=True)


@given(sampled_from(digits))
def test_isfloat_given_unicode_digit_returns_True(x):
    assert fastnumbers.isfloat(x)
    # Try padded as well
    assert fastnumbers.isfloat(u'   ' + x + u'   ')


@given(sampled_from(numeric_not_digit_not_int))
def test_isfloat_given_unicode_numeral_returns_True(x):
    assert fastnumbers.isfloat(x)
    # Try padded as well
    assert fastnumbers.isfloat(u'   ' + x + u'   ')


@given(sampled_from(not_numeric))
def test_isfloat_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isfloat(x)


@given(text(min_size=2))
def test_isfloat_given_unicode_of_more_than_one_char_returns_False(x):
    assume(not a_number(x))
    assert not fastnumbers.isfloat(x)


@given(text() | binary())
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_isfloat_returns_False_if_given_non_number_string(x):
    assume(not a_number(x))
    assert not fastnumbers.isfloat(x)


def test_isfloat_returns_False_for_nan_string_unless_allow_nan_is_True():
    assert not fastnumbers.isfloat('nan')
    assert fastnumbers.isfloat('nan', allow_nan=True)
    assert fastnumbers.isfloat('-NaN', allow_nan=True)


def test_isfloat_returns_False_for_inf_string_unless_allow_inf_is_True():
    assert not fastnumbers.isfloat('inf')
    assert fastnumbers.isfloat('inf', allow_inf=True)
    assert fastnumbers.isfloat('-INFINITY', allow_inf=True)


##########
# Is Int #
##########


@given(integers())
def test_isint_returns_True_if_given_int(x):
    assert fastnumbers.isint(x, num_only=True)


@given(floats())
def test_isint_returns_False_if_given_float(x):
    assert not fastnumbers.isint(x)


@given(integers())
def test_isint_returns_False_if_given_int_and_str_only_is_True(x):
    assert not fastnumbers.isint(x, str_only=True)


@given(integers(), integers(0, 100), integers(0, 100))
@example(40992764608243448035, 1, 1)
@example(-41538374848935286698640072416676709, 1, 1)
@example(240278958776173358420034462324117625982, 1, 1)
@example(1609422692302207451978552816956662956486, 1, 1)
@example(-121799354242674784350540853922878239740762834, 1, 1)
@example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
@example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
def test_isint_returns_True_if_given_int_string_padded_or_not(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.isint(repr(x)) is True
    assert fastnumbers.isint(repr(x), str_only=True)
    assert fastnumbers.isint(y)
    for base in range(2, 36 + 1):
        if len(repr(x)) < 30:  # Avoid recursion error because of overly simple baseN function.
            assert fastnumbers.isint(baseN(x, base), base=base)
    assert fastnumbers.isint(bin(x), base=2)
    assert fastnumbers.isint(bin(x), base=0)
    assert fastnumbers.isint(oct(x), base=8)
    assert fastnumbers.isint(oct(x), base=0)
    if python_version_tuple()[0] == '2':
        assert fastnumbers.isint(oct(x).replace('0o', '0'), base=8)
        assert fastnumbers.isint(oct(x).replace('0o', '0'), base=0)
    assert fastnumbers.isint(hex(x), base=16)
    assert fastnumbers.isint(hex(x), base=0)


@given(floats(), integers(0, 100), integers(0, 100))
def test_isint_returns_False_if_given_float_string_padded_or_not(x, y, z):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert not fastnumbers.isint(repr(x))
    assert not fastnumbers.isint(y)
    for base in range(2, 36 + 1):
        if len(y) < 30:  # Avoid recursion error because of overly simple baseN function.
            assert not fastnumbers.isint(y, base=base)


@given(integers())
def test_isint_returns_False_if_given_string_and_num_only_is_True(x):
    assert not fastnumbers.isint(repr(x), num_only=True)


@given(sampled_from(digits))
def test_isint_given_unicode_digit_returns_True(x):
    assert fastnumbers.isint(x)
    # Try padded as well
    assert fastnumbers.isint(u'   ' + x + u'   ')


@given(sampled_from(numeric_not_digit_not_int))
def test_isint_given_unicode_numeral_returns_False(x):
    assert not fastnumbers.isint(x)


@given(sampled_from(not_numeric))
def test_isint_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isint(x)


@given(text(min_size=2))
def test_isint_given_unicode_of_more_than_one_char_returns_False(x):
    assume(not a_number(x))
    assert not fastnumbers.isint(x)


@given(text() | binary())
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_isint_returns_False_if_given_non_number_string(x):
    assume(not a_number(x))
    assert not fastnumbers.isint(x)


def test_isint_returns_False_for_nan_or_inf_string():
    assert not fastnumbers.isint('nan')
    assert not fastnumbers.isint('inf')


###############
# Is Int-Like #
###############


@given(integers())
def test_isintlike_returns_True_if_given_int(x):
    assert fastnumbers.isintlike(x)
    assert fastnumbers.isintlike(x, num_only=True)


@given(floats())
def test_isintlike_returns_False_if_given_non_integer_float(x):
    assume(not x.is_integer())
    assert not fastnumbers.isintlike(x)


@given(floats())
def test_isintlike_returns_True_if_given_integer_float(x):
    assume(x.is_integer())
    assert fastnumbers.isintlike(x)


@given(integers())
def test_isintlike_returns_False_if_given_int_and_str_only_is_True(x):
    assert not fastnumbers.isintlike(x, str_only=True)


@given(floats())
def test_isintlike_returns_False_if_given_integer_float_and_str_only_is_True(x):
    assume(x.is_integer())
    assert not fastnumbers.isintlike(x, str_only=True)


@given(integers(), integers(0, 100), integers(0, 100))
@example(40992764608243448035, 1, 1)
@example(-41538374848935286698640072416676709, 1, 1)
@example(240278958776173358420034462324117625982, 1, 1)
@example(1609422692302207451978552816956662956486, 1, 1)
@example(-121799354242674784350540853922878239740762834, 1, 1)
@example(32718704454132572934419741118153895444518280065843028297496525078, 1, 1)
@example(33684944745210074227862907273261282807602986571245071790093633147269, 1, 1)
def test_isintlike_returns_True_if_given_int_string_padded_or_not(x, y, z):
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.isintlike(repr(x))
    assert fastnumbers.isintlike(repr(x), str_only=True)
    assert fastnumbers.isintlike(y)


@given(floats(), integers(0, 100), integers(0, 100))
def test_isintlike_returns_True_if_given_integer_float_string_padded_or_not(x, y, z):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assume(x.is_integer())
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert fastnumbers.isintlike(repr(x))
    assert fastnumbers.isintlike(y)


@given(floats(), integers(0, 100), integers(0, 100))
def test_isintlike_returns_False_if_given_non_integer_float_string_padded_or_not(x, y, z):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assume(not x.is_integer())
    y = ''.join(repeat(' ', y)) + repr(x) + ''.join(repeat(' ', z))
    assert not fastnumbers.isintlike(repr(x))
    assert not fastnumbers.isintlike(y)


@given(integers())
def test_isintlike_returns_False_if_given_string_and_num_only_is_True(x):
    assert not fastnumbers.isintlike(repr(x), num_only=True)


@given(sampled_from(digits))
def test_isintlike_given_unicode_digit_returns_True(x):
    assert fastnumbers.isintlike(x)
    # Try padded as well
    assert fastnumbers.isintlike(u'   ' + x + u'   ')


@given(sampled_from(numeric_not_digit_not_int))
def test_isintlike_given_unicode_non_digit_numeral_returns_False(x):
    assert not fastnumbers.isintlike(x)


@given(sampled_from(numeric_not_digit))
def test_isintlike_given_unicode_digit_numeral_returns_False(x):
    assume(unicodedata.numeric(x).is_integer())
    assert fastnumbers.isintlike(x)
    # Try padded as well
    assert fastnumbers.isintlike(u'   ' + x + u'   ')


@given(sampled_from(not_numeric))
def test_isintlike_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isintlike(x)


@given(text(min_size=2))
def test_isintlike_given_unicode_of_more_than_one_char_returns_False(x):
    assume(not a_number(x))
    assert not fastnumbers.isintlike(x)


@given(text() | binary())
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_isintlike_returns_False_if_given_non_number_string(x):
    assume(not a_number(x))
    assert not fastnumbers.isintlike(x)


def test_isintlike_returns_False_for_nan_or_inf_string():
    assert not fastnumbers.isintlike('nan')
    assert not fastnumbers.isintlike('inf')
