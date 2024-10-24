from __future__ import annotations

import array
import ctypes
from typing import Any, Callable, Iterator, NoReturn

import numpy as np
import pytest
from conftest import base_n
from hypothesis import given as hyp_given
from hypothesis.strategies import (
    binary,
    floats,
    integers,
    lists,
    text,
)
from typing_extensions import TypedDict

import fastnumbers

# Map supported data types to the Python array internal format designator
formats = {
    "signed char": "b",
    "unsigned char": "B",
    "signed short": "h",
    "unsigned short": "H",
    "signed int": "i",
    "unsigned int": "I",
    "signed long": "l",
    "unsigned long": "L",
    "signed long long": "q",
    "unsigned long long": "Q",
    "float": "f",
    "double": "d",
}

# Extreme values for signed integers of various byte widths
signed_size_extreme = {
    1: (-128, 127),
    2: (-32768, 32767),
    4: (-2147483648, 2147483647),
    8: (-9223372036854775808, 9223372036854775807),
}

# Extreme values for unsigned integers of various byte widths
unsigned_size_extreme = {
    1: (0, 255),
    2: (0, 65535),
    4: (0, 4294967295),
    8: (0, 18446744073709551615),
}

# Map all supported data types to the extreme values they support
extremes = {
    "signed char": signed_size_extreme[ctypes.sizeof(ctypes.c_byte)],
    "unsigned char": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ubyte)],
    "signed short": signed_size_extreme[ctypes.sizeof(ctypes.c_short)],
    "unsigned short": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ushort)],
    "signed int": signed_size_extreme[ctypes.sizeof(ctypes.c_int)],
    "unsigned int": unsigned_size_extreme[ctypes.sizeof(ctypes.c_uint)],
    "signed long": signed_size_extreme[ctypes.sizeof(ctypes.c_long)],
    "unsigned long": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ulong)],
    "signed long long": signed_size_extreme[ctypes.sizeof(ctypes.c_longlong)],
    "unsigned long long": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ulonglong)],
}

float_extremes = {
    "float": (1.17549e-38, 3.40282e38),
    "double": (2.22507e-308, 1.79769e308),
}

# Create shortcuts to collections of data types to test
signed_data_types = [
    "signed char",
    "signed short",
    "signed int",
    "signed long",
    "signed long long",
]
unsigned_data_types = [
    "unsigned char",
    "unsigned short",
    "unsigned int",
    "unsigned long",
    "unsigned long long",
]
int_data_types = signed_data_types + unsigned_data_types
float_data_types = ["float", "double"]
data_types = int_data_types + float_data_types


def test_invalid_argument_raises_type_error() -> None:
    given = [0, 1]
    with pytest.raises(TypeError, match="got an unexpected keyword argument 'invalid'"):
        fastnumbers.try_array(given, invalid="dummy")  # type: ignore[call-overload]


@pytest.mark.parametrize(
    "selector",
    [
        fastnumbers.RAISE,
        fastnumbers.INPUT,
        fastnumbers.DISALLOWED,
        fastnumbers.NUMBER_ONLY,
        fastnumbers.STRING_ONLY,
    ],
)
def test_selectors_are_rejected_when_invalid_for_inf_and_nan(selector: object) -> None:
    with pytest.raises(ValueError, match="values for 'inf' and 'nan'"):
        fastnumbers.try_array(["5"], inf=selector)
    with pytest.raises(ValueError, match="values for 'inf' and 'nan'"):
        fastnumbers.try_array(["5"], nan=selector)


@pytest.mark.parametrize(
    "selector",
    [
        fastnumbers.ALLOWED,
        fastnumbers.INPUT,
        fastnumbers.DISALLOWED,
        fastnumbers.NUMBER_ONLY,
        fastnumbers.STRING_ONLY,
    ],
)
def test_selectors_are_rejected_when_invalid_for_on_fail_and_friends(
    selector: object,
) -> None:
    msg = "values for 'on_fail', 'on_overflow', and 'on_type_error'"
    with pytest.raises(ValueError, match=msg):
        fastnumbers.try_array(["5"], on_fail=selector)
    with pytest.raises(ValueError, match=msg):
        fastnumbers.try_array(["5"], on_overflow=selector)
    with pytest.raises(ValueError, match=msg):
        fastnumbers.try_array(["5"], on_type_error=selector)


