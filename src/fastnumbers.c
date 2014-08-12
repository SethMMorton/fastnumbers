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
fastnumbers_safe_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL, *result = NULL;
    PyObject *intresult = NULL, *isint = NULL;
    PyObject *raise_on_invalid = Py_False;
    char *str;
    double dresult;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:safe_real", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now as-is. */
    IF_ANYNUM_RETURN_AS_IS(input);

    /* Attempt conversion of the (string) input object to a float. */
    result = PYFLOAT_FROM_PYSTRING(input);

    /* If unsuccessful, raise the ValueError if the user wants that. */
    /* Otherwise, check to make sure input is a string. */
    /* If so, clear error stack and return that string.  */
    /* If not, raise the TypeError. */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE(result == NULL);
    } else {
        IF_TRUE_RETURN_IF_STRING_OR_RAISE(result == NULL, input);
    }
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
fastnumbers_safe_float(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL, *result = NULL;
    PyObject *raise_on_invalid = Py_False;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:safe_float", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_FLOAT(input);

    /* Attempt conversion of the (string) object to a float. */
    result = PYFLOAT_FROM_PYSTRING(input);

    /* If unsuccessful, raise the ValueError if the user wants that. */
    /* Otherwise, check to make sure input is a string. */
    /* If so, clear error stack and return that string.  */
    /* If not, raise the TypeError. */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE(result == NULL);
    } else {
        IF_TRUE_RETURN_IF_STRING_OR_RAISE(result == NULL, input);
    }

    /* If successful, return this float object. */
    return result;
}


/* Safely convert to an int, depending on value. */
static PyObject *
fastnumbers_safe_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL, *result = NULL;
    PyObject *raise_on_invalid = Py_False;
    char *str;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:safe_int", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*, or raise a TypeError. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt the conversion to a int. */
    result = PYINT_FROM_STRING(str);

    /* If unsuccessful, clear error stack and return input as-is */
    /* Otherwise, clear error stack and return the input string.  */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE(result == NULL);
    } else {
        IF_TRUE_RETURN_INPUT_AS_IS(result == NULL, input);
    }

    /* Otherwise, return the int object */
    return result;
}


/* Safely convert to an int (even if in a string and as a float), depending on value. */
static PyObject *
fastnumbers_safe_forceint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL, *result = NULL;
    PyObject *intresult = NULL, *isint = NULL;
    PyObject *raise_on_invalid = Py_False;
    char *str;
    double dresult;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:safe_forceint", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now as an int. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt conversion of the (string) object to a float. */
    result = PYFLOAT_FROM_PYSTRING(input);

    /* If unsuccessful, raise the ValueError if the user wants that. */
    /* Otherwise, check to make sure input is a string. */
    /* If so, clear error stack and return that string.  */
    /* If not, raise the TypeError. */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE(result == NULL);
    } else {
        IF_TRUE_RETURN_IF_STRING_OR_RAISE(result == NULL, input);
    }

    /* Check if this float can be represented as an integer. */
    /* If so, return as an int object making sure we don't lose accuracy. */
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
    /* Otherwise it as a float object, convert directly to int (truncating). */
    /* Be careful about infinity. For infinity return sys.maxsize. */ 
    else {
        dresult = PyFloat_AsDouble(result);
        if (Py_IS_INFINITY(dresult)) {
            intresult = dresult > 0 ? PYNUM_ASINT_FROM_SIZET(PY_SSIZE_T_MAX)
                                    : PYNUM_ASINT_FROM_SIZET(-PY_SSIZE_T_MAX-1);
            Py_INCREF(intresult);
        }
        else
            intresult = PYNUM_ASINT(result);
    }
    
    Py_DECREF(isint);
    Py_DECREF(result);
    return intresult;
}

