#ifndef __FN_STRING_HANDLING
#define __FN_STRING_HANDLING

/*
 * Master header for all string (character array) handling source files.
 */

#include <Python.h>
#include "fn_bool.h"
#include "object_handling.h"
#include "options.h"

#ifdef __cplusplus
extern "C" {
#endif

/* All the awesome MACROS */

#define string_conversion_success(str) ((str) != NULL)

/* Declarations */

const char*
convert_PyString_to_str(PyObject *input, const char** end, PyObject **temp_object, char **temp_char, Py_buffer *view);

PyObject*
PyString_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const struct Options *options);

PyObject*
PyString_is_number(PyObject *obj, const PyNumberType type,
                   const struct Options *options);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_STRING_HANDLING */