def test_invalid_input_type_gives_type_error() -> None:
    """Giving an invalid output type is rejected"""
    given = [0, 1]
    expected = "Only numpy ndarray and array.array types for output are "
    expected += r"supported, not <class 'list'>"
    with pytest.raises(TypeError, match=expected):
        fastnumbers.try_array(given, [])  # type: ignore[call-overload]


def test_require_output_if_numpy_is_not_installed() -> None:
    """A missing output value requires numpy to construct the output"""
    given = [0, 1]
    orig = fastnumbers.has_numpy
    try:
        fastnumbers.has_numpy = False
        with pytest.raises(RuntimeError, match="To use fastnumbers"):
            fastnumbers.try_array(given)
    finally:
        fastnumbers.has_numpy = orig


@pytest.mark.parametrize(
    "output",
    [np.array([[0, 0], [0, 0]]), np.array(0)],
)
def test_require_identically_one_ndarray_dimension(
    output: np.ndarray[Any, np.dtype[np.int_]],
) -> None:
    with pytest.raises(ValueError, match="Can only accept arrays of dimension 1"):
        fastnumbers.try_array([0, 9], output)


def test_require_input_and_output_to_have_equal_size() -> None:
    output = array.array("d", [0, 0, 0])
    with pytest.raises(ValueError, match="input/output must be of equal size"):
        fastnumbers.try_array([0, 9], output)


# Not all dtypes exist on all platforms. Add only the ones to test
# here that exist.
other_dtypes = [
    getattr(np, x)
    for x in ("float128", "complex128", "half", "bool_", "bytes_", "str_")
    if hasattr(np, x)
]


@pytest.mark.parametrize("dtype", other_dtypes)
def test_invalid_numpy_dtypes_raises_correct_type_error(
    dtype: np.dtype[Any],
) -> None:
    """Numpy arrays as output with invalid dtypes give an error"""
    given = [0, 1]
    output = np.array([0, 0], dtype=dtype)
    expected = "The only supported numpy dtypes for output are: "
    with pytest.raises(TypeError, match=expected):
        fastnumbers.try_array(given, output)


class TestCPPProtections:
    """
    These tests check internal C++ error handling that should not
    be possible to encounter from the python interface
    """

    def test_non_memorybuffer_type_raises_correct_type_error(self) -> None:
        """Ensure we only accept well-behaved memory views as input"""
        with pytest.raises(TypeError, match="not 'list'"):
            fastnumbers._array([0, 1], [0, 0])  # type: ignore[attr-defined] # noqa: SLF001

    @pytest.mark.parametrize("dtype", other_dtypes)
    def test_invalid_memorybuffer_type_raises_correct_type_error(
        self, dtype: np.dtype[Any]
    ) -> None:
        """Ensure we only accept well-behaved memory views as input"""
        given = [0, 1]
        output = np.array([0, 0], dtype=dtype)
        exception = r"Unknown buffer format '\S+' for object"
        with pytest.raises(TypeError, match=exception):
            fastnumbers._array(given, output)  # type: ignore[attr-defined] # noqa: SLF001


kwargs = ["inf", "nan", "on_fail", "on_overflow", "on_type_error"]


class KwargsType(TypedDict, total=False):
    inf: int
    nan: int
    on_fail: int
    on_overflow: int
    on_type_error: int


