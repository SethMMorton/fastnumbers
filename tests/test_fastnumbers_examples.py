from __future__ import annotations

import math
from typing import cast

import pytest

import fastnumbers

# Each conversion test should test the following
# 1. float number
# 2. signed float string
# 3. float string with exponents
# 4. float string with padded whitespace
# 5. float-like class
# 6. int number
# 7. signed int string
# 8. int string with padded whitespace
# 9. long number
# 10. long string
# 11. int-like class
# 12. return type
# 13. TypeError for invalid input
# 14. Invalid input string
# 15. Invalid input string with numbers
# 16. Infinity
# 17. NaN
# 18. Sign/'e'/'.' only
# 19. Default value
# 20. Unicode numbers
# 21. Function to execute on failure to convert
# 22. Handling of type errors
# 23. Denoising large float to integer conversions
# 24. List conversions
# 25. Iterator conversions


class IndexCard:
    def __index__(self) -> int:
        return 42


class PerformInt:
    def __int__(self) -> int:
        return 13


class FloatationDevice:
    def __float__(self) -> float:
        return 3.14159


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
    # 5. float-like class
    assert fastnumbers.try_real(FloatationDevice()) == 3.14159
    # 6. int number
    assert fastnumbers.try_real(499) == 499
    # 7. signed int string
    assert fastnumbers.try_real("-499") == -499
    # 8. int string with padded whitespace
    assert fastnumbers.try_real("   +3001   ") == 3001
    # 9. long number
    assert fastnumbers.try_real(35892482945872302493) == 35892482945872302493
    # 10. long string
    assert fastnumbers.try_real("35892482945872302493") == 35892482945872302493
    # 11. int-like class
    assert fastnumbers.try_real(IndexCard()) == 42
    assert fastnumbers.try_real(PerformInt()) == 13
    # 12. return type
    assert isinstance(fastnumbers.try_real(4029), int)
    assert isinstance(fastnumbers.try_real(4029.0, coerce=False), float)
    assert isinstance(fastnumbers.try_real(4029), int)
    assert isinstance(fastnumbers.try_real(4029.0), int)
    assert isinstance(fastnumbers.try_real(4029.5), float)
    assert isinstance(fastnumbers.try_real("4029"), int)
    assert isinstance(fastnumbers.try_real("4029.0"), int)
    assert isinstance(fastnumbers.try_real("4029.0", coerce=False), float)
    assert isinstance(fastnumbers.try_real(FloatationDevice()), float)
    assert isinstance(fastnumbers.try_real(IndexCard()), int)
    # 13. TypeError for invalid input
    with pytest.raises(TypeError):
        fastnumbers.try_real(["hey"])
    # 14. Invalid input string
    assert fastnumbers.try_real("not_a_number") == "not_a_number"
    with pytest.raises(ValueError):
        assert fastnumbers.try_real("not_a_number", on_fail=fastnumbers.RAISE)
    # 15. Invalid input string with numbers
    assert fastnumbers.try_real("26.8 lb") == "26.8 lb"
    with pytest.raises(ValueError):
        assert fastnumbers.try_real("26.8 lb", on_fail=fastnumbers.RAISE)
    # 16. Infinity
    assert fastnumbers.try_real("inf") == float("inf")
    assert fastnumbers.try_real("-iNFinity") == float("-inf")
    assert fastnumbers.try_real("-iNFinity", inf=7608) == 7608
    # 17. NaN
    assert math.isnan(cast("float", fastnumbers.try_real("nan")))
    assert math.isnan(cast("float", fastnumbers.try_real("-NaN")))
    assert fastnumbers.try_real("-NaN", nan=0) == 0
    # 18. Sign/'e'/'.' only
    assert fastnumbers.try_real("+") == "+"
    assert fastnumbers.try_real("-") == "-"
    assert fastnumbers.try_real("e") == "e"
    assert fastnumbers.try_real("e8") == "e8"
    assert fastnumbers.try_real(".") == "."
    # 19. Default value
    assert fastnumbers.try_real("invalid", on_fail=90) == 90
    assert fastnumbers.try_real("invalid", on_fail=None) is None
    # 20. Unicode numbers
    assert fastnumbers.try_real("⑦") == 7
    assert fastnumbers.try_real("⑦", allow_unicode_chars=False) == "⑦"
    assert fastnumbers.try_real("⁸") == 8
    assert fastnumbers.try_real("⅔") == 2.0 / 3.0
    assert fastnumbers.try_real("Ⅴ") == 5
    # 21. Function to execute on failure to convert
    assert fastnumbers.try_real("76.8", on_fail=len) == 76.8
    assert fastnumbers.try_real("invalid", on_fail=len) == 7
    # 22. Handling of type errors
    assert fastnumbers.try_real(["invalid"], on_fail=6, on_type_error=7.0) == 7.0
    assert fastnumbers.try_real(["invalid"], on_type_error=None) is None
    # 23. Denoising large float to integer conversions
    assert fastnumbers.try_real(3.453e21) == 3452999999999999737856
    assert fastnumbers.try_real(3.453e21, denoise=True) == 3453000000000000000000
    assert fastnumbers.try_real("3.453e21") == 3452999999999999737856
    assert fastnumbers.try_real("3.453e21", denoise=True) == 3453000000000000000000


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
    # 5. float-like class
    assert fastnumbers.float(FloatationDevice()) == 3.14159
    # 6. int number
    assert fastnumbers.try_float(499) == 499.0
    # 7. signed int string
    assert fastnumbers.try_float("-499") == -499.0
    # 8. int string with padded whitespace
    assert fastnumbers.try_float("   +3001   ") == 3001.0
    # 9. long number
    assert fastnumbers.try_float(35892482945872302493) == 35892482945872302493.0
    # 10. long string
    assert fastnumbers.try_float("35892482945872302493") == 35892482945872302493.0
    # 11. int-like class
    assert fastnumbers.try_float(IndexCard()) == 42.0
    assert fastnumbers.try_float(PerformInt()) == 13.0
    # 12. return type
    assert isinstance(fastnumbers.try_float(4029), float)
    assert isinstance(fastnumbers.try_float("4029"), float)
    assert isinstance(fastnumbers.try_float(FloatationDevice()), float)
    assert isinstance(fastnumbers.try_float(IndexCard()), float)
    assert isinstance(fastnumbers.try_float(PerformInt()), float)
    # 13. TypeError for invalid input
    with pytest.raises(TypeError):
        fastnumbers.try_float(["hey"])
    # 14. Invalid input string
    assert fastnumbers.try_float("not_a_number") == "not_a_number"
    with pytest.raises(ValueError):
        assert fastnumbers.try_float("not_a_number", on_fail=fastnumbers.RAISE)
    # 15. Invalid input string with numbers
    assert fastnumbers.try_float("26.8 lb") == "26.8 lb"
    with pytest.raises(ValueError):
        assert fastnumbers.try_float("26.8 lb", on_fail=fastnumbers.RAISE)
    # 16. Infinity
    assert fastnumbers.try_float("inf") == float("inf")
    assert fastnumbers.try_float("-iNFinity") == float("-inf")
    assert fastnumbers.try_float("-iNFinity", inf=523.0) == 523.0
    # 17. NaN
    assert math.isnan(cast("float", fastnumbers.try_float("nAn")))
    assert math.isnan(cast("float", fastnumbers.try_float("-NaN")))
    assert fastnumbers.try_float("-NaN", nan=0.0) == 0.0
    # 18. Sign/'e'/'.' only
    assert fastnumbers.try_float("+") == "+"
    assert fastnumbers.try_float("-") == "-"
    assert fastnumbers.try_float("e") == "e"
    assert fastnumbers.try_float("e8") == "e8"
    assert fastnumbers.try_float(".") == "."
    # 19. Default value
    assert fastnumbers.try_float("invalid", on_fail=90.0) == 90.0
    assert fastnumbers.try_float("invalid", on_fail=None) is None
    # 20. Unicode numbers
    assert fastnumbers.try_float("⑦") == 7.0
    assert fastnumbers.try_float("⑦", allow_unicode_chars=False) == "⑦"
    assert fastnumbers.try_float("⁸") == 8.0
    assert fastnumbers.try_float("⅔") == 2.0 / 3.0
    assert fastnumbers.try_float("Ⅴ") == 5.0
    # 21. Function to execute on failure to convert
    assert fastnumbers.try_float("76.8", on_fail=len) == 76.8
    assert fastnumbers.try_float("invalid", on_fail=len) == 7
    # 22. Handling of type errors
    assert fastnumbers.try_float(["invalid"], on_fail=6.0, on_type_error=7.0) == 7.0
    assert fastnumbers.try_float(["invalid"], on_type_error=None) is None


