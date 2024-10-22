from __future__ import annotations

import ast
import builtins
import sys
import time
import unittest
from math import copysign, isinf, isnan
from typing import Callable

import pytest
from typing_extensions import Self

from fastnumbers import float

import builtin_support as support
from builtin_grammar import (
    INVALID_UNDERSCORE_LITERALS,
    VALID_UNDERSCORE_LITERALS,
)

INF = float("inf")
NAN = float("nan")

have_getformat = hasattr(float, "__getformat__")
requires_getformat = unittest.skipUnless(have_getformat, "requires __getformat__")
requires_setformat = unittest.skipUnless(
    hasattr(float, "__setformat__"), "requires __setformat__"
)


class FloatSubclass(builtins.float):
    pass


class OtherFloatSubclass(builtins.float):
    pass


class GeneralFloatCases(unittest.TestCase):
    def test_float(self) -> None:
        assert float(3.14) == 3.14
        assert float(314) == 314.0
        assert float("  3.14  ") == 3.14
        self.assertRaises(ValueError, float, "  0x3.1  ")
        self.assertRaises(ValueError, float, "  -0x3.p-1  ")
        self.assertRaises(ValueError, float, "  +0x3.p-1  ")
        self.assertRaises(ValueError, float, "++3.14")
        self.assertRaises(ValueError, float, "+-3.14")
        self.assertRaises(ValueError, float, "-+3.14")
        self.assertRaises(ValueError, float, "--3.14")
        self.assertRaises(ValueError, float, ".nan")
        self.assertRaises(ValueError, float, "+.inf")
        self.assertRaises(ValueError, float, ".")
        self.assertRaises(ValueError, float, "-.")
        self.assertRaises(TypeError, float, {})
        self.assertRaisesRegex(TypeError, "not 'dict'", float, {})
        # Lone surrogate
        self.assertRaises(ValueError, float, "\ud8f0")
        # check that we don't accept alternate exponent markers
        self.assertRaises(ValueError, float, "-1.7d29")
        self.assertRaises(ValueError, float, "3D-14")
        assert float("  ٣.١٤  ") == 3.14
        assert float("\u20033.14\u2002") == 3.14
        # extra long strings should not be a problem
        float(b"." + b"1" * 1000)
        float("." + "1" * 1000)
        self.assertRaises(ValueError, float, "\u3053\u3093\u306b\u3061\u306f")

    def test_noargs(self) -> None:
        assert float() == 0.0

    def test_underscores(self) -> None:
        for lit in VALID_UNDERSCORE_LITERALS:
            if not any(ch in lit for ch in "jJxXoObB"):
                assert float(lit) == ast.literal_eval(lit)
                assert float(lit) == float(lit.replace("_", ""))
        for lit in INVALID_UNDERSCORE_LITERALS:
            if lit in ("0_7", "09_99"):  # octals are not recognized here
                continue
            if not any(ch in lit for ch in "jJxXoObB"):
                self.assertRaises(ValueError, float, lit)
        # Additional test cases; nan and inf are never valid as literals,
        # only in the float() constructor, but we don't allow underscores
        # in or around them.
        self.assertRaises(ValueError, float, "_NaN")
        self.assertRaises(ValueError, float, "Na_N")
        self.assertRaises(ValueError, float, "IN_F")
        self.assertRaises(ValueError, float, "-_INF")
        self.assertRaises(ValueError, float, "-INF_")
        # Check that we handle bytes values correctly.
        self.assertRaises(ValueError, float, b"0_.\xff9")

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
            factories.append(lambda b: array("B", b))  # type: ignore[return-value,arg-type]

        for f in factories:
            x = f(b" 3.14  ")
            with self.subTest(type(x)):
                assert float(x) == 3.14
                with pytest.raises(ValueError, match="could not convert"):
                    float(f(b"A" * 0x10))

    def test_float_memoryview(self) -> None:
        assert float(memoryview(b"12.3")[1:4]) == 2.3
        assert float(memoryview(b"12.3\x00")[1:4]) == 2.3
        assert float(memoryview(b"12.3 ")[1:4]) == 2.3
        assert float(memoryview(b"12.3A")[1:4]) == 2.3
        assert float(memoryview(b"12.34")[1:4]) == 2.3

    def test_error_message_old(self) -> None:
        testlist = ("\xbd", "123\xbd", "  123 456  ")
        for s in testlist:
            try:
                float(s)
            except ValueError as e:
                assert s.strip() in e.args[0]
            else:
                self.fail(f"Expected int({s!r}) to raise a ValueError")

    def test_error_message(self) -> None:
        def check(s: str | bytes) -> None:
            with self.assertRaises(ValueError, msg=f"float({s!r})") as cm:
                float(s)
            assert str(cm.exception) == f"could not convert string to float: {s!r}"

        check("\xbd")
        check("123\xbd")
        check("  123 456  ")
        check(b"  123 456  ")
        # all whitespace (cf. https://github.com/python/cpython/issues/95605)
        check("")
        check(" ")
        check("\t \n")

        # non-ascii digits (error came from non-digit '!')
        check("\u0663\u0661\u0664!")
        # embedded NUL
        check("123\x00")
        check("123\x00 245")
        check("123\x00245")
        # byte string with embedded NUL
        check(b"123\x00")
        # non-UTF-8 byte string
        check(b"123\xa0")

    @support.run_with_locale("LC_NUMERIC", "fr_FR", "de_DE")
    def test_float_with_comma(self) -> None:
        # set locale to something that doesn't use '.' for the decimal point
        # float must not accept the locale specific decimal point but
        # it still has to accept the normal python syntax
        import locale

        if locale.localeconv()["decimal_point"] != ",":
            self.skipTest('decimal_point is not ","')

        assert float("  3.14  ") == 3.14
        assert float("+3.14  ") == 3.14
        assert float("-3.14  ") == -3.14
        assert float(".14  ") == 0.14
        assert float("3.  ") == 3.0
        assert float("3.e3  ") == 3000.0
        assert float("3.2e3  ") == 3200.0
        assert float("2.5e-1  ") == 0.25
        assert float("5e-1") == 0.5
        self.assertRaises(ValueError, float, "  3,14  ")
        self.assertRaises(ValueError, float, "  +3,14  ")
        self.assertRaises(ValueError, float, "  -3,14  ")
        self.assertRaises(ValueError, float, "  0x3.1  ")
        self.assertRaises(ValueError, float, "  -0x3.p-1  ")
        self.assertRaises(ValueError, float, "  +0x3.p-1  ")
        assert float("  25.e-1  ") == 2.5
        self.assertAlmostEqual(float("  .25e-1  "), 0.025)

    def test_floatconversion(self) -> None:  # noqa: C901
        # Make sure that calls to __float__() work properly
        class Foo1:
            def __float__(self) -> builtins.float:
                return 42.0

        class Foo2(builtins.float):
            def __float__(self) -> builtins.float:
                return 42.0

        class Foo3(builtins.float):
            def __new__(cls, value: builtins.float = 0.0) -> Self:
                return builtins.float.__new__(cls, 2 * value)

            def __float__(self) -> builtins.float:
                return self

        class Foo4(builtins.float):
            def __float__(self) -> int:
                return 42

        # Issue 5759: __float__ not called on str subclasses (though it is on
        # unicode subclasses).
        class FooStr(str):  # noqa: SLOT000
            def __float__(self) -> builtins.float:
                return float(str(self)) + 1

        assert float(Foo1()) == 42.0
        assert float(Foo2()) == 42.0
        with self.assertWarns(DeprecationWarning):
            assert float(Foo3(21)) == 42.0
        assert float(FooStr("8")) == 9.0
        self.assertRaises(TypeError, float, Foo4(42))

        class Foo5:
            def __float__(self) -> str:
                return ""

        self.assertRaises(TypeError, time.sleep, Foo5())

        # Issue #24731
        class F:
            def __float__(self) -> builtins.float:
                return OtherFloatSubclass(42.0)

        with self.assertWarns(DeprecationWarning):
            assert float(F()) == 42.0
        with self.assertWarns(DeprecationWarning):
            assert type(float(F())) is builtins.float
        with self.assertWarns(DeprecationWarning):
            assert FloatSubclass(F()) == 42.0
        with self.assertWarns(DeprecationWarning):
            assert type(FloatSubclass(F())) is FloatSubclass

        class MyIndex:
            def __init__(self, value: int) -> None:
                self.value = value

            def __index__(self) -> int:
                return self.value

        assert float(MyIndex(42)) == 42.0
        self.assertRaises(OverflowError, float, MyIndex(2**2000))

        class MyInt:
            def __int__(self) -> int:
                return 42

        self.assertRaises(TypeError, float, MyInt())

    def test_keyword_args(self) -> None:
        with pytest.raises(TypeError, match="keyword argument"):
            float(x="3.14")


