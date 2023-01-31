# -*- coding: utf-8 -*-
# Find the build location and add that to the path
import math
from typing import cast

from pytest import raises

import fastnumbers

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


def test_try_real() -> None:
    # 1. float number
    assert fastnumbers.try_real(-367.3268) == -367.3268
    assert fastnumbers.try_real(-367.3268, on_fail=fastnumbers.RAISE) == -367.3268
    # 2. signed float string
    assert fastnumbers.try_real("+367.3268") == +367.3268
    assert fastnumbers.try_real("+367.3268", on_fail=fastnumbers.RAISE) == +367.3268
    # 3. float string with exponents
    assert fastnumbers.try_real("-367.3268e207") == -367.3268e207
    assert fastnumbers.try_real("1.175494351e-3810000000") == 0.0
    # 4. float string with padded whitespace
    assert fastnumbers.try_real("   -367.04   ") == -367.04
    # 5. int number
    assert fastnumbers.try_real(499) == 499
    # 6. signed int string
    assert fastnumbers.try_real("-499") == -499
    # 7. int string with padded whitespace
    assert fastnumbers.try_real("   +3001   ") == 3001
    # 8. long number
    assert fastnumbers.try_real(35892482945872302493) == 35892482945872302493
    # 9. long string
    assert fastnumbers.try_real("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.try_real(4029), int)
    assert isinstance(fastnumbers.try_real(4029.0, coerce=False), float)
    assert isinstance(fastnumbers.try_real(4029), int)
    assert isinstance(fastnumbers.try_real(4029.0), int)
    assert isinstance(fastnumbers.try_real(4029.5), float)
    assert isinstance(fastnumbers.try_real("4029"), int)
    assert isinstance(fastnumbers.try_real("4029.0"), int)
    assert isinstance(fastnumbers.try_real("4029.0", coerce=False), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.try_real(["hey"])  # type: ignore
    # 12. Invalid input string
    assert fastnumbers.try_real("not_a_number") == "not_a_number"
    with raises(ValueError):
        assert fastnumbers.try_real("not_a_number", on_fail=fastnumbers.RAISE)
    # 13. Invalid input string with numbers
    assert fastnumbers.try_real("26.8 lb") == "26.8 lb"
    with raises(ValueError):
        assert fastnumbers.try_real("26.8 lb", on_fail=fastnumbers.RAISE)
    # 14. Infinity
    assert fastnumbers.try_real("inf") == float("inf")
    assert fastnumbers.try_real("-iNFinity") == float("-inf")
    assert fastnumbers.try_real("-iNFinity", inf=7608) == 7608
    # 15. NaN
    assert math.isnan(cast(float, fastnumbers.try_real("nan")))
    assert math.isnan(cast(float, fastnumbers.try_real("-NaN")))
    assert fastnumbers.try_real("-NaN", nan=0) == 0
    # 16. Sign/'e'/'.' only
    assert fastnumbers.try_real("+") == "+"
    assert fastnumbers.try_real("-") == "-"
    assert fastnumbers.try_real("e") == "e"
    assert fastnumbers.try_real("e8") == "e8"
    assert fastnumbers.try_real(".") == "."
    # 17. Default on invalid...
    assert fastnumbers.try_real("invalid", on_fail=90) == 90
    assert fastnumbers.try_real("invalid", on_fail=None) is None
    # 18. Unicode numbers
    assert fastnumbers.try_real("⑦") == 7
    assert fastnumbers.try_real("⁸") == 8
    assert fastnumbers.try_real("⅔") == 2.0 / 3.0
    assert fastnumbers.try_real("Ⅴ") == 5
    # 19. Function to execute on failure to convert
    assert fastnumbers.try_real("76.8", on_fail=len) == 76.8
    assert fastnumbers.try_real("invalid", on_fail=len) == 7


def test_try_float() -> None:
    # 1. float number
    assert fastnumbers.try_float(-367.3268) == -367.3268
    assert fastnumbers.try_float(-367.3268, on_fail=fastnumbers.RAISE) == -367.3268
    # 2. signed float string
    assert fastnumbers.try_float("+367.3268") == +367.3268
    assert fastnumbers.try_float("+367.3268", on_fail=fastnumbers.RAISE) == +367.3268
    # 3. float string with exponents
    assert fastnumbers.try_float("-367.3268e27") == -367.3268e27
    assert fastnumbers.try_float("-367.3268E27") == -367.3268e27
    assert fastnumbers.try_float("-367.3268e207") == -367.3268e207
    assert fastnumbers.try_float("1.175494351E-3810000000") == 0.0
    # 4. float string with padded whitespace
    assert fastnumbers.try_float("   -367.04   ") == -367.04
    # 5. int number
    assert fastnumbers.try_float(499) == 499.0
    # 6. signed int string
    assert fastnumbers.try_float("-499") == -499.0
    # 7. int string with padded whitespace
    assert fastnumbers.try_float("   +3001   ") == 3001
    # 8. long number
    assert fastnumbers.try_float(35892482945872302493) == 35892482945872302493.0
    # 9. long string
    assert fastnumbers.try_float("35892482945872302493") == 35892482945872302493.0
    # 10. return type
    assert isinstance(fastnumbers.try_float(4029), float)
    assert isinstance(fastnumbers.try_float("4029"), float)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.try_float(["hey"])  # type: ignore
    # 12. Invalid input string
    assert fastnumbers.try_float("not_a_number") == "not_a_number"
    with raises(ValueError):
        assert fastnumbers.try_float("not_a_number", on_fail=fastnumbers.RAISE)
    # 13. Invalid input string with numbers
    assert fastnumbers.try_float("26.8 lb") == "26.8 lb"
    with raises(ValueError):
        assert fastnumbers.try_float("26.8 lb", on_fail=fastnumbers.RAISE)
    # 14. Infinity
    assert fastnumbers.try_float("inf") == float("inf")
    assert fastnumbers.try_float("-iNFinity") == float("-inf")
    assert fastnumbers.try_float("-iNFinity", inf=523) == 523
    # 15. NaN
    assert math.isnan(cast(float, fastnumbers.try_float("nAn")))
    assert math.isnan(cast(float, fastnumbers.try_float("-NaN")))
    assert fastnumbers.try_float("-NaN", nan=0) == 0
    # 16. Sign/'e'/'.' only
    assert fastnumbers.try_float("+") == "+"
    assert fastnumbers.try_float("-") == "-"
    assert fastnumbers.try_float("e") == "e"
    assert fastnumbers.try_float("e8") == "e8"
    assert fastnumbers.try_float(".") == "."
    # 17. Default on invalid...
    assert fastnumbers.try_float("invalid", on_fail=90) == 90
    assert fastnumbers.try_float("invalid", on_fail=None) is None
    # 18. Unicode numbers
    assert fastnumbers.try_float("⑦") == 7.0
    assert fastnumbers.try_float("⁸") == 8.0
    assert fastnumbers.try_float("⅔") == 2.0 / 3.0
    assert fastnumbers.try_float("Ⅴ") == 5.0
    # 19. Function to execute on failure to convert
    assert fastnumbers.try_float("76.8", on_fail=len) == 76.8
    assert fastnumbers.try_float("invalid", on_fail=len) == 7


def test_try_int() -> None:
    # 1. float number
    assert fastnumbers.try_int(-367.3268) == -367
    assert fastnumbers.try_int(-367.3268, on_fail=fastnumbers.RAISE) == -367
    # 2. signed float string
    assert fastnumbers.try_int("+367.3268") == "+367.3268"
    with raises(ValueError):
        assert fastnumbers.try_int("+367.3268", on_fail=fastnumbers.RAISE)
    # 3. float string with exponents
    assert fastnumbers.try_int("-367.3268e207") == "-367.3268e207"
    # 4. float string with padded whitespace
    assert fastnumbers.try_int("   -367.04   ") == "   -367.04   "
    # 5. int number
    assert fastnumbers.try_int(499) == 499
    # 6. signed int string
    assert fastnumbers.try_int("-499") == -499
    # 7. int string with padded whitespace
    assert fastnumbers.try_int("   +3001   ") == 3001
    # 8. long number
    assert fastnumbers.try_int(35892482945872302493) == 35892482945872302493
    # 9. long string
    assert fastnumbers.try_int("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.try_int(4029.00), int)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.try_int(["hey"])  # type: ignore
    # 12. Invalid input string
    assert fastnumbers.try_int("not_a_number") == "not_a_number"
    with raises(ValueError):
        assert fastnumbers.try_int("not_a_number", on_fail=fastnumbers.RAISE)
    # 13. Invalid input string with numbers
    assert fastnumbers.try_int("26.8 lb") == "26.8 lb"
    with raises(ValueError):
        assert fastnumbers.try_int("26.8 lb", on_fail=fastnumbers.RAISE)
    # 14. Infinity
    assert fastnumbers.try_int("inf") == "inf"
    # 15. NaN
    assert fastnumbers.try_int("nan") == "nan"
    # 16. Sign/'e'/'.' only
    assert fastnumbers.try_int("+") == "+"
    assert fastnumbers.try_int("-") == "-"
    assert fastnumbers.try_int("e") == "e"
    assert fastnumbers.try_int("e8") == "e8"
    assert fastnumbers.try_int(".") == "."
    # 17. Default on invalid...
    assert fastnumbers.try_int("invalid", on_fail=90) == 90
    assert fastnumbers.try_int("invalid", on_fail=None) is None
    # 18. Unicode numbers
    assert fastnumbers.try_int("⑦") == 7
    assert fastnumbers.try_int("⁸") == 8
    assert fastnumbers.try_int("⁸", base=10) == "⁸"
    assert fastnumbers.try_int("⅔") == "⅔"
    assert fastnumbers.try_int("Ⅴ") == "Ⅴ"
    # 19. Function to execute on failure to convert
    assert fastnumbers.try_int("76", on_fail=len) == 76
    assert fastnumbers.try_int("invalid", on_fail=len) == 7


def test_try_forceint() -> None:
    # 1. float number
    assert fastnumbers.try_forceint(-367.3268) == -367
    assert fastnumbers.try_forceint(-367.3268, on_fail=fastnumbers.RAISE) == -367
    # 2. signed float string
    assert fastnumbers.try_forceint("+367.3268") == 367
    assert fastnumbers.try_forceint("+367.3268", on_fail=fastnumbers.RAISE) == 367
    # 3. float string with exponents
    assert fastnumbers.try_forceint("-367.3268e207") == int(-367.3268e207)
    # 4. float string with padded whitespace
    assert fastnumbers.try_forceint("   -367.04   ") == -367
    # 5. int number
    assert fastnumbers.try_forceint(499) == 499
    # 6. signed int string
    assert fastnumbers.try_forceint("-499") == -499
    # 7. int string with padded whitespace
    assert fastnumbers.try_forceint("   +3001   ") == 3001
    # 8. long number
    assert fastnumbers.try_forceint(35892482945872302493) == 35892482945872302493
    # 9. long string
    assert fastnumbers.try_forceint("35892482945872302493") == 35892482945872302493
    # 10. return type
    assert isinstance(fastnumbers.try_forceint(4029.00), int)
    assert isinstance(fastnumbers.try_forceint("4029.00"), int)
    # 11. TypeError for invalid input
    with raises(TypeError):
        fastnumbers.try_forceint(["hey"])  # type: ignore
    # 12. Invalid input string
    assert fastnumbers.try_forceint("not_a_number") == "not_a_number"
    with raises(ValueError):
        assert fastnumbers.try_forceint("not_a_number", on_fail=fastnumbers.RAISE)
    # 13. Invalid input string with numbers
    assert fastnumbers.try_forceint("26.8 lb") == "26.8 lb"
    with raises(ValueError):
        assert fastnumbers.try_forceint("26.8 lb", on_fail=fastnumbers.RAISE)
    # 14. Infinity
    assert fastnumbers.try_forceint("inf") == "inf"
    assert fastnumbers.try_forceint("-iNFinity") == "-iNFinity"
    # 15. NaN
    assert fastnumbers.try_forceint("nan") == "nan"
    # 16. Sign/'e'/'.' only
    assert fastnumbers.try_forceint("+") == "+"
    assert fastnumbers.try_forceint("-") == "-"
    assert fastnumbers.try_forceint("e") == "e"
    assert fastnumbers.try_forceint("e8") == "e8"
    assert fastnumbers.try_forceint(".") == "."
    # 17. Default on invalid...
    assert fastnumbers.try_forceint("invalid", on_fail=90) == 90
    assert fastnumbers.try_forceint("invalid", on_fail=None) is None
    # 18. Unicode numbers
    assert fastnumbers.try_forceint("⑦") == 7
    assert fastnumbers.try_forceint("⁸") == 8
    assert fastnumbers.try_forceint("⅔") == 0
    assert fastnumbers.try_forceint("Ⅴ") == 5
    # 19. Function to execute on failure to convert
    assert fastnumbers.try_forceint("76.8", on_fail=len) == 76
    assert fastnumbers.try_forceint("invalid", on_fail=len) == 7


def test_check_real() -> None:
    # 1. float number
    assert fastnumbers.check_real(-367.3268)
    assert not fastnumbers.check_real(-367.3268, consider=fastnumbers.STRING_ONLY)
    assert fastnumbers.check_real(-367.3268, consider=fastnumbers.NUMBER_ONLY)
    # 2. signed float string
    assert fastnumbers.check_real("+367.3268")
    assert fastnumbers.check_real("+367.3268", consider=fastnumbers.STRING_ONLY)
    assert not fastnumbers.check_real("+367.3268", consider=fastnumbers.NUMBER_ONLY)
    # 3. float string with exponents
    assert fastnumbers.check_real("-367.3268e207")
    # 4. float string with padded whitespace
    assert fastnumbers.check_real("   -367.04   ")
    # 5. int number
    assert fastnumbers.check_real(499)
    # 6. signed int string
    assert fastnumbers.check_real("-499")
    # 7. int string with padded whitespace
    assert fastnumbers.check_real("   +3001   ")
    # 8. long number
    assert fastnumbers.check_real(35892482945872302493)
    # 9. long string
    assert fastnumbers.check_real("35892482945872302493")
    # 10. return type
    assert fastnumbers.check_real(4029) is True
    assert fastnumbers.check_real(4029, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_real("4029") is True
    assert fastnumbers.check_real("4029", consider=fastnumbers.STRING_ONLY) is True
    assert fastnumbers.check_real("hey") is False
    # 11. Invalid type
    assert not fastnumbers.check_real(["hey"])
    # 12. Invalid input string
    assert not fastnumbers.check_real("not_a_number")
    # 13. Invalid input string with numbers
    assert not fastnumbers.check_real("26.8 lb")
    # 14. Infinity
    assert not fastnumbers.check_real("inf")
    assert fastnumbers.check_real("inf", inf=fastnumbers.ALLOWED)
    assert fastnumbers.check_real("-iNFinity", inf=fastnumbers.ALLOWED)
    # 15. NaN
    assert not fastnumbers.check_real("nan")
    assert fastnumbers.check_real("nan", nan=fastnumbers.ALLOWED)
    assert fastnumbers.check_real("-NaN", nan=fastnumbers.ALLOWED)
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.check_real("+")
    assert not fastnumbers.check_real("-")
    assert not fastnumbers.check_real("e")
    assert not fastnumbers.check_real("e8")
    assert not fastnumbers.check_real(".")
    # 18. Unicode numbers
    assert fastnumbers.check_real("⑦")
    assert fastnumbers.check_real("⁸")
    assert fastnumbers.check_real("⅔")
    assert fastnumbers.check_real("Ⅴ")


def test_check_float() -> None:
    # 1. float number
    assert fastnumbers.check_float(-367.3268)
    assert not fastnumbers.check_float(-367.3268, consider=fastnumbers.STRING_ONLY)
    assert fastnumbers.check_float(-367.3268, consider=fastnumbers.NUMBER_ONLY)
    # 2. signed float string
    assert fastnumbers.check_float("+367.3268")
    assert fastnumbers.check_float("+367.3268", consider=fastnumbers.STRING_ONLY)
    assert not fastnumbers.check_float("+367.3268", consider=fastnumbers.NUMBER_ONLY)
    # 3. float string with exponents
    assert fastnumbers.check_float("-367.3268e207")
    # 4. float string with padded whitespace
    assert fastnumbers.check_float("   -367.04   ")
    # 5. int number
    assert not fastnumbers.check_float(499)
    # 6. signed int string
    assert fastnumbers.check_float("-499")
    # 7. int string with padded whitespace
    assert fastnumbers.check_float("   +3001   ")
    # 8. long number
    assert not fastnumbers.check_float(35892482945872302493)
    # 9. long string
    assert fastnumbers.check_float("35892482945872302493")
    # 10. return type
    assert fastnumbers.check_float(4029) is False
    assert fastnumbers.check_float(4029.0) is True
    assert fastnumbers.check_float(4029.0, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_float("4029") is True
    assert fastnumbers.check_float("4029", consider=fastnumbers.STRING_ONLY) is True
    # 11. Invalid type
    assert not fastnumbers.check_float(["hey"])
    # 12. Invalid input string
    assert not fastnumbers.check_float("not_a_number")
    # 13. Invalid input string with numbers
    assert not fastnumbers.check_float("26.8 lb")
    # 14. Infinity
    assert not fastnumbers.check_float("inf")
    assert fastnumbers.check_float("inf", inf=fastnumbers.ALLOWED)
    assert fastnumbers.check_float("-infinity", inf=fastnumbers.ALLOWED)
    assert fastnumbers.check_float("-INFINITY", inf=fastnumbers.ALLOWED)
    # 15. NaN
    assert not fastnumbers.check_float("nAn")
    assert fastnumbers.check_float("nan", nan=fastnumbers.ALLOWED)
    assert fastnumbers.check_float("-NaN", nan=fastnumbers.ALLOWED)
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.check_float("+")
    assert not fastnumbers.check_float("-")
    assert not fastnumbers.check_float("e")
    assert not fastnumbers.check_float("e8")
    assert not fastnumbers.check_float(".")
    # 18. Unicode numbers
    assert fastnumbers.check_float("⑦")
    assert fastnumbers.check_float("⁸")
    assert fastnumbers.check_float("⅔")
    assert fastnumbers.check_float("Ⅴ")


def test_check_int() -> None:
    # 1. float number
    assert not fastnumbers.check_int(-367.3268)
    # 2. signed float string
    assert not fastnumbers.check_int("+367.3268")
    # 3. float string with exponents
    assert not fastnumbers.check_int("-367.3268e207")
    # 4. float string with padded whitespace
    assert not fastnumbers.check_int("   -367.04   ")
    # 5. int number
    assert fastnumbers.check_int(499)
    assert not fastnumbers.check_int(499, consider=fastnumbers.STRING_ONLY)
    assert fastnumbers.check_int(499, consider=fastnumbers.NUMBER_ONLY)
    # 6. signed int string
    assert fastnumbers.check_int("-499")
    assert fastnumbers.check_int("-499", consider=fastnumbers.STRING_ONLY)
    assert not fastnumbers.check_int("-499", consider=fastnumbers.NUMBER_ONLY)
    # 7. int string with padded whitespace
    assert fastnumbers.check_int("   +3001   ")
    # 8. long number
    assert fastnumbers.check_int(35892482945872302493)
    # 9. long string
    assert fastnumbers.check_int("35892482945872302493")
    # 10. return type
    assert fastnumbers.check_int(4029) is True
    assert fastnumbers.check_int(4029, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_int("4029") is True
    assert fastnumbers.check_int("4029", consider=fastnumbers.STRING_ONLY) is True
    assert fastnumbers.check_int("4029.50") is False
    assert fastnumbers.check_int(4029.50) is False
    # 11. Invalid type
    assert not fastnumbers.check_int(["hey"])
    # 12. Invalid input string
    assert not fastnumbers.check_int("not_a_number")
    # 13. Invalid input string with numbers
    assert not fastnumbers.check_int("26.8 lb")
    # 14. Infinity
    assert not fastnumbers.check_int("inf")
    # 15. NaN
    assert not fastnumbers.check_int("nan")
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.check_int("+")
    assert not fastnumbers.check_int("-")
    assert not fastnumbers.check_int("e")
    assert not fastnumbers.check_int("e8")
    assert not fastnumbers.check_int(".")
    # 18. Unicode numbers
    assert fastnumbers.check_int("⑦")
    assert fastnumbers.check_int("⁸")
    assert not fastnumbers.check_int("⅔")
    assert not fastnumbers.check_int("Ⅴ")


def test_check_intlike() -> None:
    # 1. float number
    assert not fastnumbers.check_intlike(-367.3268)
    assert fastnumbers.check_intlike(-367.0)
    assert not fastnumbers.check_intlike(-367.0, consider=fastnumbers.STRING_ONLY)
    assert fastnumbers.check_intlike(-367.0, consider=fastnumbers.NUMBER_ONLY)
    # 2. signed float string
    assert not fastnumbers.check_intlike("+367.3268")
    assert fastnumbers.check_intlike("+367.0")
    assert fastnumbers.check_intlike("+367.0", consider=fastnumbers.STRING_ONLY)
    assert not fastnumbers.check_intlike("+367.0", consider=fastnumbers.NUMBER_ONLY)
    # 3. float string with exponents
    assert fastnumbers.check_intlike("-367.3268e207")
    assert not fastnumbers.check_intlike("145343E-4")
    assert fastnumbers.check_intlike("14534.000000000e4")
    assert fastnumbers.check_intlike("1400000E-4")
    assert not fastnumbers.check_intlike("140E-4")
    assert fastnumbers.check_intlike("14.E4")
    assert fastnumbers.check_intlike("14E4")
    # 4. float string with padded whitespace
    assert not fastnumbers.check_intlike("   -367.04   ")
    # 5. int number
    assert fastnumbers.check_intlike(499)
    # 6. signed int string
    assert fastnumbers.check_intlike("-499")
    # 7. int string with padded whitespace
    assert fastnumbers.check_intlike("   +3001   ")
    # 8. long number
    assert fastnumbers.check_intlike(35892482945872302493)
    # 9. long string
    assert fastnumbers.check_intlike("35892482945872302493")
    # 10. return type
    assert fastnumbers.check_intlike(4029) is True
    assert fastnumbers.check_intlike(4029, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_intlike("4029") is True
    assert fastnumbers.check_intlike("4029", consider=fastnumbers.STRING_ONLY) is True
    assert fastnumbers.check_intlike("4029.50") is False
    assert fastnumbers.check_intlike(4029.50) is False
    # 11. Invalid type
    assert not fastnumbers.check_intlike(["hey"])
    # 12. Invalid input string
    assert not fastnumbers.check_intlike("not_a_number")
    # 13. Invalid input string with numbers
    assert not fastnumbers.check_intlike("26.8 lb")
    # 14. Infinity
    assert not fastnumbers.check_intlike("inf")
    # 15. NaN
    assert not fastnumbers.check_intlike("nan")
    # 16. Sign/'e'/'.' only
    assert not fastnumbers.check_intlike("+")
    assert not fastnumbers.check_intlike("-")
    assert not fastnumbers.check_intlike("e")
    assert not fastnumbers.check_intlike("e8")
    assert not fastnumbers.check_intlike(".")
    # 18. Unicode numbers
    assert fastnumbers.check_intlike("⑦")
    assert fastnumbers.check_intlike("⁸")
    assert not fastnumbers.check_intlike("⅔")
    assert fastnumbers.check_intlike("Ⅴ")


def test_type() -> None:
    # 1. float number
    assert fastnumbers.query_type(-367.3268) is float
    # 2. signed float string
    assert fastnumbers.query_type("+367.3268") is float
    # 3. float string with exponents
    assert fastnumbers.query_type("-367.3268e207") is float
    # 4. float string with padded whitespace
    assert fastnumbers.query_type("   -367.04   ") is float
    # 5. int number
    assert fastnumbers.query_type(499) is int
    # 6. signed int string
    assert fastnumbers.query_type("-499") is int
    # 7. int string with padded whitespace
    assert fastnumbers.query_type("   +3001   ") is int
    # 8. long number
    assert fastnumbers.query_type(35892482945872302493) is int
    # 9. long string
    assert fastnumbers.query_type("35892482945872302493") is int
    # 10. coerced type
    assert fastnumbers.query_type(4029.0) is float
    assert fastnumbers.query_type("4029.0") is float
    assert fastnumbers.query_type(4029.0, coerce=True) is int
    assert fastnumbers.query_type("4029.0", coerce=True) is int
    # 11. Invalid type
    assert fastnumbers.query_type(["hey"]) is list
    # 12. Invalid input string
    assert fastnumbers.query_type("not_a_number") is str
    assert fastnumbers.query_type("not_a_number", allowed_types=(float, int)) is None
    # 13. Invalid input string with numbers
    assert fastnumbers.query_type("26.8 lb") is str
    assert fastnumbers.query_type("26.8 lb", allowed_types=(float, int)) is None
    # 14. Infinity
    assert fastnumbers.query_type("inf") is str
    assert fastnumbers.query_type("inf", allow_inf=True) is float
    assert fastnumbers.query_type("-iNFinity", allow_inf=True) is float
    # 15. NaN
    assert fastnumbers.query_type("nan") is str
    assert fastnumbers.query_type("nan", allow_nan=True) is float
    assert fastnumbers.query_type("-NaN", allow_nan=True) is float
    # 16. Sign/'e'/'.' only
    assert fastnumbers.query_type("+") is str
    assert fastnumbers.query_type("-") is str
    assert fastnumbers.query_type("e") is str
    assert fastnumbers.query_type("e8") is str
    assert fastnumbers.query_type(".") is str
    # 18. Unicode numbers
    assert fastnumbers.query_type("⑦") is int
    assert fastnumbers.query_type("⁸") is int
    assert fastnumbers.query_type("⅔") is float
    assert fastnumbers.query_type("Ⅴ") is float
