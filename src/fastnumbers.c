/*
 * C implementation of the python library fastnumbers.
 *
 * Author: Seth M. Morton, July 30, 2014
 */

#include <Python.h>
#include "fast_conversions.h"
#include "version.h"
#include "docstrings.h"
#include "convenience.h"

/* Used to determine if a float is so large it lost precision. */
const double maxsize = 9007199254740992;  /* 2^53 */


/* Safely convert to an int or float, depending on value. */
static PyObject *
fastnumbers_safe_real(PyObject *self, PyObject *args)
{
    PyObject *input = NULL, *result = NULL;
    PyObject *intresult = NULL, *isint = NULL;
    char *str;
    double dresult;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now as-is. */
    IF_ANYNUM_RETURN_AS_IS(input);

    /* Attempt conversion of the (string) input object to a float. */
    /* If unsuccessful, check to make sure input is a string. */
    /* If so, return that string.  If not, raise the error. */
    result = PYFLOAT_FROM_PYSTRING(input);
    IF_TRUE_RETURN_IF_STRING_OR_RAISE(result == NULL, input);

    /* Check if this float can be represented as an integer. */
    /* If so, return as an int object. */
    isint = PyObject_CallMethod(result, "is_integer", NULL);
    if (PyObject_IsTrue(isint)) {
        Py_DECREF(isint);
        /* If the float is over 2^53, re-read */
        /* the input string because we may have lost some precision. */
        dresult = PyFloat_AsDouble(result);
        if (dresult > maxsize) {
            CONVERT_TO_STRING_OR_RAISE(input, str);
            intresult = PYINT_FROM_STRING(str);
            /* If it could not be read as an integer, return the float. */
            IF_TRUE_RETURN_VALUE(intresult == NULL, result);
        } else {
            intresult = PYNUM_ASINT(result);
        }
        Py_DECREF(result);
        return intresult;            
    }

    /* Otherwise as a float object. */
    Py_DECREF(isint);
    return result;
}


/* Safely convert to a float, depending on value. */
static PyObject *
fastnumbers_safe_float(PyObject *self, PyObject *args)
{
    PyObject *input = NULL, *result = NULL;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_FLOAT(input);

    /* Attempt conversion of the (string) object to a float. */
    /* If unsuccessful, check to make sure input is a string. */
    /* If so, return that string.  If not, raise the error. */
    result = PYFLOAT_FROM_PYSTRING(input);
    IF_TRUE_RETURN_IF_STRING_OR_RAISE(result == NULL, input);

    /* If successful, return this float object. */
    return result;
}


/* Safely convert to an int, depending on value. */
static PyObject *
fastnumbers_safe_int(PyObject *self, PyObject *args)
{
    PyObject *input = NULL, *result = NULL;
    char *str;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt the conversion to a int. */
    /* If unsuccessful, clear error stack and return input as-is */
    result = PYINT_FROM_STRING(str);
    IF_TRUE_RETURN_INPUT_AS_IS(result == NULL, input);

    /* Otherwise, return the int object */
    return result;
}


/* Safely convert to an int (even if in a string and as a float), depending on value. */
static PyObject *
fastnumbers_safe_forceint(PyObject *self, PyObject *args)
{
    PyObject *input = NULL, *result = NULL;
    PyObject *intresult = NULL, *isint = NULL;
    char *str;
    double dresult;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now as an int. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt conversion of the (string) input object to a float. */
    /* If unsuccessful, check to make sure input is a string. */
    /* If so, return that string.  If not, raise the error. */
    result = PYFLOAT_FROM_PYSTRING(input);
    IF_TRUE_RETURN_IF_STRING_OR_RAISE(result == NULL, input);

    /* Check if this float can be represented as an integer. */
    /* If so, return as an int object, otherwise as a float object. */
    isint = PyObject_CallMethod(result, "is_integer", NULL);
    if (PyObject_IsTrue(isint)) {
        /* If the float is over 2^53, re-read */
        /* the input string because we may have lost some precision. */
        dresult = PyFloat_AsDouble(result);
        if (dresult > maxsize) {
            CONVERT_TO_STRING_OR_RAISE(input, str);
            intresult = PYINT_FROM_STRING(str);
            /* If it could not be read as an integer, just convert the float. */
            if (intresult == NULL) {
                PyErr_Clear();
                intresult = PYNUM_ASINT(result);
            }
        } else {
            intresult = PYNUM_ASINT(result);
        }
    }
    else
        intresult = PYNUM_ASINT(result);
    
    Py_DECREF(isint);
    Py_DECREF(result);
    return intresult;
}

