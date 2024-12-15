from __future__ import annotations

import ast
import builtins
import contextlib
import sys
import unittest
from typing import Callable

import pytest

from fastnumbers import int

import builtin_support as support
from builtin_grammar import (
    INVALID_UNDERSCORE_LITERALS,
    VALID_UNDERSCORE_LITERALS,
)

L = [
    ("0", 0),
    ("1", 1),
    ("9", 9),
    ("10", 10),
    ("99", 99),
    ("100", 100),
    ("314", 314),
    (" 314", 314),
    ("314 ", 314),
    ("  \t\t  314  \t\t  ", 314),
    (repr(sys.maxsize), sys.maxsize),
    ("  1x", ValueError),
    ("  1  ", 1),
    ("  1\02  ", ValueError),
    ("", ValueError),
    (" ", ValueError),
    ("  \t\t  ", ValueError),
    ("\u0200", ValueError),
]


class IntSubclass(builtins.int):
    pass


class IntTestCases(unittest.TestCase):
    def test_basic(self) -> None:
        assert int(314) == 314
        assert int(3.14) == 3
        # Check that conversion from float truncates towards zero
        assert int(-3.14) == -3
        assert int(3.9) == 3
        assert int(-3.9) == -3
        assert int(3.5) == 3
        assert int(-3.5) == -3
        assert int("-3") == -3
        assert int(" -3 ") == -3
        assert int("\u2003-3\u2002") == -3
        # Different base:
        assert int("10", 16) == 16
        # Test conversion from strings and various anomalies
        for s, v in L:
            for sign in "", "+", "-":
                for prefix in "", " ", "\t", "  \t\t  ":
                    ss = prefix + sign + s
                    vv = v
                    if sign == "-" and v is not ValueError:
                        vv = -v  # type: ignore[operator]
                    with contextlib.suppress(ValueError):
                        assert int(ss) == vv

        s = repr(-1 - sys.maxsize)
        x = int(s)
        assert x + 1 == -sys.maxsize
        assert isinstance(x, builtins.int)
        # should return int
        assert int(s[1:]) == sys.maxsize + 1

        # should return int
        x = int(1e100)
        assert isinstance(x, builtins.int)
        x = int(-1e100)
        assert isinstance(x, builtins.int)

        # SF bug 434186:  0x80000000/2 != 0x80000000>>1.
        # Worked by accident in Windows release build, but failed in debug build.
        # Failed in all Linux builds.
        x = -1 - sys.maxsize
        assert x >> 1 == x // 2

        x = int("1" * 600)
        assert isinstance(x, builtins.int)

        self.assertRaises(TypeError, int, 1, 12)

        assert int("0o123", 0) == 83
        assert int("0x123", 16) == 291

        # Bug 1679: "0x" is not a valid hex literal
        self.assertRaises(ValueError, int, "0x", 16)
        self.assertRaises(ValueError, int, "0x", 0)

        self.assertRaises(ValueError, int, "0o", 8)
        self.assertRaises(ValueError, int, "0o", 0)

        self.assertRaises(ValueError, int, "0b", 2)
        self.assertRaises(ValueError, int, "0b", 0)

        # SF bug 1334662: int(string, base) wrong answers
        # Various representations of 2**32 evaluated to 0
        # rather than 2**32 in previous versions

        assert int("100000000000000000000000000000000", 2) == 4294967296
        assert int("102002022201221111211", 3) == 4294967296
        assert int("10000000000000000", 4) == 4294967296
        assert int("32244002423141", 5) == 4294967296
        assert int("1550104015504", 6) == 4294967296
        assert int("211301422354", 7) == 4294967296
        assert int("40000000000", 8) == 4294967296
        assert int("12068657454", 9) == 4294967296
        assert int("4294967296", 10) == 4294967296
        assert int("1904440554", 11) == 4294967296
        assert int("9ba461594", 12) == 4294967296
        assert int("535a79889", 13) == 4294967296
        assert int("2ca5b7464", 14) == 4294967296
        assert int("1a20dcd81", 15) == 4294967296
        assert int("100000000", 16) == 4294967296
        assert int("a7ffda91", 17) == 4294967296
        assert int("704he7g4", 18) == 4294967296
        assert int("4f5aff66", 19) == 4294967296
        assert int("3723ai4g", 20) == 4294967296
        assert int("281d55i4", 21) == 4294967296
        assert int("1fj8b184", 22) == 4294967296
        assert int("1606k7ic", 23) == 4294967296
        assert int("mb994ag", 24) == 4294967296
        assert int("hek2mgl", 25) == 4294967296
        assert int("dnchbnm", 26) == 4294967296
        assert int("b28jpdm", 27) == 4294967296
        assert int("8pfgih4", 28) == 4294967296
        assert int("76beigg", 29) == 4294967296
        assert int("5qmcpqg", 30) == 4294967296
        assert int("4q0jto4", 31) == 4294967296
        assert int("4000000", 32) == 4294967296
        assert int("3aokq94", 33) == 4294967296
        assert int("2qhxjli", 34) == 4294967296
        assert int("2br45qb", 35) == 4294967296
        assert int("1z141z4", 36) == 4294967296

        # tests with base 0
        # this fails on 3.0, but in 2.x the old octal syntax is allowed
        assert int(" 0o123  ", 0) == 83
        assert int(" 0o123  ", 0) == 83
        assert int("000", 0) == 0
        assert int("0o123", 0) == 83
        assert int("0x123", 0) == 291
        assert int("0b100", 0) == 4
        assert int(" 0O123   ", 0) == 83
        assert int(" 0X123  ", 0) == 291
        assert int(" 0B100 ", 0) == 4

        # without base still base 10
        assert int("0123") == 123
        assert int("0123", 10) == 123

        # tests with prefix and base != 0
        assert int("0x123", 16) == 291
        assert int("0o123", 8) == 83
        assert int("0b100", 2) == 4
        assert int("0X123", 16) == 291
        assert int("0O123", 8) == 83
        assert int("0B100", 2) == 4

        # the code has special checks for the first character after the
        #  type prefix
        self.assertRaises(ValueError, int, "0b2", 2)
        self.assertRaises(ValueError, int, "0b02", 2)
        self.assertRaises(ValueError, int, "0B2", 2)
        self.assertRaises(ValueError, int, "0B02", 2)
        self.assertRaises(ValueError, int, "0o8", 8)
        self.assertRaises(ValueError, int, "0o08", 8)
        self.assertRaises(ValueError, int, "0O8", 8)
        self.assertRaises(ValueError, int, "0O08", 8)
        self.assertRaises(ValueError, int, "0xg", 16)
        self.assertRaises(ValueError, int, "0x0g", 16)
        self.assertRaises(ValueError, int, "0Xg", 16)
        self.assertRaises(ValueError, int, "0X0g", 16)

        # SF bug 1334662: int(string, base) wrong answers
        # Checks for proper evaluation of 2**32 + 1
        assert int("100000000000000000000000000000001", 2) == 4294967297
        assert int("102002022201221111212", 3) == 4294967297
        assert int("10000000000000001", 4) == 4294967297
        assert int("32244002423142", 5) == 4294967297
        assert int("1550104015505", 6) == 4294967297
        assert int("211301422355", 7) == 4294967297
        assert int("40000000001", 8) == 4294967297
        assert int("12068657455", 9) == 4294967297
        assert int("4294967297", 10) == 4294967297
        assert int("1904440555", 11) == 4294967297
        assert int("9ba461595", 12) == 4294967297
        assert int("535a7988a", 13) == 4294967297
        assert int("2ca5b7465", 14) == 4294967297
        assert int("1a20dcd82", 15) == 4294967297
        assert int("100000001", 16) == 4294967297
        assert int("a7ffda92", 17) == 4294967297
        assert int("704he7g5", 18) == 4294967297
        assert int("4f5aff67", 19) == 4294967297
        assert int("3723ai4h", 20) == 4294967297
        assert int("281d55i5", 21) == 4294967297
        assert int("1fj8b185", 22) == 4294967297
        assert int("1606k7id", 23) == 4294967297
        assert int("mb994ah", 24) == 4294967297
        assert int("hek2mgm", 25) == 4294967297
        assert int("dnchbnn", 26) == 4294967297
        assert int("b28jpdn", 27) == 4294967297
        assert int("8pfgih5", 28) == 4294967297
        assert int("76beigh", 29) == 4294967297
        assert int("5qmcpqh", 30) == 4294967297
        assert int("4q0jto5", 31) == 4294967297
        assert int("4000001", 32) == 4294967297
        assert int("3aokq95", 33) == 4294967297
        assert int("2qhxjlj", 34) == 4294967297
        assert int("2br45qc", 35) == 4294967297
        assert int("1z141z5", 36) == 4294967297

    def test_underscores(self) -> None:
        for lit in VALID_UNDERSCORE_LITERALS:
            if any(ch in lit for ch in ".eEjJ"):
                continue
            assert int(lit, 0) == ast.literal_eval(lit)
            assert int(lit, 0) == int(lit.replace("_", ""), 0)
        for lit in INVALID_UNDERSCORE_LITERALS:
            if any(ch in lit for ch in ".eEjJ"):
                continue
            self.assertRaises(ValueError, int, lit, 0)
        # Additional test cases with bases != 0, only for the constructor:
        assert int("1_00", 3) == 9
        assert int("0_100") == 100  # not valid as a literal!
        assert int(b"1_00") == 100  # byte underscore
        self.assertRaises(ValueError, int, "_100")
        self.assertRaises(ValueError, int, "+_100")
        self.assertRaises(ValueError, int, "1__00")
        self.assertRaises(ValueError, int, "100_")

    @support.cpython_only
    def test_small_ints(self) -> None:
        # Bug #3236: Return small longs from PyLong_FromString
        assert int("10") == 10
        assert int("-1") is -1
        assert int(b"10") == 10
        assert int(b"-1") is -1

    def test_no_args(self) -> None:
        assert int() == 0

    def test_keyword_args(self) -> None:
        # Test invoking int() using keyword arguments.
        assert int("100", base=2) == 4
        with pytest.raises(TypeError, match="keyword argument"):
            int(x=1.2)
        with pytest.raises(TypeError, match="keyword argument"):
            int(x="100", base=2)
        self.assertRaises(TypeError, int, base=10)
        self.assertRaises(TypeError, int, base=0)

    def test_int_base_limits(self) -> None:
        """Testing the supported limits of the int() base parameter."""
        assert int("0", 5) == 0
        with pytest.raises(ValueError):
            int("0", 1)
        with pytest.raises(ValueError):
            int("0", 37)
        with pytest.raises(ValueError):
            int("0", -909)  # An old magic value base from Python 2.
        with pytest.raises(ValueError):
            int("0", base=0 - (2**234))
        with pytest.raises(ValueError):
            int("0", base=2**234)
        # Bases 2 through 36 are supported.
        for base in range(2, 37):
            assert int("0", base=base) == 0

    def test_int_base_bad_types(self) -> None:
        """Not integer types are not valid bases; issue16772."""
        with pytest.raises(TypeError):
            int("0", 5.5)  # type: ignore[call-overload]
        with pytest.raises(TypeError):
            int("0", 5.0)  # type: ignore[call-overload]

    def test_int_base_indexable(self) -> None:
        class MyIndexable:
            def __init__(self, value: builtins.int) -> None:
                self.value = value

            def __index__(self) -> builtins.int:
                return self.value

        # Check out of range bases.
        for base in 2**100, -(2**100), 1, 37:
            with pytest.raises(ValueError):
                int("43", base)

        # Check in-range bases.
        assert int("101", base=MyIndexable(2)) == 5
        assert int("101", base=MyIndexable(10)) == 101
        assert int("101", base=MyIndexable(36)) == 1 + 36**2

    def test_non_numeric_input_types(self) -> None:
        # Test possible non-numeric types for the argument x, including
        # subclasses of the explicitly documented accepted types.
        class CustomStr(str):  # noqa: SLOT000
            pass

        class CustomBytes(bytes):
            pass

        class CustomByteArray(bytearray):
            pass

        factories: list[Callable[[bytes], bytes | bytearray | str]] = [
            bytes,
            bytearray,
            lambda b: CustomStr(b.decode()),
            CustomBytes,
            CustomByteArray,
            memoryview,
        ]
        try:
            from array import array
        except ImportError:
            pass
        else:
            factories.append(lambda b: array("B", b))  # type: ignore[arg-type,return-value]

        for f in factories:
            x = f(b"100")
            with self.subTest(type(x)):
                assert int(x) == 100
                if isinstance(x, (str, bytes, bytearray)):
                    assert int(x, 2) == 4
                else:
                    msg = "can't convert non-string"
                    with pytest.raises(TypeError, match=msg):
                        int(x, 2)
                with pytest.raises(ValueError, match="invalid literal"):
                    int(f(b"A" * 0x10))

    def test_int_memoryview(self) -> None:
        assert int(memoryview(b"123")[1:3]) == 23
        assert int(memoryview(b"123\x00")[1:3]) == 23
        assert int(memoryview(b"123 ")[1:3]) == 23
        assert int(memoryview(b"123A")[1:3]) == 23
        assert int(memoryview(b"1234")[1:3]) == 23

    def test_string_float(self) -> None:
        self.assertRaises(ValueError, int, "1.2")

    def test_intconversion(self) -> None:
        # Test __int__()
        class ClassicMissingMethods:
            pass

        self.assertRaises(TypeError, int, ClassicMissingMethods())

        class MissingMethods:
            pass

        self.assertRaises(TypeError, int, MissingMethods())

        class Foo0:
            def __int__(self) -> builtins.int:
                return 42

        assert int(Foo0()) == 42

        class Classic:
            pass

        for base in (object, Classic):

            class IntOverridesTrunc(base):  # type: ignore[misc,valid-type]
                def __int__(self) -> builtins.int:
                    return 42

                def __trunc__(self) -> builtins.int:
                    return -12

            assert int(IntOverridesTrunc()) == 42

    @unittest.skipUnless(sys.version_info >= (3, 8), "Test introduced in Python 3.8")
    def test_int_subclass_with_index(self) -> None:
        class MyIndex(builtins.int):
            def __index__(self) -> builtins.int:
                return 42

        class BadIndex(builtins.int):
            def __index__(self) -> builtins.float:  # type: ignore[override]
                return 42.0  # noqa: PLE0305

        my_int = MyIndex(7)
        assert my_int == 7
        assert int(my_int) == 7

        assert int(BadIndex()) == 0

    def test_int_subclass_with_int(self) -> None:
        class MyInt(builtins.int):
            def __int__(self) -> builtins.int:
                return 42

        class BadInt(builtins.int):
            def __int__(self) -> builtins.float:  # type: ignore[override]
                return 42.0

        my_int: MyInt | BadInt = MyInt(7)
        assert my_int == 7
        assert int(my_int) == 42

        my_int = BadInt(7)
        assert my_int == 7
        self.assertRaises(TypeError, int, my_int)

    def test_int_returns_int_subclass(self) -> None:
        class BadIndex:
            def __index__(self) -> bool:
                return True  # noqa: PLE0305

        class BadIndex2(builtins.int):
            def __index__(self) -> bool:
                return True  # noqa: PLE0305

        class BadInt:
            def __int__(self) -> bool:
                return True

        class BadInt2(builtins.int):
            def __int__(self) -> bool:
                return True

        bad_int: BadInt | BadIndex | BadIndex2
        bad_int = BadIndex()
        with self.assertWarns(DeprecationWarning):
            n = int(bad_int)
        assert n == 1
        assert type(n) is builtins.int

        bad_int = BadIndex2()
        n = int(bad_int)
        assert n == 0
        assert type(n) is builtins.int

        bad_int = BadInt()
        with self.assertWarns(DeprecationWarning):
            n = int(bad_int)
        assert n == 1
        assert type(n) is builtins.int

        bad_int2 = BadInt2()
        with self.assertWarns(DeprecationWarning):
            n = int(bad_int2)
        assert n == 1
        assert type(n) is builtins.int

    def test_error_message(self) -> None:
        def check(s: str | bytes, base: builtins.int | None = None) -> None:
            with self.assertRaises(ValueError, msg=f"int({s!r}, {base!r})") as cm:
                if base is None:
                    int(s)
                else:
                    int(s, base)
            msg = "invalid literal for int() with base "
            msg += f"{10 if base is None else base}: {s!r}"
            assert cm.exception.args[0] == msg

        check("\xbd")
        check("123\xbd")
        check("  123 456  ")

        check("123\x00")
        # SF bug 1545497: embedded NULs were not detected with explicit base
        check("123\x00", 10)
        check("123\x00 245", 20)
        check("123\x00 245", 16)
        check("123\x00245", 20)
        check("123\x00245", 16)
        # byte string with embedded NUL
        check(b"123\x00")
        check(b"123\x00", 10)
        # non-UTF-8 byte string
        check(b"123\xbd")
        check(b"123\xbd", 10)
        # lone surrogate in Unicode string
        check("123\ud800")
        check("123\ud800", 10)

    def test_issue31619(self) -> None:
        assert (
            int("1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1_0_1", 2)
            == 1431655765
        )
        assert int("1_2_3_4_5_6_7_0_1_2_3", 8) == 1402433619
        assert int("1_2_3_4_5_6_7_8_9", 16) == 4886718345
        assert int("1_2_3_4_5_6_7", 32) == 1144132807


if __name__ == "__main__":
    unittest.main()
