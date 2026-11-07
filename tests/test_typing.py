from __future__ import annotations

import array
from collections.abc import Iterator
from typing import TYPE_CHECKING, Any, TypeVar

import numpy as np
import numpy.typing as npt
from typing_extensions import assert_type

from fastnumbers import (
    ALLOWED,
    DISALLOWED,
    INPUT,
    NUMBER_ONLY,
    RAISE,
    STRING_ONLY,
    check_float,
    check_int,
    check_intlike,
    check_real,
    fast_float,
    fast_forceint,
    fast_int,
    fast_real,
    float as fn_float,
    int as fn_int,
    isfloat,
    isint,
    isintlike,
    isreal,
    query_type,
    real,
    try_array,
    try_float,
    try_forceint,
    try_int,
    try_real,
)

if TYPE_CHECKING:
    from fastnumbers.fastnumbers import (
        ALLOWED_T,
        DISALLOWED_T,
        INPUT_T,
        NUMBER_ONLY_T,
        RAISE_T,
        STRING_ONLY_T,
    )
else:
    # Tests WILL fail at runtime - skip if not type checking.
    import pytest

    pytest.skip(allow_module_level=True)


class FloatationDevice:
    def __float__(self) -> float:
        return 1.5


class IndexCard:
    def __index__(self) -> int:
        return 42


class PerformInt:
    def __int__(self) -> int:
        return 13


def test_selector_types() -> None:
    if TYPE_CHECKING:
        assert_type(ALLOWED, ALLOWED_T)
        assert_type(DISALLOWED, DISALLOWED_T)
        assert_type(INPUT, INPUT_T)
        assert_type(RAISE, RAISE_T)
        assert_type(STRING_ONLY, STRING_ONLY_T)
        assert_type(NUMBER_ONLY, NUMBER_ONLY_T)


T = TypeVar("T")


def passthrough(x: T) -> T:
    return x


def an_int(_: object) -> int:
    return 4


def a_float(_: object) -> float:
    return 4.5


def a_str(_: object) -> str:
    return "oof"


# Annotated list types to make the type checking easier below.
l_str: list[str] = ["1", "2.0"]
l_int: list[int] = [1, 2]
l_float: list[float] = [1.0, 1.5]
l_mixed: list[int | float | str] = [1, "2", 3.1]
l_obj: list[object] = [object()]