class TestReplacements:
    """Test that the replacement mechanism works and raises intelligent exceptions"""

    @pytest.mark.parametrize("data_type", data_types)
    @pytest.mark.parametrize("kwarg", kwargs)
    def test_string_replacement_type_gives_type_error(
        self, data_type: str, kwarg: str
    ) -> None:
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = f"The default value of 'not ok' given to option '{kwarg}' "
        expected += "has type 'str' which cannot be converted to a numeric value"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.try_array(given, result, **{kwarg: "not ok"})  # type: ignore[call-overload]

    @pytest.mark.parametrize("data_type", int_data_types)
    @pytest.mark.parametrize("kwarg", kwargs)
    def test_float_replacement_type_for_int_gives_value_error(
        self, data_type: str, kwarg: str
    ) -> None:
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = rf"The default value of 1\.3 given to option '{kwarg}' "
        expected += f"cannot be converted to C type '{data_type}'"
        with pytest.raises(ValueError, match=expected):
            fastnumbers.try_array(given, result, **{kwarg: 1.3})  # type: ignore[call-overload]

    @pytest.mark.parametrize("data_type", float_data_types)
    @pytest.mark.parametrize(
        "kwargs",
        [
            KwargsType(inf=0),
            KwargsType(nan=0),
            KwargsType(on_fail=0),
            KwargsType(on_overflow=0),
            KwargsType(on_type_error=0),
        ],
    )
    def test_int_replacement_type_for_float_is_ok(
        self, data_type: str, kwargs: KwargsType
    ) -> None:
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = array.array(formats[data_type], [0.0, 1.0])
        fastnumbers.try_array(given, result, **kwargs)
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    @pytest.mark.parametrize("kwarg", kwargs)
    def test_extreme_replacement_type_for_int_gives_overflow_error(
        self, data_type: str, kwarg: str
    ) -> None:
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = rf"The default value of \S+ given to option '{kwarg}' "
        expected += f"cannot be converted to C type '{data_type}' without overflowing"
        kwargs: KwargsType = {kwarg: extremes[data_type][0] - 1}  # type: ignore[misc]
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.try_array(given, result, **kwargs)
        kwargs = {kwarg: extremes[data_type][1] + 1}  # type: ignore[misc]
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.try_array(given, result, **kwargs)

    @pytest.mark.parametrize("data_type", data_types)
    def test_replacement_callables_with_invalid_type_gives_type_error(
        self, data_type: str
    ) -> None:
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "Callable passed to 'on_fail' with input 'invalid' returned "
        expected += "the value '3' that has type 'str' which cannot be converted "
        expected += "to a numeric value"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.try_array(given, result, on_fail=lambda _: "3")

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_with_bad_number_gives_value_error(
        self, data_type: str
    ) -> None:
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "Callable passed to 'on_fail' with input 'invalid' returned "
        expected += f"the value 3.4 that cannot be converted to C type '{data_type}'"
        with pytest.raises(ValueError, match=expected):
            fastnumbers.try_array(given, result, on_fail=lambda _: 3.4)

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_with_extreme_gives_overflow_error(
        self, data_type: str
    ) -> None:
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "Callable passed to 'on_fail' with input 'invalid' returned "
        expected += r"the value \S+ that cannot be converted to C type "
        expected += f"'{data_type}' without overflowing"
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.try_array(
                given, result, on_fail=lambda _: extremes[data_type][1] + 1
            )

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_some_python_error(self, data_type: str) -> None:
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "bad operand type for abs()"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.try_array(given, result, on_fail=lambda x: abs(x))

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacements_trigger_new_values_for_int(self, data_type: str) -> None:
        given = [
            "invalid",
            [2],
            extremes[data_type][0] - 1,
            extremes[data_type][1] + 1,
            "7",
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0])
        expected = array.array(formats[data_type], [2, 3, 1, 1, 7])
        fastnumbers.try_array(given, result, on_fail=2, on_overflow=1, on_type_error=3)
        assert result == expected

    @pytest.mark.parametrize("data_type", float_data_types)
    def test_replacements_trigger_new_values_for_float(self, data_type: str) -> None:
        given = ["invalid", [2], "inf", "nan", float("inf"), float("nan"), "7"]
        result = array.array(formats[data_type], [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
        expected = array.array(formats[data_type], [2.0, 3.0, 5.0, 4.0, 5.0, 4.0, 7.0])
        fastnumbers.try_array(given, result, inf=5, nan=4, on_fail=2, on_type_error=3)
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_trigger_new_values_for_int(
        self, data_type: str
    ) -> None:
        given = [
            "invalid",
            [2],
            extremes[data_type][0] - 1,
            extremes[data_type][1] + 1,
            "7",
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0])
        expected = array.array(formats[data_type], [2, 3, 1, 1, 7])
        fastnumbers.try_array(
            given,
            result,
            on_fail=lambda _: 2,
            on_overflow=lambda _: 1,
            on_type_error=lambda _: 3,
        )
        assert result == expected

    @pytest.mark.parametrize("data_type", float_data_types)
    def test_replacement_callables_trigger_new_values_for_float(
        self, data_type: str
    ) -> None:
        given = ["invalid", [2], "inf", "nan", float("inf"), float("nan"), "7"]
        result = array.array(formats[data_type], [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
        expected = array.array(formats[data_type], [2.0, 3.0, 5.0, 4.0, 5.0, 4.0, 7.0])
        fastnumbers.try_array(
            given,
            result,
            inf=lambda _: 5,
            nan=lambda _: 4,
            on_fail=lambda _: 2,
            on_type_error=lambda _: 3,
        )
        assert result == expected


class DumbFloatClass:
    def __float__(self) -> NoReturn:
        msg = "something here might go wrong"
        raise ValueError(msg)


class DumbIntClass:
    def __int__(self) -> NoReturn:
        msg = "something here might go wrong"
        raise ValueError(msg)


class TestErrors:
    """Test that intelligent exceptions are raised on error"""

    @pytest.mark.parametrize("dumb", [DumbFloatClass(), DumbIntClass()])
    @pytest.mark.parametrize("data_type", data_types)
    @pytest.mark.parametrize("style", [list, iter])
    def test_given_junk_float_type_raises_error(
        self,
        data_type: str,
        dumb: DumbFloatClass | DumbIntClass,
        style: Callable[[Any], Any],
    ) -> None:
        given = style([dumb])
        result = array.array(formats[data_type], [0])
        expected = "Cannot convert"
        with pytest.raises(ValueError, match=expected):
            fastnumbers.try_array(given, result)

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_invalid_string_raises_value_error(self, data_type: str) -> None:
        given = ["4", "78", "46", "invalid"]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = f"Cannot convert 'invalid' to C type '{data_type}'"
        with pytest.raises(ValueError, match=expected):
            fastnumbers.try_array(given, result)

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_given_extreme_raises_overflow_error(self, data_type: str) -> None:
        given = ["4", "78", "46", extremes[data_type][1] + 1]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = rf"Cannot convert \S+ to C type '{data_type}' without overflowing"
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.try_array(given, result)

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_incorrect_type_raises_type_error(self, data_type: str) -> None:
        given = ["4", "78", "46", ["6"]]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = r"The value \['6'\] has type 'list' which cannot "
        expected += "be converted to a numeric value"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.try_array(given, result)

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_broken_generator_fails(self, data_type: str) -> None:
        """A generator's exception should be returned"""

        def broken() -> Iterator[str]:
            """Not a good generator"""
            yield "5"
            yield "6"
            msg = "Fëanor"
            raise ValueError(msg)

        output = array.array(formats[data_type], [0, 0, 0, 0])
        with pytest.raises(ValueError, match="Fëanor"):
            fastnumbers.try_array(broken(), output)

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_non_iterable_raises_type_error(self, data_type: str) -> None:
        output = array.array(formats[data_type], [0, 0, 0, 0])
        with pytest.raises(TypeError, match="'int' object is not iterable"):
            fastnumbers.try_array(5, output)  # type: ignore[call-overload]

    @pytest.mark.parametrize("data_type", data_types)
    @pytest.mark.parametrize("style", [list, iter])
    def test_given_invalid_types_behave_as_expected(
        self, data_type: str, style: Callable[[Any], Any]
    ) -> None:
        msg = r"The value \('Fëanor',\) has type 'tuple' which cannot be "
        msg += "converted to a numeric value"
        with pytest.raises(TypeError, match=msg):
            fastnumbers.try_array(style([("Fëanor",)]))
        expected = array.array(formats[data_type], [5])
        result = array.array(formats[data_type], [0])
        fastnumbers.try_array(style([("Fëanor",)]), result, on_type_error=5)
        assert result == expected


class TestSuccess:
    """Test that the function does what it says on the tin"""

    @pytest.mark.parametrize("data_type", data_types)
    @pytest.mark.parametrize("style", [list, tuple, iter])
    def test_given_valid_values_returns_correct_results(
        self, data_type: str, style: Callable[[Any], Any]
    ) -> None:
        given = style(["4", "78", 46, "⑦"])
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = array.array(formats[data_type], [4, 78, 46, 7])
        fastnumbers.try_array(given, result)
        assert result == expected

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_range_returns_correct_results(self, data_type: str) -> None:
        given = range(4)
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = array.array(formats[data_type], [0, 1, 2, 3])
        fastnumbers.try_array(given, result)
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_integer_extremes(self, data_type: str) -> None:
        given = [
            extremes[data_type][0],
            str(extremes[data_type][0]),
            extremes[data_type][0] - 1,
            str(extremes[data_type][0] - 1),
            extremes[data_type][1],
            str(extremes[data_type][1]),
            extremes[data_type][1] + 1,
            str(extremes[data_type][1] + 1),
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0, 0, 0, 0])
        expected = array.array(
            formats[data_type],
            [
                extremes[data_type][0],
                extremes[data_type][0],
                123,
                123,
                extremes[data_type][1],
                extremes[data_type][1],
                123,
                123,
            ],
        )
        fastnumbers.try_array(given, result, on_overflow=123)
        assert result == expected

    @pytest.mark.parametrize("data_type", float_data_types)
    def test_float_extremes(self, data_type: str) -> None:
        given = [
            float_extremes[data_type][0],
            str(float_extremes[data_type][0]),
            float_extremes[data_type][0] * 1e-100,  # so small, becomes 0.0
            str(float_extremes[data_type][0] * 1e-100),
            float_extremes[data_type][1],
            str(float_extremes[data_type][1]),
            float_extremes[data_type][1] * 1e10,  # so big, becomes infinity
            str(float_extremes[data_type][1] * 1e10),
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0, 0, 0, 0])
        expected = array.array(
            formats[data_type],
            [
                float_extremes[data_type][0],
                float_extremes[data_type][0],
                0.0,
                0.0,
                float_extremes[data_type][1],
                float_extremes[data_type][1],
                float("inf"),
                float("inf"),
            ],
        )
        fastnumbers.try_array(given, result)
        assert result == expected

    @pytest.mark.parametrize("base", range(2, 37))
    @pytest.mark.parametrize("data_type", int_data_types)
    def test_integer_bases(self, base: int, data_type: str) -> None:
        given = (
            base_n(x, base)
            for x in [
                extremes[data_type][0],
                extremes[data_type][0] - 1,
                extremes[data_type][1],
                extremes[data_type][1] + 1,
                0,
                100,
            ]
        )
        result = array.array(formats[data_type], [0, 0, 0, 0, 0, 0])
        expected = array.array(
            formats[data_type],
            [
                extremes[data_type][0],
                123,
                extremes[data_type][1],
                123,
                0,
                100,
            ],
        )
        fastnumbers.try_array(given, result, base=base, on_overflow=123)
        assert result == expected

    @pytest.mark.parametrize("data_type", data_types)
    def test_underscores(self, data_type: str) -> None:
        given = ["1_0", "11_0"]
        result = array.array(formats[data_type], [0, 0])
        expected = array.array(formats[data_type], [10, 110])
        fastnumbers.try_array(given, result, allow_underscores=True)
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_base_prefix(self, data_type: str) -> None:
        given = [
            bin(extremes[data_type][0]),
            oct(extremes[data_type][0]),
            hex(extremes[data_type][0]),
        ]
        result = array.array(formats[data_type], [0, 0, 0])
        expected = array.array(formats[data_type], [extremes[data_type][0]] * 3)
        fastnumbers.try_array(given, result, base=0)
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_base_prefix_and_underscores(self, data_type: str) -> None:
        given = ["0b10_01", "0x3_a", "0o5_7", "0b_0", "0x_f", "0o_5"]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0, 0])
        expected = array.array(formats[data_type], [9, 58, 47, 0, 15, 5])
        fastnumbers.try_array(given, result, base=0, allow_underscores=True)
        assert result == expected


