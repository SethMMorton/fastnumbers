#ifndef __FN_NUMBER_HANDLING
#define __FN_NUMBER_HANDLING

/*
 * Master header for all number handling source files.
 */

#include <Python.h>
#include "fn_bool.h"
#include "object_handling.h"

#ifdef __cplusplus
extern "C" {
#endif

/* All the awesome MACROS */

#if PY_MAJOR_VERSION >= 3
#define long_to_PyInt(val) PyLong_FromLong(val)
#define PyNumber_IsInt(obj) PyLong_Check(obj)
#define PyNumber_ToInt(obj) PyNumber_Long(obj)
#else
#define long_to_PyInt(val) PyInt_FromLong(val)
#define PyNumber_IsInt(obj) (PyInt_Check(obj) || PyLong_Check(obj))
#define PyNumber_ToInt(obj) PyNumber_Int(obj)
#endif

/* Declarations */

PyObject*
PyFloat_to_PyInt(PyObject * fobj);

bool
PyFloat_is_Intlike(PyObject *obj);

/* Not actually used... keeping in code for posterity's sake.
bool
double_is_intlike(const double val);
*/

PyObject*
PyNumber_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub,
                     PyObject *pycoerce);

bool
PyNumber_is_type(PyObject *obj, const PyNumberType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_NUMBER_HANDLING */
