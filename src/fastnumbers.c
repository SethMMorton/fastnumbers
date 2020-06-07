/*
 * C implementation of the python library fastnumbers.
 *
 * Author: Seth M. Morton, July 30, 2014
 */

#include <Python.h>
#include <limits.h>
#include "fastnumbers/version.h"
#include "fastnumbers/docstrings.h"
#include "fastnumbers/options.h"
#include "fastnumbers/objects.h"
#include "fastnumbers/numbers.h"
#include "fastnumbers/parsing.h"


/* Function to handle the conversion of base to integers.
 * 0 is success, 1 is failure.
 */
int
assess_integer_base_input(PyObject *pybase, int *base)
{
    Py_ssize_t longbase = 0;

    /* Default to INT_MIN.
     */
    if (pybase == NULL) {
        *base = INT_MIN;
        return 0;
    }

    /* Convert to int and check for overflow.
     */
    longbase = PyNumber_AsSsize_t(pybase, NULL);
    if (longbase == -1 && PyErr_Occurred()) {
        return 1;
    }

    /* Ensure valid integer in valid range.
     */
    if ((longbase != 0 && longbase < 2) || longbase > 36) {
        PyErr_SetString(PyExc_ValueError,
                        "int() base must be >= 2 and <= 36");
        return 1;
    }
    *base = (int) longbase;
    return 0;
}


/* If key is defined, move the value to on_fail.
 * If both were defined, error.
 */
int
handle_key_backwards_compatibility(PyObject **on_fail, PyObject **key)
{
    if (*key != NULL) {
        if (*on_fail != NULL) {
            PyErr_SetString(PyExc_ValueError, "Cannot set both on_fail and key");
            return 1;
        }
        *on_fail = *key;
        *key = NULL;
    }
    return 0;
}


/* Quickly convert to an int or float, depending on value. */
static PyObject *
fastnumbers_fast_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *default_value = NULL;
    PyObject *key = NULL;
    int raise_on_invalid =
        false;  /* cannot use bool with PyArg_ParseTupleAndKeywords */
    Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "on_fail", "inf", "nan", "coerce",
                                "allow_underscores", "key", NULL
                              };
    static const char *format = "O|O$pOOOppO:fast_real";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.on_fail, &opts.handle_inf, &opts.handle_nan,
                                     &opts.coerce, &opts.allow_underscores, &key)) {
        return NULL;
    }
    if (handle_key_backwards_compatibility(&opts.on_fail, &key)) {
        return NULL;
    }
    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);

    return PyObject_to_PyNumber(input, REAL, &opts);
}


/* Quickly convert to a float, depending on value. */
static PyObject *
fastnumbers_fast_float(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *default_value = NULL;
    PyObject *key = NULL;
    int raise_on_invalid =
        false;  /* cannot use bool with PyArg_ParseTupleAndKeywords */
    Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "on_fail", "inf", "nan",
                                "allow_underscores", "key", NULL
                              };
    static const char *format = "O|O$pOOOpO:fast_float";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.on_fail, &opts.handle_inf, &opts.handle_nan,
                                     &opts.allow_underscores, &key)) {
        return NULL;
    }
    if (handle_key_backwards_compatibility(&opts.on_fail, &key)) {
        return NULL;
    }
    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);

    return PyObject_to_PyNumber(input, FLOAT, &opts);
}


/* Quickly convert to an int, depending on value. */
static PyObject *
fastnumbers_fast_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *default_value = NULL;
    PyObject *base = NULL;
    PyObject *key = NULL;
    int raise_on_invalid =
        false;  /* cannot use bool with PyArg_ParseTupleAndKeywords */
    Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "on_fail", "base",
                                "allow_underscores", "key", NULL

                              };
    static const char *format = "O|O$pOOpO:fast_int";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.on_fail, &base,
                                     &opts.allow_underscores, &key)) {
        return NULL;
    }
    if (handle_key_backwards_compatibility(&opts.on_fail, &key)) {
        return NULL;
    }
    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);
    if (assess_integer_base_input(base, &opts.base)) {
        return NULL;
    }

    return PyObject_to_PyNumber(input, INT, &opts);
}


/* Safely convert to an int (even if in a string and as a float). */
static PyObject *
fastnumbers_fast_forceint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *default_value = NULL;
    PyObject *key = NULL;
    int raise_on_invalid =
        false;  /* cannot use bool with PyArg_ParseTupleAndKeywords */
    Options opts = init_Options_convert;
    static char *keywords[] = { "x", "default", "raise_on_invalid",
                                "on_fail", "allow_underscores", "key", NULL
                              };
    static const char *format = "O|O$pOpO:fast_forceint";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &opts.on_fail, &opts.allow_underscores, &key)) {
        return NULL;
    }
    if (handle_key_backwards_compatibility(&opts.on_fail, &key)) {
        return NULL;
    }
    Options_Set_Return_Value(opts, input, default_value, raise_on_invalid);

    return PyObject_to_PyNumber(input, FORCEINT, &opts);
}


