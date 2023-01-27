#pragma once

#include <limits>

#include <Python.h>

#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \brief Implementation for converting floats
 *
 * This is the "full" implementation.
 *
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param inf The value indicating what action to take when an infinity is encountered
 * \param nan The value indicating what action to take when an NaN is encountered
 * \param ntype The desired type of the converted object
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param coerce For ntype REAL, return int-like floats as int
 *
 * \return Converted python object or nullptr with appropriate error set
 */
PyObject* float_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    PyObject* inf,
    PyObject* nan,
    const UserType ntype,
    const bool allow_underscores,
    const bool coerce = false
);

/**
 * \brief Implementation for converting floats
 *
 * This is the "fuller" implementation.
 *
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param on_type_error The value indicating what action to take on invalid type
 * \param inf The value indicating what action to take when an infinity is encountered
 * \param nan The value indicating what action to take when an NaN is encountered
 * \param ntype The desired type of the converted object
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param coerce For ntype REAL, return int-like floats as int
 *
 * \return Converted python object or nullptr with appropriate error set
 */
PyObject* float_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    PyObject* on_type_error,
    PyObject* inf,
    PyObject* nan,
    const UserType ntype,
    const bool allow_underscores,
    const bool coerce = false
);

/**
 * \brief Implementation for converting floats
 *
 * This is the "reduced" implementation, intended for the drop-in-replacement functions
 *
 * \param input The python object to consider for conversion
 * \param ntype The desired type of the converted object
 * \param coerce For ntype REAL, return int-like floats as int
 *
 * \return Converted python object or nullptr with appropriate error set
 */
PyObject*
float_conv_impl(PyObject* input, const UserType ntype, const bool coerce = false);

/**
 * \brief Implementation for converting integers
 *
 * This is the "full" implementation.
 *
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param ntype The desired type of the converted object
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param base The integer base use when parsing ints, use INT_MIN for default
 *
 * \return Converted python object or nullptr with appropriate error set
 */
PyObject* int_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    const UserType ntype,
    const bool allow_underscores,
    const int base = std::numeric_limits<int>::min()
);

/**
 * \brief Implementation for converting integers
 *
 * This is the "full" implementation.
 *
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param on_type_error The value indicating what action to take on invalid type
 * \param ntype The desired type of the converted object
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param base The integer base use when parsing ints, use INT_MIN for default
 *
 * \return Converted python object or nullptr with appropriate error set
 */
PyObject* int_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    PyObject* on_type_error,
    const UserType ntype,
    const bool allow_underscores,
    const int base = std::numeric_limits<int>::min()
);

/**
 * \brief Implementation for converting integers
 *
 * This is the "reduced" implementation, intended for the drop-in-replacement functions
 *
 * \param input The python object to consider for conversion
 * \param ntype The desired type of the converted object
 * \param base The integer base use when parsing ints, use INT_MIN for default
 *
 * \return Converted python object or nullptr with appropriate error set
 */
PyObject* int_conv_impl(
    PyObject* input,
    const UserType ntype,
    const int base = std::numeric_limits<int>::min()
);

/**
 * \brief Implementation for checking floats
 *
 * \param input The python object to consider
 * \param inf The value indicating how infinity will be considered
 * \param nan The value indicating how NaN will be considered
 * \param consider Indicate if str, num, or both will be checked
 * \param ntype The desired type to check for
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param strict For ntype FLOAT, whether or not to consider int strings as floats
 *
 * \return Python True or False
 */
PyObject* float_check_impl(
    PyObject* input,
    const PyObject* inf,
    const PyObject* nan,
    const PyObject* consider,
    const UserType ntype,
    const bool allow_underscores,
    const bool strict = false
);

/**
 * \brief Implementation for checking integers
 *
 * \param input The python object to consider
 * \param consider Indicate if str, num, or both will be checked
 * \param ntype The desired type to check for
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param base The integer base use when parsing ints, use INT_MIN for default
 *
 * \return Python True or False
 */
PyObject* int_check_impl(
    PyObject* input,
    const PyObject* consider,
    const UserType ntype,
    const bool allow_underscores,
    const int base = std::numeric_limits<int>::min()
);

/**
 * \brief Implementation for returning object type
 *
 * \param input The python object to query
 * \param allowed_types Non-empty sequence of allowed types, or nullptr
 * \param inf The value indicating how infinity will be considered
 * \param nan The value indicating how NaN will be considered
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param coerce Consider int-like floats as int
 *
 * \return Python type of input, or None if allowed_types was not empty and the type
 *         did not appear in the sequence
 */
PyObject* type_query_impl(
    PyObject* input,
    PyObject* allowed_types,
    const PyObject* inf,
    const PyObject* nan,
    const bool allow_underscores,
    const bool coerce
);