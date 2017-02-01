#ifndef __FN_OBJECT_HANDLING
#define __FN_OBJECT_HANDLING

/*
 * Master header for all general object handling source files.
 */

#include <Python.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PyNumberType { REAL, FLOAT, INT, INTLIKE, FORCEINT } PyNumberType;

/* All the awesome MACROS */

#define PyBool_from_bool(b) ((b) ? (Py_INCREF(Py_True), Py_True) \
                                 : (Py_INCREF(Py_False), Py_False))

/* Declarations */

PyObject*
PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub,
                     PyObject *pycoerce);

PyObject*
PyObject_is_number(PyObject *obj, const PyNumberType type,
                   PyObject *allow_inf, PyObject *allow_nan,
                   PyObject *str_only, PyObject *num_only);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_OBJECT_HANDLING */