/* Quickly convert to an int or float, depending on value. */
static PyObject *
fastnumbers_fast_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    char *str;
    double result;
    long intresult;
    bool error;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:fast_real", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_AS_IS(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a float */
    result = fast_atof(str, &error);

    /* If unsuccessful, raise the ValueError if the user wants that. */
    /* Otherwise, return input as-is. */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to float: '%.200s'", str);
    } else {
        IF_TRUE_RETURN_INPUT_AS_IS(error, input);
    }

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
fastnumbers_fast_float(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    char *str;
    double result;
    bool error;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:fast_float", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_FLOAT(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a float */
    result = fast_atof(str, &error);

    /* If unsuccessful, raise the ValueError if the user wants that. */
    /* Otherwise, return input as-is. */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to float: '%.200s'", str);
    } else {
        IF_TRUE_RETURN_INPUT_AS_IS(error, input);
    }

    /* Otherwise, return the float object */
    return Py_BuildValue("d", result);
}


/* Quickly convert to an int, depending on value. */
static PyObject *
fastnumbers_fast_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    char *str;
    long result;
    bool error;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:fast_int", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a int */
    result = fast_atoi(str, &error);

    /* If unsuccessful, raise the ValueError if the user wants that. */
    /* Otherwise, return input as-is. */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to int: '%.200s'", str);
    } else {
        IF_TRUE_RETURN_INPUT_AS_IS(error, input);
    }

    /* If there was an error, return input as-is. */
    return Py_BuildValue("l", result);
}


/* Safely convert to an int (even if in a string and as a float), depending on value. */
static PyObject *
fastnumbers_fast_forceint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    char *str;
    double result;
    long intresult;
    bool error;
    static char *keywords[] = { "x", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:fast_forceint", keywords,
                                     &input, &raise_on_invalid))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a float */
    result = fast_atof(str, &error);

    /* Let's call NaN an error. */
    error = Py_IS_NAN(result) || error;

    /* If unsuccessful, raise the ValueError if the user wants that. */
    /* Otherwise, return input as-is.*/
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to float: '%.200s'", str);
    } else {
        IF_TRUE_RETURN_INPUT_AS_IS(error, input);
    }

    /* Make the integer version of the input. */
    /* If the input is infinity, return sys.maxsize. */
    if (Py_IS_INFINITY(result)) {
        intresult = result > 0 ? PY_SSIZE_T_MAX : -PY_SSIZE_T_MAX-1;
        result = intresult;  /* To make sure we return this as-is. */
    }
    /* If the value is greater than 2^53, */
    /* re-read because some precision may have been lost. */
    else if (result > maxsize) {
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
fastnumbers_isreal(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *allow_inf = Py_False;
    PyObject *allow_nan = Py_False;
    char *str;
    static char *keywords[] = { "x", "str_only", "allow_inf", "allow_nan", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOO:isreal", keywords,
                                     &input, &str_only, &allow_inf, &allow_nan))
        return NULL;

    /* If the input is a number, return True now unless */
    /* str_only is True, then return False. */
    if (ANYNUM(input)) {
        if (PyObject_IsTrue(str_only)) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    }

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return False. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be a float, return True, False otherwise. */
    if (fast_atof_test(str, PyObject_IsTrue(allow_inf), PyObject_IsTrue(allow_nan)))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}


/* Quickly determine if the input is a float. */
static PyObject *
fastnumbers_isfloat(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *allow_inf = Py_False;
    PyObject *allow_nan = Py_False;
    char *str;
    static char *keywords[] = { "x", "str_only", "allow_inf", "allow_nan", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOO:isfloat", keywords,
                                     &input, &str_only, &allow_inf, &allow_nan))
        return NULL;

    /* If str_only is True and this is a number, return False now. */
    if (PyObject_IsTrue(str_only) && ANYNUM(input)) Py_RETURN_FALSE;
    /* If the input is a float, return True now. */
    if (PyFloat_Check(input)) Py_RETURN_TRUE;
    /* If the input is an int, return False now. */
    if (PYINT_CHECK(input)) Py_RETURN_FALSE;

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return False. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be a float, return True, False otherwise. */
    if (fast_atof_test(str, PyObject_IsTrue(allow_inf), PyObject_IsTrue(allow_nan)))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}


