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
                   const struct Options *options)
{
    PyObject *pyresult = NULL;

    /* Already a number? Simple checks will work. */
    if (PyNumber_Check(obj))
        return PyBool_from_bool(!Options_String_Only(options) &&
                                PyNumber_is_type(obj, type));

    /* If we are requiring it to be a number then we must declare false now. */
    else if (Options_Number_Only(options))
        Py_RETURN_FALSE;

    /* Assume a string. */
    pyresult = PyString_is_number(obj, type, options);
    if (pyresult != Py_None)
        return pyresult;

    /* Assume unicode. */
    pyresult = PyUnicodeCharacter_is_number(obj, type);
    if (pyresult != Py_None)
        return pyresult;

    /* If we got here, the type was invalid so return False. */
    Py_RETURN_FALSE;
}