def test_try_real_types() -> None:
    # Integer inputs
    assert_type(try_real(1), int)
    assert_type(try_real(1, inf="hello"), int)
    assert_type(try_real(1, nan=ALLOWED), int)
    assert_type(try_real(1, inf=passthrough, nan=passthrough), int)
    assert_type(try_real(1, inf=an_int, nan=a_str), int)
    assert_type(try_real(1, inf=4, nan=ALLOWED), int)
    assert_type(try_real(1, on_fail={}, on_type_error=[]), int)
    assert_type(try_real(1, denoise=True, allow_underscores=False), int)

    # Float inputs
    assert_type(try_real(1.5), float | int)
    assert_type(try_real(1.5, coerce=False), float)
    assert_type(try_real(1.5, inf="hello"), Any)
    assert_type(try_real(1.5, nan=ALLOWED), float | int)
    assert_type(try_real(1.5, nan=ALLOWED, coerce=False), float)
    assert_type(try_real(1.5, inf=passthrough, nan=passthrough), float | int)
    assert_type(try_real(1.5, inf=passthrough, nan=passthrough, coerce=False), float)
    assert_type(try_real(1.5, inf=passthrough, nan=a_str), Any)
    assert_type(try_real(1.5, inf=an_int, nan=a_float), float | int)
    assert_type(try_real(1.5, inf=4, nan=ALLOWED), float | int)
    assert_type(try_real(1.5, on_fail={}, on_type_error=[]), float | int)
    assert_type(try_real(1.5, on_fail={}, on_type_error=[], coerce=False), float)
    assert_type(try_real(1.5, denoise=True, allow_underscores=False), float | int)
    assert_type(try_real(1.5, inf=ALLOWED, allow_unicode_chars=False), float | int)

    # String inputs
    assert_type(try_real("1"), int | float | str)
    assert_type(try_real("1", inf="hello"), int | float | str)
    assert_type(try_real("1", nan=ALLOWED), int | float | str)
    assert_type(try_real("1", inf=passthrough, nan=passthrough), int | float | str)
    assert_type(try_real("1", inf=passthrough, nan=a_float), int | float | str)
    assert_type(try_real("1", inf=an_int, nan=a_float), int | float | str)
    assert_type(try_real("1", inf=4, nan=ALLOWED, on_fail=RAISE), int | float)
    assert_type(try_real("1", on_fail={}, on_type_error=[]), Any)
    assert_type(try_real("1", denoise=True, allow_underscores=False), int | float | str)
    assert_type(
        try_real("1", inf=ALLOWED, allow_unicode_chars=False, on_fail=RAISE),
        int | float,
    )

    # Any old inputs
    assert_type(try_real(object()), Any)
    assert_type(try_real(object(), inf="hello"), Any)
    assert_type(try_real(object(), nan=ALLOWED), Any)
    assert_type(try_real(object(), inf=passthrough, nan=passthrough), Any)
    assert_type(try_real(object(), inf=an_int, nan=a_str), Any)
    assert_type(try_real(object(), inf=4, nan=ALLOWED), Any)
    assert_type(try_real(object(), on_fail=0, on_type_error=0), float | int)
    assert_type(try_real(object(), denoise=True, allow_underscores=False), Any)

    # Iterable inputs
    assert_type(try_real(l_int, map=list), list[int | float])
    assert_type(try_real(l_float, map=list, coerce=False), list[int | float])
    assert_type(try_real(l_obj, on_fail=RAISE, map=list), list[int | float])
    assert_type(try_real(l_obj, map=list), list[Any])
    assert_type(try_real(l_str, map=list), list[int | float | str])
    assert_type(try_real(l_mixed, map=list), list[int | float | str])
    assert_type(try_real(l_mixed, map=list, on_fail=RAISE), list[int | float])
    assert_type(try_real(l_str, map=list, on_fail=RAISE), list[int | float])
    assert_type(
        try_real(l_obj, map=list, on_fail=0, on_type_error=0), list[int | float]
    )
    assert_type(try_real(l_int, map=True), Iterator[int | float])
    assert_type(try_real(l_float, map=True, coerce=False), Iterator[int | float])
    assert_type(try_real(l_obj, map=True), Iterator[Any])
    assert_type(try_real(l_obj, on_fail=RAISE, map=True), Iterator[int | float])
    assert_type(try_real(l_str, map=True), Iterator[int | float | str])
    assert_type(try_real(l_mixed, map=True), Iterator[int | float | str])
    assert_type(try_real(l_mixed, map=True, on_fail=RAISE), Iterator[int | float])
    assert_type(try_real(l_str, map=True, on_fail=RAISE), Iterator[int | float])


