/*
 * Functions that will convert/evaluate a Python number.
 *
 * Author: Seth M. Morton
 *
 * July 2018
 */

#include <Python.h>
#include <float.h>
#include "fastnumbers/numbers.h"
#include "fastnumbers/options.h"
#include "fastnumbers/pstdint.h"


/* Ensure 64 bits are handled. */
#ifndef INT64_MAX
#error "fastnumbers requires that your compiler support 64 bit integers, but it appears that this compiler does not"
#endif


/*
 * Copy of _Py_dg_stdnan from the Python code base.
 * Dynamically generate an NaN. Needed for Windows.
 * I hate copy/pasting code, but this seems like the only way.
 */
typedef union {
    double d;
    uint32_t L[2];
} U;
#ifdef DOUBLE_IS_LITTLE_ENDIAN_IEEE754
#define word0(x) (x)->L[1]
#define word1(x) (x)->L[0]
#else
#define word0(x) (x)->L[0]
#define word1(x) (x)->L[1]
#endif
#define dval(x) (x)->d
#define NAN_WORD0 0x7ff80000
#define NAN_WORD1 0
#define Sign_bit 0x80000000
double
compat_generate_nan(int sign) {
    U rv;
    word0(&rv) = NAN_WORD0;
    word1(&rv) = NAN_WORD1;
    if (sign) {
        word0(&rv) |= Sign_bit;
    }
    return dval(&rv);
}


static bool
_PyFloat_is_Intlike(PyObject *obj) {
    /* NOTE: This code copy/pasted with modification from the CPython source.
             They did not expose it as public for some reason.
     */
    bool retval = false;
    const double x = PyFloat_AsDouble(obj);

    if (x == -1.0 && PyErr_Occurred()) {
        return PyErr_Clear(), false;
    }
    if (!Py_IS_FINITE(x)) {
        return false;
    }
    errno = 0;
    retval = (floor(x) == x) ? true : false;
    if (errno != 0) {
        return false;
    }
    return retval;
}


static PyObject *
PyNumber_to_PyInt_or_PyFloat(PyObject *pynum, const Options *options)
{
    if (Options_Has_NaN_Sub(options) && PyNumber_IsNAN(pynum)) {
        return Options_Return_NaN_Sub(options);
    }
    else if (Options_Has_INF_Sub(options) && PyNumber_IsINF(pynum)) {
        return Options_Return_INF_Sub(options);
    }
    else if (Options_Coerce_True(options)) {
        if (PyLong_Check(pynum) || PyFloat_is_Intlike(pynum)) {
            return PyNumber_Long(pynum);
        }
        else {
            return PyNumber_Float(pynum);
        }
    }
    else {
        return Py_INCREF(pynum), pynum;
    }
}


static PyObject *
PyNumber_to_PyFloat(PyObject *pynum, const Options *options)
{
    if (Options_Has_NaN_Sub(options) && PyNumber_IsNAN(pynum)) {
        return Options_Return_NaN_Sub(options);
    }
    else if (Options_Has_INF_Sub(options) && PyNumber_IsINF(pynum)) {
        return Options_Return_INF_Sub(options);
    }
    else {
        return PyNumber_Float(pynum);
    }
}


static PyObject *
PyNumber_to_PyInt(PyObject *pynum, const Options *options)
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
    return PyNumber_Long(pynum);
}


PyObject *
PyFloat_to_PyInt(PyObject *fobj, const Options *options)
{
    PyObject *tmp = PyNumber_to_PyInt(fobj, options);
    Py_DECREF(fobj);
    return tmp;
}


bool
PyFloat_is_Intlike(PyObject *obj)
{
    const double dval = PyFloat_AS_DOUBLE(obj);
    if (!PyFloat_Check(obj)) {
        return false;
    }
    if (dval < INT64_MAX && dval > INT64_MIN) {
        return dval == (int64_t) dval;
    }
    return _PyFloat_is_Intlike(obj);
}


/* Convert a PyNumber to the desired PyNumber type. */
PyObject *
PyNumber_to_PyNumber(PyObject *pynum, const PyNumberType type,
                     const Options *options)
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
    if (pyresult == NULL && !Options_Should_Raise(options)) {
        PyErr_Clear();
    }
    return pyresult;
}


/* Check that a PyNumber is the desired PyNumber type. */
bool
PyNumber_is_type(PyObject *obj, const PyNumberType type)
{
    register bool result = false;
    switch (type) {
    case REAL:
        result = true;
        break;
    case FLOAT:
        result = PyFloat_Check(obj);
        break;
    case INT:
        result = PyLong_Check(obj);
        break;
    case INTLIKE:
    case FORCEINT:
        result = PyLong_Check(obj) || PyFloat_is_Intlike(obj);
        break;
    }
    return result;
}
