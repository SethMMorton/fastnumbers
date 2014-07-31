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
#define PyClass_Check(obj) PyObject_IsInstance(obj, (PyObject *)&PyType_Type)
#define PyInt_Check(x) PyLong_Check(x)
#define PyInt_AsLong(x) PyLong_AsLong(x)
#define PyInt_FromLong(x) PyLong_FromLong(x)
#define PyInt_FromSize_t(x) PyLong_FromSize_t(x)
#define PyString_Check(name) PyBytes_Check(name)
#define PyString_FromString(x) PyUnicode_FromString(x)
#define PyString_Format(fmt, args)  PyUnicode_Format(fmt, args)
#define PyString_AsString(str) PyBytes_AsString(str)
#define PyString_Size(str) PyBytes_Size(str)    
#define PyString_InternFromString(key) PyUnicode_InternFromString(key)
#define Py_TPFLAGS_HAVE_CLASS Py_TPFLAGS_BASETYPE
#define PyString_AS_STRING(x) PyUnicode_AS_STRING(x)
#define _PyLong_FromSsize_t(x) PyLong_FromSsize_t(x)
#define PyFloat_FromString_2_3(x) PyFloat_FromString(x)
#else
/* The PyFloat_FromString API changed between Python2 and Python3. */
char *junk;
#define PyFloat_FromString_2_3(x) PyFloat_FromString(x, &junk)
#endif


/* Function to convert string to float. */
const char asfloat_docstring[] = 
"Convert a string to a *float* if possible.\n"
"\n"
"Convert a string to a *float* if possible, return input unchanged if not\n"
"possible; no exceptions will be raised for invalid input.\n"
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
"There are two differences from the above that should be noted:\n"
"\n"
"    - For performance purposes, this function assumes all non-*str* input\n"
"      should be passed directly to output, so an *int* object (for example)\n"
"      will not be converted to a *float* but will remain an *int*.\n"
"    - Much of python's exception mechanism is bypassed\n"
"      so that the attempted conversion is more efficient for large lists.\n"
"\n";
static PyObject *
fastnumbers_asfloat(PyObject *self, PyObject *args)
{
    PyObject *input, *result;
    /* Read the function argument. */
    if (!PyArg_ParseTuple(args, "O", &input)) return NULL;
    /* Attempt the conversion to a float. */
    result = PyFloat_FromString_2_3(input);
    /* If unsuccessful, clear error stack and return input as-is */
    if (result == NULL) { PyErr_Clear(); return input; }
    /* Otherwise, return the float object */
    return result;
}


/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    {"asfloat", fastnumbers_asfloat, METH_VARARGS, asfloat_docstring},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


/* We want a module-level variable that is the version. */
static PyObject *fastnumbers_version;

/* This is the module initialization step. */ 
PyMODINIT_FUNC
initfastnumbers(void)
{
    PyObject *m;

    m = Py_InitModule("fastnumbers", FastnumbersMethods);
    if (m == NULL)
        return;

    fastnumbers_version = PyString_FromString(FASTNUMBERS_VERSION);
    Py_INCREF(fastnumbers_version);
    PyModule_AddObject(m, "__version__", fastnumbers_version);
}
