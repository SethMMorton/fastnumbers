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
