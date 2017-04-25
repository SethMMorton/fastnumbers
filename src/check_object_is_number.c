/*
 * Functions that will assess if a Python object is numeric.
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
PyObject_is_number(PyObject *obj, const PyNumberType type,
                   PyObject *allow_inf, PyObject *allow_nan,
                   PyObject *str_only, PyObject *num_only,
                   const int base)
{
    PyObject *pyresult = NULL;

    /* Already a number? Simple checks will work. */
    if (PyNumber_Check(obj))
        return PyBool_from_bool(PyObject_Not(str_only) &&
                                PyNumber_is_type(obj, type));

    /* If we are requiring it to be a number then we must declare false now. */
    else if (PyObject_IsTrue(num_only))
        Py_RETURN_FALSE;

    /* Assume a string. */
    pyresult = PyString_is_number(obj, type, allow_inf, allow_nan, base);
    if (pyresult != Py_None)
        return pyresult;

    /* Assume unicode. */
    pyresult = PyUnicodeCharacter_is_number(obj, type);
    if (pyresult != Py_None)
        return pyresult;

    /* If we got here, the type was invalid so return False. */
    Py_RETURN_FALSE;
}
