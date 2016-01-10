#ifndef __PY_TO_CHAR
#define __PY_TO_CHAR

#include <Python.h>
#include "fn_bool.h"

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
 */
inline static char* convert_PyString_to_str(PyObject *input, PyObject **bytes_object)
{
    *bytes_object = NULL;
    if (PyBytes_CheckExact(input)) {
        return PyBytes_AS_STRING(input);
    }
    else if (PyUnicode_CheckExact(input)) {
        *bytes_object = PyUnicode_AsEncodedString(input, "ascii", "strict");
        if (*bytes_object != NULL) {
            return PyBytes_AS_STRING(*bytes_object);
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
