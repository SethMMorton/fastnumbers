#pragma once

#include <Python.h>

#include "fastnumbers/user_options.hpp"

/// Possible actions that can be performed on input objects
enum class ActionType {
    PY_OBJECT, ///< Return a PyObject*
    NAN_ACTION, ///< Return NaN
    INF_ACTION, ///< Return infinity
    NEG_NAN_ACTION, ///< Return negative NaN
    NEG_INF_ACTION, ///< Return negative infinity
    ERROR_INVALID_INT, ///< Raise invalid int exception
    ERROR_INVALID_FLOAT, ///< Raise invalid float exception
    ERROR_INVALID_BASE, ///< Raise invalid base exception
    ERROR_INFINITY_TO_INT, ///< Raise oveflow for infintity to int
    ERROR_NAN_TO_INT, ///< Raise NaN to int exception
    ERROR_BAD_TYPE_INT, ///< Raise invalid type for int
    ERROR_BAD_TYPE_FLOAT, ///< Raise invalid type for float
    ERROR_ILLEGAL_EXPLICIT_BASE, ///< Raise illegal explict base exception
};

/**
 * \brief Transfer data intended to be converted to Python objects
 *
 * Various functions can return different values (or errors) depending
 * on user parameters. This class uniformly stores all types for smoothest
 * transfer into "Python-land".
 */
class Payload {
public:
    /// Default construct
    Payload()
        : m_actval(ActionType::PY_OBJECT)
        , m_pyval(nullptr)
    { }

    /// Construct the payload with an action.
    explicit Payload(const ActionType atype)
        : m_actval(atype)
        , m_pyval(nullptr)
    { }

    /// Construct the payload with a double.
    explicit Payload(const double val)
        : m_actval(ActionType::PY_OBJECT)
        , m_pyval(PyFloat_FromDouble(val))
    { }

    /// Construct the payload with a double that needs to be be converted to an int.
    explicit Payload(const double val, const bool needs_int_conversion)
        : m_actval(ActionType::PY_OBJECT)
        , m_pyval(
              needs_int_conversion ? PyLong_FromDouble(val) : PyFloat_FromDouble(val)
          )
    { }

    /// Construct the payload with a long.
    explicit Payload(const long val)
        : m_actval(ActionType::PY_OBJECT)
        , m_pyval(PyLong_FromLong(val))
    { }

    /// Construct the payload with a PyObject*.
    explicit Payload(PyObject* val)
        : m_actval(ActionType::PY_OBJECT)
        , m_pyval(val)
    { }

    // Copy, assignment, and destruct are defaults
    Payload(const Payload&) = default;
    Payload(Payload&&) = default;
    Payload& operator=(const Payload&) = default;
    ~Payload() = default;

    /// Return the Payload as an ActionType.
    ActionType get_action() const { return m_actval; }

    /// Return the Payload as a PyObject*.
    PyObject* to_pyobject() const { return m_pyval; }

private:
    /// Tracker of what action is being requested
    ActionType m_actval;

    /// The Payload as a PyObject*
    PyObject* m_pyval;
};

class Resolver {
public:
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

public:
    /// Default construct
    Resolver(PyObject* input, const UserOptions& options)
        : m_input(input)
        , m_inf(ALLOWED)
        , m_nan(ALLOWED)
        , m_fail(RAISE)
        , m_type_error(RAISE)
        , m_base(options.get_base())
    { }

    // Copy, assignment, and destruct are defaults
    Resolver(const Resolver&) = default;
    Resolver(Resolver&&) = default;
    Resolver& operator=(const Resolver&) = default;
    ~Resolver() = default;

    /// Define how a value of infinity will be interpreted
    void set_inf_action(PyObject* inf_value)
    {
        m_inf = inf_value == INPUT ? m_input : inf_value;
    }

    /// Define how a value of NaN will be interpreted
    void set_nan_action(PyObject* nan_value)
    {
        m_nan = nan_value == INPUT ? m_input : nan_value;
    }

    /// Define how a conversion failure will be interpreted
    void set_fail_action(PyObject* fail_value)
    {
        m_fail = fail_value == INPUT ? m_input : fail_value;
    }

    /// Define how a type error will be interpreted
    void set_type_error_action(PyObject* type_error_value)
    {
        m_type_error = type_error_value == INPUT ? m_input : type_error_value;
    }

    /// Resolve the paylowd into a Python object
    PyObject* resolve(const Payload& payload) const
    {
        const ActionType atype = payload.get_action();
        switch (atype) {
        // Return a PyObject*
        case ActionType::PY_OBJECT: {
            PyObject* retval = payload.to_pyobject();
            if (retval == nullptr) {
                return fail_action();
            }
            return increment_reference(retval);
        }

        // Return the appropriate value for when infinity is found
        case ActionType::INF_ACTION:
            return inf_action(false);

        // Return the appropriate value for when negative infinity is found
        case ActionType::NEG_INF_ACTION:
            return inf_action(true);

        // Return the appropriate value for when NaN is found
        case ActionType::NAN_ACTION:
            return nan_action(false);

        // Return the appropriate value for when negative NaN is found
        case ActionType::NEG_NAN_ACTION:
            return nan_action(true);

        // Raise an exception due passing an invalid type to convert to
        // an integer or float, or if using an explicit integer base
        // where it shouldn't be used
        case ActionType::ERROR_BAD_TYPE_INT:
        case ActionType::ERROR_BAD_TYPE_FLOAT:
        case ActionType::ERROR_ILLEGAL_EXPLICIT_BASE:
            return raise_appropriate_exception(atype);

        default:
            return fail_action(atype);
        }

        Py_UNREACHABLE();
    }

private:
    /// The orginal Python input
    PyObject* m_input;

