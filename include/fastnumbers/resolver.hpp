#pragma once

#include <utility>
#include <variant>

#include <Python.h>

#include "fastnumbers/helpers.hpp"
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
    Resolver()
        : Resolver(10)
    { }

    /// Construct with a base
    explicit Resolver(const int base) noexcept
        : m_inf(Selectors::ALLOWED)
        , m_nan(Selectors::ALLOWED)
        , m_fail(Selectors::RAISE)
        , m_type_error(Selectors::RAISE)
        , m_base(base)
    { }

    /// Copy constructor makes sure to increment references
    Resolver(const Resolver& rhs) noexcept
        : m_inf(Selectors::incref(rhs.m_inf))
        , m_nan(Selectors::incref(rhs.m_nan))
        , m_fail(Selectors::incref(rhs.m_fail))
        , m_type_error(Selectors::incref(rhs.m_type_error))
        , m_base(rhs.m_base)
    { }

    /// Move constructor steals object, no need to re-increment
    Resolver(Resolver&& rhs) noexcept
        : m_inf(std::exchange(rhs.m_inf, nullptr))
        , m_nan(std::exchange(rhs.m_nan, nullptr))
        , m_fail(std::exchange(rhs.m_fail, nullptr))
        , m_type_error(std::exchange(rhs.m_type_error, nullptr))
        , m_base(std::exchange(rhs.m_base, 0))
    { }

    // Assignment not allowed
    Resolver& operator=(const Resolver&) = delete;

    /// Destruct
    ~Resolver() noexcept
    {
        Selectors::decref(m_inf);
        Selectors::decref(m_nan);
        Selectors::decref(m_fail);
        Selectors::decref(m_type_error);
    };

    /// Define how a value of infinity will be interpreted
    void set_inf_action(PyObject* inf_value) noexcept
    {
        m_inf = Selectors::incref(inf_value);
    }

    /// Define how a value of NaN will be interpreted
    void set_nan_action(PyObject* nan_value) noexcept
    {
        m_nan = Selectors::incref(nan_value);
    }

    /// Define how a conversion failure will be interpreted
    void set_fail_action(PyObject* fail_value) noexcept
    {
        m_fail = Selectors::incref(fail_value);
    }

    /// Define how a type error will be interpreted
    void set_type_error_action(PyObject* type_error_value) noexcept
    {
        m_type_error = Selectors::incref(type_error_value);
    }

    /// Resolve the payload into a Python object
    PyObject* resolve(PyObject* input, const Payload& payload) const noexcept
    {
        // std::visit will call the appropriate logic depending on what value
        // is currently stored in the Payload object.
        return std::visit(
            overloaded {

                // If the payload contains a Python object, just return directly
                [this, input](PyObject* retval) -> PyObject* {
                    if (retval == nullptr) {
                        return fail_action(input);
                    }
                    return retval; // do not increment, already has a refcount
                },

                // If the payload contains an action type, act on it
                [this, input](const ActionType atype) -> PyObject* {
                    switch (atype) {
                    // Return the appropriate value for when infinity is found
                    case ActionType::INF_ACTION:
                        return inf_action(input, false);

                    // Return the appropriate value for when negative infinity is found
                    case ActionType::NEG_INF_ACTION:
                        return inf_action(input, true);

                    // Return the appropriate value for when NaN is found
                    case ActionType::NAN_ACTION:
                        return nan_action(input, false);

                    // Return the appropriate value for when negative NaN is found
                    case ActionType::NEG_NAN_ACTION:
                        return nan_action(input, true);

                    // These actions are indicative of TypeErrors
                    case ActionType::ERROR_BAD_TYPE_INT:
                    case ActionType::ERROR_BAD_TYPE_FLOAT:
                    case ActionType::ERROR_ILLEGAL_EXPLICIT_BASE:
                        return type_error_action(input, atype);

                    default:
                        return fail_action(input, atype);
                    }
                },
            },
            payload
        );
    }

