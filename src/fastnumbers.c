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


/* Quickly determine if the input is a real. */
static PyObject *
fastnumbers_isreal(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is a number, return true now. */
    if (ANYNUM(input)) Py_RETURN_TRUE;

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return false. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be a float, return true, false otherwise. */
    if (fast_atof_test(str)) Py_RETURN_TRUE;
    else Py_RETURN_FALSE;
}


/* Quickly determine if the input is a float. */
static PyObject *
fastnumbers_isfloat(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is a float, return true now. */
    if (PyFloat_Check(input)) Py_RETURN_TRUE;
    /* If the input is an int, return false now. */
    if (PYINT_CHECK(input)) Py_RETURN_FALSE;

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return false. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be a float, return true, false otherwise. */
    if (fast_atof_test(str)) Py_RETURN_TRUE;
    else Py_RETURN_FALSE;
}


/* Quickly determine if the input is an int. */
static PyObject *
fastnumbers_isint(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is an int, return true now. */
    if (PYINT_CHECK(input)) Py_RETURN_TRUE;
    /* If the input is a float, return false now. */
    if (PyFloat_Check(input)) Py_RETURN_FALSE;

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return false. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be an int, return true, false otherwise. */
    if (fast_atoi_test(str)) Py_RETURN_TRUE;
    else Py_RETURN_FALSE;
}


/* Quickly determine if the input is int-like. */
static PyObject *
fastnumbers_isintlike(PyObject *self, PyObject *args)
{
    PyObject *input = NULL, *intlike = NULL, *pyresult = NULL;
    char *str;
    double result;
    bool error;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;

    /* If the input is an int, return true now. */
    if (PYINT_CHECK(input))
        Py_RETURN_TRUE;
    /* If the input is a float, return the result of "is_integer". */
    else if (PyFloat_Check(input))
        return PyObject_CallMethod(input, "is_integer", NULL);

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return false. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be an int, return true now. */
    if (fast_atoi_test(str)) Py_RETURN_TRUE;

    /* Try converting the string to a float, */
    /* and then running is_integer on that. */
    result = fast_atof(str, &error);

    /* If there was an error, return false now. */
    if (error) Py_RETURN_FALSE;

    /* Convert to float type and return the result if "is_integer". */
    pyresult = PyFloat_FromDouble(result);
    intlike = PyObject_CallMethod(pyresult, "is_integer", NULL);
    Py_DECREF(pyresult);
    return intlike;
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
    {"isreal", fastnumbers_isreal, METH_VARARGS, isreal_docstring},
    {"isfloat", fastnumbers_isfloat, METH_VARARGS, isfloat_docstring},
    {"isint", fastnumbers_isint, METH_VARARGS, isint_docstring},
    {"isintlike", fastnumbers_isintlike, METH_VARARGS, isintlike_docstring},
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
