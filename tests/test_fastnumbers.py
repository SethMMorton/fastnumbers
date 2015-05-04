# -*- coding: utf-8 -*-
# Find the build location and add that to the path
from __future__ import print_function, division
import re
import sys
import os
import math
import unicodedata
from random import randint, sample
from itertools import repeat
from platform import python_version_tuple
from pytest import raises
from hypothesis import given, assume, example
from hypothesis.specifiers import sampled_from
import fastnumbers

if python_version_tuple()[0] == '3':
    unicode = str
    long = int
    unichr = chr


# Predefine Unicode digits, numbers, and not those.
digits = []
numeric = []
not_numeric = []
for x in range(0x10FFF):
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
not_numeric = sample(not_numeric, 1000)  # This is too big otherwise


def a_number(s):
    m = bool(re.match(r'\s*([-+]?\d+\.?\d*(?:[eE][-+]?\d+)?)(\s*$|\0)', s, re.U))
    n = bool(re.match(r'\s*([-+]?\d+[lL]?)(\s*$|\0)', s, re.U))
    o = bool(re.match(r'\s*([-+]?\.\d+(?:[eE][-+]?\d+)?)(\s*$|\0)', s, re.U))
    return m or n or o


def test_version():
    assert hasattr(fastnumbers, '__version__')


# NOTE: safe_* functions are not tested because internally the code
#       for the fast_* functions is reused... only the docstrings
#       are different.

#############
# Fast Real #
#############


@given(float)
def test_fast_real_given_float_returns_float(x):
    assume(not math.isnan(x))
    assert fastnumbers.fast_real(x) == x
    assert fastnumbers.fast_real(x, raise_on_invalid=True) == x
    assert isinstance(fastnumbers.fast_real(x), float)


def test_fast_real_given_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_real(float('nan')))


@given(float)
def test_fast_real_given_float_string_returns_float(x):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = repr(x)
    assert fastnumbers.fast_real(y) == x
    assert fastnumbers.fast_real(y, True) == x
    assert isinstance(fastnumbers.fast_real(y), float)


def test_fast_real_given_nan_string_returns_nan():
    assert math.isnan(fastnumbers.fast_real('nan'))
    assert math.isnan(fastnumbers.fast_real('NaN'))


def test_fast_real_given_inf_string_returns_inf():
    assert fastnumbers.fast_real('inf') == float('inf')
    assert fastnumbers.fast_real('-INF') == float('-inf')
    assert fastnumbers.fast_real('infinity') == float('inf')
    assert fastnumbers.fast_real('-infINIty') == float('-inf')


@given(float)
def test_fast_real_given_padded_float_strings_returns_float(x):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.fast_real(y) == x
    assert isinstance(fastnumbers.fast_real(y), float)


def test_fast_real_given_padded_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_real('     nan '))


def test_fast_real_given_padded_inf_string_returns_inf():
    assert fastnumbers.fast_real('   inf        ') == float('inf')
    assert fastnumbers.fast_real('  -infINIty  ') == float('-inf')


@given(int)
def test_fast_real_given_int_returns_int(x):
    assert fastnumbers.fast_real(x) == x
    assert isinstance(fastnumbers.fast_real(x), (int, long))


@given(int)
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


@given(int)
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_fast_real_given_padded_int_string_returns_int(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.fast_real(y) == x
    assert isinstance(fastnumbers.fast_real(y), (int, long))


@given(sampled_from(digits))
def test_fast_real_given_unicode_digit_returns_int(x):
    assert fastnumbers.fast_real(x) == unicodedata.digit(x)
    assert isinstance(fastnumbers.fast_real(x), (int, long))
    # Try padded as well
    assert fastnumbers.fast_real(u'   ' + x + u'   ') == unicodedata.digit(x)


@given(sampled_from(numeric))
def test_fast_real_given_unicode_numeral_returns_float(x):
    assume(not unicodedata.numeric(x).is_integer())
    assert fastnumbers.fast_real(x) == unicodedata.numeric(x)
    assert isinstance(fastnumbers.fast_real(x), float)
    # Try padded as well
    assert fastnumbers.fast_real(u'   ' + x + u'   ') == unicodedata.numeric(x)


@given(sampled_from(not_numeric))
def test_fast_real_given_unicode_non_numeral_returns_as_is(x):
    assert fastnumbers.fast_real(x) == x


@given(unicode)
def test_fast_real_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert fastnumbers.fast_real(x) == x


@given(str)
@example('+')
@example('-')
@example('e8')
@example('.')
def test_fast_real_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_real(x) is x


@given(str)
def test_fast_real_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_real(x, raise_on_invalid=True)


@given([int])
def test_fast_real_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_real(x)


@given(str)
def test_fast_real_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_real(x, default=90) == 90


@given(str)
def test_fast_real_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        assert fastnumbers.fast_real(x, default=90, raise_on_invalid=True)


##############
# Fast Float #
##############


@given(float)
def test_fast_float_given_float_returns_float(x):
    assume(not math.isnan(x))
    assert fastnumbers.fast_float(x) == x
    assert fastnumbers.fast_float(x, raise_on_invalid=True) == x
    assert isinstance(fastnumbers.fast_float(x), float)


def test_fast_float_given_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_float(float('nan')))


