/*
 * C implementation of the python library fastnumbers.
 *
 * Author: Seth M. Morton, July 30, 2014
 */

#include <Python.h>
#include "fast_conversions.h"
#include "version.h"
#include "docstrings.h"
#include "py_to_char.h"
#include "py_shortcuts.h"


static PyObject* assess_PyNumber(PyObject *input, PyObject *retval,
                                 PyObject *default_value, PyObject *raise_on_invalid,
                                 const PyNumberType type)
{
    /* NULL and no error set? It must be a TypeError. These always raise an error. */
    if (retval == Py_None) {
        PyErr_Format(PyExc_TypeError,
                     "expected str, float, or int argument, got %.200s",
                     input->ob_type->tp_name);
        return NULL;
    }

    /* Handle error. */
    else if (retval == NULL) {
        if (raise_on_invalid == Py_True) {
            if (type == REAL || type == FLOAT)
                return PyNumber_Float(input);
            else
                return PyNumber_ToInt(input);
        }
        else if (default_value != NULL)
            return Py_INCREF(default_value), default_value;
        else
            return Py_INCREF(input), input;
    }

    /* Return correct result. */
    else
        return retval;
}


/* Quickly convert to an int or float, depending on value. */
static PyObject *
fastnumbers_fast_real(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    PyObject *inf_sub = NULL;
    PyObject *nan_sub = NULL;
    PyObject *pyreturn = NULL;
    static char *keywords[] = { "x", "default", "raise_on_invalid", "inf", "nan", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOO:fast_real", keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &inf_sub, &nan_sub))
        return NULL;

    pyreturn = PyObject_to_PyNumber(input, REAL, inf_sub, nan_sub);
    return assess_PyNumber(input, pyreturn, default_value, raise_on_invalid, REAL);
}


/* Quickly convert to a float, depending on value. */
static PyObject *
fastnumbers_fast_float(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    PyObject *inf_sub = NULL;
    PyObject *nan_sub = NULL;
    PyObject *pyreturn = NULL;
    static char *keywords[] = { "x", "default", "raise_on_invalid", "inf", "nan", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOO:fast_float", keywords,
                                     &input, &default_value, &raise_on_invalid,
                                     &inf_sub, &nan_sub))
        return NULL;

    pyreturn = PyObject_to_PyNumber(input, FLOAT, inf_sub, nan_sub);
    return assess_PyNumber(input, pyreturn, default_value, raise_on_invalid, FLOAT);
}


/* Quickly convert to an int, depending on value. */
static PyObject *
fastnumbers_fast_int(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    PyObject *pyreturn = NULL;
    static char *keywords[] = { "x", "default", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:fast_int", keywords,
                                     &input, &default_value, &raise_on_invalid))
        return NULL;

    pyreturn = PyObject_to_PyNumber(input, INT, NULL, NULL);
    return assess_PyNumber(input, pyreturn, default_value, raise_on_invalid, INT);
}


/* Safely convert to an int (even if in a string and as a float), depending on value. */
static PyObject *
fastnumbers_fast_forceint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *raise_on_invalid = Py_False;
    PyObject *default_value = NULL;
    PyObject *pyreturn = NULL;
    static char *keywords[] = { "x", "default", "raise_on_invalid", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:fast_forceint", keywords,
                                     &input, &default_value, &raise_on_invalid))
        return NULL;

    pyreturn = PyObject_to_PyNumber(input, FORCEINT, NULL, NULL);
    return assess_PyNumber(input, pyreturn, default_value, raise_on_invalid, FORCEINT);
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
    static char *keywords[] = { "x", "str_only", "num_only", "allow_inf", "allow_nan", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOO:isreal", keywords,
                                     &input, &str_only, &num_only, &allow_inf, &allow_nan))
        return NULL;

    if (PyNumber_Check(input))
        return PyBool_from_bool(PyNumber_is_correct_type(input, REAL, str_only));
    if (PyObject_IsTrue(num_only)) Py_RETURN_FALSE;
    return PyString_is_a_number(input, REAL, allow_inf, allow_nan);
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
    static char *keywords[] = { "x", "str_only", "num_only", "allow_inf", "allow_nan", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOO:isfloat", keywords,
                                     &input, &str_only, &num_only, &allow_inf, &allow_nan))
        return NULL;

    if (PyNumber_Check(input))
        return PyBool_from_bool(PyNumber_is_correct_type(input, FLOAT, str_only));
    if (PyObject_IsTrue(num_only)) Py_RETURN_FALSE;
    return PyString_is_a_number(input, FLOAT, allow_inf, allow_nan);
}


/* Quickly determine if the input is an int. */
static PyObject *
fastnumbers_isint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *num_only = Py_False;
    static char *keywords[] = { "x", "str_only", "num_only", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:isint", keywords,
                                     &input, &str_only, &num_only))
        return NULL;

    if (PyNumber_Check(input))
        return PyBool_from_bool(PyNumber_is_correct_type(input, INT, str_only));
    if (PyObject_IsTrue(num_only)) Py_RETURN_FALSE;
    return PyString_is_a_number(input, INT, NULL, NULL);
}


/* Quickly determine if the input is int-like. */
static PyObject *
fastnumbers_isintlike(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *input = NULL;
    PyObject *str_only = Py_False;
    PyObject *num_only = Py_False;
    static char *keywords[] = { "x", "str_only", "num_only", NULL };

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:isintlike", keywords,
                                     &input, &str_only, &num_only))
        return NULL;

    if (PyNumber_Check(input))
        return PyBool_from_bool(PyNumber_is_correct_type(input, INTLIKE, str_only));
    if (PyObject_IsTrue(num_only)) Py_RETURN_FALSE;
    return PyString_is_a_number(input, INTLIKE, NULL, NULL);
}


static PyObject *
fastnumbers_safe_real(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyErr_WarnEx(PyExc_DeprecationWarning, "please use fast_real instead of safe_real", 1);
    return fastnumbers_fast_real(self, args, kwargs);
}

static PyObject *
fastnumbers_safe_float(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyErr_WarnEx(PyExc_DeprecationWarning, "please use fast_float instead of safe_float", 1);
    return fastnumbers_fast_float(self, args, kwargs);
}

static PyObject *
fastnumbers_safe_int(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyErr_WarnEx(PyExc_DeprecationWarning, "please use fast_int instead of safe_int", 1);
    return fastnumbers_fast_int(self, args, kwargs);
}

static PyObject *
fastnumbers_safe_forceint(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyErr_WarnEx(PyExc_DeprecationWarning, "please use fast_forceint instead of safe_forceint", 1);
    return fastnumbers_fast_forceint(self, args, kwargs);
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
