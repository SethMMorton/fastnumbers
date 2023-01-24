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
};
