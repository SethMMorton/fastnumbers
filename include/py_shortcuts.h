#ifndef __PY_SHORTCUTS
#define __PY_SHORTCUTS

#include <Python.h>
#include "fn_bool.h"
#include "py_to_char.h"


#ifdef __cplusplus
extern "C" {
#endif
typedef enum PyNumberType { REAL, FLOAT, INT, INTLIKE, FORCEINT } PyNumberType;
bool double_is_intlike(const double val);
bool PyFloat_is_Intlike(PyObject *obj);
PyObject* PyUnicode_to_PyNumber(const Py_UCS4 uni, const PyNumberType type);
PyObject* str_to_PyNumber(const char* str, const PyNumberType type,
                          PyObject *inf_sub, PyObject *nan_sub);
PyObject* PyNumber_to_PyNumber(PyObject *pynum, const PyNumberType type,
                               PyObject *inf_sub, PyObject *nan_sub);
PyObject* PyString_is_a_number(PyObject *obj, const PyNumberType type,
                               PyObject *allow_inf, PyObject *allow_nan);
PyObject* PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                               PyObject *inf_sub, PyObject *nan_sub);


inline static PyObject* PyBool_from_bool(const bool b)
{
    if (b) Py_RETURN_TRUE;
    else   Py_RETURN_FALSE;
}


inline static PyObject* PyBool_from_bool_and_DECREF(const bool b, PyObject *obj)
{
    Py_XDECREF(obj);
    return PyBool_from_bool(b);
}


inline static PyObject* long_to_PyInt(const long val)
{
#if PY_MAJOR_VERSION >= 3
    return PyLong_FromLong(val);
#else
    return PyInt_FromLong(val);
#endif
}


inline static PyObject* str_to_PyInt(char* str)
{
#if PY_MAJOR_VERSION >= 3
    return PyLong_FromString(str, NULL, 10);
#else
    return PyNumber_Int(PyLong_FromString(str, NULL, 10));
#endif
}


inline static bool PyNumber_IsInt(PyObject *obj)
{
#if PY_MAJOR_VERSION >= 3
    return PyLong_CheckExact(obj)
#else
    return PyInt_CheckExact(obj) || PyLong_CheckExact(obj);
#endif
}


inline static PyObject* PyNumber_ToInt(PyObject *obj)
{
#if PY_MAJOR_VERSION >= 3
    return PyNumber_Long(obj);
#else
    return PyNumber_Int(obj);
#endif
}


inline static bool PyNumber_is_correct_type(PyObject *obj, const PyNumberType type, PyObject *str_only)
{
    if (PyObject_IsTrue(str_only)) return false;
    /* if (PyComplex_CheckExact(obj)) return false; */ /* Complex not allowed! */
    switch (type) {
    case REAL:
        return true;
    case FLOAT:
        return PyFloat_CheckExact(obj);
    case INT:
        return PyNumber_IsInt(obj);
    case INTLIKE:
    case FORCEINT:
        return PyNumber_IsInt(obj) || PyFloat_is_Intlike(obj);
    }
}


inline static bool PyUnicode_is_int(PyObject *obj)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (uni == NULL_UNI) return false;
    return Py_UNICODE_ISDIGIT(uni);
}


inline static bool PyUnicode_is_intlike(PyObject *obj)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (uni == NULL_UNI) return false;
    if (Py_UNICODE_ISDIGIT(uni)) return true;
    const double val = Py_UNICODE_TONUMERIC(uni);
    if (val <= -1.0) return false;  /* Lowerbound of values is > -1.0. */
    return double_is_intlike(val);
}


inline static bool PyUnicode_is_float(PyObject *obj)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (uni == NULL_UNI) return false;
    return Py_UNICODE_ISNUMERIC(uni);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PY_SHORTCUTS */
