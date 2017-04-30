#ifndef __FN_NUMBER_HANDLING
#define __FN_NUMBER_HANDLING

/*
 * Master header for all number handling source files.
 */

#include <Python.h>
#include <limits.h>
#include "fn_bool.h"
#include "parsing.h"
#include "object_handling.h"
#include "options.h"

#ifdef __cplusplus
extern "C" {
#endif

/* All the awesome MACROS */

#define PyNumber_IsNAN(pynum) (PyFloat_Check(pynum) && Py_IS_NAN(PyFloat_AS_DOUBLE(pynum)))
#define PyNumber_IsINF(pynum) (PyFloat_Check(pynum) && Py_IS_INFINITY(PyFloat_AS_DOUBLE(pynum)))

#if PY_MAJOR_VERSION >= 3
#define long_to_PyInt(val) PyLong_FromLong(val)
#define PyNumber_IsInt(obj) PyLong_Check(obj)
#define PyNumber_ToInt(obj) PyNumber_Long(obj)
#else
#define long_to_PyInt(val) PyInt_FromLong(val)
#define PyNumber_IsInt(obj) (PyInt_Check(obj) || PyLong_Check(obj))
#define PyNumber_ToInt(obj) PyNumber_Int(obj)
#endif

/* Return NaN correctly on this system. */
#if PY_MAJOR_VERSION >= 3 && !defined(PY_NO_SHORT_FLOAT_REPR)
#define PyFloat_from_NaN(negative) PyFloat_FromDouble(_Py_dg_stdnan(negative));
#else
#define PyFloat_from_NaN(negative) (negative) ? PyFloat_FromDouble(-Py_NAN) \
                                              : PyFloat_FromDouble(Py_NAN);
#endif

/* Quickies for raising errors. Try to mimic what Python would say. */
#define FN_FLOAT_MSG "could not convert string to float: %R"
#define FN_INT_MSG "invalid literal for int() with base %d: %R"

#if PY_MAJOR_VERSION == 2
#define SET_ERR_INVALID_INT(o)                                                       \
    if (Options_Should_Raise(o)) {                                                   \
        PyObject * msg = PyUnicode_FromFormat(FN_INT_MSG,                            \
                                              (o)->base == INT_MIN ? 10 : (o)->base, \
                                              (o)->input);                           \
        PyErr_SetObject(PyExc_ValueError, msg);                                      \
        Py_XDECREF(msg);                                                             \
    }
#define SET_ERR_INVALID_FLOAT(o)                                         \
    if (Options_Should_Raise(o)) {                                       \
        PyObject * msg = PyUnicode_FromFormat(FN_FLOAT_MSG, (o)->input); \
        PyErr_SetObject(PyExc_ValueError, msg);                          \
        Py_XDECREF(msg);                                                 \
    }
#else
#define SET_ERR_INVALID_INT(o)                              \
    if (Options_Should_Raise(o))                            \
        PyErr_Format(PyExc_ValueError,                      \
                     FN_INT_MSG,                            \
                     (o)->base == INT_MIN ? 10 : (o)->base, \
                     (o)->input)
#define SET_ERR_INVALID_FLOAT(o) \
    if (Options_Should_Raise(o)) \
        PyErr_Format(PyExc_ValueError, FN_FLOAT_MSG, (o)->input)
#endif

#define SET_ILLEGAL_BASE_ERROR(o)        \
    if (Options_Should_Raise(o))         \
        PyErr_SetString(PyExc_TypeError, \
                        "int() can't convert non-string with explicit base");

/* Declarations */

PyObject*
PyFloat_to_PyInt(PyObject *fobj, const struct Options *options);

bool
PyFloat_is_Intlike(PyObject *obj);

/* Not actually used... keeping in code for posterity's sake.
bool
double_is_intlike(const double val);
*/

PyObject*
PyNumber_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const struct Options *options);

bool
PyNumber_is_type(PyObject *obj, const PyNumberType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_NUMBER_HANDLING */
