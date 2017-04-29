/*
 * Conversion from PyUnicode type to a single unicode character.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "unicode_handling.h"

/* Declarations */

static Py_ssize_t
get_PyUnicode_length(PyObject *input);

static Py_UCS4
read_first_PyUnicode_char(PyObject *input);


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
    if (split == NULL)
        return ERR_UNI;

    /* Ensure that both the list and the first element have length 1. */
    if (PyList_GET_SIZE(split) != 1) {
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


static Py_ssize_t
get_PyUnicode_length(PyObject *input)
{
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    return PyUnicode_GET_LENGTH(input);
#else
    return PySequence_Length(input);
#endif
}


static Py_UCS4
read_first_PyUnicode_char(PyObject *input)
{
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    return PyUnicode_READ_CHAR(input, 0);
#else
    /* Because of how input is vetted, will not return NULL. */
    const Py_UNICODE *us = PyUnicode_AsUnicode(input);
    return (Py_UCS4) us[0];
#endif
}
