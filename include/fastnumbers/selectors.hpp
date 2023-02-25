#pragma once

#include <Python.h>

/// Namespace for options selectors
struct Selectors {
    /// Python represenation of positive infinity
    static PyObject* POS_INFINITY;

    /// Python represenation of negative infinity
    static PyObject* NEG_INFINITY;

    /// Python represenation of positive NaN
    static PyObject* POS_NAN;

    /// Python represenation of negative NaN
    static PyObject* NEG_NAN;

    /// Selector for allowed things
    static PyObject* ALLOWED;

    /// Selector for disallowed things
    static PyObject* DISALLOWED;

    /// Selector to use original input
    static PyObject* INPUT;

    /// Selector to raise exceptions
    static PyObject* RAISE;

    /// Selector to only allow strings
    static PyObject* STRING_ONLY;

    /// Selector to only allow numbers
    static PyObject* NUMBER_ONLY;

    static bool is_selector(PyObject* obj) noexcept
    {
        return obj == Selectors::POS_INFINITY || obj == Selectors::NEG_INFINITY
            || obj == Selectors::POS_NAN || obj == Selectors::NEG_NAN
            || obj == Selectors::ALLOWED || obj == Selectors::DISALLOWED
            || obj == Selectors::INPUT || obj == Selectors::RAISE
            || obj == Selectors::STRING_ONLY || obj == Selectors::NUMBER_ONLY;
    }

    /// Increment a Python object's reference count if the object is not a selector
    static PyObject* incref(PyObject* obj) noexcept
    {
        if (!Selectors::is_selector(obj)) {
            Py_XINCREF(obj);
        }
        return obj;
    }

    /// Decrement a Python object's reference count if the object is not a selector
    static PyObject* decref(PyObject* obj) noexcept
    {
        if (!Selectors::is_selector(obj)) {
            Py_XDECREF(obj);
        }
        return obj;
    }
};
