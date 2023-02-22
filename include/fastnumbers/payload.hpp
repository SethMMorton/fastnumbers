#pragma once

#include <variant>

#include <Python.h>

#include "fastnumbers/user_options.hpp"

/// Possible actions that can be performed on input objects
enum class ActionType {
    NAN_ACTION, ///< Return NaN
    INF_ACTION, ///< Return infinity
    NEG_NAN_ACTION, ///< Return negative NaN
    NEG_INF_ACTION, ///< Return negative infinity
    ERROR_INVALID_INT, ///< Raise invalid int exception
    ERROR_INVALID_FLOAT, ///< Raise invalid float exception
    ERROR_INVALID_BASE, ///< Raise invalid base exception
    ERROR_BAD_TYPE_INT, ///< Raise invalid type for int
    ERROR_BAD_TYPE_FLOAT, ///< Raise invalid type for float
    ERROR_ILLEGAL_EXPLICIT_BASE, ///< Raise illegal explict base exception
};

/// The types of errors this class can encounter
enum class ErrorType {
    BAD_VALUE, ///< Error because the given value was not valid
    OVERFLOW_, ///< Error because the given value was out-of-range
    TYPE_ERROR, ///< Error because the input was not of correct type
};

/**
 * \brief Transfer data intended to be converted to Python objects
 *
 * Various functions can return different values (or errors) depending
 * on user parameters. This class uniformly stores all types for smoothest
 * transfer into "Python-land".
 */
using Payload = std::variant<PyObject*, ActionType>;

template <typename T>
/**
 * \brief Transfer data intended to be kept as C-types
 *
 * Use of this class removes the need to keep track of error state
 * after returning a value.
 */
using RawPayload = std::variant<T, ErrorType>;
