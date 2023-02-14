import array
import ctypes

import fastnumbers

import pytest


# Map supported data types to the Python array internal format designator
formats = {
    "char": "b",
    "unsigned char": "B",
    "short": "h",
    "unsigned short": "H",
    "int": "i",
    "unsigned int": "I",
    "long": "l",
    "unsigned long": "L",
    "long long": "q",
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
    "char": signed_size_extreme[ctypes.sizeof(ctypes.c_byte)],
    "unsigned char": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ubyte)],
    "short": signed_size_extreme[ctypes.sizeof(ctypes.c_short)],
    "unsigned short": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ushort)],
    "int": signed_size_extreme[ctypes.sizeof(ctypes.c_int)],
    "unsigned int": unsigned_size_extreme[ctypes.sizeof(ctypes.c_uint)],
    "long": signed_size_extreme[ctypes.sizeof(ctypes.c_long)],
    "unsigned long": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ulong)],
    "long long": signed_size_extreme[ctypes.sizeof(ctypes.c_longlong)],
    "unsigned long long": unsigned_size_extreme[ctypes.sizeof(ctypes.c_ulonglong)],
    "float": (1.17549e-38, 3.40282e38),
    "double": (2.22507e-308, 1.79769e308),
}

# Create shortcuts to collections of data types to test
signed_data_types = [
    "char",
    "short",
    "int",
    "long",
    "long long",
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


@pytest.mark.parametrize("data_type", data_types)
def test_invalid_input_type_gives_type_error(data_type):
    """Giving an invalid output type is rejected"""
    assert data_type  # To silence flake8
    given = [0, 1]
    with pytest.raises(TypeError):
        fastnumbers.fastnumbers.array(given, [])


kwargs = ["inf", "nan", "on_fail", "on_overflow", "on_type_error"]


class TestReplacements:
    """Test that the replacement mechanism works and raises intelligent exceptions"""

    @pytest.mark.parametrize("data_type", data_types)
    @pytest.mark.parametrize("kwarg", kwargs)
    def test_string_replacement_type_gives_type_error(self, data_type, kwarg):
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = f"The default value of 'not ok' given to option '{kwarg}' "
        expected += "has type 'str' which cannot be converted to a numeric value"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.fastnumbers.array(given, result, **{kwarg: "not ok"})

    @pytest.mark.parametrize("data_type", int_data_types)
    @pytest.mark.parametrize("kwarg", kwargs)
    def test_float_replacement_type_for_int_gives_value_error(self, data_type, kwarg):
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = rf"The default value of 1\.3 given to option '{kwarg}' "
        expected += f"cannot be converted to C type '{data_type}'"
        with pytest.raises(ValueError, match=expected):
            fastnumbers.fastnumbers.array(given, result, **{kwarg: 1.3})

    @pytest.mark.parametrize("data_type", float_data_types)
    @pytest.mark.parametrize("kwarg", kwargs)
    def test_int_replacement_type_for_float_is_OK(self, data_type, kwarg):
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = array.array(formats[data_type], [0.0, 1.0])
        fastnumbers.fastnumbers.array(given, result, **{kwarg: 0})
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    @pytest.mark.parametrize("kwarg", kwargs)
    def test_extreme_replacement_type_for_int_gives_overflow_error(
        self, data_type, kwarg
    ):
        given = [0, 1]
        result = array.array(formats[data_type], [0, 0])
        expected = rf"The default value of \S+ given to option '{kwarg}' "
        expected += f"cannot be converted to C type '{data_type}' without overflowing"
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.fastnumbers.array(
                given, result, **{kwarg: extremes[data_type][0] - 1}
            )
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.fastnumbers.array(
                given, result, **{kwarg: extremes[data_type][1] + 1}
            )

    @pytest.mark.parametrize("data_type", data_types)
    def test_replacement_callables_with_invalid_type_gives_type_error(self, data_type):
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "Callable passed to 'on_fail' with input 'invalid' returned "
        expected += "the value '3' that has type 'str' which cannot be converted "
        expected += "to a numeric value"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.fastnumbers.array(given, result, on_fail=lambda _: "3")

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_with_bad_number_gives_value_error(self, data_type):
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "Callable passed to 'on_fail' with input 'invalid' returned "
        expected += f"the value 3.4 that cannot be converted to C type '{data_type}'"
        with pytest.raises(ValueError, match=expected):
            fastnumbers.fastnumbers.array(given, result, on_fail=lambda _: 3.4)

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_with_extreme_gives_overflow_error(self, data_type):
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "Callable passed to 'on_fail' with input 'invalid' returned "
        expected += r"the value \S+ that cannot be converted to C type "
        expected += f"'{data_type}' without overflowing"
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.fastnumbers.array(
                given, result, on_fail=lambda _: extremes[data_type][1] + 1
            )

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_some_python_error(self, data_type):
        given = ["invalid"]
        result = array.array(formats[data_type], [0])
        expected = "bad operand type for abs()"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.fastnumbers.array(given, result, on_fail=lambda x: abs(x))

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacements_trigger_new_values_for_int(self, data_type):
        given = [
            "invalid",
            [2],
            extremes[data_type][0] - 1,
            extremes[data_type][1] + 1,
            "7",
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0])
        expected = array.array(formats[data_type], [2, 3, 1, 1, 7])
        fastnumbers.fastnumbers.array(
            given, result, on_fail=2, on_overflow=1, on_type_error=3
        )
        assert result == expected

    @pytest.mark.parametrize("data_type", float_data_types)
    def test_replacements_trigger_new_values_for_float(self, data_type):
        given = ["invalid", [2], "inf", "nan", float("inf"), float("nan"), "7"]
        result = array.array(formats[data_type], [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
        expected = array.array(formats[data_type], [2.0, 3.0, 5.0, 4.0, 5.0, 4.0, 7.0])
        fastnumbers.fastnumbers.array(
            given, result, inf=5, nan=4, on_fail=2, on_type_error=3
        )
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_replacement_callables_trigger_new_values_for_int(self, data_type):
        given = [
            "invalid",
            [2],
            extremes[data_type][0] - 1,
            extremes[data_type][1] + 1,
            "7",
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0])
        expected = array.array(formats[data_type], [2, 3, 1, 1, 7])
        fastnumbers.fastnumbers.array(
            given,
            result,
            on_fail=lambda _: 2,
            on_overflow=lambda _: 1,
            on_type_error=lambda _: 3,
        )
        assert result == expected

    @pytest.mark.parametrize("data_type", float_data_types)
    def test_replacement_callables_trigger_new_values_for_float(self, data_type):
        given = ["invalid", [2], "inf", "nan", float("inf"), float("nan"), "7"]
        result = array.array(formats[data_type], [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
        expected = array.array(formats[data_type], [2.0, 3.0, 5.0, 4.0, 5.0, 4.0, 7.0])
        fastnumbers.fastnumbers.array(
            given,
            result,
            inf=lambda _: 5,
            nan=lambda _: 4,
            on_fail=lambda _: 2,
            on_type_error=lambda _: 3,
        )
        assert result == expected


class TestErrors:
    """Test that intelligent exceptions are raised on error"""

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_invalid_string_raises_value_error(self, data_type):
        given = ["4", "78", "46", "invalid"]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = f"Cannot convert 'invalid' to C type '{data_type}'"
        with pytest.raises(ValueError, match=expected):
            fastnumbers.fastnumbers.array(given, result)

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_given_extreme_raises_overflow_error(self, data_type):
        given = ["4", "78", "46", extremes[data_type][1] + 1]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = rf"Cannot convert \S+ to C type '{data_type}' without overflowing"
        with pytest.raises(OverflowError, match=expected):
            fastnumbers.fastnumbers.array(given, result)

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_incorrect_type_raises_type_error(self, data_type):
        given = ["4", "78", "46", ["6"]]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = r"The value \['6'\] has type 'list' which cannot "
        expected += "be converted to a numeric value"
        with pytest.raises(TypeError, match=expected):
            fastnumbers.fastnumbers.array(given, result)


class TestSuccess:
    """Test that the function does what it says on the tin"""

    @pytest.mark.parametrize("data_type", data_types)
    def test_given_valid_values_returns_correct_results(self, data_type):
        given = ["4", "78", 46, "⑦"]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = array.array(formats[data_type], [4, 78, 46, 7])
        fastnumbers.fastnumbers.array(given, result)
        assert result == expected

    @pytest.mark.parametrize("data_type", int_data_types)
    def test_integer_extremes(self, data_type):
        given = [
            extremes[data_type][0],
            str(extremes[data_type][0]),
            extremes[data_type][1],
            str(extremes[data_type][1]),
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0])
        expected = array.array(
            formats[data_type],
            [
                extremes[data_type][0],
                extremes[data_type][0],
                extremes[data_type][1],
                extremes[data_type][1],
            ],
        )
        fastnumbers.fastnumbers.array(given, result)
        assert result == expected

    @pytest.mark.parametrize("data_type", float_data_types)
    def test_float_extremes(self, data_type):
        given = [
            extremes[data_type][0],
            str(extremes[data_type][0]),
            extremes[data_type][0] * 1e-100,  # so small, becomes 0.0
            str(extremes[data_type][0] * 1e-100),
            extremes[data_type][1],
            str(extremes[data_type][1]),
            extremes[data_type][1] * 1e10,  # so big, becomes infinity
            str(extremes[data_type][1] * 1e10),
        ]
        result = array.array(formats[data_type], [0, 0, 0, 0, 0, 0, 0, 0])
        expected = array.array(
            formats[data_type],
            [
                extremes[data_type][0],
                extremes[data_type][0],
                0.0,
                0.0,
                extremes[data_type][1],
                extremes[data_type][1],
                float("inf"),
                float("inf"),
            ],
        )
        fastnumbers.fastnumbers.array(given, result)
        assert result == expected
