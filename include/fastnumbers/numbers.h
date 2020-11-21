#ifndef __FN_NUMBER_HANDLING
#define __FN_NUMBER_HANDLING

/*
 * Master header for all number handling source files.
 */

#include <Python.h>
#include <limits.h>
#include "fastnumbers/fn_bool.h"
#include "fastnumbers/options.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copy of _Py_dg_stdnan from the Python code base.
 * Needed to generate NaN on Windows.
 */
double
compat_generate_nan(int sign);

/* All the awesome MACROS */

#define PyNumber_IsNAN(pynum) (PyFloat_Check(pynum) && Py_IS_NAN(PyFloat_AS_DOUBLE(pynum)))
#define PyNumber_IsINF(pynum) (PyFloat_Check(pynum) && Py_IS_INFINITY(PyFloat_AS_DOUBLE(pynum)))

/* Return NaN correctly on this system. */
#if !defined(PY_NO_SHORT_FLOAT_REPR)
#define PyFloat_from_NaN(negative) PyFloat_FromDouble(compat_generate_nan(negative));
#else
#define PyFloat_from_NaN(negative) \
    (negative) ? PyFloat_FromDouble(-Py_NAN) : PyFloat_FromDouble(Py_NAN);
#endif

/* Quickies for raising errors. Try to mimic what Python would say. */
#define FN_FLOAT_MSG "could not convert string to float: %R"
#define FN_INT_MSG "invalid literal for int() with base %d: %R"

#define SET_ERR_INVALID_INT(o)                              \
    if (Options_Should_Raise(o))                            \
        PyErr_Format(PyExc_ValueError,                      \
                     FN_INT_MSG,                            \
                     (o)->base == INT_MIN ? 10 : (o)->base, \
                     (o)->input)
#define SET_ERR_INVALID_FLOAT(o) \
    if (Options_Should_Raise(o)) \
        PyErr_Format(PyExc_ValueError, FN_FLOAT_MSG, (o)->input)
#define SET_ILLEGAL_BASE_ERROR(o)        \
    if (Options_Should_Raise(o))         \
        PyErr_SetString(PyExc_TypeError, \
                        "int() can't convert non-string with explicit base");

/* Declarations */

PyObject *
PyFloat_to_PyInt(PyObject *fobj, const Options *options);

bool
PyFloat_is_Intlike(PyObject *obj);

PyObject *
PyNumber_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const Options *options);

bool
PyNumber_is_type(PyObject *obj, const PyNumberType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_NUMBER_HANDLING */
