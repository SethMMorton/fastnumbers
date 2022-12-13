#pragma once

#include <Python.h>

extern PyObject** FN_POS_INFINITY_PTR;
extern PyObject** FN_NEG_INFINITY_PTR;
extern PyObject** FN_POS_NAN_PTR;
extern PyObject** FN_NEG_NAN_PTR;

/// Possible actions that can be performed on input objects
enum class ActionType {
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

/// The types of data that Payload can store
enum class PayloadType {
    ACTION, ///< An ActionType enum value
    LONG, ///< A C-long
    DOUBLE, ///< A C-double
    DOUBLE_TO_LONG, ///< A C-double type that must be converted to long
    PYOBJECT, ///< A Python object pointer
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
    /// Default construct - needed for use with Cython.
    Payload()
        : m_type(PayloadType::PYOBJECT)
        , m_pyval(nullptr)
    { }

    /// Construct the payload with an action.
    explicit Payload(const ActionType atype)
        : m_type(PayloadType::ACTION)
        , m_actval(atype)
    { }

    /// Construct the payload with a double.
    explicit Payload(const double val)
        : m_type(PayloadType::DOUBLE)
        , m_dval(val)
    { }

    /// Construct the payload with a double that needs to be be converted to an int.
    explicit Payload(const double val, const bool needs_int_conversion)
        : m_type(
            needs_int_conversion ? PayloadType::DOUBLE_TO_LONG : PayloadType::DOUBLE
        )
        , m_dval(val)
    { }

    /// Construct the payload with a long.
    explicit Payload(const long val)
        : m_type(PayloadType::LONG)
        , m_ival(val)
    { }

    /// Construct the payload with a PyObject*.
    explicit Payload(PyObject* val)
        : m_type(PayloadType::PYOBJECT)
        , m_pyval(val)
    { }

    // Copy, assignment, and destruct are defaults
    Payload(const Payload&) = default;
    Payload(Payload&&) = default;
    Payload& operator=(const Payload&) = default;
    ~Payload() = default;

    /// What type of payload is being carried?
    PayloadType payload_type() const { return m_type; }

    /// Return the Payload as an ActionType.
    ActionType get_action() const { return m_actval; }

    /// Return the Payload as a double.
    double to_double() const { return m_dval; }

    /// Return the Payload as a long.
    long to_long() const { return m_ival; }

    /// Return the Payload as a PyObject*.
    PyObject* to_pyobject() const { return m_pyval; }

    /// Resolve the paylowd into a Python object
    PyObject* resolve(
        PyObject* input,
        const UserOptions& options,
        PyObject* infinity,
        PyObject* nan,
        PyObject* default_,
        PyObject* on_fail,
        bool raise_on_invalid
    ) const
    {
        PyObject* return_object
            = failure_return_value(input, raise_on_invalid, default_);

        // Level 1: If the payload contains an actual number,
        //          convert to PyObject directly
        switch (payload_type()) {
        case PayloadType::LONG:
            return PyLong_FromLong(to_long());

        case PayloadType::DOUBLE:
            return PyFloat_FromDouble(to_double());

        case PayloadType::DOUBLE_TO_LONG:
            return PyLong_FromDouble(to_double());

        case PayloadType::PYOBJECT: {
            PyObject* retval = to_pyobject();
            if (retval == nullptr) {
                if (return_object == nullptr) {
                    return nullptr;
                }

                PyErr_Clear();

                if (on_fail != nullptr) {
                    return PyObject_CallFunctionObjArgs(on_fail, return_object, nullptr);
                } else {
                    return increment_reference(return_object);
                }
            }
            return increment_reference(retval);
        }

        // Level 2: We need to instruct Cython as to what action to take
        case PayloadType::ACTION: {
            const ActionType atype = get_action();

            switch (atype) {
            // Return the appropriate value for when infinity is found
            case ActionType::INF_ACTION:
                return increment_reference(
                    infinity == nullptr ? *FN_POS_INFINITY_PTR : infinity
                );

            // Return the appropriate value for when negative infinity is found
            case ActionType::NEG_INF_ACTION:
                return increment_reference(
                    infinity == nullptr ? *FN_NEG_INFINITY_PTR : infinity
                );

            // Return the appropriate value for when NaN is found
            case ActionType::NAN_ACTION:
                return increment_reference(nan == nullptr ? *FN_POS_NAN_PTR : nan);

            // Return the appropriate value for when negative NaN is found
            case ActionType::NEG_NAN_ACTION:
                return increment_reference(nan == nullptr ? *FN_NEG_NAN_PTR : nan);

            // Raise an exception due passing an invalid type to convert to
            // an integer or float, or if using an explicit integer base
            // where it shouldn't be used
            case ActionType::ERROR_BAD_TYPE_INT:
            case ActionType::ERROR_BAD_TYPE_FLOAT:
            case ActionType::ERROR_ILLEGAL_EXPLICIT_BASE:
                return raise_appropriate_exception(input, atype, options);

            default:
                // Raise an exception if that is what the user has asked for, otherwise
                // transform the input via a function, otherwise return the input as-is
                if (return_object == nullptr) {
                    return raise_appropriate_exception(input, atype, options);
                }

                PyErr_Clear();

                if (on_fail != nullptr) {
                    return PyObject_CallFunctionObjArgs(on_fail, return_object, nullptr);
                } else {
                    return increment_reference(return_object);
                }
            }
        }
        }

        Py_UNREACHABLE();
    }

private:
    /// Tracker of what type is being stored
    PayloadType m_type;

    /**
     * \brief All possible Payload types, occupying the same memory
     *
     * To minimize copy overhead, all possible Payload types are
     * placed in a union so that there is no wasted space. Only one
     * of these can be valid at a time.
     */
    union {
        /// The Payload as a long
        long m_ival;
        /// The Payload as a double
        double m_dval;
        /// The Payload as a PyObject*
        PyObject* m_pyval;
        /// The Payload as an ActionType
        ActionType m_actval;
    };

private:
    /// Increment the refcount of a non-null object, then return the object
    static PyObject* increment_reference(PyObject* obj)
    {
        Py_IncRef(obj);
        return obj;
    }

    /// Return the best return value for when an error occurs
    static PyObject*
    failure_return_value(PyObject* obj, bool raise_on_invalid, PyObject* default_)
    {
        if (raise_on_invalid) {
            return nullptr;
        } else if (default_ != nullptr) {
            return default_;
        } else {
            return obj;
        }
    }

    /// Prepare and raise the appropriate exception given an action type
    PyObject* raise_appropriate_exception(
        PyObject* input, const ActionType atype, const UserOptions& options
    ) const
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
                options.get_base(),
                input
            );
            break;

        case ActionType::ERROR_INVALID_FLOAT:
            // Raise an exception due to an invalid float
            PyErr_Format(
                PyExc_ValueError, "could not convert string to float: %.200R", input
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