def test_try_int() -> None:
    # 1. float number
    assert fastnumbers.try_int(-367.3268) == -367
    assert fastnumbers.try_int(-367.3268, on_fail=fastnumbers.RAISE) == -367
    # 2. signed float string
    assert fastnumbers.try_int("+367.3268") == "+367.3268"
    with pytest.raises(ValueError):
        assert fastnumbers.try_int("+367.3268", on_fail=fastnumbers.RAISE)
    # 3. float string with exponents
    assert fastnumbers.try_int("-367.3268e207") == "-367.3268e207"
    # 4. float string with padded whitespace
    assert fastnumbers.try_int("   -367.04   ") == "   -367.04   "
    # 5. float-like class
    assert fastnumbers.try_int(FloatationDevice()) == 3
    # 6. int number
    assert fastnumbers.try_int(499) == 499
    # 7. signed int string
    assert fastnumbers.try_int("-499") == -499
    # 8. int string with padded whitespace
    assert fastnumbers.try_int("   +3001   ") == 3001
    # 9. long number
    assert fastnumbers.try_int(35892482945872302493) == 35892482945872302493
    # 10. long string
    assert fastnumbers.try_int("35892482945872302493") == 35892482945872302493
    # 11. int-like class
    assert fastnumbers.try_int(IndexCard()) == 42
    assert fastnumbers.try_int(PerformInt()) == 13
    # 12. return type
    assert isinstance(fastnumbers.try_int(4029.00), int)
    assert isinstance(fastnumbers.try_int(FloatationDevice()), int)
    assert isinstance(fastnumbers.try_int(IndexCard()), int)
    assert isinstance(fastnumbers.try_int(PerformInt()), int)
    # 13. TypeError for invalid input
    with pytest.raises(TypeError):
        fastnumbers.try_int(["hey"])
    # 14. Invalid input string
    assert fastnumbers.try_int("not_a_number") == "not_a_number"
    with pytest.raises(ValueError):
        assert fastnumbers.try_int("not_a_number", on_fail=fastnumbers.RAISE)
    # 15. Invalid input string with numbers
    assert fastnumbers.try_int("26.8 lb") == "26.8 lb"
    with pytest.raises(ValueError):
        assert fastnumbers.try_int("26.8 lb", on_fail=fastnumbers.RAISE)
    # 16. Infinity
    assert fastnumbers.try_int("inf") == "inf"
    # 17. NaN
    assert fastnumbers.try_int("nan") == "nan"
    # 18. Sign/'e'/'.' only
    assert fastnumbers.try_int("+") == "+"
    assert fastnumbers.try_int("-") == "-"
    assert fastnumbers.try_int("e") == "e"
    assert fastnumbers.try_int("e8") == "e8"
    assert fastnumbers.try_int(".") == "."
    # 19. Default value
    assert fastnumbers.try_int("invalid", on_fail=90) == 90
    assert fastnumbers.try_int("invalid", on_fail=None) is None
    # 20. Unicode numbers
    assert fastnumbers.try_int("⑦") == 7
    assert fastnumbers.try_int("⑦", allow_unicode_chars=False) == "⑦"
    assert fastnumbers.try_int("⁸") == 8
    assert fastnumbers.try_int("⁸", base=10) == "⁸"
    assert fastnumbers.try_int("⅔") == "⅔"
    assert fastnumbers.try_int("Ⅴ") == "Ⅴ"
    # 21. Function to execute on failure to convert
    assert fastnumbers.try_int("76", on_fail=len) == 76
    assert fastnumbers.try_int("invalid", on_fail=len) == 7
    # 22. Handling of type errors
    assert fastnumbers.try_int(["invalid"], on_fail=6, on_type_error=7) == 7
    assert fastnumbers.try_int(["invalid"], on_type_error=None) is None


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
    # 5. float-like class
    assert fastnumbers.try_forceint(FloatationDevice()) == 3
    # 6. int number
    assert fastnumbers.try_forceint(499) == 499
    # 7. signed int string
    assert fastnumbers.try_forceint("-499") == -499
    # 8. int string with padded whitespace
    assert fastnumbers.try_forceint("   +3001   ") == 3001
    # 9. long number
    assert fastnumbers.try_forceint(35892482945872302493) == 35892482945872302493
    # 10. long string
    assert fastnumbers.try_forceint("35892482945872302493") == 35892482945872302493
    # 11. int-like class
    assert fastnumbers.try_forceint(IndexCard()) == 42
    assert fastnumbers.try_forceint(PerformInt()) == 13
    # 12. return type
    assert isinstance(fastnumbers.try_forceint(4029.00), int)
    assert isinstance(fastnumbers.try_forceint("4029.00"), int)
    assert isinstance(fastnumbers.try_forceint(FloatationDevice()), int)
    assert isinstance(fastnumbers.try_forceint(IndexCard()), int)
    assert isinstance(fastnumbers.try_forceint(PerformInt()), int)
    # 13. TypeError for invalid input
    with pytest.raises(TypeError):
        fastnumbers.try_forceint(["hey"])
    # 14. Invalid input string
    assert fastnumbers.try_forceint("not_a_number") == "not_a_number"
    with pytest.raises(ValueError):
        assert fastnumbers.try_forceint("not_a_number", on_fail=fastnumbers.RAISE)
    # 15. Invalid input string with numbers
    assert fastnumbers.try_forceint("26.8 lb") == "26.8 lb"
    with pytest.raises(ValueError):
        assert fastnumbers.try_forceint("26.8 lb", on_fail=fastnumbers.RAISE)
    # 16. Infinity
    assert fastnumbers.try_forceint("inf") == "inf"
    assert fastnumbers.try_forceint("-iNFinity") == "-iNFinity"
    # 17. NaN
    assert fastnumbers.try_forceint("nan") == "nan"
    # 18. Sign/'e'/'.' only
    assert fastnumbers.try_forceint("+") == "+"
    assert fastnumbers.try_forceint("-") == "-"
    assert fastnumbers.try_forceint("e") == "e"
    assert fastnumbers.try_forceint("e8") == "e8"
    assert fastnumbers.try_forceint(".") == "."
    # 19. Default value
    assert fastnumbers.try_forceint("invalid", on_fail=90) == 90
    assert fastnumbers.try_forceint("invalid", on_fail=None) is None
    # 20. Unicode numbers
    assert fastnumbers.try_forceint("⑦") == 7
    assert fastnumbers.try_forceint("⑦", allow_unicode_chars=False) == "⑦"
    assert fastnumbers.try_forceint("⁸") == 8
    assert fastnumbers.try_forceint("⅔") == 0
    assert fastnumbers.try_forceint("Ⅴ") == 5
    # 21. Function to execute on failure to convert
    assert fastnumbers.try_forceint("76.8", on_fail=len) == 76
    assert fastnumbers.try_forceint("invalid", on_fail=len) == 7
    # 22. Handling of type errors
    assert fastnumbers.try_forceint(["invalid"], on_fail=6, on_type_error=7) == 7
    assert fastnumbers.try_forceint(["invalid"], on_type_error=None) is None
    # 23. Denoising large float to integer conversions
    assert fastnumbers.try_forceint(3.453e21) == 3452999999999999737856
    assert fastnumbers.try_forceint(3.453e21, denoise=True) == 3453000000000000000000
    assert fastnumbers.try_forceint("3.453e21") == 3452999999999999737856
    assert fastnumbers.try_forceint("3.453e21", denoise=True) == 3453000000000000000000


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
    # 5. float-like class
    assert fastnumbers.check_real(FloatationDevice())
    # 6. int number
    assert fastnumbers.check_real(499)
    # 7. signed int string
    assert fastnumbers.check_real("-499")
    # 8. int string with padded whitespace
    assert fastnumbers.check_real("   +3001   ")
    # 9. long number
    assert fastnumbers.check_real(35892482945872302493)
    # 10. long string
    assert fastnumbers.check_real("35892482945872302493")
    # 11. int-like class
    assert fastnumbers.check_real(IndexCard())
    assert fastnumbers.check_real(PerformInt())
    # 12. return type
    assert fastnumbers.check_real(4029) is True
    assert fastnumbers.check_real(4029, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_real("4029") is True
    assert fastnumbers.check_real("4029", consider=fastnumbers.STRING_ONLY) is True
    assert fastnumbers.check_real("hey") is False
    # 11. Invalid type
    assert not fastnumbers.check_real(["hey"])
    # 14. Invalid input string
    assert not fastnumbers.check_real("not_a_number")
    # 15. Invalid input string with numbers
    assert not fastnumbers.check_real("26.8 lb")
    # 16. Infinity
    assert not fastnumbers.check_real("inf")
    assert fastnumbers.check_real("inf", inf=fastnumbers.ALLOWED)
    assert fastnumbers.check_real("-iNFinity", inf=fastnumbers.ALLOWED)
    # 17. NaN
    assert not fastnumbers.check_real("nan")
    assert fastnumbers.check_real("nan", nan=fastnumbers.ALLOWED)
    assert fastnumbers.check_real("-NaN", nan=fastnumbers.ALLOWED)
    # 18. Sign/'e'/'.' only
    assert not fastnumbers.check_real("+")
    assert not fastnumbers.check_real("-")
    assert not fastnumbers.check_real("e")
    assert not fastnumbers.check_real("e8")
    assert not fastnumbers.check_real(".")
    # 20. Unicode numbers
    assert fastnumbers.check_real("⑦")
    assert not fastnumbers.check_real("⑦", allow_unicode_chars=False)
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
    # 5. float-like class
    assert fastnumbers.check_float(FloatationDevice())
    # 6. int number
    assert not fastnumbers.check_float(499)
    # 7. signed int string
    assert fastnumbers.check_float("-499")
    # 8. int string with padded whitespace
    assert fastnumbers.check_float("   +3001   ")
    # 9. long number
    assert not fastnumbers.check_float(35892482945872302493)
    # 10. long string
    assert fastnumbers.check_float("35892482945872302493")
    # 11. int-like class
    assert not fastnumbers.check_float(IndexCard())
    assert not fastnumbers.check_float(PerformInt())
    # 12. return type
    assert fastnumbers.check_float(4029) is False
    assert fastnumbers.check_float(4029.0) is True
    assert fastnumbers.check_float(4029.0, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_float("4029") is True
    assert fastnumbers.check_float("4029", consider=fastnumbers.STRING_ONLY) is True
    # 11. Invalid type
    assert not fastnumbers.check_float(["hey"])
    # 14. Invalid input string
    assert not fastnumbers.check_float("not_a_number")
    # 15. Invalid input string with numbers
    assert not fastnumbers.check_float("26.8 lb")
    # 16. Infinity
    assert not fastnumbers.check_float("inf")
    assert fastnumbers.check_float("inf", inf=fastnumbers.ALLOWED)
    assert fastnumbers.check_float("-infinity", inf=fastnumbers.ALLOWED)
    assert fastnumbers.check_float("-INFINITY", inf=fastnumbers.ALLOWED)
    # 17. NaN
    assert not fastnumbers.check_float("nAn")
    assert fastnumbers.check_float("nan", nan=fastnumbers.ALLOWED)
    assert fastnumbers.check_float("-NaN", nan=fastnumbers.ALLOWED)
    # 18. Sign/'e'/'.' only
    assert not fastnumbers.check_float("+")
    assert not fastnumbers.check_float("-")
    assert not fastnumbers.check_float("e")
    assert not fastnumbers.check_float("e8")
    assert not fastnumbers.check_float(".")
    # 20. Unicode numbers
    assert fastnumbers.check_float("⑦")
    assert not fastnumbers.check_float("⑦", allow_unicode_chars=False)
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
    # 5. float-like class
    assert not fastnumbers.check_int(FloatationDevice())
    # 6. int number
    assert fastnumbers.check_int(499)
    assert not fastnumbers.check_int(499, consider=fastnumbers.STRING_ONLY)
    assert fastnumbers.check_int(499, consider=fastnumbers.NUMBER_ONLY)
    # 7. signed int string
    assert fastnumbers.check_int("-499")
    assert fastnumbers.check_int("-499", consider=fastnumbers.STRING_ONLY)
    assert not fastnumbers.check_int("-499", consider=fastnumbers.NUMBER_ONLY)
    # 8. int string with padded whitespace
    assert fastnumbers.check_int("   +3001   ")
    # 9. long number
    assert fastnumbers.check_int(35892482945872302493)
    # 10. long string
    assert fastnumbers.check_int("35892482945872302493")
    # 11. int-like class
    assert fastnumbers.check_int(IndexCard())
    assert fastnumbers.check_int(PerformInt())
    # 12. return type
    assert fastnumbers.check_int(4029) is True
    assert fastnumbers.check_int(4029, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_int("4029") is True
    assert fastnumbers.check_int("4029", consider=fastnumbers.STRING_ONLY) is True
    assert fastnumbers.check_int("4029.50") is False
    assert fastnumbers.check_int(4029.50) is False
    # 11. Invalid type
    assert not fastnumbers.check_int(["hey"])
    # 14. Invalid input string
    assert not fastnumbers.check_int("not_a_number")
    # 15. Invalid input string with numbers
    assert not fastnumbers.check_int("26.8 lb")
    # 16. Infinity
    assert not fastnumbers.check_int("inf")
    # 17. NaN
    assert not fastnumbers.check_int("nan")
    # 18. Sign/'e'/'.' only
    assert not fastnumbers.check_int("+")
    assert not fastnumbers.check_int("-")
    assert not fastnumbers.check_int("e")
    assert not fastnumbers.check_int("e8")
    assert not fastnumbers.check_int(".")
    # 20. Unicode numbers
    assert fastnumbers.check_int("⑦")
    assert not fastnumbers.check_int("⑦", allow_unicode_chars=False)
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
    # 5. float-like class
    assert not fastnumbers.check_intlike(FloatationDevice())
    # 6. int number
    assert fastnumbers.check_intlike(499)
    # 7. signed int string
    assert fastnumbers.check_intlike("-499")
    # 8. int string with padded whitespace
    assert fastnumbers.check_intlike("   +3001   ")
    # 9. long number
    assert fastnumbers.check_intlike(35892482945872302493)
    # 10. long string
    assert fastnumbers.check_intlike("35892482945872302493")
    # 11. int-like class
    assert fastnumbers.check_intlike(IndexCard())
    assert fastnumbers.check_intlike(PerformInt())
    # 12. return type
    assert fastnumbers.check_intlike(4029) is True
    assert fastnumbers.check_intlike(4029, consider=fastnumbers.STRING_ONLY) is False
    assert fastnumbers.check_intlike("4029") is True
    assert fastnumbers.check_intlike("4029", consider=fastnumbers.STRING_ONLY) is True
    assert fastnumbers.check_intlike("4029.50") is False
    assert fastnumbers.check_intlike(4029.50) is False
    # 11. Invalid type
    assert not fastnumbers.check_intlike(["hey"])
    # 14. Invalid input string
    assert not fastnumbers.check_intlike("not_a_number")
    # 15. Invalid input string with numbers
    assert not fastnumbers.check_intlike("26.8 lb")
    # 16. Infinity
    assert not fastnumbers.check_intlike("inf")
    # 17. NaN
    assert not fastnumbers.check_intlike("nan")
    # 18. Sign/'e'/'.' only
    assert not fastnumbers.check_intlike("+")
    assert not fastnumbers.check_intlike("-")
    assert not fastnumbers.check_intlike("e")
    assert not fastnumbers.check_intlike("e8")
    assert not fastnumbers.check_intlike(".")
    # 20. Unicode numbers
    assert fastnumbers.check_intlike("⑦")
    assert not fastnumbers.check_intlike("⑦", allow_unicode_chars=False)
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
    # 5. float-like class
    assert fastnumbers.query_type(FloatationDevice()) is float
    # 6. int number
    assert fastnumbers.query_type(499) is int
    # 7. signed int string
    assert fastnumbers.query_type("-499") is int
    # 8. int string with padded whitespace
    assert fastnumbers.query_type("   +3001   ") is int
    # 9. long number
    assert fastnumbers.query_type(35892482945872302493) is int
    # 10. long string
    assert fastnumbers.query_type("35892482945872302493") is int
    # 11. int-like class
    assert fastnumbers.query_type(IndexCard()) is int
    assert fastnumbers.query_type(PerformInt()) is int
    # 12. coerced type
    assert fastnumbers.query_type(4029.0) is float
    assert fastnumbers.query_type("4029.0") is float
    assert fastnumbers.query_type(4029.0, coerce=True) is int
    assert fastnumbers.query_type("4029.0", coerce=True) is int
    # 13. Invalid type
    assert fastnumbers.query_type(["hey"]) is list
    # 14. Invalid input string
    assert fastnumbers.query_type("not_a_number") is str
    assert fastnumbers.query_type("not_a_number", allowed_types=(float, int)) is None
    # 15. Invalid input string with numbers
    assert fastnumbers.query_type("26.8 lb") is str
    assert fastnumbers.query_type("26.8 lb", allowed_types=(float, int)) is None
    # 16. Infinity
    assert fastnumbers.query_type("inf") is str
    assert fastnumbers.query_type("inf", allow_inf=True) is float
    assert fastnumbers.query_type("-iNFinity", allow_inf=True) is float
    # 17. NaN
    assert fastnumbers.query_type("nan") is str
    assert fastnumbers.query_type("nan", allow_nan=True) is float
    assert fastnumbers.query_type("-NaN", allow_nan=True) is float
    # 18. Sign/'e'/'.' only
    assert fastnumbers.query_type("+") is str
    assert fastnumbers.query_type("-") is str
    assert fastnumbers.query_type("e") is str
    assert fastnumbers.query_type("e8") is str
    assert fastnumbers.query_type(".") is str
    # 20. Unicode numbers
    assert fastnumbers.query_type("⑦") is int
    assert fastnumbers.query_type("⑦", allow_unicode_chars=False) is not int
    assert fastnumbers.query_type("⁸") is int
    assert fastnumbers.query_type("⅔") is float
    assert fastnumbers.query_type("Ⅴ") is float
