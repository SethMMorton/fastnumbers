# Find the build location and add that to the path
from __future__ import print_function, division
import sys
import os
from platform import python_version_tuple
from sysconfig import get_platform, get_python_version
distutilsname = 'lib.' + '-'.join([get_platform(),
                                   get_python_version()
                                   ])
fastnumberspath = os.path.join('build', distutilsname, 'fastnumbers')
sys.path.append(fastnumberspath)
import fastnumbers
from pytest import raises

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

def test_safe_float():
    # 1. float number
    assert fastnumbers.safe_float(-367.3268) == -367.3268
    # 2. signed float string
    assert fastnumbers.safe_float("+367.3268") == +367.3268
    # 3. float string with exponents
    assert fastnumbers.safe_float("-367.3268e207") == -367.3268e207
    # 4. float string with padded whitespace
    assert fastnumbers.safe_float("   -367.04   ") == -367.04
    # 5. int number
    assert fastnumbers.safe_float(499) == 499.0
    # 6. signed int string
    assert fastnumbers.safe_float('-499') == -499.0
    # 7. int string with padded whitespace
    assert fastnumbers.safe_float('   +3001   ') == 3001
    # 8. long number
    assert fastnumbers.safe_float(35892482945872302493) == 35892482945872302493.0
    # 9. long string
    assert fastnumbers.safe_float("35892482945872302493") == 35892482945872302493.0
    # 10. return type
    assert isinstance(fastnumbers.safe_float(4029), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.safe_float(['hey'])
    # 12. Invalid input string
    assert fastnumbers.safe_float('not_a_number') == 'not_a_number'
    # 13. Invalid input string with numbers
    assert fastnumbers.safe_float('26.8 lb') == '26.8 lb'



def test_safe_int():
    # 1. float number
    assert fastnumbers.safe_int(-367.3268) == -367
    # 2. signed float string
    assert fastnumbers.safe_int("+367.3268") == "+367.3268"
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
    # 13. Invalid input string with numbers
    assert fastnumbers.safe_int('26.8 lb') == '26.8 lb'


def test_fast_float():
    # 1. float number
    assert fastnumbers.fast_float(-367.3268) == -367.3268
    # 2. signed float string
    assert fastnumbers.fast_float("+367.3268") == +367.3268
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
    # 10. return type
    assert isinstance(fastnumbers.fast_float(4029), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.fast_float(['hey'])
    # 12. Invalid input string
    assert fastnumbers.fast_float('not_a_number') == 'not_a_number'
    # 13. Invalid input string with numbers
    assert fastnumbers.fast_float('26.8 lb') == '26.8 lb'


def test_fast_int():
    # 1. float number
    assert fastnumbers.fast_int(-367.3268) == -367
    # 2. signed float string
    assert fastnumbers.fast_int("+367.3268") == "+367.3268"
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
    # 10. return type
    assert isinstance(fastnumbers.fast_int(4029.00), int)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.fast_int(['hey'])
    # 12. Invalid input string
    assert fastnumbers.fast_int('not_a_number') == 'not_a_number'
    # 13. Invalid input string with numbers
    assert fastnumbers.fast_int('26.8 lb') == '26.8 lb'