def test_try_float_types() -> None:
    # Integer inputs
    assert_type(try_float(1), float)
    assert_type(try_float(1, inf="hello"), Any)
    assert_type(try_float(1, nan=ALLOWED), float)
    assert_type(try_float(1, inf=passthrough, nan=passthrough), float)
    assert_type(try_float(1, inf=an_int, nan=a_str), Any)
    assert_type(try_float(1, inf=4, nan=ALLOWED), float)
    assert_type(try_float(1, on_fail={}, on_type_error=[]), float)
    assert_type(try_float(1, allow_underscores=False), float)

    # Float inputs
    assert_type(try_float(1.5), float)
    assert_type(try_float(1.5, inf="hello"), Any)
    assert_type(try_float(1.5, nan=ALLOWED), float)
    assert_type(try_float(1.5, inf=passthrough, nan=passthrough), float)
    assert_type(try_float(1.5, inf=an_int, nan=a_float), float)
    assert_type(try_float(1.5, inf=4, nan=ALLOWED), float)
    assert_type(try_float(1.5, on_fail={}, on_type_error=[]), float)
    assert_type(try_float(1.5, allow_underscores=False), float)
    assert_type(try_float(1.5, inf=ALLOWED, allow_unicode_chars=False), float)

    # String inputs
    assert_type(try_float("1.5"), float | str)
    assert_type(try_float("bad"), float | str)
    assert_type(try_float("bad", inf="hello"), float | str)
    assert_type(try_float("bad", nan=ALLOWED), float | str)
    assert_type(try_float("bad", inf=passthrough, nan=passthrough), float | str)
    assert_type(try_float("bad", inf=passthrough, nan=a_float), float | str)
    assert_type(try_float("bad", inf=an_int, nan=a_float), float | str)
    assert_type(try_float("bad", inf=4, nan=ALLOWED, on_fail=RAISE), float)
    assert_type(try_float("bad", on_fail={}, on_type_error=[]), Any)
    assert_type(try_float("bad", allow_underscores=False), float | str)
    assert_type(try_float("bad", inf=ALLOWED, on_fail=RAISE), float)
    assert_type(try_float("bad", on_fail=0.5), float)
    assert_type(try_float("bad", on_fail=passthrough), float | str)

    # Any old inputs
    assert_type(try_float(object()), Any)
    assert_type(try_float(object(), inf="hello"), Any)
    assert_type(try_float(object(), nan=ALLOWED), Any)
    assert_type(try_float(object(), inf=passthrough, nan=passthrough), Any)
    assert_type(try_float(object(), inf=an_int, nan=a_str), Any)
    assert_type(try_float(object(), inf=4, nan=ALLOWED), Any)
    assert_type(try_float(object(), on_fail=0.0, on_type_error=0.0), float)
    assert_type(try_float(object(), allow_underscores=False), Any)

    # Iterable inputs
    assert_type(try_float(l_int, map=list), list[float])
    assert_type(try_float(l_float, map=list), list[float])
    assert_type(try_float(l_obj, on_fail=RAISE, map=list), list[float])
    assert_type(try_float(l_obj, map=list), list[Any])
    assert_type(try_float(l_str, map=list), list[float | str])
    assert_type(try_float(l_mixed, map=list), list[float | str])
    assert_type(try_float(l_mixed, map=list, on_fail=RAISE), list[float])
    assert_type(try_float(l_str, map=list, on_fail=RAISE), list[float])
    assert_type(try_float(l_obj, map=list, on_fail=0.0, on_type_error=0.0), list[float])
    assert_type(try_float(l_obj, map=list), list[Any])
    assert_type(try_float(l_int, map=True), Iterator[float])
    assert_type(try_float(l_float, map=True), Iterator[float])
    assert_type(try_float(l_obj, map=True), Iterator[Any])
    assert_type(try_float(l_obj, on_fail=RAISE, map=True), Iterator[float])
    assert_type(try_float(l_str, map=True), Iterator[float | str])
    assert_type(try_float(l_mixed, map=True), Iterator[float | str])
    assert_type(try_float(l_mixed, map=True, on_fail=RAISE), Iterator[float])
    assert_type(try_float(l_str, map=True, on_fail=RAISE), Iterator[float])


