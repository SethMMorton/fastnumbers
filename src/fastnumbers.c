/*
 * C implementation of the python library fastnumbers.
 *
 * Author: Seth M. Morton, July 30, 2014
 */

#include <Python.h>
#include <limits.h>
#include "version.h"
#include "docstrings.h"
#include "options.h"
#include "object_handling.h"
#include "number_handling.h"
#include "quick_detection.h"


/* Function to handle the conversion of base to integers.
 * 0 is success, 1 is failure.
 */
int
assess_integer_base_input(const int base)
{
    /* Default already is OK.
     */
    if (base == INT_MIN) return 0;

    /* Ensure valid integer in valid range.
     */
    if ((base != 0 && base < 2) || base > 36) {
        PyErr_SetString(PyExc_ValueError,
                        "int() base must be >= 2 and <= 36");
        return 1;
    }
    return 0;
}


/* Quickly convert to an int or float, depending on value. */
static PyObject *
fastnumbers_fast_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    struct Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "key", "inf", "nan", "coerce", NULL };
    static const char *format = "O|OOOOOO:fast_real";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.key, &opts.handle_inf, &opts.handle_nan,
                                     &opts.coerce))
        return NULL;
    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);

    return PyObject_to_PyNumber(input, REAL, &opts);
}


/* Quickly convert to a float, depending on value. */
static PyObject *
fastnumbers_fast_float(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    struct Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "key", "inf", "nan", NULL };
    static const char *format = "O|OOOOO:fast_float";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.key, &opts.handle_inf, &opts.handle_nan))
        return NULL;

    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);

    return PyObject_to_PyNumber(input, FLOAT, &opts);
}


/* Quickly convert to an int, depending on value. */
static PyObject *
fastnumbers_fast_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    struct Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "key", "base", NULL };
    static const char *format = "O|OOOi:fast_int";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.key, &opts.base))
        return NULL;
    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);
    if (assess_integer_base_input(opts.base)) return NULL;

    return PyObject_to_PyNumber(input, INT, &opts);
}


/* Safely convert to an int (even if in a string and as a float). */
static PyObject *
fastnumbers_fast_forceint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    struct Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "key", NULL };
    static const char *format = "O|OOO:fast_forceint";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.key))
        return NULL;
    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);

    return PyObject_to_PyNumber(input, FORCEINT, &opts);
}


/* Quickly determine if the input is a real. */
static PyObject *
fastnumbers_isreal(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *num_only = Py_False;
    PyObject *allow_inf = Py_False;
    PyObject *allow_nan = Py_False;
    static char *keywords[] = { "x", "str_only", "num_only",
                                "allow_inf", "allow_nan", NULL };
    static const char *format = "O|OOOO:isreal";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &str_only, &num_only,
                                     &allow_inf, &allow_nan))
        return NULL;

    return PyObject_is_number(input, REAL,
                              allow_inf, allow_nan,
                              str_only, num_only, INT_MIN);
}


/* Quickly determine if the input is a float. */
static PyObject *
fastnumbers_isfloat(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *num_only = Py_False;
    PyObject *allow_inf = Py_False;
    PyObject *allow_nan = Py_False;
    static char *keywords[] = { "x", "str_only", "num_only",
                                "allow_inf", "allow_nan", NULL };
    static const char *format = "O|OOOO:isfloat";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &str_only, &num_only,
                                     &allow_inf, &allow_nan))
        return NULL;

    return PyObject_is_number(input, FLOAT,
                              allow_inf, allow_nan,
                              str_only, num_only, INT_MIN);
}


/* Quickly determine if the input is an int. */
static PyObject *
fastnumbers_isint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *num_only = Py_False;
    int base = INT_MIN;
    static char *keywords[] = { "x", "str_only", "num_only", "base", NULL };
    static const char *format = "O|OOi:isint";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &str_only, &num_only, &base))
        return NULL;

    /* Ensure the base is in a valid range. */
    if (base != INT_MIN && (base == 1 || base > 36 || base < 0)) {
        PyErr_SetString(PyExc_ValueError,
                        "ValueError: int() base must be >= 2 and <= 36");
        return NULL;
    }
    return PyObject_is_number(input, INT, NULL, NULL, str_only, num_only, base);
}


/* Quickly determine if the input is int-like. */
static PyObject *
fastnumbers_isintlike(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *num_only = Py_False;
    static char *keywords[] = { "x", "str_only", "num_only", NULL };
    static const char *format = "O|OO:isintlike";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &str_only, &num_only))
        return NULL;

    return PyObject_is_number(input, INTLIKE, NULL, NULL,
                              str_only, num_only, INT_MIN);
}


/* Drop-in replacement for int, float */
static PyObject *
fastnumbers_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    struct Options opts = init_Options_convert;
    static char *keywords[] = { "x", "base", NULL };
    static const char *format = "|Oi:int";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.base))
        return NULL;
    if (assess_integer_base_input(opts.base)) return NULL;
    /* No arguments returns 0. */
    if (input == NULL) {
        if (!Options_Default_Base(&opts)) {
            PyErr_SetString(PyExc_TypeError, "int() missing string argument");
            return NULL;
        }
        return long_to_PyInt(0);
    }
    Options_Set_Return_Value(opts, input, NULL, Py_True);
    Options_Set_Disallow_UnicodeCharacter(&opts);
    return PyObject_to_PyNumber(input, INT, &opts);
}


