/*
 * Functions that will assess if a string (character array) is numeric.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "string_handling.h"
#include "parsing.h"
#include <limits.h>

PyObject*
PyString_is_number(PyObject *obj, const PyNumberType type,
                   const struct Options *options)
{
    const char* end;
    bool result = false;
    PyObject *bytes = NULL;  /* Keep a reference to the character array */
    Py_buffer view = {NULL, NULL}; /* Reference to a buffer object */
    char *temp_char = NULL;  /* Reference to a character array */
    const char *str = convert_PyString_to_str(obj, &end, &bytes, &temp_char, &view);

    if (string_conversion_success(str)) {
        switch (type) {
        case REAL:
        case FLOAT:
            result = string_contains_float(str, end,
                                           Options_Allow_Infinity(options),
                                           Options_Allow_NAN(options));
            break;
        case INT:
            if (options->base == INT_MIN || options->base == 10)
                result = string_contains_integer(str, end);
            else
                result = string_contains_integer_arbitrary_base(str, end, options->base);
            break;
        case FORCEINT:
        case INTLIKE:
            result = string_contains_intlike_float(str, end);
            break;
        }
    }
    else
        return Py_None;  /* Not a string. */

    PyBuffer_Release(&view);
    if (temp_char)
        PyMem_FREE(temp_char);
    Py_XDECREF(bytes);
    return PyBool_from_bool(result);
}