# Beginning with Python 2.6 float has cross platform compatible
# ways to create and represent inf and nan
class InfNanTest(unittest.TestCase):
    def test_inf_from_str(self) -> None:
        assert isinf(float("inf"))
        assert isinf(float("+inf"))
        assert isinf(float("-inf"))
        assert isinf(float("infinity"))
        assert isinf(float("+infinity"))
        assert isinf(float("-infinity"))

        assert repr(float("inf")) == "inf"
        assert repr(float("+inf")) == "inf"
        assert repr(float("-inf")) == "-inf"
        assert repr(float("infinity")) == "inf"
        assert repr(float("+infinity")) == "inf"
        assert repr(float("-infinity")) == "-inf"

        assert repr(float("INF")) == "inf"
        assert repr(float("+Inf")) == "inf"
        assert repr(float("-iNF")) == "-inf"
        assert repr(float("Infinity")) == "inf"
        assert repr(float("+iNfInItY")) == "inf"
        assert repr(float("-INFINITY")) == "-inf"

        assert str(float("inf")) == "inf"
        assert str(float("+inf")) == "inf"
        assert str(float("-inf")) == "-inf"
        assert str(float("infinity")) == "inf"
        assert str(float("+infinity")) == "inf"
        assert str(float("-infinity")) == "-inf"

        self.assertRaises(ValueError, float, "info")
        self.assertRaises(ValueError, float, "+info")
        self.assertRaises(ValueError, float, "-info")
        self.assertRaises(ValueError, float, "in")
        self.assertRaises(ValueError, float, "+in")
        self.assertRaises(ValueError, float, "-in")
        self.assertRaises(ValueError, float, "infinit")
        self.assertRaises(ValueError, float, "+Infin")
        self.assertRaises(ValueError, float, "-INFI")
        self.assertRaises(ValueError, float, "infinitys")

        self.assertRaises(ValueError, float, "++Inf")
        self.assertRaises(ValueError, float, "-+inf")
        self.assertRaises(ValueError, float, "+-infinity")
        self.assertRaises(ValueError, float, "--Infinity")

    def test_inf_as_str(self) -> None:
        assert repr(1e300 * 1e300) == "inf"
        assert repr(-1e300 * 1e300) == "-inf"

        assert str(1e300 * 1e300) == "inf"
        assert str(-1e300 * 1e300) == "-inf"

    def test_nan_from_str(self) -> None:
        assert isnan(float("nan"))
        assert isnan(float("+nan"))
        assert isnan(float("-nan"))

        assert repr(float("nan")) == "nan"
        assert repr(float("+nan")) == "nan"
        assert repr(float("-nan")) == "nan"

        assert repr(float("NAN")) == "nan"
        assert repr(float("+NAn")) == "nan"
        assert repr(float("-NaN")) == "nan"

        assert str(float("nan")) == "nan"
        assert str(float("+nan")) == "nan"
        assert str(float("-nan")) == "nan"

        self.assertRaises(ValueError, float, "nana")
        self.assertRaises(ValueError, float, "+nana")
        self.assertRaises(ValueError, float, "-nana")
        self.assertRaises(ValueError, float, "na")
        self.assertRaises(ValueError, float, "+na")
        self.assertRaises(ValueError, float, "-na")

        self.assertRaises(ValueError, float, "++nan")
        self.assertRaises(ValueError, float, "-+NAN")
        self.assertRaises(ValueError, float, "+-NaN")
        self.assertRaises(ValueError, float, "--nAn")

    def test_nan_as_str(self) -> None:
        assert repr(1e300 * 1e300 * 0) == "nan"
        assert repr(-1e300 * 1e300 * 0) == "nan"

        assert str(1e300 * 1e300 * 0) == "nan"
        assert str(-1e300 * 1e300 * 0) == "nan"

    def test_inf_signs(self) -> None:
        assert copysign(1.0, float("inf")) == 1.0
        assert copysign(1.0, float("-inf")) == -1.0

    @unittest.skipUnless(
        getattr(sys, "float_repr_style", "") == "short",
        "applies only when using short float repr style",
    )
    def test_nan_signs(self) -> None:
        # When using the dtoa.c code, the sign of float('nan') should
        # be predictable.
        assert copysign(1.0, float("nan")) == 1.0
        assert copysign(1.0, float("-nan")) == -1.0


if __name__ == "__main__":
    unittest.main()
