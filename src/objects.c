/*
 * Functions that will convert/evaluate an arbitrary Python object.
 *
 * Author: Seth M. Morton
 *
 * July 2018
 */

#include <Python.h>
#include <limits.h>
#include "fastnumbers/objects.h"
#include "fastnumbers/numbers.h"
#include "fastnumbers/strings.h"
#include "fastnumbers/unicode_character.h"
#include "fastnumbers/options.h"


#define RETURN_CORRECT_RESULT(ret, opt) \
    ((ret) ? (ret) : Options_Return_Correct_Result_On_Error(opt))


/* Attempt to convert an arbitrary PyObject to a PyNumber. */
PyObject *
PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const Options *options)
{
    PyObject *pyresult = NULL;

    /* Already a number? Simple conversion will work.
     * Do not accept numbers if base was explicitly given.
     */
    if (PyNumber_Check(obj)) {
        if (Options_Default_Base(options)) {
            pyresult = PyNumber_to_PyNumber(obj, type, options);
            return RETURN_CORRECT_RESULT(pyresult, options);
        }
        else {
            SET_ILLEGAL_BASE_ERROR(options);
            return RETURN_CORRECT_RESULT(NULL, options);
        }
    }

    /* Assume a string. */
    pyresult = PyString_to_PyNumber(obj, type, options);
    if (errno == ENOMEM) {
        return NULL;  /* ALWAYS raise on out-of-memory errors. */
    }
    if (pyresult != Py_None) {
        return RETURN_CORRECT_RESULT(pyresult, options);
    }

    /* If the base was given explicitly, unicode should not be accepted. */
    if (!Options_Default_Base(options)) {
        SET_ERR_INVALID_INT(options);
        return RETURN_CORRECT_RESULT(NULL, options);
    }

    /* Assume unicode character. */
    if (PyUnicode_Check(obj)) {
        if (Options_Allow_UnicodeCharacter(options)) {
            pyresult = PyUnicodeCharacter_to_PyNumber(obj, type, options);
            if (pyresult != Py_None) {
                return RETURN_CORRECT_RESULT(pyresult, options);
            }
        }
        else {
            /* If unicode characters are not allowed, return an error. */
            if (type == REAL || type == FLOAT) {
                SET_ERR_INVALID_FLOAT(options);
            }
            else {
                SET_ERR_INVALID_INT(options);
            }
            return NULL;
        }
    }

    /* Nothing worked - must be a TypeError */
    PyErr_Format(PyExc_TypeError,
                 (type == REAL || type == FLOAT) ?
                 "float() argument must be a string or a number, not '%s'" :
                 "int() argument must be a string, a bytes-like object "
                 "or a number, not '%s'",
                 Py_TYPE(options->input)->tp_name);
    return NULL;
}


/* Check if an arbitrary PyObject is a PyNumber. */
PyObject *
PyObject_is_number(PyObject *obj, const PyNumberType type,
                   const Options *options)
{
    PyObject *pyresult = NULL;

    /* Already a number? Simple checks will work. */
    if (PyNumber_Check(obj)) {
        if (!Options_String_Only(options) && PyNumber_is_type(obj, type)) {
            Py_RETURN_TRUE;
        }
        else {
            Py_RETURN_FALSE;
        }
    }

    /* If we are requiring it to be a number then we must declare false now. */
    else if (Options_Number_Only(options)) {
        Py_RETURN_FALSE;
    }

    /* Assume a string. */
    pyresult = PyString_is_number(obj, type, options);
    if (pyresult == NULL || errno == ENOMEM) {
        return NULL; /* ALWAYS raise on out-of-memory errors. */
    }
    else if (pyresult != Py_None) {
        return pyresult;
    }

    /* Assume unicode. */
    pyresult = PyUnicodeCharacter_is_number(obj, type);
    if (pyresult != Py_None) {
        return pyresult;
    }

    /* If we got here, the type was invalid so return False. */
    Py_RETURN_FALSE;
}


/* Return the type contained the object. */
PyObject *
PyObject_contains_type(PyObject *obj, const Options *options)
{
    PyObject *pyresult = NULL;

    /* Already a number? Just return the type directly. */
    if (PyLong_Check(obj) || (Options_Coerce_True(options) &&
                              PyFloat_is_Intlike(obj))) {
        pyresult = (PyObject *) &PyLong_Type;
        Py_INCREF(pyresult);
        return pyresult;
    }
    else if (PyFloat_Check(obj)) {
        pyresult = (PyObject *) &PyFloat_Type;
        Py_INCREF(pyresult);
        return pyresult;
    }

    /* Assume a string. */
    pyresult = PyString_contains_type(obj, options);
    if (pyresult == NULL || errno == ENOMEM) {
        return NULL; /* ALWAYS raise on out-of-memory errors. */
    }
    else if (pyresult != Py_None) {
        return pyresult;
    }

    /* Assume unicode. */
    pyresult = PyUnicodeCharacter_contains_type(obj);
    if (pyresult != Py_None) {
        return pyresult;
    }

    /* If we got here, the type was invalid. */
    return PyObject_Type(obj);
}
