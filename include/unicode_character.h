#ifndef __FN_UNICODE_HANDLING
#define __FN_UNICODE_HANDLING

/*
 * Master header for all unicode handling source files.
 */

#include <Python.h>
#include "objects.h"
#include "options.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Unicode handling is different between Python 2 and Python 3. */
#if PY_MAJOR_VERSION == 2
typedef Py_UNICODE uchar;

/* Extract character from the data. */
#define UREAD(kind, data, index) (((const uchar *) (data))[(index)])

#else
typedef Py_UCS4 uchar;

/* Short for PyUnicode_READ. */
#define UREAD(kind, data, index) PyUnicode_READ((kind), (data), (index))

#endif

/* Declarations */

PyObject *
convert_PyUnicode_to_PyNumber(PyObject *input);

PyObject *
PyUnicodeCharacter_to_PyNumber(PyObject *obj, const PyNumberType type,
                               const struct Options *options);

PyObject *
PyUnicodeCharacter_is_number(PyObject *obj, const PyNumberType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_UNICODE_HANDLING */
