/*
 * Convenience functions for fastnumbers.
 *
 * Author: Seth M. Morton, Aug. 2, 2014
 */

#include <Python.h>
#include "convenience.h"

/* Used to determine if a float is so large it lost precision. */
const double maxsize = 9007199254740992;  /* 2^53 */

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

#if PY_MAJOR_VERSION == 2
/* Convert a PyString to a PyFloat, possibly removing trailing 'L' because */
/* of a long literal. Should only be used on Python 2. */
/* Assume input has been checked for legality prior to use. */
PyObject * convert_PyString_to_PyFloat_possible_long_literal(PyObject *s)
{
    PyObject *result = NULL, *stripped1 = NULL, *stripped2 = NULL;
    result = PyFloat_FromString(s, NULL);
    if (result == NULL) {
        PyErr_Clear();
        stripped1 = PyObject_CallMethod(s, "rstrip", NULL);
        stripped2 = PyObject_CallMethod(stripped1, "rstrip", "s", "lL");
        result = PyFloat_FromString(stripped2, NULL);
        Py_DECREF(stripped1);
        Py_DECREF(stripped2);
    }
    return result;
}
#endif
