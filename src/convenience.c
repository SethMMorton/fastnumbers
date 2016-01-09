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
void convert_string(PyObject *input, char **str, Py_UCS4 *uni) {
    PyObject *temp_bytes = NULL;
    Py_ssize_t s_len, u_len;
    bool found_char = false;
    char *s;       /* char string */
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    Py_UCS4 *us;    /* Unicode string */
#else
    Py_UNICODE *us; /* Unicode string */
#endif
    Py_UCS4 uc;     /* Unicode character */
    Py_ssize_t i;   /* Loop index */
    *str = NULL;
    *uni = NULL_UNI;
    /* Try Bytes (Python2 str). */
    if (PyBytes_Check(input)) {
        PyBytes_AsStringAndSize(input, &s, &s_len);
        *str = malloc((size_t)s_len + 1);
        memcpy(*str, s, (size_t)s_len+1);
    /* Try Unicode. */
    } else if (PyUnicode_Check(input)) {
        /* Now convert this unicode object to a char* as ASCII, if possible. */
        temp_bytes = PyUnicode_AsEncodedString(input, "ascii", "strict");
        if (temp_bytes != NULL) {
            PyBytes_AsStringAndSize(temp_bytes, &s, &s_len);
            *str = malloc((size_t) s_len + 1);
            memcpy(*str, s, (size_t) s_len+1);
            Py_DECREF(temp_bytes);
        }
        /* If char* didn't work, try a single Py_UCS4 character. */
        /* If at any point it is found that the input is not valid Unicode */
        /* or more than one character, simply return a space. */
        /* Strip whitespace from input first if not of length 1. */
        else {
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
            if (PyUnicode_READY(input)) {
                *uni = (Py_UCS4) ' ';
            } else {
                u_len = PyUnicode_GET_LENGTH(input);
                /* For length 1, simply get the one character. */
                if (u_len == 1) {
                    *uni = PyUnicode_READ_CHAR(input, 0);
                /* For length > 1, try to strip whitespace */
                /* and hope there is only one character without whitespace. */
                } else {
                    us = PyUnicode_AsUCS4Copy(input);
                    if (us == NULL) {
                        *uni = (Py_UCS4) ' ';  /* Error */
                    } else {
                        /* Loop over each character of the unicode array. */
                        /* Skip whitespace, looking to find only a single */
                        /* non-whitespace character. If multiple found, */
                        /* call it an error and quit. */
                        for (i = 0; i < u_len; ++i) {
                            uc = PyUnicode_READ(PyUnicode_4BYTE_KIND, us, i);
                            if (!Py_UNICODE_ISSPACE(uc)) {
                                if (found_char) {
                                    *uni = (Py_UCS4) ' ';  /* Error */
                                    break;
                                } else {
                                    found_char = true;
                                    *uni = uc;
                                }
                            }
                        }
                        free(us);
                        /* Only whitespace found, error. */
                        if (*uni == NULL_UNI) *uni = (Py_UCS4) ' ';
                    }
                }
            }
#else
            u_len = PySequence_Length(input);
            us = PyUnicode_AsUnicode(input);
            if (us == NULL) {
                *uni = (Py_UCS4) ' ';  /* Error */
            /* For length 1, simply get the one character. */
            } else if (u_len == 1) {
                *uni = (Py_UCS4) us[0];
            /* For length > 1, try to strip whitespace */
            /* and hope there is only one character without whitespace. */
            } else {
                /* See above IFDEF section for explanation */
                for (i = 0; i < u_len; ++i) {
                    uc = (Py_UCS4) us[i];
                    // uc = PyUnicode_READ(PyUnicode_4BYTE_KIND, us, i);
                    if (!Py_UNICODE_ISSPACE(uc)) {
                        if (found_char) {
                            *uni = (Py_UCS4) ' ';  /* Error */
                            break;
                        } else {
                            found_char = true;
                            *uni = (Py_UCS4) uc;
                        }
                    }
                }
                /* Only whitespace found, error. */
                if (*uni == NULL_UNI) *uni = (Py_UCS4) ' ';
            }
#endif
            PyErr_Clear();
        }
    /* If none of the above, not a string type. */
    } else {
        PyErr_Format(PyExc_TypeError,
                     "expected str, float, or int argument, got %.200s",
                     input->ob_type->tp_name);
    }
}

/* Handle errors. Return the appropriate return value for the error situation. */
PyObject * handle_error(PyObject *input,
                        PyObject *default_value,
                        const bool raise_on_invalid,
                        const bool bad_inf,
                        const bool bad_nan,
                        const char* str,
                        const Py_UCS4 uni)
{
    /* If an error should be raised, raise the proper error. */
    if (raise_on_invalid) {
        if (bad_inf) {
            PyErr_SetString(PyExc_OverflowError,
                            "cannot convert Infinity to integer");
        }
        else if (bad_nan) {
            PyErr_SetString(PyExc_ValueError,
                            "cannot convert NaN to integer");
        }
        else if (str != NULL) {
            PyErr_Format(PyExc_ValueError,
                         "could not convert string to float or int: '%s'",
                         str);
        }
        else {
            PyErr_Format(PyExc_ValueError,
                         "could not convert string to float or int: '%c'",
                         uni);
        }
        return NULL;
    }

    /* If a default value is given, return that. */
    else if (default_value != NULL) {
        return default_value;
    }

    /* Otherwise, return the input as given. */
    else {
        return input;
    }
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
