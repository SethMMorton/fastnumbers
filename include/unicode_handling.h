#ifndef __FN_UNICODE_HANDLING
#define __FN_UNICODE_HANDLING

/*
 * Master header for all unicode handling source files.
 */

#include <Python.h>
#include "fn_bool.h"
#include "object_handling.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bad unicode. */

#define NULL_UNI ((Py_UCS4)-1)
#define ERR_UNI (Py_UCS4) ' '

/* All the awesome MACROS */

#define unicode_conversion_success(x) ((x) != NULL_UNI)
#if PY_MAJOR_VERSION >= 3
#define cast_unicode(uni) ((uni))
#else
#define cast_unicode(uni) ((Py_UNICODE) (uni))
#endif
#define uni_isspace(uni) Py_UNICODE_ISSPACE(cast_unicode(uni))
#define uni_isnumeric(uni) Py_UNICODE_ISNUMERIC(cast_unicode(uni))
#define uni_isdigit(uni) Py_UNICODE_ISDIGIT(cast_unicode(uni))
#define uni_tonumeric(uni) Py_UNICODE_TONUMERIC(cast_unicode(uni))
#define uni_todigit(uni) Py_UNICODE_TODIGIT(cast_unicode(uni))

/* Declarations */

Py_UCS4
convert_PyUnicode_to_unicode_char(PyObject *input);

PyObject*
PyUnicodeCharacter_to_PyNumber(PyObject *obj, const PyNumberType type, const struct Options *options);

PyObject*
PyUnicodeCharacter_is_number(PyObject *obj, const PyNumberType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_UNICODE_HANDLING */