def test_try_int_types() -> None:
    # Integer inputs
    assert_type(try_int(1), int)
    assert_type(try_int(1, on_fail={}, on_type_error=[]), int)
    assert_type(try_int(1, allow_underscores=False), int)

    # Float inputs
    assert_type(try_int(1.5), int)
    assert_type(try_int(1.5, allow_underscores=True), int)
    assert_type(try_int(1.5, on_fail={}, on_type_error=[]), int)
    assert_type(try_int(1.5, allow_unicode_chars=False), int)

    # String inputs
    assert_type(try_int("42"), int | str)
    assert_type(try_int("bad"), int | str)
    assert_type(try_int("bad", base=10), int | str)
    assert_type(try_int("bad", on_fail=1), int)
    assert_type(try_int("bad", on_fail=1.5), Any)
    assert_type(try_int("bad", on_fail={}, on_type_error=[]), Any)
    assert_type(try_int("bad", allow_underscores=False), int | str)
    assert_type(try_int("bad", on_fail=RAISE), int)
    assert_type(try_int("bad", on_fail=passthrough, base=10), int | str)
    assert_type(try_int("42", base=16, allow_unicode_chars=True), int | str)

    # Any old inputs
    assert_type(try_int(object()), Any)
    assert_type(try_int(object(), on_fail=0, on_type_error=0), int)
    assert_type(try_int(object(), allow_underscores=False), Any)

    # Iterable inputs
    assert_type(try_int(l_int, map=list), list[int])
    assert_type(try_int(l_float, map=list), list[int])
    assert_type(try_int(l_obj, on_fail=RAISE, map=list), list[int])
    assert_type(try_int(l_obj, map=list), list[Any])
    assert_type(try_int(l_str, map=list), list[int | str])
    assert_type(try_int(l_mixed, map=list), list[int | str])
    assert_type(try_int(l_mixed, map=list, on_fail=RAISE), list[int])
    assert_type(try_int(l_str, map=list, on_fail=RAISE, base=10), list[int])
    assert_type(try_int(l_obj, map=list, on_fail=0, on_type_error=0), list[int])
    assert_type(try_int(l_int, map=True), Iterator[int])
    assert_type(try_int(l_float, map=True), Iterator[int])
    assert_type(try_int(l_obj, map=True), Iterator[Any])
    assert_type(try_int(l_obj, on_fail=RAISE, map=True), Iterator[int])
    assert_type(try_int(l_str, map=True), Iterator[int | str])
    assert_type(try_int(l_mixed, map=True), Iterator[int | str])
    assert_type(try_int(l_mixed, map=True, on_fail=RAISE), Iterator[int])
    assert_type(try_int(l_str, map=True, on_fail=RAISE, base=10), Iterator[int])


def test_try_forceint_types() -> None:
    # Integer inputs
    assert_type(try_forceint(1), int)
    assert_type(try_forceint(1, denoise=True), int)
    assert_type(try_forceint(1, on_fail={}, on_type_error=[]), int)
    assert_type(try_forceint(1, allow_underscores=False), int)

    # Float inputs
    assert_type(try_forceint(1.5), int)
    assert_type(try_forceint(1.5, denoise=True), int)
    assert_type(try_forceint(1.5, allow_underscores=True), int)
    assert_type(try_forceint(1.5, on_fail={}, on_type_error=[]), int)
    assert_type(try_forceint(1.5, allow_unicode_chars=False), int)

    # String inputs
    assert_type(try_forceint("42.9"), int | str)
    assert_type(try_forceint("bad"), int | str)
    assert_type(try_forceint("bad", on_fail=0), int)
    assert_type(try_forceint("bad", on_fail=1.5), Any)
    assert_type(try_forceint("bad", on_fail={}, on_type_error=[]), Any)
    assert_type(try_forceint("bad", allow_underscores=False), int | str)
    assert_type(try_forceint("bad", on_fail=RAISE), int)
    assert_type(try_forceint("bad", on_fail=passthrough), int | str)
    assert_type(try_forceint("42.9", allow_unicode_chars=True), int | str)

    # Any old inputs
    assert_type(try_forceint(object()), Any)
    assert_type(try_forceint(object(), on_fail=0, on_type_error=0), int)
    assert_type(try_forceint(object(), allow_underscores=False), Any)

    # Iterable inputs
    assert_type(try_forceint(l_int, map=list), list[int])
    assert_type(try_forceint(l_float, map=list), list[int])
    assert_type(try_forceint(l_obj, on_fail=RAISE, map=list), list[int])
    assert_type(try_forceint(l_str, map=list), list[int | str])
    assert_type(try_forceint(l_mixed, map=list), list[int | str])
    assert_type(try_forceint(l_mixed, map=list, on_fail=RAISE), list[int])
    assert_type(try_forceint(l_str, map=list, on_fail=RAISE), list[int])
    assert_type(try_forceint(l_int, map=True), Iterator[int])
    assert_type(try_forceint(l_float, map=True), Iterator[int])
    assert_type(try_forceint(l_obj, map=True), Iterator[Any])
    assert_type(try_forceint(l_obj, on_fail=RAISE, map=True), Iterator[int])
    assert_type(try_forceint(l_str, map=True), Iterator[int | str])
    assert_type(try_forceint(l_mixed, map=True), Iterator[int | str])
    assert_type(try_forceint(l_mixed, map=True, on_fail=RAISE), Iterator[int])
    assert_type(try_forceint(l_str, map=True, on_fail=RAISE), Iterator[int])