/* Quickly determine if the input is an int. */
static PyObject *
fastnumbers_isint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    char *str;
    static char *keywords[] = { "x", "str_only", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:isint", keywords,
                                     &input, &str_only))
        return NULL;

    /* If str_only is True and this is a number, return False now. */
    if (PyObject_IsTrue(str_only) && ANYNUM(input)) Py_RETURN_FALSE;
    /* If the input is an int, return True now. */
    if (PYINT_CHECK(input)) Py_RETURN_TRUE;
    /* If the input is a float, return False now. */
    if (PyFloat_Check(input)) Py_RETURN_FALSE;

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return False. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be an int, return True, False otherwise. */
    if (fast_atoi_test(str)) Py_RETURN_TRUE;
    else Py_RETURN_FALSE;
}


/* Quickly determine if the input is int-like. */
static PyObject *
fastnumbers_isintlike(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL, *intlike = NULL, *pyresult = NULL;
    PyObject *str_only = Py_False;
    char *str;
    double result;
    bool error;
    static char *keywords[] = { "x", "str_only", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:isintlike", keywords,
                                     &input, &str_only))
        return NULL;

    /* If str_only is True and this is a number, return False now. */
    if (PyObject_IsTrue(str_only) && ANYNUM(input)) Py_RETURN_FALSE;
    /* If the input is an int, return True now. */
    if (PYINT_CHECK(input)) Py_RETURN_TRUE;
    /* If the input is a float, return the result of "is_integer". */
    else if (PyFloat_Check(input)) return PyObject_CallMethod(input, "is_integer", NULL);

    /* Attempt to convert to char*. */
    str = convert_string(input);

    /* If it cannot be converted to a string, return False. */
    if (str == NULL) { PyErr_Clear(); Py_RETURN_FALSE; }

    /* If the string can be an int, return True now. */
    if (fast_atoi_test(str)) Py_RETURN_TRUE;

    /* Try converting the string to a float, */
    /* and then running is_integer on that. */
    result = fast_atof(str, &error);

    /* If there was an error, return False now. */
    if (error) Py_RETURN_FALSE;

    /* Convert to float type and return the result of "is_integer". */
    pyresult = PyFloat_FromDouble(result);
    intlike = PyObject_CallMethod(pyresult, "is_integer", NULL);
    Py_DECREF(pyresult);
    return intlike;
}


/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    {"safe_real", (PyCFunction) fastnumbers_safe_real, METH_VARARGS | METH_KEYWORDS, safe_real_docstring},
    {"safe_float", (PyCFunction) fastnumbers_safe_float, METH_VARARGS | METH_KEYWORDS, safe_float_docstring},
    {"safe_int", (PyCFunction) fastnumbers_safe_int, METH_VARARGS | METH_KEYWORDS, safe_int_docstring},
    {"safe_forceint", (PyCFunction) fastnumbers_safe_forceint, METH_VARARGS | METH_KEYWORDS, safe_forceint_docstring},
    {"fast_real", (PyCFunction) fastnumbers_fast_real, METH_VARARGS | METH_KEYWORDS, fast_real_docstring},
    {"fast_float", (PyCFunction) fastnumbers_fast_float, METH_VARARGS | METH_KEYWORDS, fast_float_docstring},
    {"fast_int", (PyCFunction) fastnumbers_fast_int, METH_VARARGS | METH_KEYWORDS, fast_int_docstring},
    {"fast_forceint", (PyCFunction) fastnumbers_fast_forceint, METH_VARARGS | METH_KEYWORDS, fast_forceint_docstring},
    {"isreal", (PyCFunction) fastnumbers_isreal, METH_VARARGS | METH_KEYWORDS, isreal_docstring},
    {"isfloat", (PyCFunction) fastnumbers_isfloat, METH_VARARGS | METH_KEYWORDS, isfloat_docstring},
    {"isint", (PyCFunction) fastnumbers_isint, METH_VARARGS | METH_KEYWORDS, isint_docstring},
    {"isintlike", (PyCFunction) fastnumbers_isintlike, METH_VARARGS | METH_KEYWORDS, isintlike_docstring},
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
