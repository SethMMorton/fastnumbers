/*
 * Functions that will convert a Python number to another number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "number_handling.h"
#include "options.h"
#include "pstdint.h"

/* Ensure 64 bits are handled. */
#ifndef INT64_MAX
#error "fastnumbers requires that your compiler support 64 bit integers, but it appears that this compiler does not"
#endif

static bool
_PyFloat_is_Intlike(PyObject *obj) {
    PyObject *py_is_intlike = PyObject_CallMethod(obj, "is_integer", NULL);
    if (py_is_intlike == NULL)  /* Unlikely. */
        return PyErr_Clear(), false;
    else {
        const bool is_intlike = PyObject_IsTrue(py_is_intlike);
        Py_DECREF(py_is_intlike);
        return is_intlike;
    }
}


static PyObject*
PyNumber_to_PyInt_or_PyFloat(PyObject *pynum, const struct Options *options)
{
    if (Options_Has_NaN_Sub(options) && PyNumber_IsNAN(pynum))
        return Options_Return_NaN_Sub(options);
    else if (Options_Has_INF_Sub(options) && PyNumber_IsINF(pynum))
        return Options_Return_INF_Sub(options);
    else if (Options_Coerce_True(options)) {
        if (PyNumber_IsInt(pynum) || PyFloat_is_Intlike(pynum)) {
            return PyNumber_ToInt(pynum);
        }
        else
            return PyNumber_Float(pynum);
    }
    else
        return Py_INCREF(pynum), pynum;
}


static PyObject*
PyNumber_to_PyFloat(PyObject *pynum, const struct Options *options)
{
    if (Options_Has_NaN_Sub(options) && PyNumber_IsNAN(pynum))
        return Options_Return_NaN_Sub(options);
    else if (Options_Has_INF_Sub(options) && PyNumber_IsINF(pynum))
        return Options_Return_INF_Sub(options);
    else
        return PyNumber_Float(pynum);
}


static PyObject*
PyNumber_to_PyInt(PyObject *pynum, const struct Options *options)
{
    if (PyFloat_Check(pynum)) { /* Watch out for un-intable numbers. */
        const double d = PyFloat_AS_DOUBLE(pynum);
        if (Py_IS_INFINITY(d)) {
            if (Options_Should_Raise(options))
                PyErr_SetString(PyExc_OverflowError,
                    "cannot convert float infinity to integer");
            return NULL;
        }
        if (Py_IS_NAN(d)) {
            if (Options_Should_Raise(options))
                PyErr_SetString(PyExc_ValueError,
                    "cannot convert float NaN to integer");
            return NULL;
        }
    }
    return PyNumber_ToInt(pynum);
}


PyObject*
PyFloat_to_PyInt(PyObject * fobj, const struct Options *options)
{
    PyObject *tmp = PyNumber_to_PyInt(fobj, options);
    Py_DECREF(fobj);
    return tmp;
}


bool
PyFloat_is_Intlike(PyObject *obj)
{
    const double dval = PyFloat_AS_DOUBLE(obj);
    if (!PyFloat_Check(obj))
        return false;
    if (dval < INT64_MAX && dval > INT64_MIN)
        return dval == (int64_t) dval;
    return _PyFloat_is_Intlike(obj);
}


/* Not actually used... keeping in code for posterity's sake.
bool
double_is_intlike(const double val)
{
    if (val < INT64_MAX && val > INT64_MIN)
        return val == (int64_t) val;
    else {
        PyObject *pyval = PyFloat_FromDouble(val);
        const bool result = pyval == NULL
                          ? (PyErr_Clear(), false)
                          : _PyFloat_is_Intlike(pyval);
        Py_XDECREF(pyval);
        return result;
    }
}
*/


PyObject*
PyNumber_to_PyNumber(PyObject *pynum, const PyNumberType type,
                     const struct Options *options)
{
    PyObject *pyresult = NULL;
    switch (type) {
    case REAL:
        pyresult = PyNumber_to_PyInt_or_PyFloat(pynum, options);
        break;
    case FLOAT:
        pyresult = PyNumber_to_PyFloat(pynum, options);
        break;
    case INT:
    case FORCEINT:
    case INTLIKE:
        pyresult = PyNumber_to_PyInt(pynum, options);
        break;
    }
    /* Clear any error if the result is NULL
     * and we do not want to raise on errors.
     */
    if (pyresult == NULL && !Options_Should_Raise(options))
        PyErr_Clear();
    return pyresult;
}