    /// The desired return action for infinity
    PyObject* m_inf;

    /// The desired return action for NaN
    PyObject* m_nan;

    /// The desired return action for conversion failure
    PyObject* m_fail;

    /// The desired return action for invalid types
    PyObject* m_type_error;

    /// Desired integer base - used in error message generation
    int m_base;

private:
    /// Increment the refcount of a non-null object, then return the object
    static PyObject* increment_reference(PyObject* obj)
    {
        Py_IncRef(obj);
        return obj;
    }

    /// Return the appropriate value if infinity was detected
    PyObject* inf_action(bool negative) const
    {
        if (m_inf == ALLOWED) {
            return increment_reference(negative ? NEG_INFINITY : POS_INFINITY);
        } else if (m_inf == RAISE) {
            PyErr_SetString(PyExc_ValueError, "infinity is disallowed");
            return nullptr;
        } else if (PyCallable_Check(m_inf)) {
            return PyObject_CallFunctionObjArgs(m_inf, m_input, nullptr);
        } else { // handles INPUT and a custom default value
            return increment_reference(m_inf);
        }
    }

    /// Return the appropriate value if NaN was detected
    PyObject* nan_action(bool negative) const
    {
        if (m_nan == ALLOWED) {
            return increment_reference(negative ? NEG_NAN : POS_NAN);
        } else if (m_nan == RAISE) {
            PyErr_SetString(PyExc_ValueError, "NaN is disallowed");
            return nullptr;
        } else if (PyCallable_Check(m_nan)) {
            return PyObject_CallFunctionObjArgs(m_nan, m_input, nullptr);
        } else { // handles INPUT and a custom default value
            return increment_reference(m_nan);
        }
    }

    /// Return the appropriate value if a conversion failure occured
    PyObject* fail_action(ActionType atype) const
    {
        if (m_fail == RAISE) {
            return raise_appropriate_exception(atype);
        }
        return fail_action_impl();
    }

    /// Return the appropriate value if a conversion failure occured and
    /// an error has already been set
    PyObject* fail_action() const
    {
        if (m_fail == RAISE) {
            return nullptr; // an error has already been set
        }
        return fail_action_impl();
    }

    /// Return the appropriate value if a type error occured
    PyObject* type_error_action(ActionType atype) const
    {
        if (m_type_error == RAISE) {
            return raise_appropriate_exception(atype);
        }
        return fail_action_impl();
    }

    /// Implementation for non-raising fail action
    PyObject* fail_action_impl() const
    {
        PyErr_Clear();
        if (PyCallable_Check(m_fail)) {
            return PyObject_CallFunctionObjArgs(m_fail, m_input, nullptr);
        } else { // handles INPUT and a custom default value
            return increment_reference(m_fail);
        }
    }

    /// Prepare and raise the appropriate exception given an action type
    PyObject* raise_appropriate_exception(const ActionType atype) const
    {
        switch (atype) {
        case ActionType::ERROR_BAD_TYPE_INT:
            // Raise an exception due passing an invalid type to convert to an integer
            PyErr_Format(
                PyExc_TypeError,
                "int() argument must be a string, a bytes-like object or a number, not "
                "'%s'",
                Py_TYPE(m_input)->tp_name
            );
            break;

        case ActionType::ERROR_BAD_TYPE_FLOAT:
            // Raise an exception due passing an invalid type to convert to a float
            PyErr_Format(
                PyExc_TypeError,
                "float() argument must be a string or a number, not '%s'",
                Py_TYPE(m_input)->tp_name
            );
            break;

        case ActionType::ERROR_ILLEGAL_EXPLICIT_BASE: // TODO - duplciate
            // Raise an exception due to useing an explict integer base where it
            // shouldn't
            PyErr_SetString(
                PyExc_TypeError, "int() can't convert non-string with explicit base"
            );
            break;

        case ActionType::ERROR_INVALID_INT:
            // Raise an exception due to an invalid integer
            PyErr_Format(
                PyExc_ValueError,
                "invalid literal for int() with base %d: %.200R",
                m_base,
                m_input
            );
            break;

        case ActionType::ERROR_INVALID_FLOAT:
            // Raise an exception due to an invalid float
            PyErr_Format(
                PyExc_ValueError, "could not convert string to float: %.200R", m_input
            );
            break;

        case ActionType::ERROR_INVALID_BASE: // TODO - duplciate
            // Raise an exception due to an invalid base for integer conversion
            PyErr_SetString(
                PyExc_TypeError, "int() can't convert non-string with explicit base"
            );
            break;

        case ActionType::ERROR_INFINITY_TO_INT:
            // Raise an exception due to attempting to convert infininty to an integer
            PyErr_SetString(
                PyExc_OverflowError, "cannot convert float infinity to integer"
            );
            break;

        case ActionType::ERROR_NAN_TO_INT:
            // Raise an exception due to attempting to convert NaN to an integer
            PyErr_SetString(PyExc_ValueError, "cannot convert float NaN to integer");

        default:
            Py_UNREACHABLE();
        }

        return nullptr;
    }
};