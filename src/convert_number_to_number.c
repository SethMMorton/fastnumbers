/*
 * Functions that will convert a Python number to another number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "number_handling.h"

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
PyNumber_to_PyInt_or_PyFloat(PyObject *pynum,
                             PyObject *inf_sub, PyObject *nan_sub,
                             PyObject *pycoerce)
{
    if (nan_sub != NULL && PyFloat_Check(pynum) &&
            Py_IS_NAN(PyFloat_AS_DOUBLE(pynum)))
        return Py_INCREF(nan_sub), nan_sub;
    else if (inf_sub != NULL && PyFloat_Check(pynum) &&
            Py_IS_INFINITY(PyFloat_AS_DOUBLE(pynum)))
        return Py_INCREF(inf_sub), inf_sub;
    else if (PyObject_IsTrue(pycoerce)) {
        if (PyNumber_IsInt(pynum) || PyFloat_is_Intlike(pynum))
            return PyNumber_ToInt(pynum);
        else
            return PyNumber_Float(pynum);
    }
    else
        return Py_INCREF(pynum), pynum;
}


static PyObject*
PyNumber_to_PyFloat(PyObject *pynum, PyObject *inf_sub, PyObject *nan_sub)
{
    if (nan_sub != NULL && PyFloat_Check(pynum) &&
            Py_IS_NAN(PyFloat_AS_DOUBLE(pynum)))
        return Py_INCREF(nan_sub), nan_sub;
    else if (inf_sub != NULL && PyFloat_Check(pynum) &&
            Py_IS_INFINITY(PyFloat_AS_DOUBLE(pynum)))
        return Py_INCREF(inf_sub), inf_sub;
    else
        return PyNumber_Float(pynum);
}


static PyObject*
PyNumber_to_PyInt(PyObject *pynum)
{
    if (PyFloat_Check(pynum)) { /* Watch out for un-intable numbers. */
        const double d = PyFloat_AS_DOUBLE(pynum);
        if (Py_IS_NAN(d) || Py_IS_INFINITY(d)) return NULL;
    }
    return PyNumber_ToInt(pynum);
}


PyObject*
PyFloat_to_PyInt(PyObject * fobj)
{
    PyObject *tmp = PyNumber_to_PyInt(fobj);
    Py_DECREF(fobj);
    return tmp;
}


bool
PyFloat_is_Intlike(PyObject *obj)
{
    const double dval = PyFloat_AS_DOUBLE(obj);
#ifdef _MSC_VER
    if (dval < _I64_MAX && dval > _I64_MIN)
        return dval == (long long) dval;
#else
    if (dval < LONG_MAX && dval > LONG_MIN)
        return dval == (long) dval;
#endif
    return _PyFloat_is_Intlike(obj);
}


/* Not actually used... keeping in code for posterity's sake.
bool
double_is_intlike(const double val)
{
    if (val < LONG_MAX && val > LONG_MIN)
        return val == (long) val;
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
                     PyObject *inf_sub, PyObject *nan_sub, PyObject *pycoerce)
{
    PyObject *pyresult = NULL;
    switch (type) {
    case REAL:
        pyresult = PyNumber_to_PyInt_or_PyFloat(pynum, inf_sub, nan_sub, pycoerce);
        break;
    case FLOAT:
        pyresult = PyNumber_to_PyFloat(pynum, inf_sub, nan_sub);
        break;
    case INT:
    case FORCEINT:
    case INTLIKE:
        pyresult = PyNumber_to_PyInt(pynum);
        break;
    }
    return PyErr_Clear(), pyresult;
}
