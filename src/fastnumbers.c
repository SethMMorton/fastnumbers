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


/* Quickly convert to an int or float, depending on value. */
static PyObject *
fastnumbers_fast_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = Py_None;
    PyObject *isint = NULL, *pyresult = NULL;
    char *str;
    double result;
    long intresult;
    bool error = false, overflow = false, isintbool;
    static char *keywords[] = { "x", "raise_on_invalid", "default", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:fast_real", keywords,
                                     &input, &raise_on_invalid, &default_value))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_AS_IS(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* First attempt to convert to an int */
    intresult = fast_atoi(str, &error, &overflow);

    /* If successful, return this integer now. */
    if (! error) {
        /* If there was overflow, use Python's conversion function. */
        if (overflow) {
            return PYINT_FROM_STRING(str);
        }
        /* Otherwise return the integer. */
        return Py_BuildValue("l", intresult);
    }

    /* Conversion to an integer was unsuccessful. Try converting to a float. */
    /* Attempt to convert to a float */
    result = fast_atof(str, &error, &overflow);

    /* If unsuccessful, either: */
    /*   a) raise a ValueError */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to float or int: '%.200s'", str);
    }
    /*   b) return a default value */
    IF_TRUE_RETURN_INPUT_AS_IS(error && default_value != Py_None,
                               default_value);
    /*   c) return input as-is */
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);

    /* Determine if this float can be represented as an integer. */
    /* If there was an overflow error, */
    /* use Python's float function to read string. */
    if (overflow) {
        /* We already know this string parses as a float, */
        /* so no error checking is needed. */
        pyresult = PYFLOAT_FROM_PYSTRING(input);
        isint = PyObject_CallMethod(pyresult, "is_integer", NULL);
        isintbool = PyObject_IsTrue(isint);
        Py_DECREF(isint);
    } else {
        /* Convert from double form to PyFloat. */
        /* Use fast method to determine int-ness of */
        /* float if float is not too large. */
        pyresult = PyFloat_FromDouble(result);
        if (result > maxsize) {
            isint = PyObject_CallMethod(pyresult, "is_integer", NULL);
            isintbool = PyObject_IsTrue(isint);
            Py_DECREF(isint);
        } else {
            isintbool = result == (long) result;
        }
    }

    /* Return as int or float dependent on the int-ness. */
    if (isintbool) {
        return PYNUM_ASINT(pyresult);
    } else {
        return pyresult;
    }

}


/* Quickly convert to a float, depending on value. */
static PyObject *
fastnumbers_fast_float(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = Py_None;
    char *str;
    double result;
    bool error = false, overflow = false;
    static char *keywords[] = { "x", "raise_on_invalid", "default", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:fast_float", keywords,
                                     &input, &raise_on_invalid, &default_value))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_FLOAT(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a float */
    result = fast_atof(str, &error, &overflow);

    /* If unsuccessful, either: */
    /*   a) raise a ValueError */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to float: '%.200s'", str);
    }
    /*   b) return a default value */
    IF_TRUE_RETURN_INPUT_AS_IS(error && default_value != Py_None,
                               default_value);
    /*   c) return input as-is */
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);

    /* If there was an overflow error, use Python's float function. */
    /* We already know this string parses as a float, */
    /* so no error checking is needed and we can return directly. */
    if (overflow) {
        return PYFLOAT_FROM_PYSTRING(input);
    }

    /* Otherwise, return the float result. */
    else {
        return Py_BuildValue("d", result);
    }
}


