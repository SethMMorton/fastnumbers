/*
 * Functions that will convert a Python string to a number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "string_handling.h"
#include "number_handling.h"
#include "parsing.h"
#include "quick_detection.h"

/* Declarations for "private" static functions. */

static PyObject*
str_to_PyInt(const char *str, const char *end);

static PyObject*
str_to_PyFloat(const char *str, const char *end, PyObject *inf_sub, PyObject *nan_sub);

/* Definitions. */

static PyObject*
str_to_PyInt_or_PyFloat(const char *str, const char *end,
                        PyObject *inf_sub, PyObject *nan_sub,
                        PyObject *pycoerce)
{
    /* If the input contains an integer, convert to int directly. */
    if (string_contains_integer(str, end))
        return str_to_PyInt(str, end);

    /* If not an int, assume the input is a float. */
    PyObject *pyresult = str_to_PyFloat(str, end, inf_sub, nan_sub);
    if (pyresult == NULL) return NULL;

    /* Coerce to int if needed. */
    return (PyObject_IsTrue(pycoerce) && string_contains_intlike_float(str, end))
         ? PyFloat_to_PyInt(pyresult)
         : pyresult;
}


static PyObject*
str_to_PyFloat(const char *str, const char *end, PyObject *inf_sub, PyObject *nan_sub)
{
    /* Use some simple heuristics to determine if the the string
     * is likely a float - first and last characters must be digits.
     * Also quick detect NaN and INFINITY.
     */
    const char* start = str + (unsigned) is_sign(str);
    const unsigned len = end - start;
    if (quick_detect_infinity(start, len)) {
        if (inf_sub == NULL)
            Py_RETURN_INF(is_negative_sign(str) ? -1.0 : 1.0);
        else
            return Py_INCREF(inf_sub), inf_sub;
    }
    else if (quick_detect_nan(start, len)) {
        if (nan_sub == NULL)
            Py_RETURN_NAN;
        else
            return Py_INCREF(nan_sub), nan_sub;
    }
    else if (!is_likely_float(start, end))
        return NULL;

    /* Perform the actual parse using either the "fast" parser
     * or Python's built-in parser. Pre-determine if the "fast"
     * method might overflow based on the length of the string,
     * and if it might stay on the safe side and go straight to
     * Python's built-in version, otherwise use the "fast" version.
     */
    else if (float_might_overflow(str, end)) {
        char *pend = NULL;
        /* Building an exception takes a long time. The tiny
         * performance hit of checking that the input is a valid
         * float before converting to float is well worth it
         * compared to trying and failing and then waiting for the
         * exception to be created, only to clear it and move on.
         */
        const double result = string_contains_float(str, end, true, true)
                            ? python_lib_str_to_double(str, &pend)
                            : -1.0;
        return pend == end ? PyFloat_FromDouble(result) : (PyErr_Clear(), NULL);
    }
    else {
        bool error = false;
        double result = parse_float_from_string(str, end, &error);
        return error ? NULL : PyFloat_FromDouble(result);
    }
}


/* We need to take some extra care on Python 2 for the str2long conversion. */
#if PY_MAJOR_VERSION == 2
static PyObject*
python_lib_str_to_PyInt(const char* str, char** pend)
{
    PyObject *num = PyLong_FromString((char *) str, pend, 10);
    PyObject *num_swap = NULL;
    if (num == NULL)
        return PyErr_Clear(), NULL;
    /* Convert to int from long if possible. */
    num_swap = num;
    num = PyNumber_Int(num_swap);
    Py_XDECREF(num_swap);
    return num;
}
#else
#define python_lib_str_to_PyInt(str, pend) \
    PyLong_FromString((char *) (str), (pend), 10)
#endif


static PyObject*
str_to_PyInt(const char *str, const char *end)
{
    /* Use some simple heuristics to determine if the the string
     * is likely an int - first and last characters must be digits.
     */
    const char* start = str + (unsigned) is_sign(str);
    if (!is_likely_int(start, end))
        return NULL;

    /* Perform the actual parse using either the "fast" parser
     * or Python's built-in parser. Pre-determine if the "fast"
     * method might overflow based on the length of the string,
     * and if it might stay on the safe side and go straight to
     * Python's built-in version, otherwise use the "fast" version.
     */
    else if (int_might_overflow(str, end)) {
        char *pend;
        /* Building an exception takes a long time. The tiny
         * performance hit of checking that the input is a valid
         * integer before converting to integer is well worth it
         * compared to trying and failing and then waiting for the
         * exception to be created, only to clear it and move on.
         */
        PyObject *num = string_contains_integer(str, end)
                      ? python_lib_str_to_PyInt(str, &pend)
                      : NULL;
        /* If the expected end matches the parsed end, it was a success.
         * This function includes trailing whitespace in "end" definition
         * so we must do the same. */
        consume_white_space(end);
        return num != NULL && pend == end ? num : (PyErr_Clear(), NULL);
    }
    else {
        bool error = false;
        long result = parse_integer_from_string(str, end, &error);
        return error ? NULL : long_to_PyInt(result);
    }
}


static PyObject*
str_to_PyInt_forced(const char *str, const char *end)
{
    /* Convert the input to an int or float. */
    PyObject *pyresult = str_to_PyInt_or_PyFloat(str, end, NULL, NULL, Py_True);
    if (pyresult == NULL) return NULL;

    /* If a float was returned, convert to an int. Otherwise return as-is. */
    return PyFloat_Check(pyresult) ? PyFloat_to_PyInt(pyresult) : pyresult;
}


PyObject*
PyString_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub, PyObject *pycoerce)
{
    const char* end;
    PyObject *pyresult = Py_None;  /* None indicates TypeError, not ValueError. */
    PyObject *bytes = NULL;  /* Keep a reference to the character array */
    const char *str = convert_PyString_to_str(obj, &end, &bytes);

    /* If we could extract the string, convert it! */
    if (string_conversion_success(str)) {
        switch (type) {
        case REAL:
            pyresult = str_to_PyInt_or_PyFloat(str, end, inf_sub, nan_sub, pycoerce);
            break;
        case FLOAT:
            pyresult = str_to_PyFloat(str, end, inf_sub, nan_sub);
            break;
        case INT:
            pyresult = str_to_PyInt(str, end);
            break;
        case FORCEINT:
        case INTLIKE:
            pyresult = str_to_PyInt_forced(str, end);
            break;
        }
    }

    Py_XDECREF(bytes);
    return pyresult;
}