/*
 * Functions that will convert if a unicode character to a number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "unicode_handling.h"
#include "number_handling.h"
#include "options.h"
#define result_OK(result) ((result) > -1)


static PyObject*
PyUnicodeCharacter_to_PyInt_or_PyFloat(const Py_UCS4 uni, const struct Options *options)
{
    const double dresult = uni_tonumeric(uni);
    const long lresult = uni_todigit(uni);
    /* If invalid for double, outright fail.
     * Otherwise check if it can be made an int, then
     * fall back on double.
     */
    if (result_OK(dresult)) {
        return result_OK(lresult) ? long_to_PyInt(lresult)
                                  : PyFloat_FromDouble(dresult);
    }
    else {
        SET_ERR_INVALID_FLOAT(options);
        return NULL;
    }
}


static PyObject*
PyUnicodeCharacter_to_PyFloat(const Py_UCS4 uni, const struct Options *options)
{
    const double dresult = uni_tonumeric(uni);
    if (result_OK(dresult))
        return PyFloat_FromDouble(dresult);
    else {
        SET_ERR_INVALID_FLOAT(options);
        return NULL;
    }
}


static PyObject*
PyUnicodeCharacter_to_PyInt(const Py_UCS4 uni, const struct Options *options)
{
    const long lresult = uni_todigit(uni);
    if (result_OK(lresult))
        return long_to_PyInt(lresult);
    else {
        SET_ERR_INVALID_INT(options);
        return NULL;
    }
}


static PyObject*
PyUnicodeCharacter_to_PyInt_forced(const Py_UCS4 uni, const struct Options *options)
{
    PyObject *pyint = PyUnicodeCharacter_to_PyInt(uni, options);
    /* If the int conversion is fine, use the int result */
    if (pyint != NULL)
        return pyint;
    /* If it was not fine, convert to double then to int if possible */
    else {
        PyObject *pyfloat = PyUnicodeCharacter_to_PyFloat(uni, options);
        if (pyfloat == NULL)
            return NULL;
        else {
            pyint = PyNumber_ToInt(pyfloat);
            Py_DECREF(pyfloat);
            return pyint;
        }
    }
}


PyObject*
PyUnicodeCharacter_to_PyNumber(PyObject *obj, const PyNumberType type, const struct Options *options)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (unicode_conversion_success(uni)) {
        switch (type) {
        case REAL:
            return PyUnicodeCharacter_to_PyInt_or_PyFloat(uni, options);
        case FLOAT:
            return PyUnicodeCharacter_to_PyFloat(uni, options);
        case INT:
            return PyUnicodeCharacter_to_PyInt(uni, options);
        case FORCEINT:
        case INTLIKE:
        default:
            return PyUnicodeCharacter_to_PyInt_forced(uni, options);
        }
    }
    else
        return Py_None;  /* Indicates TypeError, not ValueError. */
}
