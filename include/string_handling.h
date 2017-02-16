#ifndef __FN_STRING_HANDLING
#define __FN_STRING_HANDLING

/*
 * Master header for all string (character array) handling source files.
 */

#include <Python.h>
#include "fn_bool.h"
#include "object_handling.h"

#ifdef __cplusplus
extern "C" {
#endif

/* All the awesome MACROS */

#define string_conversion_success(str) ((str) != NULL)

#if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION == 6
#define python_lib_str_to_double(str, endptr) PyOS_ascii_strtod((str), (endptr))
#else
#define python_lib_str_to_double(str, endptr) PyOS_string_to_double((str), (endptr), NULL)
#endif

/* Declarations */

const char*
convert_PyString_to_str(PyObject *input, const char** end, PyObject **bytes_object);

PyObject*
PyString_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub,
                     PyObject *pycoerce, const int base);

PyObject*
PyString_is_number(PyObject *obj, const PyNumberType type,
                   PyObject *allow_inf, PyObject *allow_nan,
                   const int base);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_STRING_HANDLING */