/* Quickly convert to an int or float, depending on value. */
static PyObject *
fastnumbers_fast_real(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;
    double result;
    long intresult;
    bool error;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_AS_IS(input);

    /* Attempt to convert to char*. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a float */
    result = fast_atof(str, &error);

    /* If there was an error, return input as-is. */
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);

    /* Make the integer version of the input. */
    /* If the value is greater than 2^53, */
    /* re-read because some precision may have been lost. */
    if (result > maxsize) {
        intresult = fast_atoi(str, &error);
        if (error) intresult = 0;  /* Set to 0 on error. */
    }
    else
        intresult = (long) result;

    /* Otherwise, return the float or int object */
    if (result == intresult)
        return Py_BuildValue("l", intresult);
    else
        return Py_BuildValue("d", result);
}


/* Quickly convert to a float, depending on value. */
static PyObject *
fastnumbers_fast_float(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;
    double result;
    bool error;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_FLOAT(input);

    /* Attempt to convert to char*. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a float */
    /* If there was an error, return input as-is. */
    result = fast_atof(str, &error);
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);

    /* Otherwise, return the float object */
    return Py_BuildValue("d", result);
}


/* Quickly convert to an int, depending on value. */
static PyObject *
fastnumbers_fast_int(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;
    long result;
    bool error;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a int */
    /* Otherwise, return the int object */
    result = fast_atoi(str, &error);
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);

    /* If there was an error, return input as-is. */
    return Py_BuildValue("l", result);
}


/* Safely convert to an int (even if in a string and as a float), depending on value. */
static PyObject *
fastnumbers_fast_forceint(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;
    double result;
    long intresult;
    bool error;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a float */
    result = fast_atof(str, &error);

    /* If there was an error, return input as-is. */
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);

    /* Make the integer version of the input. */
    /* If the value is greater than 2^53, */
    /* re-read because some precision may have been lost. */
    if (result > maxsize) {
        intresult = fast_atoi(str, &error);
        if (error) intresult = 0;  /* Set to 0 on error. */
    }
    else
        intresult = (long) result;

    /* Otherwise, return the float or int object */
    if (result == intresult)
        return Py_BuildValue("l", intresult);
    else
        return Py_BuildValue("l", (long) result);
}


/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    {"safe_real", fastnumbers_safe_real, METH_VARARGS, safe_real_docstring},
    {"safe_float", fastnumbers_safe_float, METH_VARARGS, safe_float_docstring},
    {"safe_int", fastnumbers_safe_int, METH_VARARGS, safe_int_docstring},
    {"safe_forceint", fastnumbers_safe_forceint, METH_VARARGS, safe_forceint_docstring},
    {"fast_real", fastnumbers_fast_real, METH_VARARGS, fast_real_docstring},
    {"fast_float", fastnumbers_fast_float, METH_VARARGS, fast_float_docstring},
    {"fast_int", fastnumbers_fast_int, METH_VARARGS, fast_int_docstring},
    {"fast_forceint", fastnumbers_fast_forceint, METH_VARARGS, fast_forceint_docstring},
    // {"str_isreal", fastnumbers_str_isreal, METH_VARARGS, str_isreal_docstring},
    // {"str_isfloat", fastnumbers_str_isfloat, METH_VARARGS, str_isfloat_docstring},
    // {"str_isint", fastnumbers_str_isint, METH_VARARGS, str_isint_docstring},
    // {"fast_isreal", fastnumbers_fast_isreal, METH_VARARGS, fast_isreal_docstring},
    // {"fast_isfloat", fastnumbers_fast_isfloat, METH_VARARGS, fast_isfloat_docstring},
    // {"fast_isint", fastnumbers_fast_isint, METH_VARARGS, fast_isint_docstring},
    // {"fast_isintlike", fastnumbers_fast_isintlike, METH_VARARGS, fast_isintlike_docstring},}
    {NULL, NULL, 0, NULL} /* Sentinel */
};


/* We want a module-level variable that is the version. */
static PyObject *fastnumbers_version;


/* Define the module interface.  This is different for Python2 and Python3. */
#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "fastnumbers",
        module_docstring,
        -1,
        FastnumbersMethods,
        NULL,
        NULL,
        NULL,
        NULL
};
PyObject *
PyInit_fastnumbers(void)
{
    PyObject *m = PyModule_Create(&moduledef);
    if (m == NULL) return NULL;

    fastnumbers_version = PyUnicode_FromString(FASTNUMBERS_VERSION);
    Py_INCREF(fastnumbers_version);
    PyModule_AddObject(m, "__version__", fastnumbers_version);

    return m;
}
#else
PyMODINIT_FUNC
initfastnumbers(void)
{
    PyObject *m = Py_InitModule3("fastnumbers",
                                 FastnumbersMethods,
                                 module_docstring);
    if (m == NULL) return;

    fastnumbers_version = PyUnicode_FromString(FASTNUMBERS_VERSION);
    Py_INCREF(fastnumbers_version);
    PyModule_AddObject(m, "__version__", fastnumbers_version);
}
#endif