static PyObject *
fastnumbers_float(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    struct Options opts = init_Options_convert;
    static char *keywords[] = { "x", NULL };
    static const char *format = "|O:float";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords, &input))
        return NULL;
    /* No arguments returns 0.0. */
    if (input == NULL) return PyFloat_FromDouble(0.0);
    Options_Set_Return_Value(opts, input, NULL, Py_True);
    Options_Set_Disallow_UnicodeCharacter(&opts);

    return PyObject_to_PyNumber(input, FLOAT, &opts);
}


/* Behaves like float or int, but returns correct type. */
static PyObject *
fastnumbers_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    struct Options opts = init_Options_convert;
    static char *keywords[] = { "x", "coerce", NULL };
    static const char *format = "|OO:real";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.coerce))
        return NULL;
    /* No arguments returns 0.0 or 0 depending on the state of coerce. */
    if (input == NULL) return Options_Coerce_True(&opts) ? long_to_PyInt(0)
                                                         : PyFloat_FromDouble(0.0);
    Options_Set_Return_Value(opts, input, NULL, Py_True);
    Options_Set_Disallow_UnicodeCharacter(&opts);

    return PyObject_to_PyNumber(input, REAL, &opts);
}


/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    { "fast_real",     (PyCFunction) fastnumbers_fast_real,
                       METH_VARARGS | METH_KEYWORDS, fast_real__doc__ },
    { "fast_float",    (PyCFunction) fastnumbers_fast_float,
                       METH_VARARGS | METH_KEYWORDS, fast_float__doc__ },
    { "fast_int",      (PyCFunction) fastnumbers_fast_int,
                       METH_VARARGS | METH_KEYWORDS, fast_int__doc__ },
    { "fast_forceint", (PyCFunction) fastnumbers_fast_forceint,
                       METH_VARARGS | METH_KEYWORDS, fast_forceint__doc__ },
    { "isreal",        (PyCFunction) fastnumbers_isreal,
                       METH_VARARGS | METH_KEYWORDS, isreal__doc__ },
    { "isfloat",       (PyCFunction) fastnumbers_isfloat,
                       METH_VARARGS | METH_KEYWORDS, isfloat__doc__ },
    { "isint",         (PyCFunction) fastnumbers_isint,
                       METH_VARARGS | METH_KEYWORDS, isint__doc__ },
    { "isintlike",     (PyCFunction) fastnumbers_isintlike,
                       METH_VARARGS | METH_KEYWORDS, isintlike__doc__ },
    { "int",           (PyCFunction) fastnumbers_int,
                       METH_VARARGS | METH_KEYWORDS, "" },
    { "float",         (PyCFunction) fastnumbers_float,
                       METH_VARARGS | METH_KEYWORDS, "" },
    { "real",          (PyCFunction) fastnumbers_real,
                       METH_VARARGS | METH_KEYWORDS, "" },
    {NULL, NULL, 0, NULL} /* Sentinel */
};


/* We want a module-level variable that is the version. */
static PyObject *fastnumbers__version__;

/* Some constants that may be useful for debugging. */
static PyObject *fastnumbers_FN_MAX_INT_LEN;
static PyObject *fastnumbers_FN_DBL_DIG;
static PyObject *fastnumbers_FN_MAX_EXP;
static PyObject *fastnumbers_FN_MIN_EXP;

/* Define the module interface. This is different for Python2 and Python3. */
#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "fastnumbers",
        fastnumbers__doc__,
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

#else

PyMODINIT_FUNC
initfastnumbers(void)
{
    PyObject *m = Py_InitModule3("fastnumbers",
                                 FastnumbersMethods,
                                 fastnumbers__doc__);
    if (m == NULL) return;

#endif

    /* Add module level constants. */
    fastnumbers__version__ = PyUnicode_FromString(FASTNUMBERS_VERSION);
    fastnumbers_FN_MAX_INT_LEN = long_to_PyInt(FN_MAX_INT_LEN);
    fastnumbers_FN_DBL_DIG = long_to_PyInt(FN_DBL_DIG);
    fastnumbers_FN_MAX_EXP = long_to_PyInt(FN_MAX_EXP);
    fastnumbers_FN_MIN_EXP = long_to_PyInt(FN_MIN_EXP);
    Py_INCREF(fastnumbers__version__);
    Py_INCREF(fastnumbers_FN_MAX_INT_LEN);
    Py_INCREF(fastnumbers_FN_DBL_DIG);
    Py_INCREF(fastnumbers_FN_MAX_EXP);
    Py_INCREF(fastnumbers_FN_MIN_EXP);
    PyModule_AddObject(m, "__version__", fastnumbers__version__);
    PyModule_AddObject(m, "max_int_len", fastnumbers_FN_MAX_INT_LEN);
    PyModule_AddObject(m, "dig", fastnumbers_FN_DBL_DIG);
    PyModule_AddObject(m, "max_exp", fastnumbers_FN_MAX_EXP);
    PyModule_AddObject(m, "min_exp", fastnumbers_FN_MIN_EXP);

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}

