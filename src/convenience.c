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
    PyObject *temp_bytes = NULL;
    /* Try Bytes (Python2 str). */
    if (PyBytes_Check(input)) {
        str = PyBytes_AS_STRING(input);        
    /* Try Unicode. */
    } else if (PyUnicode_Check(input)) {
        /* Now convert this unicode object to a char*. */
        temp_bytes = PyUnicode_AsEncodedString(input, "utf-8", "ignore");
        if (temp_bytes != NULL) {
            str = PyBytes_AS_STRING(temp_bytes);
            Py_DECREF(temp_bytes);
        }
        else
            return NULL; // UnicodeEncodeError
    /* If none of the above, not a string type. */
    } else {
        return (char*) PyErr_Format(PyExc_TypeError,
                                    "expected str, float, or int argument, got %.200s",
                                    input->ob_type->tp_name);
    }
    /* There was an error with conversion. */
    if (str == NULL) return NULL;
    return str;
}

/* Case-insensitive string match used for nan and inf detection; t should be
   lower-case.  Returns 1 for a successful match, 0 otherwise.
   Taken from the Python pystrtod.c source code. */
bool case_insensitive_match(const char *s, const char *t)
{
    while(*t && Py_TOLOWER(*s) == *t) {
        s++;
        t++;
    }
    return *t ? 0 : 1;
}