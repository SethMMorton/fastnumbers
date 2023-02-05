#pragma once

#include <Python.h>

#include "fastnumbers/payload.hpp"
#include "fastnumbers/selectors.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class Resolver
 * \brief Can resolve user input into the desired data type
 */
class Resolver {
public:
    /// Default construct
    Resolver(PyObject* input, const UserOptions& options)
        : m_input(input)
        , m_inf(Selectors::ALLOWED)
        , m_nan(Selectors::ALLOWED)
        , m_fail(Selectors::RAISE)
        , m_type_error(Selectors::RAISE)
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
        m_inf = inf_value == Selectors::INPUT ? m_input : inf_value;
    }

    /// Define how a value of NaN will be interpreted
    void set_nan_action(PyObject* nan_value)
    {
        m_nan = nan_value == Selectors::INPUT ? m_input : nan_value;
    }

    /// Define how a conversion failure will be interpreted
    void set_fail_action(PyObject* fail_value)
    {
        m_fail = fail_value == Selectors::INPUT ? m_input : fail_value;
    }

    /// Define how a type error will be interpreted
    void set_type_error_action(PyObject* type_error_value)
    {
        m_type_error = type_error_value == Selectors::INPUT ? m_input : type_error_value;
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
            return retval; // do not increment, already has a refcount
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

        // These actions are indicative of TypeErrors
        case ActionType::ERROR_BAD_TYPE_INT:
        case ActionType::ERROR_BAD_TYPE_FLOAT:
        case ActionType::ERROR_ILLEGAL_EXPLICIT_BASE:
            return type_error_action(atype);

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
        if (m_inf == Selectors::ALLOWED) {
            return increment_reference(
                negative ? Selectors::NEG_INFINITY : Selectors::POS_INFINITY
            );
        } else if (m_inf == Selectors::RAISE) {
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
        if (m_nan == Selectors::ALLOWED) {
            return increment_reference(
                negative ? Selectors::NEG_NAN : Selectors::POS_NAN
            );
        } else if (m_nan == Selectors::RAISE) {
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
        if (m_fail == Selectors::RAISE) {
            return raise_appropriate_exception(atype);
        }
        return fail_action_impl(m_fail);
    }

    /// Return the appropriate value if a conversion failure occured and
    /// an error has already been set
    PyObject* fail_action() const
    {
        if (m_fail == Selectors::RAISE) {
            return nullptr; // an error has already been set
        }
        return fail_action_impl(m_fail);
    }

    /// Return the appropriate value if a type error occured
    PyObject* type_error_action(ActionType atype) const
    {
        if (m_type_error == Selectors::RAISE) {
            return raise_appropriate_exception(atype);
        }
        return fail_action_impl(m_type_error);
    }

    /// Implementation for non-raising fail action
    PyObject* fail_action_impl(PyObject* actionable) const
    {
        PyErr_Clear();
        if (PyCallable_Check(actionable)) {
            return PyObject_CallFunctionObjArgs(actionable, m_input, nullptr);
        } else { // handles INPUT and a custom default value
            return increment_reference(actionable);
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