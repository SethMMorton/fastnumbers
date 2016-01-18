#ifndef __PY_SHORTCUTS
#define __PY_SHORTCUTS

#include <Python.h>
#include "fn_bool.h"
#include "py_to_char.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum PyNumberType { REAL, FLOAT, INT, INTLIKE, FORCEINT } PyNumberType;

bool 
PyNumber_is_correct_type(PyObject *obj, const PyNumberType type, PyObject *str_only);

PyObject* 
PyString_is_a_number(PyObject *obj, const PyNumberType type,
                     PyObject *allow_inf, PyObject *allow_nan);

PyObject*
PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub);

PyObject*
PyBool_from_bool_and_DECREF(const bool b, PyObject *obj);

#define PyBool_from_bool(b) ((b) ? (Py_INCREF(Py_True), Py_True) \
                                 : (Py_INCREF(Py_False), Py_False))
#if PY_MAJOR_VERSION >= 3
#define long_to_PyInt(val) PyLong_FromLong(val)
#define str_to_PyInt(str) PyLong_FromString((str), NULL, 10)
#define PyNumber_IsInt(obj) PyLong_CheckExact(obj)
#define PyNumber_ToInt(obj) PyNumber_Long(obj)
#else
#define long_to_PyInt(val) PyInt_FromLong(val)
#define str_to_PyInt(str) PyNumber_Int(PyLong_FromString((str), NULL, 10))
#define PyNumber_IsInt(obj) (PyInt_CheckExact(obj) || PyLong_CheckExact(obj))
#define PyNumber_ToInt(obj) PyNumber_Int(obj)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PY_SHORTCUTS */
