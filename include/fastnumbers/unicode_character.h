#ifndef __FN_UNICODE_HANDLING
#define __FN_UNICODE_HANDLING

/*
 * Master header for all unicode handling source files.
 */

#include <Python.h>
#include "fastnumbers/options.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declarations */

PyObject *
convert_PyUnicode_to_PyNumber(PyObject *input);

PyObject *
PyUnicodeCharacter_to_PyNumber(PyObject *obj, const PyNumberType type,
                               const Options *options);

PyObject *
PyUnicodeCharacter_is_number(PyObject *obj, const PyNumberType type);

PyObject *
PyUnicodeCharacter_contains_type(PyObject *obj);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_UNICODE_HANDLING */
