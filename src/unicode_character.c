/*
 * Functions to convert or evaluate a (possibly signed) single
 * unicode character representing a non-decimal numerical value.
 *
 * Author: Seth M. Morton
 *
 * July 2018
 */

#include <Python.h>
#include "fastnumbers/unicode_character.h"
#include "fastnumbers/numbers.h"
#include "fastnumbers/options.h"

/* Obtain a PyLong or PyFloat from this unicode string assuming
 * it is a single unicode non-decimal numeric character.
 * Return NULL if not possible. Return Py_None not PyUnicode.
 */
PyObject *
convert_PyUnicode_to_PyNumber(PyObject *input)
{
    const int kind = PyUnicode_KIND(input);  /* Unicode storage format. */
    const void *data = PyUnicode_DATA(input);  /* Raw data */
    Py_ssize_t len = PyUnicode_GET_LENGTH(input);
    Py_ssize_t index = 0;
    bool negative = false;

    /* Ensure input is a valid unicode object. */
    if (!PyUnicode_Check(input)) {
        return Py_None;
    }
    if (PyUnicode_READY(input)) {  /* If true, then not OK for conversion. */
        return Py_None;
    }

    /* Strip whitespace from both ends of the data. */
    while (Py_UNICODE_ISSPACE(PyUnicode_READ(kind, data, index))) {
        index += 1;
        len -= 1;
    }
    while (Py_UNICODE_ISSPACE(PyUnicode_READ(kind, data, index + len - 1))) {
        len -= 1;
    }

    /* Remove the sign - remember if it is negative. */
    if (PyUnicode_READ(kind, data, index) == '-') {
        negative = true;
        index += 1;
        len -= 1;
    }
    else if (PyUnicode_READ(kind, data, index) == '+') {
        index += 1;
        len -= 1;
    }

    /* Anything longer than a length of 1 is not valid. */
    if (len == 1) {
        const Py_UCS4 u = (Py_UCS4) PyUnicode_READ(kind, data, index);
        const double number = Py_UNICODE_TONUMERIC(u);
        const long digit = Py_UNICODE_TODIGIT(u);
        if (number > -1.0) {
            if (digit > -1) {
                return PyLong_FromLong(negative ? -digit : digit);
            }
            else {
                return PyFloat_FromDouble(negative ? -number : number);
            }
        }
    }
    return NULL;
}

/* Return a number of the desired type from a (possibly signed)
 * single unicode character representing a non-decimal value.
 */
PyObject *
PyUnicodeCharacter_to_PyNumber(PyObject *obj, const PyNumberType type,
                               const Options *options)
{
    PyObject *number = convert_PyUnicode_to_PyNumber(obj);
    PyObject *result = NULL;

    if (number == NULL) {
        if (type == REAL || type == FLOAT) {
            SET_ERR_INVALID_FLOAT(options);
        }
        else {
            SET_ERR_INVALID_INT(options);
        }
        return NULL;
    }
    else if (number != Py_None) {
        switch (type) {
        case REAL:
            if (PyLong_Check(number)) {
                result = number;
                Py_INCREF(result);
                break;
            }
            else if (PyFloat_is_Intlike(number)) {
                result = PyNumber_Long(number);
                break;
            }
        /* Deliberate fall-through to FLOAT. */

        case FLOAT:
            result = PyNumber_Float(number);
            break;

        case INT:
            if (PyLong_Check(number)) {
                result = number;
                Py_INCREF(result);
            }
            else {
                SET_ERR_INVALID_INT(options);
            }
            break;

        case FORCEINT:
        case INTLIKE:
        default:
            if (PyLong_Check(number)) {
                result = number;
                Py_INCREF(result);
                break;
            }
            else {
                result = PyNumber_Long(number);
                if (result == NULL) {
                    PyErr_Clear();
                    SET_ERR_INVALID_INT(options);
                }
            }
        }

        Py_DECREF(number);
        return result;
    }
    else {
        return Py_None;  /* Indicates TypeError, not ValueError. */
    }
}

/* Evaluate if this Unicode object contains a (possibly signed)
 * single unicode character representing a non-decimal value.
 */
PyObject *
PyUnicodeCharacter_is_number(PyObject *obj, const PyNumberType type)
{
    PyObject *number = convert_PyUnicode_to_PyNumber(obj);
    bool isok = false;

    if (number == NULL) {
        Py_RETURN_FALSE;
    }
    else if (number != Py_None) {
        switch (type) {
        case REAL:
        case FLOAT:
            isok = PyLong_Check(number) || PyFloat_Check(number);
            break;

        case INT:
            isok = PyLong_Check(number);
            break;

        case FORCEINT:
        case INTLIKE:
        default:
            isok = PyLong_Check(number) || PyFloat_is_Intlike(number);
            break;
        }
        Py_DECREF(number);
        if (isok) {
            Py_RETURN_TRUE;
        }
        else {
            Py_RETURN_FALSE;
        }
    }
    else {
        return Py_None;  /* Not unicode. */
    }
}


PyObject *
PyUnicodeCharacter_contains_type(PyObject *obj)
{
    PyObject *number = convert_PyUnicode_to_PyNumber(obj);
    PyObject *retval = NULL;

    if (number == NULL) {
        return PyObject_Type(obj);
    }
    else if (number != Py_None) {
        retval = PyObject_Type(number);
        Py_DECREF(number);
        return retval;
    }
    else {
        return Py_None;  /* Not unicode. */
    }
}