@given(float)
def test_fast_float_given_float_string_returns_float(x):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = repr(x)
    assert fastnumbers.fast_float(y) == x
    assert fastnumbers.fast_float(y, True) == x
    assert isinstance(fastnumbers.fast_float(y), float)


def test_fast_float_given_nan_string_returns_nan():
    assert math.isnan(fastnumbers.fast_float('nan'))
    assert math.isnan(fastnumbers.fast_float('NaN'))


def test_fast_float_given_inf_string_returns_inf():
    assert fastnumbers.fast_float('inf') == float('inf')
    assert fastnumbers.fast_float('-INF') == float('-inf')
    assert fastnumbers.fast_float('infinity') == float('inf')
    assert fastnumbers.fast_float('-infINIty') == float('-inf')


@given(float)
def test_fast_float_given_padded_float_strings_returns_float(x):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.fast_float(y) == x
    assert isinstance(fastnumbers.fast_float(y), float)


def test_fast_float_given_padded_nan_returns_nan():
    assert math.isnan(fastnumbers.fast_float('     nan '))


def test_fast_float_given_padded_inf_string_returns_inf():
    assert fastnumbers.fast_float('   inf        ') == float('inf')
    assert fastnumbers.fast_float('  -infINIty  ') == float('-inf')


@given(int)
def test_fast_float_given_int_returns_float(x):
    assert fastnumbers.fast_float(x) == float(x)
    assert isinstance(fastnumbers.fast_float(x), float)


@given(int)
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


