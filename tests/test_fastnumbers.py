# Find the build location and add that to the path
from __future__ import print_function, division
import sys
import os
import math
from platform import python_version_tuple
from pytest import raises
import fastnumbers

if python_version_tuple()[0] == '3':
    long = int


def test_version():
    assert hasattr(fastnumbers, '__version__')


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
# 16. Sign only

def test_safe_real():
    # 1. float number
    assert fastnumbers.safe_real(-367.3268) == -367.3268
    assert fastnumbers.safe_real(-367.3268, raise_on_invalid=True) == -367.3268
    # 2. signed float string
    assert fastnumbers.safe_real("+367.3268") == +367.3268
    assert fastnumbers.safe_real("+367.3268", True) == +367.3268
    # 3. float string with exponents
    assert fastnumbers.safe_real("-367.3268e207") == -367.3268e207
    # 4. float string with padded whitespace
    assert fastnumbers.safe_real("   -367.04   ") == -367.04
    # 5. int number
    assert fastnumbers.safe_real(499) == 499
    # 6. signed int string
    assert fastnumbers.safe_real('-499') == -499
    # 7. int string with padded whitespace
    assert fastnumbers.safe_real('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.safe_real(35892482945872302493) == 35892482945872302493
    # 9. long string
    assert fastnumbers.safe_real("35892482945872302493") == 35892482945872302493
    assert fastnumbers.safe_real("35892482945872302E3") == 3.5892482945872302e+19
    # 10. return type
    assert isinstance(fastnumbers.safe_real(4029), int)
    assert isinstance(fastnumbers.safe_real(4029.0), float)
    assert isinstance(fastnumbers.safe_real("4029"), int)
    assert isinstance(fastnumbers.safe_real("4029.0"), int)
    assert isinstance(fastnumbers.safe_real("4029.5"), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.safe_real(['hey'])
    # 12. Invalid input string
    assert fastnumbers.safe_real('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.safe_real('not_a_number', raise_on_invalid=True)
    # 13. Invalid input string with numbers
    assert fastnumbers.safe_real('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.safe_real('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.safe_real('inf') == float('inf')
    assert fastnumbers.safe_real('-iNFinity') == float('-inf')
    # 15. NaN
    assert math.isnan(fastnumbers.safe_real('nan'))
    assert math.isnan(fastnumbers.safe_real('-NaN'))
    # 16. Sign only
    assert fastnumbers.safe_real('+') == '+'
    assert fastnumbers.safe_real('-') == '-'


def test_safe_float():
    # 1. float number
    assert fastnumbers.safe_float(-367.3268) == -367.3268
    assert fastnumbers.safe_float(-367.3268, raise_on_invalid=True) == -367.3268
    # 2. signed float string
    assert fastnumbers.safe_float("+367.3268") == +367.3268
    assert fastnumbers.safe_float("+367.3268", True) == +367.3268
    # 3. float string with exponents
    assert fastnumbers.safe_float("-367.3268e207") == -367.3268e207
    # 4. float string with padded whitespace
    assert fastnumbers.safe_float("   -367.04   ") == -367.04
    # 5. int number
    assert fastnumbers.safe_float(499) == 499.0
    # 6. signed int string
    assert fastnumbers.safe_float('-499') == -499.0
    # 7. int string with padded whitespace
    assert fastnumbers.safe_float('   +3001   ') == 3001.0
    # 8. long number
    assert fastnumbers.safe_float(35892482945872302493) == 35892482945872302493.0
    # 9. long string
    assert fastnumbers.safe_float("35892482945872302493") == 35892482945872302493.0
    # 10. return type
    assert isinstance(fastnumbers.safe_float(4029), float)
    assert isinstance(fastnumbers.safe_float("4029"), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.safe_float(['hey'])
    # 12. Invalid input string
    assert fastnumbers.safe_float('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.safe_float('not_a_number', raise_on_invalid=True)
    # 13. Invalid input string with numbers
    assert fastnumbers.safe_float('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.safe_float('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.safe_float('inf') == float('inf')
    assert fastnumbers.safe_float('-iNFinity') == float('-inf')
    # 15. NaN
    assert math.isnan(fastnumbers.safe_float('nan'))
    assert math.isnan(fastnumbers.safe_float('-NaN'))
    # 16. Sign only
    assert fastnumbers.safe_float('+') == '+'
    assert fastnumbers.safe_float('-') == '-'


def test_safe_int():
    # 1. float number
    assert fastnumbers.safe_int(-367.3268) == -367
    assert fastnumbers.safe_int(-367.3268, raise_on_invalid=True) == -367
    # 2. signed float string
    assert fastnumbers.safe_int("+367.3268") == "+367.3268"
    with raises(ValueError):
        assert fastnumbers.safe_int("+367.3268", True)
    # 3. float string with exponents
    assert fastnumbers.safe_int("-367.3268e207") == "-367.3268e207"
    # 4. float string with padded whitespace
    assert fastnumbers.safe_int("   -367.04   ") == "   -367.04   "
    # 5. int number
    assert fastnumbers.safe_int(499) == 499
    # 6. signed int string
    assert fastnumbers.safe_int('-499') == -499
    # 7. int string with padded whitespace
    assert fastnumbers.safe_int('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.safe_int(35892482945872302493) == 35892482945872302493
    # 9. long string
    assert fastnumbers.safe_int("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.safe_int(4029.00), int)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.safe_int(['hey'])
    # 12. Invalid input string
    assert fastnumbers.safe_int('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.safe_int('not_a_number', raise_on_invalid=True)
    # 13. Invalid input string with numbers
    assert fastnumbers.safe_int('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.safe_int('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.safe_int('inf') == 'inf'
    # 15. NaN
    assert fastnumbers.safe_int('nan') == 'nan'
    # 16. Sign only
    assert fastnumbers.safe_int('+') == '+'
    assert fastnumbers.safe_int('-') == '-'


def test_safe_forceint():
    # 1. float number
    assert fastnumbers.safe_forceint(-367.3268) == -367
    assert fastnumbers.safe_forceint(-367.3268, raise_on_invalid=True) == -367
    # 2. signed float string
    assert fastnumbers.safe_forceint("+367.3268") == 367
    assert fastnumbers.safe_forceint("+367.3268", True) == 367
    # 3. float string with exponents
    assert fastnumbers.safe_forceint("-367.3268e207") == long(-367.3268e207)
    assert fastnumbers.safe_forceint("-367.3268e-2") == -3
    # 4. float string with padded whitespace
    assert fastnumbers.safe_forceint("   -367.04   ") == -367
    # 5. int number
    assert fastnumbers.safe_forceint(499) == 499
    # 6. signed int string
    assert fastnumbers.safe_forceint('-499') == -499
    # 7. int string with padded whitespace
    assert fastnumbers.safe_forceint('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.safe_forceint(35892482945872302493) == 35892482945872302493
    # 9. long string
    assert fastnumbers.safe_forceint("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.safe_forceint(4029.00), int)
    assert isinstance(fastnumbers.safe_forceint("4029.00"), int)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.safe_forceint(['hey'])
    # 12. Invalid input string
    assert fastnumbers.safe_forceint('not_a_number') == 'not_a_number'
    with raises(ValueError):
        assert fastnumbers.safe_forceint('not_a_number', raise_on_invalid=True)
    # 13. Invalid input string with numbers
    assert fastnumbers.safe_forceint('26.8 lb') == '26.8 lb'
    with raises(ValueError):
        assert fastnumbers.safe_forceint('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.safe_forceint('inf') == sys.maxsize
    assert fastnumbers.safe_forceint('-iNFinity') == -sys.maxsize - 1
    # 15. NaN
    assert fastnumbers.safe_forceint('nan') == 'nan'
    # 16. Sign only
    assert fastnumbers.safe_forceint('+') == '+'
    assert fastnumbers.safe_forceint('-') == '-'


def test_fast_real():
    # 1. float number
    assert fastnumbers.fast_real(-367.3268) == -367.3268
    assert fastnumbers.fast_real(-367.3268, raise_on_invalid=True) == -367.3268
    # 2. signed float string
    assert fastnumbers.fast_real("+367.3268") == +367.3268
    assert fastnumbers.fast_real("+367.3268", True) == +367.3268
    # 3. float string with exponents
    #    The fast_real function is not always as accurate for large numbers
    #    so the results are *almost* equal (12 digits isn't bad!).
    assert fastnumbers.fast_real("-367.3268e207") != -367.3268e207
    assert abs(fastnumbers.fast_real("-367.3268e207") - -367.3268e207) < 1e195
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
    assert fastnumbers.fast_real("35892482945872302493") != 35892482945872302493
    assert abs(fastnumbers.fast_real("35892482945872302493") - 35892482945872302493) < 1e4
    assert fastnumbers.fast_real("35892482945872302E3") == 3.5892482945872302e+19
    assert fastnumbers.fast_real("10007199254740992") == 10007199254740992
    # 10. return type
    assert isinstance(fastnumbers.fast_real(4029), int)
    assert isinstance(fastnumbers.fast_real(4029.0), float)
    assert isinstance(fastnumbers.fast_real("4029"), int)
    assert isinstance(fastnumbers.fast_real("4029.0"), int)
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
        assert fastnumbers.fast_real('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.fast_real('inf') == float('inf')
    assert fastnumbers.fast_real('-iNFinity') == float('-inf')
    # 15. NaN
    assert math.isnan(fastnumbers.fast_real('nan'))
    assert math.isnan(fastnumbers.fast_real('-NaN'))
    # 16. Sign only
    assert fastnumbers.fast_real('+') == '+'
    assert fastnumbers.fast_real('-') == '-'


def test_fast_float():
    # 1. float number
    assert fastnumbers.fast_float(-367.3268) == -367.3268
    assert fastnumbers.fast_float(-367.3268, raise_on_invalid=True) == -367.3268
    # 2. signed float string
    assert fastnumbers.fast_float("+367.3268") == +367.3268
    assert fastnumbers.fast_float("+367.3268", True) == +367.3268
    # 3. float string with exponents
    #    The fast_float function is not always as accurate for large numbers
    #    so the results are *almost* equal (12 digits isn't bad!).
    assert fastnumbers.fast_float("-367.3268e207") != -367.3268e207
    assert abs(fastnumbers.fast_float("-367.3268e207") - -367.3268e207) < 1e195
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
    assert fastnumbers.fast_float("35892482945872302493") != 35892482945872302493.0
    assert abs(fastnumbers.fast_float("35892482945872302493") - 35892482945872302493.0) < 1e4
    assert fastnumbers.fast_float("10007199254740992") == 10007199254740992.0
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
        assert fastnumbers.fast_float('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.fast_float('inf') == float('inf')
    assert fastnumbers.fast_float('-iNFinity') == float('-inf')
    # 15. NaN
    assert math.isnan(fastnumbers.fast_float('nan'))
    assert math.isnan(fastnumbers.fast_float('-NaN'))
    # 16. Sign only
    assert fastnumbers.fast_float('+') == '+'
    assert fastnumbers.fast_float('-') == '-'


def test_fast_int():
    # 1. float number
    assert fastnumbers.fast_int(-367.3268) == -367
    assert fastnumbers.fast_int(-367.3268, raise_on_invalid=True) == -367
    # 2. signed float string
    assert fastnumbers.fast_int("+367.3268") == "+367.3268"
    with raises(ValueError):
        assert fastnumbers.fast_int("+367.3268", True)
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
    #    overflow is not checked in the fast_int method
    assert fastnumbers.fast_int("35892482945872302493") == -1001005201546800739
    assert fastnumbers.fast_int("10007199254740992") == 10007199254740992
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
        assert fastnumbers.fast_int('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.fast_int('inf') == 'inf'
    # 15. NaN
    assert fastnumbers.fast_int('nan') == 'nan'
    # 16. Sign only
    assert fastnumbers.fast_int('+') == '+'
    assert fastnumbers.fast_int('-') == '-'


def test_fast_forceint():
    # 1. float number
    assert fastnumbers.fast_forceint(-367.3268) == -367
    assert fastnumbers.fast_forceint(-367.3268, raise_on_invalid=True) == -367
    # 2. signed float string
    assert fastnumbers.fast_forceint("+367.3268") == 367
    assert fastnumbers.fast_forceint("+367.3268", raise_on_invalid=True) == 367
    # 3. float string with exponents
    #    watch out for overflow!
    assert fastnumbers.fast_forceint("-367.3268e207") == -9223372036854775808
    assert fastnumbers.fast_forceint("-367.3268e12") == long(-367.3268e12)
    assert fastnumbers.safe_forceint("-367.3268e-2") == -3
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
    #    overflow is not checked in the fast_forceint method
    #    the overflow value can come be different from fast_int
    assert fastnumbers.fast_forceint("35892482945872302493") == -9223372036854775808
    assert fastnumbers.fast_forceint("10007199254740992") == 10007199254740992
    # 10. return type
    assert isinstance(fastnumbers.safe_forceint(4029.00), int)
    assert isinstance(fastnumbers.safe_forceint("4029.00"), int)
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
        assert fastnumbers.fast_forceint('26.8 lb', True)
    # 14. Infinity
    assert fastnumbers.fast_forceint('inf') == sys.maxsize
    assert fastnumbers.fast_forceint('-iNFinity') == -sys.maxsize - 1
    # 15. NaN
    assert fastnumbers.fast_forceint('nan') == 'nan'
    # 16. Sign only
    assert fastnumbers.fast_forceint('+') == '+'
    assert fastnumbers.fast_forceint('-') == '-'


def test_isreal():
    # 1. float number
    assert fastnumbers.isreal(-367.3268)
    assert not fastnumbers.isreal(-367.3268, str_only=True)
    # 2. signed float string
    assert fastnumbers.isreal("+367.3268")
    assert fastnumbers.isreal("+367.3268", True)
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
    # 16. Sign only
    assert not fastnumbers.isreal('+')
    assert not fastnumbers.isreal('-')


def test_isfloat():
    # 1. float number
    assert fastnumbers.isfloat(-367.3268)
    assert not fastnumbers.isfloat(-367.3268, str_only=True)
    # 2. signed float string
    assert fastnumbers.isfloat("+367.3268")
    assert fastnumbers.isfloat("+367.3268", True)
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
    assert fastnumbers.isfloat('-iNFinity', allow_inf=True)
    # 15. NaN
    assert not fastnumbers.isfloat('nan')
    assert fastnumbers.isfloat('nan', allow_nan=True)
    assert fastnumbers.isfloat('-NaN', allow_nan=True)
    # 16. Sign only
    assert not fastnumbers.isfloat('+')
    assert not fastnumbers.isfloat('-')


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
    # 6. signed int string
    assert fastnumbers.isint('-499')
    assert fastnumbers.isint('-499', True)
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
    # 16. Sign only
    assert not fastnumbers.isint('+')
    assert not fastnumbers.isint('-')


def test_isintlike():
    # 1. float number
    assert not fastnumbers.isintlike(-367.3268)
    assert fastnumbers.isintlike(-367.0)
    assert not fastnumbers.isintlike(-367.0, str_only=True)
    # 2. signed float string
    assert not fastnumbers.isintlike("+367.3268")
    assert fastnumbers.isintlike("+367.0")
    assert fastnumbers.isintlike("+367.0", True)
    # 3. float string with exponents
    assert fastnumbers.isintlike("-367.3268e207")
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
    # 16. Sign only
    assert not fastnumbers.isintlike('+')
    assert not fastnumbers.isintlike('-')
