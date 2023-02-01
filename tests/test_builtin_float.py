import builtins
import sys
import time
import unittest
from math import copysign, isinf, isnan
from typing import Callable, List, Union

import builtin_support as support
from builtin_grammar import (
    INVALID_UNDERSCORE_LITERALS,
    VALID_UNDERSCORE_LITERALS,
)

from fastnumbers import float

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
        self.assertEqual(float(3.14), 3.14)
        self.assertEqual(float(314), 314.0)
        self.assertEqual(float("  3.14  "), 3.14)
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
        if sys.version_info >= (3, 7):
            self.assertRaises(ValueError, float, "\uD8F0")
        # check that we don't accept alternate exponent markers
        self.assertRaises(ValueError, float, "-1.7d29")
        self.assertRaises(ValueError, float, "3D-14")
        self.assertEqual(float("  \u0663.\u0661\u0664  "), 3.14)
        self.assertEqual(float("\N{EM SPACE}3.14\N{EN SPACE}"), 3.14)
        # extra long strings should not be a problem
        float(b"." + b"1" * 1000)
        float("." + "1" * 1000)
        if sys.version_info >= (3, 7):
            # Invalid unicode string
            # See bpo-34087
            self.assertRaises(ValueError, float, "\u3053\u3093\u306b\u3061\u306f")

    def test_noargs(self) -> None:
        self.assertEqual(float(), 0.0)

    def test_underscores(self) -> None:
        for lit in VALID_UNDERSCORE_LITERALS:
            if not any(ch in lit for ch in "jJxXoObB"):
                self.assertEqual(float(lit), eval(lit))
                self.assertEqual(float(lit), float(lit.replace("_", "")))
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
        class CustomStr(str):
            pass

        class CustomBytes(bytes):
            pass

        class CustomByteArray(bytearray):
            pass

        factories: List[Callable[[bytes], Union[bytes, bytearray, str]]] = [
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
            factories.append(lambda b: array("B", b))  # type: ignore

        for f in factories:
            x = f(b" 3.14  ")
            with self.subTest(type(x)):
                self.assertEqual(float(x), 3.14)
                with self.assertRaisesRegex(ValueError, "could not convert"):
                    float(f(b"A" * 0x10))

    def test_float_memoryview(self) -> None:
        self.assertEqual(float(memoryview(b"12.3")[1:4]), 2.3)
        self.assertEqual(float(memoryview(b"12.3\x00")[1:4]), 2.3)
        self.assertEqual(float(memoryview(b"12.3 ")[1:4]), 2.3)
        self.assertEqual(float(memoryview(b"12.3A")[1:4]), 2.3)
        self.assertEqual(float(memoryview(b"12.34")[1:4]), 2.3)

    def test_error_message_old(self) -> None:
        testlist = ("\xbd", "123\xbd", "  123 456  ")
        for s in testlist:
            try:
                float(s)
            except ValueError as e:
                self.assertIn(s.strip(), e.args[0])
            else:
                self.fail("Expected int(%r) to raise a ValueError" % s)

    def test_error_message(self) -> None:
        def check(s: Union[str, bytes]) -> None:
            with self.assertRaises(ValueError, msg="float(%r)" % (s,)) as cm:
                float(s)
            self.assertEqual(
                str(cm.exception), "could not convert string to float: %r" % (s,)
            )

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

        if not locale.localeconv()["decimal_point"] == ",":
            self.skipTest('decimal_point is not ","')

        self.assertEqual(float("  3.14  "), 3.14)
        self.assertEqual(float("+3.14  "), 3.14)
        self.assertEqual(float("-3.14  "), -3.14)
        self.assertEqual(float(".14  "), 0.14)
        self.assertEqual(float("3.  "), 3.0)
        self.assertEqual(float("3.e3  "), 3000.0)
        self.assertEqual(float("3.2e3  "), 3200.0)
        self.assertEqual(float("2.5e-1  "), 0.25)
        self.assertEqual(float("5e-1"), 0.5)
        self.assertRaises(ValueError, float, "  3,14  ")
        self.assertRaises(ValueError, float, "  +3,14  ")
        self.assertRaises(ValueError, float, "  -3,14  ")
        self.assertRaises(ValueError, float, "  0x3.1  ")
        self.assertRaises(ValueError, float, "  -0x3.p-1  ")
        self.assertRaises(ValueError, float, "  +0x3.p-1  ")
        self.assertEqual(float("  25.e-1  "), 2.5)
        self.assertAlmostEqual(float("  .25e-1  "), 0.025)

    def test_floatconversion(self) -> None:  # noqa: C901
        # Make sure that calls to __float__() work properly
        class Foo1(object):
            def __float__(self) -> builtins.float:
                return 42.0

        class Foo2(builtins.float):
            def __float__(self) -> builtins.float:
                return 42.0

        class Foo3(builtins.float):
            def __new__(cls, value: builtins.float = 0.0) -> "Foo3":
                return builtins.float.__new__(cls, 2 * value)

            def __float__(self) -> builtins.float:
                return self

        class Foo4(builtins.float):
            def __float__(self) -> int:
                return 42

        # Issue 5759: __float__ not called on str subclasses (though it is on
        # unicode subclasses).
        class FooStr(str):
            def __float__(self) -> builtins.float:
                return float(str(self)) + 1

        self.assertEqual(float(Foo1()), 42.0)
        self.assertEqual(float(Foo2()), 42.0)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(float(Foo3(21)), 42.0)
        self.assertEqual(float(FooStr("8")), 9.0)
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
            self.assertEqual(float(F()), 42.0)
        with self.assertWarns(DeprecationWarning):
            self.assertIs(type(float(F())), builtins.float)
        with self.assertWarns(DeprecationWarning):
            self.assertEqual(FloatSubclass(F()), 42.0)
        with self.assertWarns(DeprecationWarning):
            self.assertIs(type(FloatSubclass(F())), FloatSubclass)

        if sys.version_info >= (3, 8):

            class MyIndex:
                def __init__(self, value: int) -> None:
                    self.value = value

                def __index__(self) -> int:
                    return self.value

            self.assertEqual(float(MyIndex(42)), 42.0)
            self.assertRaises(OverflowError, float, MyIndex(2**2000))

            class MyInt:
                def __int__(self) -> int:
                    return 42

            self.assertRaises(TypeError, float, MyInt())

    def test_keyword_args(self) -> None:
        with self.assertRaisesRegex(TypeError, "keyword argument"):
            float(x="3.14")


# Beginning with Python 2.6 float has cross platform compatible
# ways to create and represent inf and nan
class InfNanTest(unittest.TestCase):
    def test_inf_from_str(self) -> None:
        self.assertTrue(isinf(float("inf")))
        self.assertTrue(isinf(float("+inf")))
        self.assertTrue(isinf(float("-inf")))
        self.assertTrue(isinf(float("infinity")))
        self.assertTrue(isinf(float("+infinity")))
        self.assertTrue(isinf(float("-infinity")))

        self.assertEqual(repr(float("inf")), "inf")
        self.assertEqual(repr(float("+inf")), "inf")
        self.assertEqual(repr(float("-inf")), "-inf")
        self.assertEqual(repr(float("infinity")), "inf")
        self.assertEqual(repr(float("+infinity")), "inf")
        self.assertEqual(repr(float("-infinity")), "-inf")

        self.assertEqual(repr(float("INF")), "inf")
        self.assertEqual(repr(float("+Inf")), "inf")
        self.assertEqual(repr(float("-iNF")), "-inf")
        self.assertEqual(repr(float("Infinity")), "inf")
        self.assertEqual(repr(float("+iNfInItY")), "inf")
        self.assertEqual(repr(float("-INFINITY")), "-inf")

        self.assertEqual(str(float("inf")), "inf")
        self.assertEqual(str(float("+inf")), "inf")
        self.assertEqual(str(float("-inf")), "-inf")
        self.assertEqual(str(float("infinity")), "inf")
        self.assertEqual(str(float("+infinity")), "inf")
        self.assertEqual(str(float("-infinity")), "-inf")

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
        self.assertEqual(repr(1e300 * 1e300), "inf")
        self.assertEqual(repr(-1e300 * 1e300), "-inf")

        self.assertEqual(str(1e300 * 1e300), "inf")
        self.assertEqual(str(-1e300 * 1e300), "-inf")

    def test_nan_from_str(self) -> None:
        self.assertTrue(isnan(float("nan")))
        self.assertTrue(isnan(float("+nan")))
        self.assertTrue(isnan(float("-nan")))

        self.assertEqual(repr(float("nan")), "nan")
        self.assertEqual(repr(float("+nan")), "nan")
        self.assertEqual(repr(float("-nan")), "nan")

        self.assertEqual(repr(float("NAN")), "nan")
        self.assertEqual(repr(float("+NAn")), "nan")
        self.assertEqual(repr(float("-NaN")), "nan")

        self.assertEqual(str(float("nan")), "nan")
        self.assertEqual(str(float("+nan")), "nan")
        self.assertEqual(str(float("-nan")), "nan")

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
        self.assertEqual(repr(1e300 * 1e300 * 0), "nan")
        self.assertEqual(repr(-1e300 * 1e300 * 0), "nan")

        self.assertEqual(str(1e300 * 1e300 * 0), "nan")
        self.assertEqual(str(-1e300 * 1e300 * 0), "nan")

    def test_inf_signs(self) -> None:
        self.assertEqual(copysign(1.0, float("inf")), 1.0)
        self.assertEqual(copysign(1.0, float("-inf")), -1.0)

    @unittest.skipUnless(
        getattr(sys, "float_repr_style", "") == "short",
        "applies only when using short float repr style",
    )
    def test_nan_signs(self) -> None:
        # When using the dtoa.c code, the sign of float('nan') should
        # be predictable.
        self.assertEqual(copysign(1.0, float("nan")), 1.0)
        self.assertEqual(copysign(1.0, float("-nan")), -1.0)


if __name__ == "__main__":
    unittest.main()