@given(int)
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_fast_float_given_padded_int_string_returns_float(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
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


@given(unicode)
def test_fast_float_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert fastnumbers.fast_float(x) == x


@given(str)
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_fast_float_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_float(x) is x


@given(str)
def test_fast_float_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_float(x, raise_on_invalid=True)


@given([int])
def test_fast_float_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_float(x)


@given(str)
def test_fast_float_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_float(x, default=90.0) == 90.0


@given(str)
def test_fast_float_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        assert fastnumbers.fast_float(x, default=90.0, raise_on_invalid=True)


############
# Fast Int #
############


@given(float)
def test_fast_int_given_float_returns_int(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assert fastnumbers.fast_int(x) == int(x)
    assert fastnumbers.fast_int(x, raise_on_invalid=True) == int(x)
    assert isinstance(fastnumbers.fast_int(x), (int, long))


def test_fast_int_given_nan_raises_ValueError():
    with raises(ValueError):
        fastnumbers.fast_int(float('nan'))


def test_fast_int_given_inf_raises_OverflowError():
    with raises(OverflowError):
        fastnumbers.fast_int(float('inf'))


@given(float)
def test_fast_int_given_float_string_returns_string_as_is(x):
    assume(not math.isnan(x))
    assume(not x.is_integer())
    y = repr(x)
    assert fastnumbers.fast_int(y) is y


@given(float)
def test_fast_int_given_float_intlike_string_returns_string_as_is(x):
    assume(not math.isnan(x))
    assume(x.is_integer())
    y = repr(x)
    assert fastnumbers.fast_int(y) is y


@given(float)
@example(float('nan'))
@example(float('inf'))
def test_fast_int_given_float_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not x.is_integer())
    y = repr(x)
    with raises(ValueError):
        fastnumbers.fast_int(y, True)


@given(int)
def test_fast_int_given_int_returns_int(x):
    assert fastnumbers.fast_int(x) == x
    assert isinstance(fastnumbers.fast_int(x), (int, long))


@given(int)
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


@given(int)
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_fast_int_given_padded_int_string_returns_int(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.fast_int(y) == x
    assert isinstance(fastnumbers.fast_int(y), (int, long))


@given(sampled_from(digits))
def test_fast_int_given_unicode_digit_returns_int(x):
    assert fastnumbers.fast_int(x) == unicodedata.digit(x)
    assert isinstance(fastnumbers.fast_int(x), (int, long))
    # Try padded as well
    assert fastnumbers.fast_int(u'   ' + x + u'   ') == unicodedata.digit(x)


@given(sampled_from(numeric))
def test_fast_int_given_unicode_numeral_returns_as_is(x):
    assume(x not in digits)
    assert fastnumbers.fast_int(x) == x


@given(sampled_from(not_numeric))
def test_fast_int_given_unicode_non_numeral_returns_as_is(x):
    assert fastnumbers.fast_int(x) == x


@given(unicode)
def test_fast_int_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert fastnumbers.fast_int(x) == x


@given(str)
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_fast_int_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_int(x) is x


@given(str)
def test_fast_int_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_int(x, raise_on_invalid=True)


@given([int])
def test_fast_int_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_int(x)


@given(str)
def test_fast_int_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_int(x, default=90) == 90


@given(str)
def test_fast_int_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        assert fastnumbers.fast_int(x, default=90, raise_on_invalid=True)


#################
# Fast Forceint #
#################


@given(float)
def test_fast_forceint_given_float_returns_int(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assert fastnumbers.fast_forceint(x) == int(x)
    assert fastnumbers.fast_forceint(x, raise_on_invalid=True) == int(x)
    assert isinstance(fastnumbers.fast_forceint(x), (int, long))


def test_fast_forceint_given_nan_raises_ValueError():
    with raises(ValueError):
        fastnumbers.fast_forceint(float('nan'))


def test_fast_forceint_given_inf_raises_OverflowError():
    with raises(OverflowError):
        fastnumbers.fast_forceint(float('inf'))


@given(float)
def test_fast_forceint_given_float_string_returns_int(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = repr(x)
    assert fastnumbers.fast_forceint(y) == int(x)
    assert fastnumbers.fast_forceint(y, True) == int(x)
    assert isinstance(fastnumbers.fast_forceint(y), (int, long))


def test_fast_forceint_given_nan_string_raises_ValueError_with_raise_on_invalid_as_True():
    with raises(ValueError): 
        fastnumbers.fast_forceint('nan', raise_on_invalid=True)


def test_fast_forceint_given_inf_string_raises_OverflowError_with_raise_on_invalid_as_True():
    with raises(OverflowError): 
        fastnumbers.fast_forceint('inf', raise_on_invalid=True)
        fastnumbers.fast_forceint('-infinity', raise_on_invalid=True)


@given(float)
def test_fast_forceint_given_padded_float_strings_returns_int(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.fast_forceint(y) == int(x)
    assert isinstance(fastnumbers.fast_forceint(y), (int, long))


@given(int)
def test_fast_forceint_given_int_returns_int(x):
    assert fastnumbers.fast_forceint(x) == x
    assert isinstance(fastnumbers.fast_forceint(x), (int, long))


@given(int)
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


@given(int)
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_fast_forceint_given_padded_int_string_returns_int(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
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


@given(unicode)
def test_fast_forceint_given_unicode_of_more_than_one_char_returns_as_is(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert fastnumbers.fast_int(x) == x


@given(str)
@example('+')
@example('-')
@example('e')
@example('e8')
@example('.')
def test_fast_forceint_given_invalid_string_returns_string_as_is(x):
    assume(not a_number(x))
    assert fastnumbers.fast_forceint(x) is x


@given(str)
def test_fast_forceint_given_invalid_string_raises_ValueError_if_raise_on_invalid_is_True(x):
    assume(not a_number(x))
    with raises(ValueError):
        fastnumbers.fast_forceint(x, raise_on_invalid=True)


@given([int])
def test_fast_forceint_given_invalid_type_raises_TypeError(x):
    with raises(TypeError):
        fastnumbers.fast_forceint(x)


@given(str)
def test_fast_forceint_returns_default_value_if_given_invalid_string(x):
    assume(not a_number(x))
    assert fastnumbers.fast_forceint(x, default=90.0) == 90.0


@given(str)
def test_fast_forceint_returns_raises_ValueError_if_raise_on_invalid_is_True_and_default_is_given(x):
    assume(not a_number(x))
    with raises(ValueError):
        assert fastnumbers.fast_forceint(x, default=90.0, raise_on_invalid=True)


###########
# Is Real #
###########


@given(int)
def test_isreal_returns_True_if_given_int(x):
    assert fastnumbers.isreal(x)


@given(float)
def test_isreal_returns_True_if_given_float(x):
    assert fastnumbers.isreal(x)


@given(int)
def test_isreal_returns_False_if_given_int_and_str_only_is_True(x):
    assert not fastnumbers.isreal(x, str_only=True)


@given(float)
def test_isreal_returns_False_if_given_float_and_str_only_is_True(x):
    assert not fastnumbers.isreal(x, str_only=True)


@given(int)
def test_isreal_returns_True_if_given_int_string_padded_or_not(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.isreal(repr(x))
    assert fastnumbers.isreal(repr(x), str_only=True)
    assert fastnumbers.isreal(y)


@given(float)
def test_isreal_returns_True_if_given_float_string_padded_or_not(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.isreal(repr(x))
    assert fastnumbers.isreal(repr(x), str_only=True)
    assert fastnumbers.isreal(y)


@given(sampled_from(digits))
def test_isreal_given_unicode_digit_returns_True(x):
    assert fastnumbers.isreal(x)
    # Try padded as well
    assert fastnumbers.isreal(u'   ' + x + u'   ')


@given(sampled_from(numeric))
def test_isreal_given_unicode_numeral_returns_True(x):
    assume(x not in digits)
    assume(not unicodedata.numeric(x).is_integer())
    assert fastnumbers.isreal(x)
    # Try padded as well
    assert fastnumbers.isreal(u'   ' + x + u'   ')


@given(sampled_from(not_numeric))
def test_isreal_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isreal(x)


@given(unicode)
def test_isreal_given_unicode_of_more_than_one_char_returns_False(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert not fastnumbers.isreal(x)


@given(str)
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


@given(int)
def test_isfloat_returns_False_if_given_int(x):
    assert not fastnumbers.isfloat(x)


@given(float)
def test_isfloat_returns_True_if_given_float(x):
    assert fastnumbers.isfloat(x)


@given(float)
def test_isfloat_returns_False_if_given_float_and_str_only_is_True(x):
    assert not fastnumbers.isfloat(x, str_only=True)


@given(int)
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_isfloat_returns_True_if_given_int_string_padded_or_not(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.isfloat(repr(x))
    assert fastnumbers.isfloat(repr(x), str_only=True)
    assert fastnumbers.isfloat(y)


@given(float)
def test_isfloat_returns_True_if_given_float_string_padded_or_not(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.isfloat(repr(x))
    assert fastnumbers.isfloat(repr(x), str_only=True)
    assert fastnumbers.isfloat(y)


@given(sampled_from(digits))
def test_isfloat_given_unicode_digit_returns_True(x):
    assert fastnumbers.isfloat(x)
    # Try padded as well
    assert fastnumbers.isfloat(u'   ' + x + u'   ')


@given(sampled_from(numeric))
def test_isfloat_given_unicode_numeral_returns_True(x):
    assume(x not in digits)
    assume(not unicodedata.numeric(x).is_integer())
    assert fastnumbers.isfloat(x)
    # Try padded as well
    assert fastnumbers.isfloat(u'   ' + x + u'   ')


@given(sampled_from(not_numeric))
def test_isfloat_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isfloat(x)


@given(unicode)
def test_isfloat_given_unicode_of_more_than_one_char_returns_False(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert not fastnumbers.isfloat(x)


@given(str)
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


@given(int)
def test_isint_returns_True_if_given_int(x):
    assert fastnumbers.isint(x)


@given(float)
def test_isint_returns_False_if_given_float(x):
    assert not fastnumbers.isint(x)


@given(int)
def test_isint_returns_False_if_given_int_and_str_only_is_True(x):
    assert not fastnumbers.isint(x, str_only=True)


@given(int)
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_isint_returns_True_if_given_int_string_padded_or_not(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.isint(repr(x)) is True
    assert fastnumbers.isint(repr(x), str_only=True)
    assert fastnumbers.isint(y)


@given(float)
def test_isint_returns_False_if_given_float_string_padded_or_not(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert not fastnumbers.isint(repr(x))
    assert not fastnumbers.isint(y)


@given(sampled_from(digits))
def test_isint_given_unicode_digit_returns_True(x):
    assert fastnumbers.isint(x)
    # Try padded as well
    assert fastnumbers.isint(u'   ' + x + u'   ')


@given(sampled_from(numeric))
def test_isint_given_unicode_numeral_returns_False(x):
    assume(x not in digits)
    assume(not unicodedata.numeric(x).is_integer())
    assert not fastnumbers.isint(x)


@given(sampled_from(not_numeric))
def test_isint_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isint(x)


@given(unicode)
def test_isint_given_unicode_of_more_than_one_char_returns_False(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert not fastnumbers.isint(x)


@given(str)
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


@given(int)
def test_isintlike_returns_True_if_given_int(x):
    assert fastnumbers.isintlike(x)


@given(float)
def test_isintlike_returns_False_if_given_non_integer_float(x):
    assume(not x.is_integer())
    assert not fastnumbers.isintlike(x)


@given(float)
def test_isintlike_returns_True_if_given_integer_float(x):
    assume(x.is_integer())
    assert fastnumbers.isintlike(x)


@given(int)
def test_isintlike_returns_False_if_given_int_and_str_only_is_True(x):
    assert not fastnumbers.isintlike(x, str_only=True)


@given(float)
def test_isintlike_returns_False_if_given_integer_float_and_str_only_is_True(x):
    assume(x.is_integer())
    assert not fastnumbers.isintlike(x, str_only=True)


@given(int)
@example(40992764608243448035)
@example(-41538374848935286698640072416676709)
@example(240278958776173358420034462324117625982)
@example(1609422692302207451978552816956662956486)
@example(-121799354242674784350540853922878239740762834)
@example(32718704454132572934419741118153895444518280065843028297496525078)
@example(33684944745210074227862907273261282807602986571245071790093633147269)
def test_isintlike_returns_True_if_given_int_string_padded_or_not(x):
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.isintlike(repr(x))
    assert fastnumbers.isintlike(repr(x), str_only=True)
    assert fastnumbers.isintlike(y)


@given(float)
def test_isintlike_returns_True_if_given_integer_float_string_padded_or_not(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assume(x.is_integer())
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert fastnumbers.isintlike(repr(x))
    assert fastnumbers.isintlike(y)


@given(float)
def test_isintlike_returns_False_if_given_non_integer_float_string_padded_or_not(x):
    assume(not math.isnan(x))
    assume(not math.isinf(x))
    assume(not x.is_integer())
    y = ''.join(repeat(' ', randint(0, 100))) + repr(x) + ''.join(repeat(' ', randint(0, 100)))
    assert not fastnumbers.isintlike(repr(x))
    assert not fastnumbers.isintlike(y)


@given(sampled_from(digits))
def test_isintlike_given_unicode_digit_returns_True(x):
    assert fastnumbers.isintlike(x)
    # Try padded as well
    assert fastnumbers.isintlike(u'   ' + x + u'   ')


@given(sampled_from(numeric))
def test_isintlike_given_unicode_non_digit_numeral_returns_False(x):
    assume(x not in digits)
    assume(not unicodedata.numeric(x).is_integer())
    assert not fastnumbers.isintlike(x)


@given(sampled_from(numeric))
def test_isintlike_given_unicode_digit_numeral_returns_False(x):
    assume(x not in digits)
    assume(unicodedata.numeric(x).is_integer())
    assert fastnumbers.isintlike(x)
    # Try padded as well
    assert fastnumbers.isintlike(u'   ' + x + u'   ')


@given(sampled_from(not_numeric))
def test_isintlike_given_unicode_non_numeral_returns_False(x):
    assert not fastnumbers.isintlike(x)


@given(unicode)
def test_isintlike_given_unicode_of_more_than_one_char_returns_False(x):
    assume(len(x) > 1)
    assume(not a_number(x))
    assert not fastnumbers.isintlike(x)


@given(str)
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
