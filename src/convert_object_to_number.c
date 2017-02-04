/*
 * Functions that will convert a Python object to a number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "object_handling.h"
#include "number_handling.h"
#include "string_handling.h"
#include "unicode_handling.h"


PyObject*
PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub,
                     PyObject *pycoerce)
{
    PyObject *pyresult = NULL;

    /* Already a number? Simple conversion will work. */
    if (PyNumber_Check(obj))
        return PyNumber_to_PyNumber(obj, type, inf_sub, nan_sub, pycoerce);

    /* Assume a string. */
    pyresult = PyString_to_PyNumber(obj, type, inf_sub, nan_sub, pycoerce);
    if (pyresult != Py_None)
        return pyresult;

    /* Assume unicode. */
    pyresult = PyUnicode_to_PyNumber(obj, type);
    if (pyresult != Py_None)
        return pyresult;

    /* None indicates nothing worked. */
    return Py_None;
}
