#pragma once

/// Always evaluates to false - helps with static_assert messages
template <class>
inline constexpr bool always_false_v = false;

/// Aid in constructing multiple overloads from lambdas for std::visit
/// See https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/// In-place addition of a number, handling reference counting.
inline void in_place_add(PyObject*& self, PyObject* x)
{
    // The in-place method *might* return a new object. As a result
    // we should be good citizens and decref the after the operation.
    PyObject* temp = self;
    self = PyNumber_InPlaceAdd(temp, x);
    Py_DECREF(temp);
}

/// In-place multiplication of a number, handling reference counting.
inline void in_place_multiply(PyObject*& self, PyObject* x)
{
    // The in-place method *might* return a new object. As a result
    // we should be good citizens and decref the after the operation.
    PyObject* temp = self;
    self = PyNumber_InPlaceMultiply(temp, x);
    Py_DECREF(temp);
}

/// In-place division of a number, handling reference counting.
inline void in_place_divide(PyObject*& self, PyObject* x)
{
    // The in-place method *might* return a new object. As a result
    // we should be good citizens and decref the after the operation.
    PyObject* temp = self;
    self = PyNumber_InPlaceFloorDivide(temp, x);
    Py_DECREF(temp);
}

/// In-place power of a number, handling reference counting.
inline void in_place_pow(PyObject*& self, PyObject* x)
{
    // The in-place method *might* return a new object. As a result
    // we should be good citizens and decref the after the operation.
    PyObject* temp = self;
    self = PyNumber_InPlacePower(temp, x, Py_None);
    Py_DECREF(temp);
}

/// Convert an integer to a Python long
template <typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline PyObject* pyobject_from_int(const T value) noexcept
{
    if constexpr (std::is_same_v<T, long>) {
        return PyLong_FromLong(value);
    } else if constexpr (std::is_same_v<T, unsigned long>) {
        return PyLong_FromUnsignedLong(value);
    } else if constexpr (std::is_same_v<T, long long>) {
        return PyLong_FromLongLong(value);
    } else if constexpr (std::is_same_v<T, unsigned long long>) {
        return PyLong_FromUnsignedLongLong(value);
    } else if constexpr (std::is_signed_v<T>) {
        return PyLong_FromLong(static_cast<long>(value));
    } else {
        return PyLong_FromUnsignedLong(static_cast<unsigned long>(value));
    };
}
