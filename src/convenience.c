/*
 * Convenience functions for fastnumbers.
 *
 * Author: Seth M. Morton, Aug. 2, 2014
 */

#include <Python.h>
#include "convenience.h"

/* 
 * Convert a string to a character array.
 * If unsuccessful, raise a TypeError.
 * A return value of NULL means an error occurred.
 */
char* convert_string(PyObject *input) {
    char* str;
#if PY_MAJOR_VERSION < 3 || (PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION < 3)
    PyObject *temp_bytes = NULL;
#endif
    /* Try Bytes (Python2 str). */
    if (PyBytes_Check(input)) {
        str = PyBytes_AS_STRING(input);        
    /* Try Unicode. */
    } else if (PyUnicode_Check(input)) {
        /* Now convert this unicode object to a char*. */
        /* There is a convenient way in newer Python versions. */
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 3
        str = PyUnicode_AsUTF8(input);
#else
        temp_bytes = PyUnicode_AsASCIIString(input);
        if (temp_bytes != NULL) {
            str = PyBytes_AS_STRING(temp_bytes);
            Py_DECREF(temp_bytes);
        }
        else
            return NULL;
#endif
    /* If none of the above, not a string type. */
    } else {
        PyErr_Format(PyExc_TypeError,
                     "expected str, float, or int argument, got %.200s",
                     input->ob_type->tp_name);
        return NULL;
    }
    /* There was an error with conversion. */
    if (str == NULL) {
        PyErr_Format(PyExc_TypeError,
                     "expected str, float, or int argument, got %.200s",
                     input->ob_type->tp_name);
        return NULL;
    }
    return str;
}
