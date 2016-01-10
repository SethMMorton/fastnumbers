/*
 * Convenience functions for fastnumbers.
 *
 * Author: Seth M. Morton, Aug. 2, 2014
 */

#include <Python.h>
#include "py_to_char.h"


static Py_ssize_t get_PyUnicode_length(PyObject *input);
static Py_UCS4 read_first_PyUnicode_char(PyObject *input);
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
static Py_UCS4 read_PyUnicode_char(const Py_UCS4 *input, const Py_ssize_t i);
#else
static Py_UCS4 read_PyUnicode_char(const Py_UNICODE *input, const Py_ssize_t i);
#endif


/*
 * Extract a single Py_UCS4 character.
 * If at any point it is found that the input is not valid Unicode
 * or more than one character, simply return a space.
 * Allow padded whitespace.
 */
Py_UCS4 convert_PyUnicode_to_unicode_char(PyObject *input)
{
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
     static const bool needs_free = true;
#else
     static const bool needs_free = false;
#endif

    if (!PyUnicode_CheckExact(input))
        return NULL_UNI;
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    if (PyUnicode_READY(input))  /* If true, then not OK for conversion. */
        return NULL_UNI;
#endif

    const Py_ssize_t u_len = get_PyUnicode_length(input);
    if (u_len == 1)  /* Only one character - return now. */
        return read_first_PyUnicode_char(input);


#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    const Py_UCS4 *us = PyUnicode_AsUCS4Copy(input);
#else
    Py_UNICODE *us = PyUnicode_AsUnicode(input);
#endif
    if (us == NULL) {
        PyErr_Clear();
        return ERR_UNI;
    }

    /* Loop over each character of the unicode array. */
    /* Skip whitespace, looking to find only a single */
    /* non-whitespace character. If multiple found, */
    /* call it an error and quit. */
    Py_UCS4 uni = NULL_UNI;
    bool found_char = false;
    for (Py_ssize_t i = 0; i < u_len; ++i) {
        const Py_UCS4 uc = read_PyUnicode_char(us, i);
        if (!Py_UNICODE_ISSPACE(uc)) {
            if (found_char) {
                if (needs_free) free(us);
                return ERR_UNI;
            } else {
                found_char = true;
                uni = uc;
            }
        }
    }
    if (needs_free) free(us);
    if (uni == NULL_UNI)  /* Only whitespace found */
        return ERR_UNI;
    return uni;
}


/* Retrieve length of a unicode string */
inline static Py_ssize_t get_PyUnicode_length(PyObject *input)
{
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    return PyUnicode_GET_LENGTH(input);
#else
    return PySequence_Length(input);
#endif 
}


/* Get the first character only from a unicode string. */
inline static Py_UCS4 read_first_PyUnicode_char(PyObject *input)
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


/* Get a particular character from a unicode char array by index. */
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
inline static Py_UCS4 read_PyUnicode_char(const Py_UCS4 *input, const Py_ssize_t i)
{
    return PyUnicode_READ(PyUnicode_4BYTE_KIND, input, i);
}
#else
inline static Py_UCS4 read_PyUnicode_char(const Py_UNICODE *input, const Py_ssize_t i)
{
    return (Py_UCS4) input[i];
}
#endif
