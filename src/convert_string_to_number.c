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

/* Declarations for "private" static functions. */

static PyObject*
str_to_PyInt(const char *str);

static PyObject*
str_to_PyFloat(const char *str, PyObject *inf_sub, PyObject *nan_sub);

/* Definitions. */

static PyObject*
str_to_PyInt_or_PyFloat(const char *str,
                        PyObject *inf_sub, PyObject *nan_sub,
                        PyObject *pycoerce)
{
    /* If the input contains an integer, convert to int directly. */
    if (string_contains_integer(str))
        return str_to_PyInt(str);

    /* If not an int, assume the input is a float. */
    PyObject *pyresult = str_to_PyFloat(str, inf_sub, nan_sub);
    if (pyresult == NULL) return NULL;

    /* Coerce to int if needed. */
    return (PyObject_IsTrue(pycoerce) && string_contains_intlike_float(str))
         ? PyFloat_to_PyInt(pyresult)
         : pyresult;
}


static PyObject*
str_to_PyFloat(const char *str, PyObject *inf_sub, PyObject *nan_sub)
{
    bool error = false, overflow = false;

    /* Perform the actual parse. */
    double result = parse_float_from_string(str, &error, &overflow);
    if (error) return NULL;

    /* If there was an overflow (or a possible overflow),
     * see if Python can do better.
     */
    if (overflow) {
        char* end;  /* To avoid errors for trailing whitespace. */
        result = python_lib_str_to_double(str, &end);
    }

    /* Return the converted number. */
    if (inf_sub != NULL && Py_IS_INFINITY(result))
        return Py_INCREF(inf_sub), inf_sub;
    else if (nan_sub != NULL && Py_IS_NAN(result))
        return Py_INCREF(nan_sub), nan_sub;
    else
        return PyFloat_FromDouble(result);

}


static PyObject*
str_to_PyInt(const char *str)
{
    bool error = false, overflow = false;

    /* Perform the actual parse. */
    long result = parse_integer_from_string(str, &error, &overflow);
    if (error) return NULL;

    /* If there was an overflow (or a possible overflow),
     * see if Python can do better. Otherwise use our result.
     */
    return overflow ? python_lib_str_to_PyInt((char*) str) : long_to_PyInt(result);
}


static PyObject*
str_to_PyInt_forced(const char *str)
{
    /* Convert the input to an int or float. */
    PyObject *pyresult = str_to_PyInt_or_PyFloat(str, NULL, NULL, Py_True);
    if (pyresult == NULL) return NULL;

    /* If a float was returned, convert to an int. Otherwise return as-is. */
    return PyFloat_Check(pyresult) ? PyFloat_to_PyInt(pyresult) : pyresult;
}


PyObject*
PyString_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub, PyObject *pycoerce)
{
    PyObject *pyresult = Py_None;  /* None indicates TypeError, not ValueError. */
    PyObject *bytes = NULL;  /* Keep a reference to the character array */
    const char *str = convert_PyString_to_str(obj, &bytes);

    /* If we could extract the string, convert it! */
    if (string_conversion_success(str)) {
        switch (type) {
        case REAL:
            pyresult = str_to_PyInt_or_PyFloat(str, inf_sub, nan_sub, pycoerce);
            break;
        case FLOAT:
            pyresult = str_to_PyFloat(str, inf_sub, nan_sub);
            break;
        case INT:
            pyresult = str_to_PyInt(str);
            break;
        case FORCEINT:
        case INTLIKE:
            pyresult = str_to_PyInt_forced(str);
            break;
        }
    }

    Py_XDECREF(bytes);
    return pyresult;
}