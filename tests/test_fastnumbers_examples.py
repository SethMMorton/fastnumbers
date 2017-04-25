# -*- coding: utf-8 -*-
# Find the build location and add that to the path
from __future__ import print_function, division
import sys
import math
from platform import python_version_tuple
import pytest
from pytest import raises
import fastnumbers

if python_version_tuple()[0] == '3':
    long = int


# Each conversion test should test the following
# 1. float number
# 2. signed float string
# 3. float string with exponents
# 4. float string with padded whitespace
# 5. int number
# 6. signed int string
# 7. int string with padded whitespace
# 8. long number
# 9. long number
# 10. return type
# 11. TypeError for invalid input
# 12. Invalid input string
# 13. Invalid input string with numbers
# 14. Infinity
# 15. NaN
# 16. Sign/'e'/'.' only
# 17. Default value.
# 18. Unicode numbers


def test_fast_real():
    # 1. float number
    assert fastnumbers.fast_real(-367.3268) == -367.3268
    assert fastnumbers.fast_real(-367.3268, raise_on_invalid=True) == -367.3268
    # 2. signed float string
    assert fastnumbers.fast_real("+367.3268") == +367.3268
    assert fastnumbers.fast_real("+367.3268", True) == +367.3268
    # 3. float string with exponents
    assert fastnumbers.fast_real("-367.3268e207") == -367.3268e207
    assert fastnumbers.fast_real('1.175494351e-3810000000') == 0.0
    # 4. float string with padded whitespace
    assert fastnumbers.fast_real("   -367.04   ") == -367.04
    # 5. int number
    assert fastnumbers.fast_real(499) == 499
    # 6. signed int string
    assert fastnumbers.fast_real('-499') == -499
    # 7. int string with padded whitespace
    assert fastnumbers.fast_real('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.fast_real(35892482945872302493) == 35892482945872302493
    # 9. long string
    if python_version_tuple()[0] == '2':
        assert fastnumbers.fast_real("35892482945872302493L") == 35892482945872302493
        assert fastnumbers.fast_real("35892482945872302493l") == 35892482945872302493
    assert fastnumbers.fast_real("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.fast_real(4029), int)
    assert isinstance(fastnumbers.fast_real(4029.0, coerce=False), float)
    assert isinstance(fastnumbers.fast_real(4029), int)
    assert isinstance(fastnumbers.fast_real(4029.0), int)
    assert isinstance(fastnumbers.fast_real(4029.5), float)
    assert isinstance(fastnumbers.fast_real("4029"), int)
    assert isinstance(fastnumbers.fast_real("4029.0"), int)
    assert isinstance(fastnumbers.fast_real("4029.0", coerce=False), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.fast_real(['hey'])
    # 12. Invalid input string
    assert fastnumbers.fast_real('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.fast_real('not_a_number', raise_on_invalid=True)
    # 13. Invalid input string with numbers
    assert fastnumbers.fast_real('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.fast_real('26.8 lb', None, True)
    # 14. Infinity
    assert fastnumbers.fast_real('inf') == float('inf')
    assert fastnumbers.fast_real('-iNFinity') == float('-inf')
    assert fastnumbers.fast_real('-iNFinity', inf=7608) == 7608
    # 15. NaN
    assert math.isnan(fastnumbers.fast_real('nan'))
    assert math.isnan(fastnumbers.fast_real('-NaN'))
    assert fastnumbers.fast_real('-NaN', nan=0) == 0
    # 16. Sign/'e'/'.' only
    assert fastnumbers.fast_real('+') == '+'
    assert fastnumbers.fast_real('-') == '-'
    assert fastnumbers.fast_real('e') == 'e'
    assert fastnumbers.fast_real('.') == '.'
    # 17. Default on invalid... 'raise_on_invalid' supersedes
    assert fastnumbers.fast_real('invalid', default=90) == 90
    assert fastnumbers.fast_real('invalid', default=None) is None
    with raises(ValueError):
        assert fastnumbers.fast_real('invalid', 90, True)
    # 18. Unicode numbers
    assert fastnumbers.fast_real(u'⑦') == 7
    assert fastnumbers.fast_real(u'⁸') == 8
    assert fastnumbers.fast_real(u'⅔') == 2.0 / 3.0
    assert fastnumbers.fast_real(u'Ⅴ') == 5
    # 19. Key function
    assert fastnumbers.fast_real(76.8, key=len) == 76.8
    assert fastnumbers.fast_real('76.8', key=len) == 76.8
    assert fastnumbers.fast_real('invalid', key=len) == 7


def test_fast_float():
    # 1. float number
    assert fastnumbers.fast_float(-367.3268) == -367.3268
    assert fastnumbers.fast_float(-367.3268, raise_on_invalid=True) == -367.3268
    # 2. signed float string
    assert fastnumbers.fast_float("+367.3268") == +367.3268
    assert fastnumbers.fast_float("+367.3268", True) == +367.3268
    # 3. float string with exponents
    assert fastnumbers.fast_float("-367.3268e27") == -367.3268e27
    assert fastnumbers.fast_float("-367.3268E27") == -367.3268E27
    assert fastnumbers.fast_float("-367.3268e207") == -367.3268e207
    assert fastnumbers.fast_float('1.175494351E-3810000000') == 0.0
    # 4. float string with padded whitespace
    assert fastnumbers.fast_float("   -367.04   ") == -367.04
    # 5. int number
    assert fastnumbers.fast_float(499) == 499.0
    # 6. signed int string
    assert fastnumbers.fast_float('-499') == -499.0
    # 7. int string with padded whitespace
    assert fastnumbers.fast_float('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.fast_float(35892482945872302493) == 35892482945872302493.0
    # 9. long string
    if python_version_tuple()[0] == '2':
        assert fastnumbers.fast_float("35892482945872302493L") == 35892482945872302493.0
        assert fastnumbers.fast_float("35892482945872302493l") == 35892482945872302493.0
    assert fastnumbers.fast_float("35892482945872302493") == 35892482945872302493.0
    # 10. return type
    assert isinstance(fastnumbers.fast_float(4029), float)
    assert isinstance(fastnumbers.fast_float("4029"), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.fast_float(['hey'])
    # 12. Invalid input string
    assert fastnumbers.fast_float('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.fast_float('not_a_number', raise_on_invalid=True)
    # 13. Invalid input string with numbers
    assert fastnumbers.fast_float('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.fast_float('26.8 lb', None, True)
    # 14. Infinity
    assert fastnumbers.fast_float('inf') == float('inf')
    assert fastnumbers.fast_float('-iNFinity') == float('-inf')
    assert fastnumbers.fast_float('-iNFinity', inf=523) == 523
    # 15. NaN
    assert math.isnan(fastnumbers.fast_float('nAn'))
    assert math.isnan(fastnumbers.fast_float('-NaN'))
    assert fastnumbers.fast_float('-NaN', nan=0) == 0
    # 16. Sign/'e'/'.' only
    assert fastnumbers.fast_float('+') == '+'
    assert fastnumbers.fast_float('-') == '-'
    assert fastnumbers.fast_float('e') == 'e'
    assert fastnumbers.fast_float('.') == '.'
    # 17. Default on invalid... 'raise_on_invalid' supersedes
    assert fastnumbers.fast_float('invalid', default=90) == 90
    assert fastnumbers.fast_float('invalid', default=None) is None
    with raises(ValueError):
        assert fastnumbers.fast_float('invalid', 90, True)
    # 18. Unicode numbers
    assert fastnumbers.fast_float(u'⑦') == 7.0
    assert fastnumbers.fast_float(u'⁸') == 8.0
    assert fastnumbers.fast_float(u'⅔') == 2.0 / 3.0
    assert fastnumbers.fast_float(u'Ⅴ') == 5.0
    # 19. Key function
    assert fastnumbers.fast_float(76.8, key=len) == 76.8
    assert fastnumbers.fast_float('76.8', key=len) == 76.8
    assert fastnumbers.fast_float('invalid', key=len) == 7


def test_fast_int():
    # 1. float number
    assert fastnumbers.fast_int(-367.3268) == -367
    assert fastnumbers.fast_int(-367.3268, raise_on_invalid=True) == -367
    # 2. signed float string
    assert fastnumbers.fast_int("+367.3268") == "+367.3268"
    with raises(ValueError):
        assert fastnumbers.fast_int("+367.3268", None, True)
    # 3. float string with exponents
    assert fastnumbers.fast_int("-367.3268e207") == "-367.3268e207"
    # 4. float string with padded whitespace
    assert fastnumbers.fast_int("   -367.04   ") == "   -367.04   "
    # 5. int number
    assert fastnumbers.fast_int(499) == 499
    # 6. signed int string
    assert fastnumbers.fast_int('-499') == -499
    # 7. int string with padded whitespace
    assert fastnumbers.fast_int('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.fast_int(35892482945872302493) == 35892482945872302493
    # 9. long string
    if python_version_tuple()[0] == '2':
        assert fastnumbers.fast_int("35892482945872302493L") == 35892482945872302493
        assert fastnumbers.fast_int("35892482945872302493l") == 35892482945872302493
    assert fastnumbers.fast_int("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.fast_int(4029.00), int)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.fast_int(['hey'])
    # 12. Invalid input string
    assert fastnumbers.fast_int('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.fast_int('not_a_number', raise_on_invalid=True)
   # 13. Invalid input string with numbers
    assert fastnumbers.fast_int('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.fast_int('26.8 lb', None, True)
    # 14. Infinity
    assert fastnumbers.fast_int('inf') == 'inf'
    # 15. NaN
    assert fastnumbers.fast_int('nan') == 'nan'
    # 16. Sign/'e'/'.' only
    assert fastnumbers.fast_int('+') == '+'
    assert fastnumbers.fast_int('-') == '-'
    assert fastnumbers.fast_int('e') == 'e'
    assert fastnumbers.fast_int('.') == '.'
    # 17. Default on invalid... 'raise_on_invalid' supersedes
    assert fastnumbers.fast_int('invalid', default=90) == 90
    assert fastnumbers.fast_int('invalid', default=None) is None
    with raises(ValueError):
        assert fastnumbers.fast_int('invalid', 90, True)
    # 18. Unicode numbers
    assert fastnumbers.fast_int(u'⑦') == 7
    assert fastnumbers.fast_int(u'⁸') == 8
    assert fastnumbers.fast_int(u'⁸', base=10) == u'⁸'
    assert fastnumbers.fast_int(u'⅔') == u'⅔'
    assert fastnumbers.fast_int(u'Ⅴ') == u'Ⅴ'
    # 19. Key function
    assert fastnumbers.fast_int(76, key=len) == 76
    assert fastnumbers.fast_int('76', key=len) == 76
    assert fastnumbers.fast_int('invalid', key=len) == 7


def test_fast_forceint():
    # 1. float number
    assert fastnumbers.fast_forceint(-367.3268) == -367
    assert fastnumbers.fast_forceint(-367.3268, raise_on_invalid=True) == -367
    # 2. signed float string
    assert fastnumbers.fast_forceint("+367.3268") == 367
    assert fastnumbers.fast_forceint("+367.3268", raise_on_invalid=True) == 367
    # 3. float string with exponents
    assert fastnumbers.fast_forceint("-367.3268e207") == long(-367.3268e207)
    # 4. float string with padded whitespace
    assert fastnumbers.fast_forceint("   -367.04   ") == -367
    # 5. int number
    assert fastnumbers.fast_forceint(499) == 499
    # 6. signed int string
    assert fastnumbers.fast_forceint('-499') == -499
    # 7. int string with padded whitespace
    assert fastnumbers.fast_forceint('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.fast_forceint(35892482945872302493) == 35892482945872302493
    # 9. long string
    assert fastnumbers.fast_forceint("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.fast_forceint(4029.00), int)
    assert isinstance(fastnumbers.fast_forceint("4029.00"), int)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.fast_forceint(['hey'])
    # 12. Invalid input string
    assert fastnumbers.fast_forceint('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.fast_forceint('not_a_number', raise_on_invalid=True)
    # 13. Invalid input string with numbers
    assert fastnumbers.fast_forceint('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.fast_forceint('26.8 lb', None, True)
    # 14. Infinity
    assert fastnumbers.fast_forceint('inf') == 'inf'
    assert fastnumbers.fast_forceint('-iNFinity') == '-iNFinity'
    # 15. NaN
    assert fastnumbers.fast_forceint('nan') == 'nan'
    # 16. Sign/'e'/'.' only
    assert fastnumbers.fast_forceint('+') == '+'
    assert fastnumbers.fast_forceint('-') == '-'
    assert fastnumbers.fast_forceint('e') == 'e'
    assert fastnumbers.fast_forceint('.') == '.'
    # 17. Default on invalid... 'raise_on_invalid' supersedes
    assert fastnumbers.fast_forceint('invalid', default=90) == 90
    assert fastnumbers.fast_forceint('invalid', default=None) is None
    with raises(ValueError):
        assert fastnumbers.fast_forceint('invalid', 90, True)
    # 18. Unicode numbers
    assert fastnumbers.fast_forceint(u'⑦') == 7
    assert fastnumbers.fast_forceint(u'⁸') == 8
    assert fastnumbers.fast_forceint(u'⅔') == 0
    assert fastnumbers.fast_forceint(u'Ⅴ') == 5
    # 19. Key function
    assert fastnumbers.fast_forceint(76.8, key=len) == 76
    assert fastnumbers.fast_forceint('76.8', key=len) == 76
    assert fastnumbers.fast_forceint('invalid', key=len) == 7


def test_isreal():
    # 1. float number
    assert fastnumbers.isreal(-367.3268)
    assert not fastnumbers.isreal(-367.3268, str_only=True)
    assert fastnumbers.isreal(-367.3268, num_only=True)
    # 2. signed float string
    assert fastnumbers.isreal("+367.3268")
    assert fastnumbers.isreal("+367.3268", True)
    assert not fastnumbers.isreal("+367.3268", num_only=True)
    # 3. float string with exponents
    assert fastnumbers.isreal("-367.3268e207")
    # 4. float string with padded whitespace
    assert fastnumbers.isreal("   -367.04   ")
    # 5. int number
    assert fastnumbers.isreal(499)
    # 6. signed int string
    assert fastnumbers.isreal('-499')
    # 7. int string with padded whitespace
    assert fastnumbers.isreal('   +3001   ')
    # 8. long number
    assert fastnumbers.isreal(35892482945872302493)
    # 9. long string
    assert fastnumbers.isreal("35892482945872302493")
    # 10. return type
    assert fastnumbers.isreal(4029) is True
    assert fastnumbers.isreal(4029, str_only=True) is False
    assert fastnumbers.isreal("4029") is True
    assert fastnumbers.isreal("4029", True) is True
    assert fastnumbers.isreal("hey") is False
    # 11. TypeError for invalid input
    assert not fastnumbers.isreal(['hey'])
    # 12. Invalid input string
    assert not fastnumbers.isreal('not_a_number')
    # 13. Invalid input string with numbers
    assert not fastnumbers.isreal('26.8 lb')
    # 14. Infinity
    assert not fastnumbers.isreal('inf')
    assert fastnumbers.isreal('inf', allow_inf=True)
    assert fastnumbers.isreal('-iNFinity', allow_inf=True)
    # 15. NaN
    assert not fastnumbers.isreal('nan')
    assert fastnumbers.isreal('nan', allow_nan=True)
    assert fastnumbers.isreal('-NaN', allow_nan=True)
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.isreal('+')
    assert not fastnumbers.isreal('-')
    assert not fastnumbers.isreal('e')
    assert not fastnumbers.isreal('.')
    # 18. Unicode numbers
    assert fastnumbers.isreal(u'⑦')
    assert fastnumbers.isreal(u'⁸')
    assert fastnumbers.isreal(u'⅔')
    assert fastnumbers.isreal(u'Ⅴ')


def test_isfloat():
    # 1. float number
    assert fastnumbers.isfloat(-367.3268)
    assert not fastnumbers.isfloat(-367.3268, str_only=True)
    assert fastnumbers.isfloat(-367.3268, num_only=True)
    # 2. signed float string
    assert fastnumbers.isfloat("+367.3268")
    assert fastnumbers.isfloat("+367.3268", True)
    assert not fastnumbers.isfloat("+367.3268", num_only=True)
    # 3. float string with exponents
    assert fastnumbers.isfloat("-367.3268e207")
    # 4. float string with padded whitespace
    assert fastnumbers.isfloat("   -367.04   ")
    # 5. int number
    assert not fastnumbers.isfloat(499)
    # 6. signed int string
    assert fastnumbers.isfloat('-499')
    # 7. int string with padded whitespace
    assert fastnumbers.isfloat('   +3001   ')
    # 8. long number
    assert not fastnumbers.isfloat(35892482945872302493)
    # 9. long string
    assert fastnumbers.isfloat("35892482945872302493")
    # 10. return type
    assert fastnumbers.isfloat(4029) is False
    assert fastnumbers.isfloat(4029.0) is True
    assert fastnumbers.isfloat(4029.0, str_only=True) is False
    assert fastnumbers.isfloat("4029") is True
    assert fastnumbers.isfloat("4029", True) is True
    # 11. TypeError for invalid input
    assert not fastnumbers.isfloat(['hey'])
    # 12. Invalid input string
    assert not fastnumbers.isfloat('not_a_number')
    # 13. Invalid input string with numbers
    assert not fastnumbers.isfloat('26.8 lb')
    # 14. Infinity
    assert not fastnumbers.isfloat('inf')
    assert fastnumbers.isfloat('inf', allow_inf=True)
    assert fastnumbers.isfloat('-infinity', allow_inf=True)
    assert fastnumbers.isfloat('-INFINITY', allow_inf=True)
    # 15. NaN
    assert not fastnumbers.isfloat('nAn')
    assert fastnumbers.isfloat('nan', allow_nan=True)
    assert fastnumbers.isfloat('-NaN', allow_nan=True)
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.isfloat('+')
    assert not fastnumbers.isfloat('-')
    assert not fastnumbers.isfloat('e')
    assert not fastnumbers.isfloat('.')
    # 18. Unicode numbers
    assert fastnumbers.isfloat(u'⑦')
    assert fastnumbers.isfloat(u'⁸')
    assert fastnumbers.isfloat(u'⅔')
    assert fastnumbers.isfloat(u'Ⅴ')


def test_isint():
    # 1. float number
    assert not fastnumbers.isint(-367.3268)
    # 2. signed float string
    assert not fastnumbers.isint("+367.3268")
    # 3. float string with exponents
    assert not fastnumbers.isint("-367.3268e207")
    # 4. float string with padded whitespace
    assert not fastnumbers.isint("   -367.04   ")
    # 5. int number
    assert fastnumbers.isint(499)
    assert not fastnumbers.isint(499, str_only=True)
    assert fastnumbers.isint(499, num_only=True)
    # 6. signed int string
    assert fastnumbers.isint('-499')
    assert fastnumbers.isint('-499', True)
    assert not fastnumbers.isint('-499', num_only=True)
    # 7. int string with padded whitespace
    assert fastnumbers.isint('   +3001   ')
    # 8. long number
    assert fastnumbers.isint(35892482945872302493)
    # 9. long string
    assert fastnumbers.isint("35892482945872302493")
    # 10. return type
    assert fastnumbers.isint(4029) is True
    assert fastnumbers.isint(4029, str_only=True) is False
    assert fastnumbers.isint("4029") is True
    assert fastnumbers.isint("4029", True) is True
    assert fastnumbers.isint("4029.50") is False
    assert fastnumbers.isint(4029.50) is False
    # 11. TypeError for invalid input
    assert not fastnumbers.isint(['hey'])
    # 12. Invalid input string
    assert not fastnumbers.isint('not_a_number')
    # 13. Invalid input string with numbers
    assert not fastnumbers.isint('26.8 lb')
    # 14. Infinity
    assert not fastnumbers.isint('inf')
    # 15. NaN
    assert not fastnumbers.isint('nan')
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.isint('+')
    assert not fastnumbers.isint('-')
    assert not fastnumbers.isint('e')
    assert not fastnumbers.isint('.')
    # 18. Unicode numbers
    assert fastnumbers.isint(u'⑦')
    assert fastnumbers.isint(u'⁸')
    assert not fastnumbers.isint(u'⅔')
    assert not fastnumbers.isint(u'Ⅴ')


def test_isintlike():
    # 1. float number
    assert not fastnumbers.isintlike(-367.3268)
    assert fastnumbers.isintlike(-367.0)
    assert not fastnumbers.isintlike(-367.0, str_only=True)
    assert fastnumbers.isintlike(-367.0, num_only=True)
    # 2. signed float string
    assert not fastnumbers.isintlike("+367.3268")
    assert fastnumbers.isintlike("+367.0")
    assert fastnumbers.isintlike("+367.0", True)
    assert not fastnumbers.isintlike("+367.0", num_only=True)
    # 3. float string with exponents
    assert fastnumbers.isintlike("-367.3268e207")
    assert not fastnumbers.isintlike("145343E-4")
    assert fastnumbers.isintlike("14534.000000000e4")
    assert fastnumbers.isintlike("1400000E-4")
    assert not fastnumbers.isintlike("140E-4")
    assert fastnumbers.isintlike("14.E4")
    assert fastnumbers.isintlike("14E4")
    # 4. float string with padded whitespace
    assert not fastnumbers.isintlike("   -367.04   ")
    # 5. int number
    assert fastnumbers.isintlike(499)
    # 6. signed int string
    assert fastnumbers.isintlike('-499')
    # 7. int string with padded whitespace
    assert fastnumbers.isintlike('   +3001   ')
    # 8. long number
    assert fastnumbers.isintlike(35892482945872302493)
    # 9. long string
    if python_version_tuple()[0] == '2':
        assert fastnumbers.isintlike("35892482945872302493l")
        assert fastnumbers.isintlike("35892482945872302493L")
    assert fastnumbers.isintlike("35892482945872302493")
    # 10. return type
    assert fastnumbers.isintlike(4029) is True
    assert fastnumbers.isintlike(4029, str_only=True) is False
    assert fastnumbers.isintlike("4029") is True
    assert fastnumbers.isintlike("4029", True) is True
    assert fastnumbers.isintlike("4029.50") is False
    assert fastnumbers.isintlike(4029.50) is False
    # 11. TypeError for invalid input
    assert not fastnumbers.isintlike(['hey'])
    # 12. Invalid input string
    assert not fastnumbers.isintlike('not_a_number')
    # 13. Invalid input string with numbers
    assert not fastnumbers.isintlike('26.8 lb')
    # 14. Infinity
    assert not fastnumbers.isintlike('inf')
    # 15. NaN
    assert not fastnumbers.isintlike('nan')
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.isintlike('+')
    assert not fastnumbers.isintlike('-')
    assert not fastnumbers.isintlike('e')
    assert not fastnumbers.isintlike('.')
    # 18. Unicode numbers
    assert fastnumbers.isintlike(u'⑦')
    assert fastnumbers.isintlike(u'⁸')
    assert not fastnumbers.isintlike(u'⅔')
    assert fastnumbers.isintlike(u'Ⅴ')


@pytest.fixture()
def tprint(capsys):
    """Fixture for printing info after test, not supressed by pytest stdout/stderr capture"""
    lines = []
    yield lines.append

    with capsys.disabled():
        for line in lines:
            sys.stdout.write('\n{}'.format(line))


def test_print_limits(tprint):
    tprint('\nFASNUMBERS NUMERICAL LIMITS FOR THIS COMPILER BEFORE PYTHON FALLBACK:')
    tprint('MAXIMUM INTEGER LENTH: {}'.format(fastnumbers.max_int_len))
    tprint('MAX NUMBER FLOAT DIGITS: {}'.format(fastnumbers.dig))
    tprint('MAXIMUM FLOAT EXPONENT: {}'.format(fastnumbers.max_exp))
    tprint('MINIMUM FLOAT EXPONENT: {}'.format(fastnumbers.min_exp))
    tprint('')