/* Quickly determine if the input is a real. */
static PyObject *
fastnumbers_isreal(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    Options opts = init_Options_check;
    static char *keywords[] = { "x", "str_only", "num_only",
                                "allow_inf", "allow_nan",
                                "allow_underscores", NULL
                              };
    static const char *format = "O|$ppOOp:isreal";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.str_only, &opts.num_only,
                                     &opts.handle_inf, &opts.handle_nan,
                                     &opts.allow_underscores)) {
        return NULL;
    }

    return PyObject_is_number(input, REAL, &opts);
}


/* Quickly determine if the input is a float. */
static PyObject *
fastnumbers_isfloat(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    Options opts = init_Options_check;
    static char *keywords[] = { "x", "str_only", "num_only",
                                "allow_inf", "allow_nan",
                                "allow_underscores", NULL
                              };
    static const char *format = "O|$ppOOp:isfloat";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.str_only, &opts.num_only,
                                     &opts.handle_inf, &opts.handle_nan,
                                     &opts.allow_underscores)) {
        return NULL;
    }

    return PyObject_is_number(input, FLOAT, &opts);
}


/* Quickly determine if the input is an int. */
static PyObject *
fastnumbers_isint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *base = NULL;
    Options opts = init_Options_check;
    static char *keywords[] = { "x", "str_only", "num_only", "base",
                                "allow_underscores", NULL
                              };
    static const char *format = "O|$ppOp:isint";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.str_only, &opts.num_only,
                                     &base, &opts.allow_underscores)) {
        return NULL;
    }
    if (assess_integer_base_input(base, &opts.base)) {
        return NULL;
    }

    return PyObject_is_number(input, INT, &opts);
}


/* Quickly determine if the input is int-like. */
static PyObject *
fastnumbers_isintlike(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    Options opts = init_Options_check;
    static char *keywords[] = { "x", "str_only", "num_only",
                                "allow_underscores", NULL
                              };
    static const char *format = "O|$ppp:isintlike";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.str_only, &opts.num_only,
                                     &opts.allow_underscores)) {
        return NULL;
    }

    return PyObject_is_number(input, INTLIKE, &opts);
}


/* Quickly detect the type. */
static PyObject *
fastnumbers_query_type(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *allowed_types = NULL;
    PyObject *result = NULL;
    Options opts = init_Options_check;
    static char *keywords[] = { "x", "allow_inf", "allow_nan", "coerce",
                                "allowed_types", "allow_underscores", NULL
                              };
    static const char *format = "O|$OOOOp:type";

    /* Coerce is false by default here. */
    opts.coerce = false;

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.handle_inf, &opts.handle_nan,
                                     &opts.coerce, &allowed_types,
                                     &opts.allow_underscores)) {
        return NULL;
    }
    // Allowed types must be a non-empty sequence.
    if (allowed_types != NULL) {
        if (!PySequence_Check(allowed_types)) {
            PyErr_Format(
                PyExc_TypeError,
                "allowed_type is not a sequence type: %R",
                allowed_types
            );
            return NULL;
        }
        if (PySequence_Length(allowed_types) < 1) {
            PyErr_SetString(
                PyExc_ValueError,
                "allowed_type must not be an empty sequence"
            );
            return NULL;
        }
    }

    result = PyObject_contains_type(input, &opts);

    /* If the result is not one of the allowed types, return None instead */
    if (result != NULL && allowed_types != NULL &&
            !PySequence_Contains(allowed_types, result))
    {
        Py_DECREF(result);
        Py_RETURN_NONE;
    }
    else {
        return result;
    }
}


/* Drop-in replacement for int, float */
static PyObject *
fastnumbers_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *base = NULL;
    Options opts = init_Options_convert;
    static const char *format = "|OO:int";

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
    /* Do not accept number as a keyword argument. */
    static char *keywords[] = { "", "base", NULL };
#else
    /* Do accept number as a keyword argument. */
    static char *keywords[] = { "x", "base", NULL };
#endif

    /* Read the function argument */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &base)) {
        return NULL;
    }
    if (assess_integer_base_input(base, &opts.base)) {
        return NULL;
    }
    /* No arguments returns 0. */
    if (input == NULL) {
        if (!Options_Default_Base(&opts)) {
            PyErr_SetString(PyExc_TypeError, "int() missing string argument");
            return NULL;
        }
        return PyLong_FromLong(0);
    }
    Options_Set_Return_Value(opts, input, NULL, true);
    Options_Set_Disallow_UnicodeCharacter(&opts);

    return PyObject_to_PyNumber(input, INT, &opts);
}