/* Quickly convert to an int, depending on value. */
static PyObject *
fastnumbers_fast_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = Py_None;
    char *str;
    long result;
    bool error = false, overflow = false;
    static char *keywords[] = { "x", "raise_on_invalid", "default", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:fast_int", keywords,
                                     &input, &raise_on_invalid, &default_value))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* Attempt to convert to a int */
    result = fast_atoi(str, &error, &overflow);

    /* If unsuccessful, either: */
    /*   a) raise a ValueError */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to int: '%.200s'", str);
    }
    /*   b) return a default value */
    IF_TRUE_RETURN_INPUT_AS_IS(error && default_value != Py_None,
                               default_value);
    /*   c) return input as-is */
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);

    /* If there was an overflow error, use Python's integer function. */
    /* We already know this string parses as an integer, */
    /* so no error checking is needed and we can return directly. */
    if (overflow) {
        return PYINT_FROM_STRING(str);
    }

    /* Otherwise, return the integer result. */
    else {
        return Py_BuildValue("l", result);
    }
}


/* Safely convert to an int (even if in a string and as a float), depending on value. */
static PyObject *
fastnumbers_fast_forceint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = Py_None;
    PyObject *pytemp = NULL, *pyreturn = NULL;
    char *str;
    double result;
    long intresult;
    bool error = false, overflow = false;
    static char *keywords[] = { "x", "raise_on_invalid", "default", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:fast_forceint", keywords,
                                     &input, &raise_on_invalid, &default_value))
        return NULL;

    /* If the input is already a number, return now. */
    IF_ANYNUM_RETURN_INT(input);

    /* Attempt to convert to char*. Raise a TypeError if not. */
    CONVERT_TO_STRING_OR_RAISE(input, str);

    /* First attempt to convert to an int */
    intresult = fast_atoi(str, &error, &overflow);

    /* If successful, return this integer now. */
    if (! error) {
        /* If there was overflow, use Python's conversion function. */
        if (overflow) {
            return PYINT_FROM_STRING(str);
        }
        /* Otherwise return the integer. */
        return Py_BuildValue("l", intresult);
    }

    /* Attempt to convert to a float */
    result = fast_atof(str, &error, &overflow);

    /* Call NaN and infinity errors. */
    error = error || Py_IS_INFINITY(result) || Py_IS_NAN(result);

    /* If unsuccessful, either: */
    /*   a) raise a ValueError */
    if (PyObject_IsTrue(raise_on_invalid)) {
        IF_TRUE_RAISE_ERR_STR(Py_IS_INFINITY(result), PyExc_OverflowError,
            "could not convert infinity to int");
        IF_TRUE_RAISE_ERR_STR(Py_IS_NAN(result), PyExc_ValueError,
            "could not convert NaN to int");
        IF_TRUE_RAISE_ERR_FMT(error, PyExc_ValueError,
            "could not convert string to int: '%.200s'", str);
    }
    /*   b) return a default value */
    IF_TRUE_RETURN_INPUT_AS_IS(error && default_value != Py_None,
                               default_value);
    /*   c) return input as-is */
    IF_TRUE_RETURN_INPUT_AS_IS(error, input);


    /* If there was an overflow error, use Python's float function. */
    /* We already know this string parses as a float, */
    /* so no error checking is needed and we can return directly. */
    if (overflow) {
        pytemp = PYFLOAT_FROM_PYSTRING(input);
        pyreturn = PYNUM_ASINT(pytemp);
        Py_DECREF(pytemp);
        return pyreturn;
    }

    /* Otherwise, return the float as an int using Python's conversion. */
    else {
        pytemp = PyFloat_FromDouble(result);
        pyreturn = PYNUM_ASINT(pytemp);
        Py_DECREF(pytemp);
        return pyreturn;
    }
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
    bool error, overflow;
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
    result = fast_atof(str, &error, &overflow);

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
    {"safe_real", (PyCFunction) fastnumbers_fast_real, METH_VARARGS | METH_KEYWORDS, safe_real_docstring},
    {"safe_float", (PyCFunction) fastnumbers_fast_float, METH_VARARGS | METH_KEYWORDS, safe_float_docstring},
    {"safe_int", (PyCFunction) fastnumbers_fast_int, METH_VARARGS | METH_KEYWORDS, safe_int_docstring},
    {"safe_forceint", (PyCFunction) fastnumbers_fast_forceint, METH_VARARGS | METH_KEYWORDS, safe_forceint_docstring},
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
