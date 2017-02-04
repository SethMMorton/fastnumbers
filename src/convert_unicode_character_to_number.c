/*
 * Functions that will convert if a unicode character to a number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "unicode_handling.h"
#include "number_handling.h"
#define result_OK(result) ((result) > -1)


static PyObject*
PyUnicode_to_PyInt_or_PyFloat(const Py_UCS4 uni)
{
    const double dresult = uni_tonumeric(uni);
    const long lresult = uni_todigit(uni);
    /* If invalid for double, outright fail.
     * Otherwise check if it can be made an int, then
     * fall back on double.
     */
    return result_OK(dresult)
         ? (result_OK(lresult) ? long_to_PyInt(lresult)
                               : PyFloat_FromDouble(dresult))
         : NULL;
}


static PyObject*
PyUnicode_to_PyFloat(const Py_UCS4 uni)
{
    const double dresult = uni_tonumeric(uni);
    return result_OK(dresult) ? PyFloat_FromDouble(dresult) : NULL;
}


static PyObject*
PyUnicode_to_PyInt(const Py_UCS4 uni)
{
    const long lresult = uni_todigit(uni);
    return result_OK(lresult) ? long_to_PyInt(lresult) : NULL;
}


static PyObject*
PyUnicode_to_PyInt_forced(const Py_UCS4 uni)
{
    PyObject *pyint = PyUnicode_to_PyInt(uni);
    /* If the int conversion is fine, use the int result */
    if (pyint != NULL)
        return pyint;
    /* If it was not fine, convert to double then to int if possible */
    else {
        PyObject *pyfloat = PyUnicode_to_PyFloat(uni);
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
PyUnicode_to_PyNumber(PyObject *obj, const PyNumberType type)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (unicode_conversion_success(uni)) {
        switch (type) {
        case REAL:
            return PyUnicode_to_PyInt_or_PyFloat(uni);
        case FLOAT:
            return PyUnicode_to_PyFloat(uni);
        case INT:
            return PyUnicode_to_PyInt(uni);
        case FORCEINT:
        case INTLIKE:
            return PyUnicode_to_PyInt_forced(uni);
        }
    }
    else
        return Py_None;  /* Indicates TypeError, not ValueError. */
}