def test_try_array_types() -> None:
    assert_type(try_array([0, 1], dtype=np.int8), npt.NDArray[np.int8])
    assert_type(try_array([0, 1], dtype=np.int16), npt.NDArray[np.int16])
    assert_type(try_array([0, 1], dtype=np.int32), npt.NDArray[np.int32])
    assert_type(try_array([0, 1], dtype=np.int64), npt.NDArray[np.int64])
    assert_type(try_array([0, 1], dtype=np.uint8), npt.NDArray[np.uint8])
    assert_type(try_array([0, 1], dtype=np.uint16), npt.NDArray[np.uint16])
    assert_type(try_array([0, 1], dtype=np.uint32), npt.NDArray[np.uint32])
    assert_type(try_array([0, 1], dtype=np.uint64), npt.NDArray[np.uint64])
    assert_type(try_array([0, 1], dtype=np.float32), npt.NDArray[np.float32])
    assert_type(try_array([0, 1], dtype=np.float64), npt.NDArray[np.float64])

    assert_type(try_array([0, 1], dtype=np.dtype(np.int8)), npt.NDArray[np.int8])
    assert_type(try_array([0, 1], dtype=np.dtype(np.int16)), npt.NDArray[np.int16])
    assert_type(try_array([0, 1], dtype=np.dtype(np.int32)), npt.NDArray[np.int32])
    assert_type(try_array([0, 1], dtype=np.dtype(np.int64)), npt.NDArray[np.int64])
    assert_type(try_array([0, 1], dtype=np.dtype(np.uint8)), npt.NDArray[np.uint8])
    assert_type(try_array([0, 1], dtype=np.dtype(np.uint16)), npt.NDArray[np.uint16])
    assert_type(try_array([0, 1], dtype=np.dtype(np.uint32)), npt.NDArray[np.uint32])
    assert_type(try_array([0, 1], dtype=np.dtype(np.uint64)), npt.NDArray[np.uint64])
    assert_type(try_array([0, 1], dtype=np.dtype(np.float32)), npt.NDArray[np.float32])
    assert_type(try_array([0, 1], dtype=np.dtype(np.float64)), npt.NDArray[np.float64])

    int_np = np.empty(shape=(2,), dtype=np.int32)
    float_np = np.empty(shape=(2,), dtype=np.float64)
    int_array = array.array("l", [0, 0])
    float_array = array.array("d", [0.0, 0.0])
    assert_type(try_array([1, 2], int_np), None)
    assert_type(try_array([1, 2], float_np), None)
    assert_type(try_array([1, 2], int_array), None)
    assert_type(try_array([1, 2], float_array), None)


def test_fast_real_types() -> None:
    assert_type(fast_real(1), int)
    assert_type(fast_real(1.5), float | int)
    assert_type(fast_real("bad"), float | int | str)
    assert_type(fast_real("bad", default=0), float | int)
    assert_type(fast_real(1, on_fail=passthrough), int)
    assert_type(fast_real(1, key=passthrough), int)
    assert_type(fast_real(1.5, coerce=False, on_fail=str), float)
    assert_type(fast_real(1.5, coerce=False, key=str), float)
    assert_type(fast_real("1.5", 0, coerce=True), int | float)
    assert_type(fast_real("1.5", raise_on_invalid=True), int | float)
    assert_type(fast_real("bad", on_fail=an_int), float | int)
    assert_type(fast_real("bad", key=an_int), float | int)
    assert_type(fast_real("bad", on_fail=passthrough), float | int | str)
    assert_type(fast_real("bad", key=passthrough), float | int | str)
    assert_type(fast_real("bad", default=None), float | int | Any)
    assert_type(
        fast_real("bad", default=None, raise_on_invalid=False), float | int | Any
    )


