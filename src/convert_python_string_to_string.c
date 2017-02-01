/*
 * Conversion from PyUnicode/PyBytes type to a string (character array).
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include <Python.h>
#include "string_handling.h"
#include "parsing.h"

/*
 * Try to convert the Python object to bytes (i.e. char*).
 * Possibly convert unicode to bytes object first.
 * If the string contains nul characters, return "\0".
 */
const char*
convert_PyString_to_str(PyObject *input, PyObject **bytes_object)
{
    /* A reference to the bytes object will need to be given to the
     * caller if it was created to ensure the character array is not
     * destroyed.
     */
    *bytes_object = NULL;

    /* If the input is already bytes, then just extract the
     * underlying data and return.
     */
    if (PyBytes_Check(input)) {
        const char *str = PyBytes_AS_STRING(input);
        if (strlen(str) != (size_t) PyBytes_GET_SIZE(input))
            return "\0";
        consume_white_space(str);
        return str;
    }

    /* If the input was in unicode format, encode to bytes in ASCII format
     * and fail on any error. Extract the bytes as before.
     */
    else if (PyUnicode_Check(input)) {
        *bytes_object = PyUnicode_AsEncodedString(input, "ascii", "strict");
        if (*bytes_object != NULL) {
            const char *str = PyBytes_AS_STRING(*bytes_object);
            if (strlen(str) != (size_t) PyBytes_GET_SIZE(*bytes_object))
                return "\0";
            consume_white_space(str);
            return str;
        }
        else {
            PyErr_Clear();
            return NULL;
        }
    }

    /* Return NULL if the data type was invalid. */
    else
        return NULL;
}
