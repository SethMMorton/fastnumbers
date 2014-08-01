/*
 * C implementation of the python library fastnumbers.
 *
 * Author: Seth M. Morton, July 30, 2014
 */

#include <Python.h>
#include "fast_conversions.h"
#include "version.h"

/* Compatibility macros for Python 3. */
#if PY_MAJOR_VERSION >= 3
#define PyString_FromString(x) PyUnicode_FromString(x)
#define PyString_AsString(str) PyBytes_AsString(str)
#define PyFloat_FromString_2_3(x) PyFloat_FromString(x)
#define PyInt_FromString(x, y, z) PyLong_FromString((x), (y), (z))
#else
/* The PyFloat_FromString API changed between Python2 and Python3. */
#define PyFloat_FromString_2_3(x) PyFloat_FromString((x), NULL)
#endif


const char safe_float_docstring[] = 
"Convert input to a *float* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *float* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str* or *float*."
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def safe_float(input):\n"
"    ...   try:\n"
"    ...      return float(input)\n"
"    ...   except ValueError:\n"
"    ...      return input\n"
"    ...\n"
"\n"
"The main difference is that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data.\n"
"\n";
static PyObject *
fastnumbers_safe_float(PyObject *self, PyObject *args)
{
    PyObject *result = NULL;
    char *strinput = NULL;
    double dinput;

    /* Read the function argument as a string. */
    if (!PyArg_ParseTuple(args, "s", &strinput)) {

        /* If cannot be read as a string, clear error stack
           and read it as a float, then write as a float. */
        PyErr_Clear();
        if (!PyArg_ParseTuple(args, "d", &dinput))
            return NULL;  /* Raise TypeError for invalid input. */
        return Py_BuildValue("d", dinput);

    }

    /* Attempt the conversion to a float. */
    result = PyFloat_FromString_2_3(PyString_FromString(strinput));

    /* If unsuccessful, clear error stack and return input as-is */
    if (result == NULL) { PyErr_Clear(); return Py_BuildValue("s", strinput); }

    /* Otherwise, return the float object */
    return Py_BuildValue("O", result);
}


const char safe_int_docstring[] = 
"Convert input to a *int* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *int* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid string input, although\n"
"a TypeError error will be raised for types the *int* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str* or *int* (or *long* on Python2 if the input was *long*)."
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def safe_int(input):\n"
"    ...   try:\n"
"    ...      return int(input)\n"
"    ...   except ValueError:\n"
"    ...      return input\n"
"    ...\n"
"\n"
"The main difference is that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data.\n"
"\n";
static PyObject *
fastnumbers_safe_int(PyObject *self, PyObject *args)
{
    PyObject *input = NULL, *result = NULL;
    char *str;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input))
        return NULL;

    /* If the input is already an integer, return now. */
    #if PY_MAJOR_VERSION >= 3
    if (PyLong_Check(input)) return Py_BuildValue("O", input);
    #else
    if (PyInt_Check(input) || PyLong_Check(input)) return Py_BuildValue("O", input);
    #endif

    /* If the input is a float, convert to int and return */
    if (PyFloat_Check(input))
        return Py_BuildValue("l", (long) PyFloat_AS_DOUBLE(input));

    /* Attempt conversion of the object to a string */
    /* If unsuccessful, raise a TypeError. */
    str = PyString_AsString(input);
    if (str == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "expected str(), float(), or int() argument");
        return NULL;
    }

    /* Attempt the conversion to a int. */
    result = PyInt_FromString(str, NULL, 10);

    /* If unsuccessful, clear error stack and return input as-is */
    if (result == NULL) { PyErr_Clear(); return Py_BuildValue("O", input); }

    /* Otherwise, return the int object */
    return Py_BuildValue("O", result);
}


