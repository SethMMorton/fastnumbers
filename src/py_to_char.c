/*
 * Convenience functions for fastnumbers.
 *
 * Author: Seth M. Morton, Aug. 2, 2014
 */

#include <Python.h>
#include "py_to_char.h"

#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
#define uni_isspace(uni) Py_UNICODE_ISSPACE(uni)
#else
#define uni_isspace(uni) Py_UNICODE_ISSPACE((Py_UNICODE) (uni))
#endif

static Py_ssize_t
get_PyUnicode_length(PyObject *input);

static Py_UCS4
read_first_PyUnicode_char(PyObject *input);


/*
 * Try to convert the Python object to bytes (i.e. char*).
 * Possibly convert unicode to bytes object first.
 * If the string contains nul characters, return "\0".
 */
const char*
convert_PyString_to_str(PyObject *input, PyObject **bytes_object)
{
    *bytes_object = NULL;
    if (PyBytes_Check(input)) {
        const char *str = PyBytes_AS_STRING(input);
        if (strlen(str) != (size_t) PyBytes_GET_SIZE(input))
            return "\0";
        return str;
    }
    else if (PyUnicode_Check(input)) {
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


/*
 * Extract a single Py_UCS4 character.
 * If at any point it is found that the input is not valid Unicode
 * or more than one character, simply return a space.
 * Allow padded whitespace.
 */
Py_UCS4
convert_PyUnicode_to_unicode_char(PyObject *input)
{
    Py_UCS4 uni = NULL_UNI;
    PyObject * element = NULL;
    PyObject * split = NULL;

    /* Ensure input is a valid unicode object. */
    if (!PyUnicode_Check(input))
        return NULL_UNI;
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    if (PyUnicode_READY(input))  /* If true, then not OK for conversion. */
        return NULL_UNI;
#endif

    /* Is the input already of length 1? If so, return now. */
    if (get_PyUnicode_length(input) == 1) {
        return read_first_PyUnicode_char(input);
    }

    /* For some reason, the C API does not give good access to the unicode
     * strip operation. Luckily split also strips off whitespace, so we can
     * use this fact to remove any whitespace from our input.
     */
    split = PyUnicode_Split(input, NULL, -1);
    if (split == NULL) {
        PyErr_Clear();
        return ERR_UNI;
    }

    /* Ensure that both the list and the first element have length 1. */
    if (PySequence_Length(split) != 1) {
        Py_DECREF(split);
        return ERR_UNI;
    }
    element = PyList_GET_ITEM(split, 0);
    if (get_PyUnicode_length(element) != 1) {
        Py_DECREF(split);
        return ERR_UNI;
    }

    /* If only a single element, return it. */
    uni = read_first_PyUnicode_char(element);
    Py_DECREF(split);
    return uni;
}


/* Retrieve length of a unicode string */
static Py_ssize_t
get_PyUnicode_length(PyObject *input)
{
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    return PyUnicode_GET_LENGTH(input);
#else
    return PySequence_Length(input);
#endif 
}


/* Get the first character only from a unicode string. */
static Py_UCS4
read_first_PyUnicode_char(PyObject *input)
{
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    return PyUnicode_READ_CHAR(input, 0);
#else
    const Py_UNICODE *us = PyUnicode_AsUnicode(input);
    if (us == NULL) {
        PyErr_Clear();
        return ERR_UNI;
    }
    return (Py_UCS4) us[0];
#endif
}