def test_fast_float_types() -> None:
    assert_type(fast_float(1), float)
    assert_type(fast_float("bad"), float | str)
    assert_type(fast_float("bad", default=0.0), float)
    assert_type(fast_float(1, on_fail=passthrough), float)
    assert_type(fast_float(1, key=passthrough), float)
    assert_type(fast_float("1.5", 0.0), float)
    assert_type(fast_float("1.5", raise_on_invalid=True), float)
    assert_type(fast_float("bad", on_fail=a_float), float)
    assert_type(fast_float("bad", key=a_float), float)
    assert_type(fast_float("bad", on_fail=passthrough), float | str)
    assert_type(fast_float("bad", key=passthrough), float | str)
    assert_type(fast_float("bad", default=None), float | Any)
    assert_type(fast_float("bad", default=None, raise_on_invalid=False), float | Any)


def test_fast_int_types() -> None:
    assert_type(fast_int(1.5), int)
    assert_type(fast_int("bad"), int | str)
    assert_type(fast_int("bad", default=0), int)
    assert_type(fast_int(1.5, on_fail=passthrough), int)
    assert_type(fast_int(1.5, key=passthrough), int)
    assert_type(fast_int("42", 0, base=16), int)
    assert_type(fast_int("42", raise_on_invalid=True, base=16), int)
    assert_type(fast_int("bad", on_fail=an_int, base=10), int)
    assert_type(fast_int("bad", key=an_int, base=10), int)
    assert_type(fast_int("bad", on_fail=passthrough, base=10), int | str)
    assert_type(fast_int("bad", key=passthrough, base=10), int | str)
    assert_type(fast_int("bad", default=None, base=10), int | Any)
    assert_type(
        fast_int("bad", default=None, raise_on_invalid=False, base=10), int | Any
    )


def test_fast_forceint_types() -> None:
    assert_type(fast_forceint(1.5), int)
    assert_type(fast_forceint("bad", default=0), int)
    assert_type(fast_forceint(1.5, on_fail=passthrough), int)
    assert_type(fast_forceint(1.5, key=passthrough), int)
    assert_type(fast_forceint("42.9", 0), int)
    assert_type(fast_forceint("42.9", raise_on_invalid=True), int)
    assert_type(fast_forceint("bad", on_fail=an_int), int)
    assert_type(fast_forceint("bad", key=an_int), int)
    assert_type(fast_forceint("bad", on_fail=passthrough), int | str)
    assert_type(fast_forceint("bad", key=passthrough), int | str)
    assert_type(fast_forceint("bad", default=None), int | Any)
    assert_type(fast_forceint("bad", default=None, raise_on_invalid=False), int | Any)


def test_real_types() -> None:
    assert_type(real(1), int)
    assert_type(real(1, coerce=False), int)
    assert_type(real(1.5, coerce=False), float)
    assert_type(real(FloatationDevice()), int | float)


def test_float_types() -> None:
    assert_type(fn_float(1), float)


def test_int_types() -> None:
    assert_type(fn_int("42"), int)
    assert_type(fn_int("1f", 16), int)


def test_check_real_types() -> None:
    assert_type(check_real(1), bool)


def test_check_float_types() -> None:
    assert_type(check_float(FloatationDevice()), bool)


def test_check_int_types() -> None:
    assert_type(check_int(IndexCard()), bool)


def test_check_intlike_types() -> None:
    assert_type(check_intlike(PerformInt()), bool)


def test_isreal_types() -> None:
    assert_type(isreal(1), bool)


def test_isfloat_types() -> None:
    assert_type(isfloat(1.0), bool)


def test_isint_types() -> None:
    assert_type(isint(1), bool)


def test_isintlike_types() -> None:
    assert_type(isintlike("1"), bool)


def test_query_type_types() -> None:
    assert_type(query_type(1), type[int | float])
    assert_type(query_type(1.0), type[int | float])
    assert_type(query_type("1"), type[int | float | str])
    assert_type(query_type(["1"]), type[list[str]] | type[int | float])
    assert_type(
        query_type("1", allowed_types=(int, float)),
        type[str] | type[int] | type[float] | None,
    )
