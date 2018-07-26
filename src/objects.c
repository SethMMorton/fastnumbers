/*
 * Functions that will convert/evaluate an arbitrary Python object.
 *
 * Author: Seth M. Morton
 *
 * July 2018
 */

#include <Python.h>
#include <limits.h>
#include "objects.h"
#include "numbers.h"
#include "strings.h"
#include "unicode_character.h"
#include "options.h"


#define RETURN_CORRECT_RESULT(ret, opt) \
    ((ret) ? (ret) : Options_Return_Correct_Result_On_Error(opt))


/* Attempt to convert an arbitrary PyObject to a PyNumber. */
PyObject *
PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const struct Options *options)
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
                   const struct Options *options)
{
    PyObject *pyresult = NULL;

    /* Already a number? Simple checks will work. */
    if (PyNumber_Check(obj))
        return PyBool_from_bool(!Options_String_Only(options) &&
                                PyNumber_is_type(obj, type));

    /* If we are requiring it to be a number then we must declare false now. */
    else if (Options_Number_Only(options)) {
        Py_RETURN_FALSE;
    }

    /* Assume a string. */
    pyresult = PyString_is_number(obj, type, options);
    if (pyresult != Py_None) {
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