const char fast_float_docstring[] = 
"Convert a string to a *float* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *float* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str* or *float*."
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def fast_float(input):\n"
"    ...   try:\n"
"    ...      return float(input)\n"
"    ...   except ValueError:\n"
"    ...      return input\n"
"    ...\n"
"\n"
"The main differences are that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data, and overflow/underflow is not checked so the\n"
"conversion to a *float* from a string is very fast.\n"
"\n";
static PyObject *
fastnumbers_fast_float(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;
    double result;
    bool error;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input))
        return NULL;

    /* If the input is already a float, return now. */
    if (PyFloat_Check(input)) return Py_BuildValue("O", input);

    /* If the input is an integer, convert to float and return */
    #if PY_MAJOR_VERSION >= 3
    if (PyLong_Check(input))
        return Py_BuildValue("d", PyLong_AsDouble(input));
    #else
    if (PyInt_Check(input))
        return Py_BuildValue("d", (double) PyInt_AS_LONG(input));
    if (PyLong_Check(input))
        return Py_BuildValue("d", PyLong_AsDouble(input));
    #endif

    /* Attempt conversion of the object to a string. */
    /* If unsuccessful, raise a TypeError. */
    str = PyString_AsString(input);
    if (str == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "expected str(), float(), or int() argument");
        return NULL;
    }

    /* Attempt to convert to a float */
    result = fast_atof(str, &error);

    /* If there was an error, return input as-is. Otherwise, return the float object */
    if (error) { PyErr_Clear(); return Py_BuildValue("O", input); }
    return Py_BuildValue("d", result);
}


const char fast_int_docstring[] = 
"Convert a string to a *int* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *int* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid string input, although\n"
"a TypeError error will be raised for types the *int* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str*, or *int* (or *long* on Python2 if the input was *long*)."
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def fast_int(input):\n"
"    ...   try:\n"
"    ...      return int(input)\n"
"    ...   except ValueError:\n"
"    ...      return input\n"
"    ...\n"
"\n"
"The main differences are that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data, and overflow/underflow is not checked so the\n"
"conversion to a *int* from a string is very fast.\n"
"\n";
static PyObject *
fastnumbers_fast_int(PyObject *self, PyObject *args)
{
    PyObject *input = NULL;
    char *str;
    long result;
    bool error;

    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input))
        return NULL;

    /* If the input is already an integer, return now. */
    #if PY_MAJOR_VERSION >= 3
    if (PyLong_Check(input)) return Py_BuildValue("O", input);
    #else
    if (PyInt_Check(input) || PyLong_Check(input)) return Py_BuildValue("O", input);
    #endif

    /* If the input is a float, convert to int and return. */
    if (PyFloat_Check(input))
        return Py_BuildValue("l", (long) PyFloat_AS_DOUBLE(input));

    /* Attempt conversion of the object to a string. */
    /* If unsuccessful, raise a TypeError. */
    str = PyString_AsString(input);
    if (str == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "expected str(), float(), or int() argument");
        return NULL;
    }

    /* Attempt to convert to a int */
    result = fast_atoi(str, &error);

    /* If there was an error, return input as-is. Otherwise, return the int object */
    if (error) { PyErr_Clear(); return Py_BuildValue("O", input); }
    return Py_BuildValue("l", result);
}


/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    {"safe_float", fastnumbers_safe_float, METH_VARARGS, safe_float_docstring},
    {"safe_int", fastnumbers_safe_int, METH_VARARGS, safe_int_docstring},
    {"fast_float", fastnumbers_fast_float, METH_VARARGS, fast_float_docstring},
    {"fast_int", fastnumbers_fast_int, METH_VARARGS, fast_int_docstring},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


/* We want a module-level variable that is the version. */
static PyObject *fastnumbers_version;

/* This is the module initialization step. */ 
PyMODINIT_FUNC
initfastnumbers(void)
{
    PyObject *m;

    m = Py_InitModule3("fastnumbers",
                       FastnumbersMethods,
                       "Quickly convert strings to numbers.");
    if (m == NULL)
        return;

    fastnumbers_version = PyString_FromString(FASTNUMBERS_VERSION);
    Py_INCREF(fastnumbers_version);
    PyModule_AddObject(m, "__version__", fastnumbers_version);
}
