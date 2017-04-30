/*
 * Functions that will convert a Python object to a number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include <Python.h>
#include <limits.h>
#include "object_handling.h"
#include "number_handling.h"
#include "string_handling.h"
#include "unicode_handling.h"
#include "options.h"

#define RETURN_CORRECT_RESULT(ret, opt) \
    ((ret) ? (ret) : Options_Return_Correct_Result_On_Error(opt))

PyObject*
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
        } else {
            SET_ILLEGAL_BASE_ERROR(options);
            return RETURN_CORRECT_RESULT(NULL, options);
        }
    }

    /* Assume a string. */
    pyresult = PyString_to_PyNumber(obj, type, options);
    if (pyresult != Py_None)
        return RETURN_CORRECT_RESULT(pyresult, options);

    /* If the base was given explicitly, unicode should not be accepted. */
    if (!Options_Default_Base(options)) {
        SET_ERR_INVALID_INT(options);
        return RETURN_CORRECT_RESULT(NULL, options);
    }

    /* Assume unicode character. */
    if (PyUnicode_Check(obj)) {
        if (Options_Allow_UnicodeCharacter(options)) {
            pyresult = PyUnicodeCharacter_to_PyNumber(obj, type, options);
            if (pyresult != Py_None)
                return RETURN_CORRECT_RESULT(pyresult, options);
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
