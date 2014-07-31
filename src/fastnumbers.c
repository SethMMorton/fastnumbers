/*
 * C implementation of the python library fastnumbers.
 *
 * Author: Seth M. Morton, July 30, 2014
 */

#include <Python.h>
#include "fast_conversions.h"
#include "version.h"

/* Compatibility macros for Python 3.
 * Taken from SWIG wrapper output.
 */
#if PY_VERSION_HEX >= 0x03000000
#define PyInt_AsLong(x) PyLong_AsLong(x)
#define PyInt_FromLong(x) PyLong_FromLong(x)
#define PyString_FromString(x) PyUnicode_FromString(x)
#define PyString_AsString(str) PyBytes_AsString(str)
#define PyString_AS_STRING(x) PyUnicode_AS_STRING(x)
#define PyFloat_FromString_2_3(x) PyFloat_FromString(x)
#else
/* The PyFloat_FromString API changed between Python2 and Python3. */
#define PyFloat_FromString_2_3(x) PyFloat_FromString((x), NULL)
#endif


const char safe_float_docstring[] = 
"Convert a string to a *float* if possible, return the string otherwise.\n"
"\n"
"Convert a string to a *float* if possible, return input unchanged if not\n"
"possible; no ValueError will be raised for invalid input., although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def asfloat(input):\n"
"    ...   try:\n"
"    ...      return float(input)\n"
"    ...   except:\n"
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
        PyErr_Clear();  /*  */
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
"Convert a string to a *int* if possible, return the string otherwise.\n"
"\n"
"Convert a string to a *int* if possible, return input unchanged if not\n"
"possible; no ValueError will be raised for invalid string input, although\n"
"a TypeError error will be raised for types the *int* function also does\n"
"not accept, like a *list*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def asint(input):\n"
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
    PyObject *result = NULL;
    char *strinput = NULL;
    double dinput;

    /* Read the function argument as a string. */
    if (!PyArg_ParseTuple(args, "s", &strinput)) {

        /* If cannot be read as a string, clear error stack
           and read it as a float, then write as an int. */
        PyErr_Clear();  /*  */
        if (!PyArg_ParseTuple(args, "d", &dinput))
            return NULL;  /* Raise TypeError for invalid input. */
        return Py_BuildValue("l", (long) dinput);

    }

    /* Attempt the conversion to a int. */
    result = PyLong_FromString(strinput, NULL, 10);

    /* If unsuccessful, clear error stack and return input as-is */
    if (result == NULL) { PyErr_Clear(); return Py_BuildValue("s", strinput); }

    /* Otherwise, return the int object */
    return Py_BuildValue("O", result);
}


/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    {"safe_float", fastnumbers_safe_float, METH_VARARGS, safe_float_docstring},
    {"safe_int", fastnumbers_safe_int, METH_VARARGS, safe_int_docstring},
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
