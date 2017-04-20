/*
 * Functions that will convert a Python string to a number.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include <limits.h>
#include "string_handling.h"
#include "number_handling.h"
#include "parsing.h"
#include "quick_detection.h"
#include "options.h"

/* Declarations for "private" static functions. */

static PyObject*
str_to_PyInt(const char *str, const char *end, const struct Options *options);

static PyObject*
str_to_PyFloat(const char *str, const char *end, const struct Options *options);

/* Definitions. */

static PyObject*
str_to_PyInt_or_PyFloat(const char *str, const char *end,
                        const struct Options *options)
{
    PyObject *pyresult = NULL;
    /* If the input contains an integer, convert to int directly. */
    if (string_contains_integer(str, end))
        return str_to_PyInt(str, end, options);

    /* If not an int, assume the input is a float. */
    pyresult = str_to_PyFloat(str, end, options);
    if (pyresult == NULL) return NULL;

    /* Coerce to int if needed. */
    return (Options_Coerce_True(options) && string_contains_intlike_float(str, end))
         ? PyFloat_to_PyInt(pyresult, options)
         : pyresult;
}


static PyObject*
str_to_PyFloat(const char *str, const char *end, const struct Options *options)
{
    /* Use some simple heuristics to determine if the the string
     * is likely a float - first and last characters must be digits.
     * Also quick detect NaN and INFINITY.
     */
    const char* start = str + (unsigned) is_sign(str);
    const unsigned len = (unsigned) (end - start);
    if (quick_detect_infinity(start, len)) {
        if (Options_Has_INF_Sub(options))
            return Options_Return_INF_Sub(options);
        else
            Py_RETURN_INF(is_negative_sign(str) ? -1.0 : 1.0);
    }
    else if (quick_detect_nan(start, len)) {
        if (Options_Has_NaN_Sub(options))
            return Options_Return_NaN_Sub(options);
        else
            Py_RETURN_NAN;
    }
    else if (!is_likely_float(start, end)) {
        SET_ERR_INVALID_FLOAT(options);
        return NULL;
    }

    /* Perform the actual parse using either the "fast" parser
     * or Python's built-in parser. Pre-determine if the "fast"
     * method might overflow based on the length of the string,
     * and if it might stay on the safe side and go straight to
     * Python's built-in version, otherwise use the "fast" version.
     */
    else if (float_might_overflow(str, end)) {
        char *pend = NULL, *nend = (char *) end;
        double result = -10.0;
#if PY_MAJOR_VERSION == 2
        /* If this is a long literal, don't include the L. */
        if (is_l_or_L(end - 1))
            pend = nend = (char *) end - 1;
#endif
        /* Building an exception takes a long time. The tiny
         * performance hit of checking that the input is a valid
         * float before converting to float is well worth it
         * compared to trying and failing and then waiting for the
         * exception to be created, only to clear it and move on.
         */
        if (!string_contains_float(str, end, true, true)) {
            SET_ERR_INVALID_FLOAT(options);
            return NULL;
        }
        result = python_lib_str_to_double(str, &pend);
        if (pend == nend)
            return PyFloat_FromDouble(result);
        /* Clear error if we should not raise. */
        if (!Options_Should_Raise(options))
            PyErr_Clear();
        return NULL;
    }
    else {
        bool error = false;
        double result = parse_float_from_string(str, end, &error);
        if (error) {
            SET_ERR_INVALID_FLOAT(options);
            return NULL;
        }
        return PyFloat_FromDouble(result);
    }
}


/* We need to take some extra care on Python 2 for the str2long conversion. */
#if PY_MAJOR_VERSION == 2
static PyObject*
python_lib_str_to_PyInt(const char* str, char** pend, const int base)
{
    PyObject *num = PyLong_FromString((char *) str, pend, (int) base);
    PyObject *num_swap = NULL;
    if (num == NULL)
        return NULL;
    /* Convert to int from long if possible. */
    num_swap = num;
    num = PyNumber_Int(num_swap);
    Py_XDECREF(num_swap);
    return num;
}
#else
#define python_lib_str_to_PyInt(str, pend, base) \
    PyLong_FromString((char *) (str), (pend), (int) (base))
