#ifndef __FN_STRING_HANDLING
#define __FN_STRING_HANDLING

/*
 * Master header for all string (character array) handling source files.
 */

#include <Python.h>
#include "options.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declarations */

PyObject *
PyString_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const Options *options);

PyObject *
PyString_is_number(PyObject *obj, const PyNumberType type,
                   const Options *options);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_STRING_HANDLING */
