/*
 * Functions that will assess if a string (character array) is numeric.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include <limits.h>
#include "string_handling.h"
#include "parsing.h"

PyObject*
PyString_is_number(PyObject *obj, const PyNumberType type,
                   PyObject *allow_inf, PyObject *allow_nan,
                   const int base)
{
    const char* end;
    bool result = false;
    PyObject *bytes = NULL;  /* Keep a reference to the character array */
    const char *str = convert_PyString_to_str(obj, &end, &bytes);

    if (string_conversion_success(str)) {
        switch (type) {
        case REAL:
        case FLOAT:
            result = string_contains_float(str, end,
                                           PyObject_IsTrue(allow_inf),
                                           PyObject_IsTrue(allow_nan));
            break;
        case INT:
            if (base == INT_MIN || base == 10)
                result = string_contains_integer(str, end);
            else
                result = string_contains_integer_arbitrary_base(str, end, base);
            break;
        case FORCEINT:
        case INTLIKE:
            result = string_contains_intlike_float(str, end);
            break;
        }
    }
    else
        return Py_None;  /* Not a string. */

    Py_XDECREF(bytes);
    return PyBool_from_bool(result);
}