#endif


static PyObject*
handle_possible_conversion_error(const char* end, char* pend,
                                 PyObject* val, const struct Options *options)
{
    /* If the expected end matches the parsed end, it was a success.
     * This function includes trailing whitespace in "end" definition
     * so we must do the same. */
    consume_white_space(end);
    /* If an error occurred, clear exception (if needed) and return NULL. */
    if (val == NULL || pend != end) {
        if (!Options_Should_Raise(options))
            PyErr_Clear();
        Py_XDECREF(val);  /* Probably redundant. */
        val = NULL;  /* Probably redundant. */
    }
    return val;
}


static PyObject*
str_to_PyInt(const char *str, const char *end, const struct Options *options)
{
    /* Use some simple heuristics to determine if the the string
     * is likely an int - first and last characters must be digits.
     */
    const char* start = str + (unsigned) is_sign(str);
    if (!is_likely_int(start, end)) {
        SET_ERR_INVALID_INT(options);
        return NULL;
    }

    /* Perform the actual parse using either the "fast" parser
     * or Python's built-in parser. Pre-determine if the "fast"
     * method might overflow based on the length of the string,
     * and if it might stay on the safe side and go straight to
     * Python's built-in version, otherwise use the "fast" version.
     */
    else if (int_might_overflow(str, end)) {
        PyObject *num = NULL;
        char *pend = "\0";
        /* Building an exception takes a long time. The tiny
         * performance hit of checking that the input is a valid
         * integer before converting to integer is well worth it
         * compared to trying and failing and then waiting for the
         * exception to be created, only to clear it and move on.
         */
        if (!string_contains_integer(str, end)) {
            SET_ERR_INVALID_INT(options);
            return NULL;
        }
        num = python_lib_str_to_PyInt(str, &pend, 10);
        return handle_possible_conversion_error(end, pend, num, options);
    }
    else {
        bool error = false;
        long result = parse_integer_from_string(str, end, &error);
        if (error) {
            SET_ERR_INVALID_INT(options);
            return NULL;
        }
        return long_to_PyInt(result);
    }
}


static PyObject*
str_to_PyInt_forced(const char *str, const char *end, const struct Options *options)
{
    /* Convert the input to an int or float. */
    PyObject *pyresult = str_to_PyInt_or_PyFloat(str, end, options);
    if (pyresult == NULL) return NULL;

    /* If a float was returned, convert to an int. Otherwise return as-is. */
    return PyFloat_Check(pyresult)
         ? PyFloat_to_PyInt(pyresult, options)
         : pyresult;
}


PyObject*
PyString_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const struct Options *options)
{
    const char* end;
    PyObject *pyresult = Py_None;  /* None indicates TypeError, not ValueError. */
    PyObject *bytes = NULL;  /* Keep a reference to the character array */
    const char *str = convert_PyString_to_str(obj, &end, &bytes);

    /* If we could extract the string, convert it! */
    if (string_conversion_success(str)) {
        switch (type) {
        case REAL:
            pyresult = str_to_PyInt_or_PyFloat(str, end, options);
            break;
        case FLOAT:
            pyresult = str_to_PyFloat(str, end, options);
            break;
        case INT:
            if (Options_Default_Base(options) || options->base == 10)
                pyresult = str_to_PyInt(str, end, options);
            else {
                char* pend = "\0";
                pyresult = python_lib_str_to_PyInt(str, &pend, options->base);
                pyresult = handle_possible_conversion_error(end, pend,
                                                            pyresult,
                                                            options);
            }
            break;
        case FORCEINT:
        case INTLIKE:
            pyresult = str_to_PyInt_forced(str, end, options);
            break;
        }
    }

    Py_XDECREF(bytes);
    return pyresult;
}