private:
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
    static PyObject* increment_reference(PyObject* obj) noexcept
    {
        Py_IncRef(obj);
        return obj;
    }

    /// Return the appropriate action for a infinity
    PyObject* inf_obj(PyObject* input) const noexcept
    {
        return m_inf == Selectors::INPUT ? input : m_inf;
    }

    /// Return the appropriate action for a NaN
    PyObject* nan_obj(PyObject* input) const noexcept
    {
        return m_nan == Selectors::INPUT ? input : m_nan;
    }

    /// Return the appropriate action for a conversion failure
    PyObject* fail_obj(PyObject* input) const noexcept
    {
        return m_fail == Selectors::INPUT ? input : m_fail;
    }

    /// Return the appropriate action for a type error
    PyObject* type_error_obj(PyObject* input) const noexcept
    {
        return m_type_error == Selectors::INPUT ? input : m_type_error;
    }

    /// Return the appropriate value if infinity was detected
    PyObject* inf_action(PyObject* input, bool negative) const noexcept
    {
        PyObject* my_inf = inf_obj(input);
        if (my_inf == Selectors::ALLOWED) {
            return increment_reference(
                negative ? Selectors::NEG_INFINITY : Selectors::POS_INFINITY
            );
        } else if (my_inf == Selectors::RAISE) {
            PyErr_SetString(PyExc_ValueError, "infinity is disallowed");
            return nullptr;
        } else if (PyCallable_Check(my_inf)) {
            return PyObject_CallFunctionObjArgs(my_inf, input, nullptr);
        } else { // handles INPUT and a custom default value
            return increment_reference(my_inf);
        }
    }

    /// Return the appropriate value if NaN was detected
    PyObject* nan_action(PyObject* input, bool negative) const noexcept
    {
        PyObject* my_nan = nan_obj(input);
        if (my_nan == Selectors::ALLOWED) {
            return increment_reference(
                negative ? Selectors::NEG_NAN : Selectors::POS_NAN
            );
        } else if (my_nan == Selectors::RAISE) {
            PyErr_SetString(PyExc_ValueError, "NaN is disallowed");
            return nullptr;
        } else if (PyCallable_Check(my_nan)) {
            return PyObject_CallFunctionObjArgs(my_nan, input, nullptr);
        } else { // handles INPUT and a custom default value
            return increment_reference(my_nan);
        }
    }

    /// Return the appropriate value if a conversion failure occured
    PyObject* fail_action(PyObject* input, ActionType atype) const noexcept
    {
        PyObject* my_fail = fail_obj(input);
        if (my_fail == Selectors::RAISE) {
            return raise_appropriate_exception(input, atype);
        }
        return fail_action_impl(input, my_fail);
    }

    /// Return the appropriate value if a conversion failure occured and
    /// an error has already been set
    PyObject* fail_action(PyObject* input) const noexcept
    {
        PyObject* my_fail = fail_obj(input);
        if (my_fail == Selectors::RAISE) {
            return nullptr; // an error has already been set
        }
        return fail_action_impl(input, my_fail);
    }

    /// Return the appropriate value if a type error occured
    PyObject* type_error_action(PyObject* input, ActionType atype) const noexcept
    {
        PyObject* my_type_error = type_error_obj(input);
        if (my_type_error == Selectors::RAISE) {
            return raise_appropriate_exception(input, atype);
        }
        return fail_action_impl(input, my_type_error);
    }

    /// Implementation for non-raising fail action
    PyObject* fail_action_impl(PyObject* input, PyObject* actionable) const noexcept
    {
        PyErr_Clear();
        if (PyCallable_Check(actionable)) {
            return PyObject_CallFunctionObjArgs(actionable, input, nullptr);
        } else { // handles INPUT and a custom default value
            return increment_reference(actionable);
        }
    }

    /// Prepare and raise the appropriate exception given an action type
    PyObject*
    raise_appropriate_exception(PyObject* input, const ActionType atype) const noexcept
    {
        switch (atype) {
        case ActionType::ERROR_BAD_TYPE_INT:
            // Raise an exception due passing an invalid type to convert to an integer
            PyErr_Format(
                PyExc_TypeError,
                "int() argument must be a string, a bytes-like object or a number, not "
                "'%s'",
                Py_TYPE(input)->tp_name
            );
            break;

        case ActionType::ERROR_BAD_TYPE_FLOAT:
            // Raise an exception due passing an invalid type to convert to a float
            PyErr_Format(
                PyExc_TypeError,
                "float() argument must be a string or a number, not '%s'",
                Py_TYPE(input)->tp_name
            );
            break;

        case ActionType::ERROR_INVALID_INT:
            // Raise an exception due to an invalid integer
            PyErr_Format(
                PyExc_ValueError,
                "invalid literal for int() with base %d: %.200R",
                m_base,
                input
            );
            break;

        case ActionType::ERROR_INVALID_FLOAT:
            // Raise an exception due to an invalid float
            PyErr_Format(
                PyExc_ValueError, "could not convert string to float: %.200R", input
            );
            break;

        default:
            // ERROR_ILLEGAL_EXPLICIT_BASE
            // ERROR_INVALID_BASE
            // Raise an exception due to an invalid base for integer conversion
            PyErr_SetString(
                PyExc_TypeError, "int() can't convert non-string with explicit base"
            );
            break;
        }

        return nullptr;
    }
};