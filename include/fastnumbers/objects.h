#ifndef __FN_OBJECT_HANDLING
#define __FN_OBJECT_HANDLING

/*
 * Master header for all general object handling source files.
 */

#include <Python.h>
#include "fastnumbers/options.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declarations */

PyObject *
PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const Options *options);

PyObject *
PyObject_is_number(PyObject *obj, const PyNumberType type,
                   const Options *options);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_OBJECT_HANDLING */