static PyObject *
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
fastnumbers_float(PyObject *self, PyObject *args)
#else
fastnumbers_float(PyObject *self, PyObject *args, PyObject *kwargs)
#endif
{
    PyObject *input = NULL;
    Options opts = init_Options_convert;
    static const char *format = "|O:float";

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
    /* Read the function argument - do not accept it as a keyword argument. */
    if (!PyArg_ParseTuple(args, format, &input)) {
        return NULL;
    }
#else
    static char *keywords[] = { "x", NULL };

    /* Read the function argument - accept it as a keyword argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords, &input)) {
        return NULL;
    }
#endif

    /* No arguments returns 0.0. */
    if (input == NULL) {
        return PyFloat_FromDouble(0.0);
    }
    Options_Set_Return_Value(opts, input, NULL, true);
    Options_Set_Disallow_UnicodeCharacter(&opts);

    return PyObject_to_PyNumber(input, FLOAT, &opts);
}


/* Behaves like float or int, but returns correct type. */
static PyObject *
fastnumbers_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    Options opts = init_Options_convert;
    static const char *format = "|O$p:real";

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
    /* Do not accept number as a keyword argument. */
    static char *keywords[] = { "", "coerce", NULL };
#else
    /* Do accept number as a keyword argument. */
    static char *keywords[] = { "x", "coerce", NULL };
#endif

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, format, keywords,
                                     &input, &opts.coerce)) {
        return NULL;
    }
    /* No arguments returns 0.0 or 0 depending on the state of coerce. */
    if (input == NULL)
        return Options_Coerce_True(&opts) ? PyLong_FromLong(0)
               : PyFloat_FromDouble(0.0);
    Options_Set_Return_Value(opts, input, NULL, true);
    Options_Set_Disallow_UnicodeCharacter(&opts);

    return PyObject_to_PyNumber(input, REAL, &opts);
}


/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    {   "fast_real", (PyCFunction) fastnumbers_fast_real,
        METH_VARARGS | METH_KEYWORDS, fast_real__doc__
    },
    {   "fast_float", (PyCFunction) fastnumbers_fast_float,
        METH_VARARGS | METH_KEYWORDS, fast_float__doc__
    },
    {   "fast_int", (PyCFunction) fastnumbers_fast_int,
        METH_VARARGS | METH_KEYWORDS, fast_int__doc__
    },
    {   "fast_forceint", (PyCFunction) fastnumbers_fast_forceint,
        METH_VARARGS | METH_KEYWORDS, fast_forceint__doc__
    },
    {   "isreal", (PyCFunction) fastnumbers_isreal,
        METH_VARARGS | METH_KEYWORDS, isreal__doc__
    },
    {   "isfloat", (PyCFunction) fastnumbers_isfloat,
        METH_VARARGS | METH_KEYWORDS, isfloat__doc__
    },
    {   "isint", (PyCFunction) fastnumbers_isint,
        METH_VARARGS | METH_KEYWORDS, isint__doc__
    },
    {   "isintlike", (PyCFunction) fastnumbers_isintlike,
        METH_VARARGS | METH_KEYWORDS, isintlike__doc__
    },
    {   "query_type", (PyCFunction) fastnumbers_query_type,
        METH_VARARGS | METH_KEYWORDS, query_type__doc__
    },
    {   "int", (PyCFunction) fastnumbers_int,
        METH_VARARGS | METH_KEYWORDS, fastnumbers_int__doc__
    },
    {   "float", (PyCFunction) fastnumbers_float,
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
        METH_VARARGS, fastnumbers_float__doc__
#else
        METH_VARARGS | METH_KEYWORDS, fastnumbers_float__doc__
#endif
    },
    {   "real", (PyCFunction) fastnumbers_real,
        METH_VARARGS | METH_KEYWORDS, fastnumbers_real__doc__
    },
    {NULL, NULL, 0, NULL} /* Sentinel */
};


/* We want a module-level variable that is the version. */
static PyObject *fastnumbers__version__;

/* Some constants that may be useful for debugging. */
static PyObject *fastnumbers_FN_MAX_INT_LEN;
static PyObject *fastnumbers_FN_DBL_DIG;
static PyObject *fastnumbers_FN_MAX_EXP;
static PyObject *fastnumbers_FN_MIN_EXP;

/* Define the module interface. */
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
    if (m == NULL) {
        return NULL;
    }

    /* Add module level constants. */
    fastnumbers__version__ = PyUnicode_FromString(FASTNUMBERS_VERSION);
    fastnumbers_FN_MAX_INT_LEN = PyLong_FromLong(FN_MAX_INT_LEN);
    fastnumbers_FN_DBL_DIG = PyLong_FromLong(FN_DBL_DIG);
    fastnumbers_FN_MAX_EXP = PyLong_FromLong(FN_MAX_EXP);
    fastnumbers_FN_MIN_EXP = PyLong_FromLong(FN_MIN_EXP);
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

    return m;
}

