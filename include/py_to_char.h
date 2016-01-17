#ifndef __PY_TO_CHAR
#define __PY_TO_CHAR

#include <Python.h>
#include "fn_bool.h"
#include "parsing.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bad unicode. */
#define NULL_UNI ((Py_UCS4)-1)
#define ERR_UNI (Py_UCS4) ' '


Py_UCS4 convert_PyUnicode_to_unicode_char(PyObject *input);


/*
 * Try to convert the Python object to bytes (i.e. char*).
 * Possibly convert unicode to bytes object first.
 * If the string contains nul characters, return "\0".
 */
inline static const char* convert_PyString_to_str(PyObject *input, PyObject **bytes_object)
{
    *bytes_object = NULL;
    if (PyBytes_CheckExact(input)) {
        const char *str = PyBytes_AS_STRING(input);
        if (strlen(str) != (size_t) PyBytes_GET_SIZE(input))
            return "\0";
        return str;
    }
    else if (PyUnicode_CheckExact(input)) {
        *bytes_object = PyUnicode_AsEncodedString(input, "ascii", "strict");
        if (*bytes_object != NULL) {
            const char *str = PyBytes_AS_STRING(*bytes_object);
            if (strlen(str) != (size_t) PyBytes_GET_SIZE(*bytes_object))
                return "\0";
            return str;
        }
        else {
            PyErr_Clear();
            return NULL;
        }
    }
    else
        return NULL;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PY_TO_CHAR */