# Create shortcuts to collections of dtypes to test
signed_dtypes: list[Any] = [
    np.int8,
    np.int16,
    np.int32,
    np.int64,
]
unsigned_dtypes: list[Any] = [
    np.uint8,
    np.uint16,
    np.uint32,
    np.uint64,
]
int_dtypes = signed_dtypes + unsigned_dtypes
float_dtypes: list[Any] = [np.float32, np.float64]
dtypes = int_dtypes + float_dtypes
dtype_extremes: dict[Any, tuple[int, int]] = {
    np.int8: signed_size_extreme[1],
    np.uint8: unsigned_size_extreme[1],
    np.int16: signed_size_extreme[2],
    np.uint16: unsigned_size_extreme[2],
    np.int32: signed_size_extreme[4],
    np.uint32: unsigned_size_extreme[4],
    np.int64: signed_size_extreme[8],
    np.uint64: unsigned_size_extreme[8],
}

dtype_float_extremes: dict[Any, tuple[float, float]] = {
    np.float32: (1.17549e-38, 3.40282e38),
    np.float64: (2.22507e-308, 1.79769e308),
}


class TestNumpy:
    """Ensure that try_array well supports numpy arrays"""

    def test_default_dtype_is_float64(self) -> None:
        given = [4, 4.5, "5", "5.6", "nan", "inf"]
        expected = np.array([4, 4.5, 5, 5.6, np.nan, np.inf], dtype=np.float64)
        result = fastnumbers.try_array(given)
        assert result.dtype == np.float64
        assert np.array_equal(result, expected, equal_nan=True)

    def test_accepts_iterable_as_input(self) -> None:
        given = iter([4, 4.5, "5", "5.6", "nan", "inf"])
        expected = np.array([4, 4.5, 5, 5.6, np.nan, np.inf], dtype=np.float64)
        result = fastnumbers.try_array(given)
        assert np.array_equal(result, expected, equal_nan=True)

    @pytest.mark.parametrize("dtype", dtypes)
    def test_supported_dtypes(
        self, dtype: np.dtype[np.int_] | np.dtype[np.float64]
    ) -> None:
        given = [4, "5", "⑦"]
        expected = np.array([4, 5, 7], dtype=dtype)
        result = fastnumbers.try_array(given, dtype=dtype)
        assert result.dtype == dtype
        assert np.array_equal(result, expected)

    @pytest.mark.parametrize("dtype", int_dtypes)
    def test_integer_extremes(self, dtype: np.dtype[np.int_]) -> None:
        given = [
            dtype_extremes[dtype][0],
            str(dtype_extremes[dtype][0]),
            dtype_extremes[dtype][0] - 1,
            str(dtype_extremes[dtype][0] - 1),
            dtype_extremes[dtype][1],
            str(dtype_extremes[dtype][1]),
            dtype_extremes[dtype][1] + 1,
            str(dtype_extremes[dtype][1] + 1),
        ]
        expected = np.array(
            [
                dtype_extremes[dtype][0],
                dtype_extremes[dtype][0],
                123,
                123,
                dtype_extremes[dtype][1],
                dtype_extremes[dtype][1],
                123,
                123,
            ],
            dtype=dtype,
        )
        result = fastnumbers.try_array(given, dtype=dtype, on_overflow=123)
        assert np.array_equal(result, expected)

    @pytest.mark.parametrize("dtype", float_dtypes)
    def test_float_extremes(self, dtype: np.dtype[np.float64]) -> None:
        given = [
            dtype_float_extremes[dtype][0],
            str(dtype_float_extremes[dtype][0]),
            dtype_float_extremes[dtype][0] * 1e-100,  # so small, becomes 0.0
            str(dtype_float_extremes[dtype][0] * 1e-100),
            dtype_float_extremes[dtype][1],
            str(dtype_float_extremes[dtype][1]),
            dtype_float_extremes[dtype][1] * 1e10,  # so big, becomes infinity
            str(dtype_float_extremes[dtype][1] * 1e10),
        ]
        expected = np.array(
            [
                dtype_float_extremes[dtype][0],
                dtype_float_extremes[dtype][0],
                0.0,
                0.0,
                dtype_float_extremes[dtype][1],
                dtype_float_extremes[dtype][1],
                float("inf"),
                float("inf"),
            ],
            dtype=dtype,
        )
        result = fastnumbers.try_array(given, dtype=dtype)
        assert np.array_equal(result, expected)

    @pytest.mark.parametrize("dtype", dtypes)
    def test_accepts_output_array(
        self, dtype: np.dtype[np.int_] | np.dtype[np.float64]
    ) -> None:
        given = [4, "5", "⑦"]
        result = np.array([0, 0, 0], dtype=dtype)
        expected = np.array([4, 5, 7], dtype=dtype)
        fastnumbers.try_array(given, result)
        assert np.array_equal(result, expected)

    def test_slice(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array([0, 0, 0, 0, 0])
        expected = np.array([0, 4, 5, 7, 0])
        fastnumbers.try_array(given, result[1:4])
        assert np.array_equal(result, expected)

    def test_strides(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array([0, 0, 0, 0, 0, 0])
        expected = np.array([4, 0, 5, 0, 7, 0])
        fastnumbers.try_array(given, result[::2])
        assert np.array_equal(result, expected)

    def test_strides_offset(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array([0, 0, 0, 0, 0, 0])
        expected = np.array([0, 4, 0, 5, 0, 7])
        fastnumbers.try_array(given, result[1::2])
        assert np.array_equal(result, expected)

    def test_negative_strides(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array([0, 0, 0, 0, 0, 0])
        expected = np.array([0, 7, 0, 5, 0, 4])
        fastnumbers.try_array(given, result[::-2])
        assert np.array_equal(result, expected)

    def test_slice_2d(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array([[0, 0, 0], [0, 0, 0], [0, 0, 0]])
        expected = np.array([[0, 0, 0], [4, 5, 7], [0, 0, 0]])
        fastnumbers.try_array(given, result[1, :])
        assert np.array_equal(result, expected)

    def test_slice_2d_column(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array([[0, 0, 0], [0, 0, 0], [0, 0, 0]])
        expected = np.array([[0, 4, 0], [0, 5, 0], [0, 7, 0]])
        fastnumbers.try_array(given, result[:, 1])
        assert np.array_equal(result, expected)

    def test_stride_2d(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array([[0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0]])
        expected = np.array(
            [[0, 0, 0, 0, 0, 0], [4, 0, 5, 0, 7, 0], [0, 0, 0, 0, 0, 0]]
        )
        fastnumbers.try_array(given, result[1, ::2])
        assert np.array_equal(result, expected)

    def test_stride_2d_column(self) -> None:
        given = [4, "5", "⑦"]
        result = np.array(
            [[0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0]]
        )
        expected = np.array(
            [[0, 0, 0], [0, 4, 0], [0, 0, 0], [0, 5, 0], [0, 0, 0], [0, 7, 0]]
        )
        fastnumbers.try_array(given, result[1::2, 1])
        assert np.array_equal(result, expected)


@hyp_given(
    lists(
        floats() | integers() | text() | binary() | lists(integers(), max_size=1),
        max_size=50,
    )
)
@pytest.mark.parametrize("dtype", int_dtypes)
def test_all_the_things_for_ints(dtype: np.dtype[np.int_], x: list[Any]) -> None:
    # Using try_array should give the same results
    # as try_int with map=list then converted to an array.
    # Under-the-hood, the on_fail, etc. replacements use a different code path
    # so this test is not just wasting time.
    expected_pre = fastnumbers.try_int(
        x,
        on_fail=lambda x: 5 if isinstance(x, str) else 6,
        on_type_error=7,
        map=list,
    )
    expected_pre = [9 if x < dtype_extremes[dtype][0] else x for x in expected_pre]
    expected_pre = [9 if x > dtype_extremes[dtype][1] else x for x in expected_pre]
    expected = np.array(expected_pre, dtype=dtype)
    result = fastnumbers.try_array(
        x,
        dtype=dtype,
        on_fail=lambda x: 5 if isinstance(x, str) else 6,
        on_type_error=7,
        on_overflow=9,
    )
    np.array_equal(result, expected)


@hyp_given(
    lists(
        floats() | integers() | text() | binary() | lists(integers(), max_size=1),
        max_size=50,
    )
)
@pytest.mark.parametrize("dtype", float_dtypes)
@pytest.mark.filterwarnings("ignore:overflow encountered in cast")
def test_all_the_things_for_floats(dtype: np.dtype[np.float64], x: list[Any]) -> None:
    # Using try_array should give the same results
    # as try_float with map=list then converted to an array.
    # Under-the-hood, the on_fail, etc. replacements use a different code path
    # so this test is not just wasting time.
    expected_pre = fastnumbers.try_float(
        x,
        on_fail=lambda x: 5.0 if isinstance(x, str) else 6.0,
        on_type_error=7.0,
        map=list,
    )
    expected = np.array(expected_pre, dtype=dtype)
    result = fastnumbers.try_array(
        x,
        dtype=dtype,
        on_fail=lambda x: 5.0 if isinstance(x, str) else 6.0,
        on_type_error=7.0,
    )
    np.array_equal(result, expected, equal_nan=True)


@hyp_given(
    lists(
        floats() | integers() | text() | binary() | lists(integers(), max_size=1),
        max_size=50,
    )
)
@pytest.mark.parametrize("dtype", float_dtypes)
@pytest.mark.filterwarnings("ignore:overflow encountered in cast")
def test_all_the_things_for_floats_with_nan_inf_replacement(
    dtype: np.dtype[np.float64], x: list[Any]
) -> None:
    # Using try_array should give the same results
    # as try_float with map=list then converted to an array.
    # Under-the-hood, the on_fail, etc. replacements use a different code path
    # so this test is not just wasting time.
    expected_pre = fastnumbers.try_float(
        x,
        inf=1.0,
        nan=3.0,
        on_fail=lambda x: 5.0 if isinstance(x, str) else 6.0,
        on_type_error=7.0,
        map=list,
    )
    expected = np.array(expected_pre, dtype=dtype)
    result = fastnumbers.try_array(
        x,
        dtype=dtype,
        inf=1.0,
        nan=3.0,
        on_fail=lambda x: 5.0 if isinstance(x, str) else 6.0,
        on_type_error=7.0,
    )
    np.array_equal(result, expected)


def test_sanity() -> None:
    """Make sure explictly passing dtypes makes mypy happy"""
    fastnumbers.try_array([0, 1], dtype=np.int8)
    fastnumbers.try_array([0, 1], dtype=np.int16)
    fastnumbers.try_array([0, 1], dtype=np.int32)
    fastnumbers.try_array([0, 1], dtype=np.int64)
    fastnumbers.try_array([0, 1], dtype=np.uint8)
    fastnumbers.try_array([0, 1], dtype=np.uint16)
    fastnumbers.try_array([0, 1], dtype=np.uint32)
    fastnumbers.try_array([0, 1], dtype=np.uint64)
    fastnumbers.try_array([0, 1], dtype=np.float32)
    fastnumbers.try_array([0, 1], dtype=np.float64)
