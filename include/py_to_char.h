#ifndef __PY_TO_CHAR
#define __PY_TO_CHAR

#include <Python.h>
#include "fn_bool.h"
#include "parsing.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bad unicode. */
#define NULL_UNI ((Py_UCS4)-1)
#define ERR_UNI (Py_UCS4) ' '

Py_UCS4
convert_PyUnicode_to_unicode_char(PyObject *input);

const char*
convert_PyString_to_str(PyObject *input, PyObject **bytes_object);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PY_TO_CHAR */
