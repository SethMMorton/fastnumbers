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

/* The unicode handling methods in the C API essentially completely
 * changed starting with Python 3.3. The method that fastnumbers uses
 * to get a character array from the PyUnicode object is very different
 * between Python < 3.3 and Python >= 3.3. The older method is
 * much slower because it involves first converting to PyBytes type,
 * while the newer method can get the character array directly.
 */

/* PRE-PYTHON 3.3. */
#if (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 3) || PY_MAJOR_VERSION == 2

static const char*
PyUnicode_as_ascii_string(PyObject *obj, PyObject **bobj)
{
    /* Attempt to convert to a PyBytes object encoded as
     * ASCII, and if possible extract the character array from
     * that object. The PyBytes object must stay alive till
     * we are done with the string to avoid the garbage collector
     * de-allocating our character array and causing a seg fault;
     * to do this we return the PyBytes object so it can be de-referenced
     * at a later time.
     */
    *bobj = PyUnicode_AsASCIIString(obj);
    if (*bobj == NULL) {
        PyErr_Clear();
        return NULL;
    }
    else {
        const char *str = PyBytes_AS_STRING(*bobj);
        return strlen(str) != (size_t) PyBytes_GET_SIZE(*bobj) ? "\0" : str;
    }
}

/* PYTHON 3.3 AND AFTER. */
#else

static const char*
PyUnicode_as_ascii_string(PyObject *obj, PyObject **bytes_object)
{
    /* If the input is not ASCII, return NULL.
     * Otherwise we return the character array.
     * The bytes_object input is not used and is only
     * for compatibility with Python < 3.3.
     */
     if (PyUnicode_IS_ASCII(obj)) {
        const char* str = PyUnicode_AsUTF8(obj);
        return strlen(str) != (size_t) PyUnicode_GET_LENGTH(obj) ? "\0" : str;
     }
     else
        return NULL;
}

#endif


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
     * destroyed. Only needed for Python < 3.3.
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

    /* If the input was in unicode format, extract as ASCII if we can.
     */
    else if (PyUnicode_Check(input)) {
        const char* str = PyUnicode_as_ascii_string(input, bytes_object);
        if (str == NULL) return NULL;
        consume_white_space(str);
        return str;
    }

    /* Return NULL if the data type was invalid. */
    else
        return NULL;
}
