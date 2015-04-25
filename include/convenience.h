#ifndef CONVENIENCE
#define CONVENIENCE

#include <Python.h>
#include <string.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Used to determine if a float is so large it lost precision. */
extern const double maxsize;

/**********
 * MACROS *
 **********/

/* Convert character to lower.  Only needed for Python 2.6. */
#if PY_MAJOR_VERSION == 2 || PY_MINOR_VERSION == 6
#define Py_CHARMASK(c) ((unsigned char)((c) & 0xff))
extern const unsigned char _Py_ctype_tolower[256];
#define Py_TOLOWER(c) (_Py_ctype_tolower[Py_CHARMASK(c)])
#endif

/* Check if a number is an int */
#if PY_MAJOR_VERSION >= 3
#define PYINT_CHECK(x) PyLong_Check(x)
#else
#define PYINT_CHECK(x) (PyInt_Check(x) || PyLong_Check(x))
#endif

/* Check for numeric types. */
#define ANYNUM(x) (PYINT_CHECK(x) || PyFloat_Check(x))

/* Turn a number to an int */
#if PY_MAJOR_VERSION >= 3
#define PYNUM_ASINT(x) PyNumber_Long(x)
#else
#define PYNUM_ASINT(x) PyNumber_Int(x)
#endif

/* Turn a size type into an int */
#if PY_MAJOR_VERSION >= 3
#define PYNUM_ASINT_FROM_SIZET(x) PyLong_FromSsize_t(x)
#else
#define PYNUM_ASINT_FROM_SIZET(x) PyInt_FromSsize_t(x)
#endif

/* Different return conditions for numeric types. */
#define IF_ANYNUM_RETURN_AS_IS(x) if (ANYNUM(x)) return Py_BuildValue("O", (x));
#define IF_ANYNUM_RETURN_FLOAT(x) if (ANYNUM(x)) return PyNumber_Float(x);
#define IF_ANYNUM_RETURN_INT(x)   if (ANYNUM(x)) return PYNUM_ASINT(x);

/* Turn a python string into a float. */
#if PY_MAJOR_VERSION >= 3
#define PYFLOAT_FROM_PYSTRING(x) PyFloat_FromString(x)
#else
#define PYFLOAT_FROM_PYSTRING(x) convert_PyString_to_PyFloat_possible_long_literal(x)
#endif

/* Turn a character string into an int. */
#if PY_MAJOR_VERSION >= 3
#define PYINT_FROM_STRING(x) PyLong_FromString((x), NULL, 10)
#else
#define PYINT_FROM_STRING(x) (NULL == strpbrk((x), "lL")) ? \
                             PyInt_FromString((x), NULL, 10) : \
                             PyLong_FromString((x), NULL, 10)
#endif

/* Convert PyObject to a string, or raise an error. */
#define CONVERT_TO_STRING_OR_RAISE(in, out) \
    out = convert_string(in); if (out == NULL) return NULL;

/* Return an error unless the variable is a string. */
#define RETURN_IF_STRING_OR_RAISE(x) \
    if (PyUnicode_Check(x) || PyBytes_Check(x)) { \
        PyErr_Clear(); \
        return Py_INCREF(x), (x); \
    } \
    else return NULL;

/* Check a condition, and if true return an error unless the variable is a string. */
#define IF_TRUE_RETURN_IF_STRING_OR_RAISE(condition, x) \
    if (condition) { RETURN_IF_STRING_OR_RAISE(x) }

/* Check a condition, and if true return NULL (i.e. raise error). */
#define IF_TRUE_RAISE(condition) \
    if (condition) { return NULL; }

/* Check a condition, and if true return NULL, first setting an exception. */
#define IF_TRUE_RAISE_ERR_STR(condition, err, str) \
    if (condition) { PyErr_SetString(err, str); return NULL; }

/* Check a condition, and if true return NULL, first setting an exception by formatting. */
#define IF_TRUE_RAISE_ERR_FMT(condition, err, fmt, val) \
    if (condition) { return PyErr_Format(err, fmt, val); }

/* Check a condition, and if true, clear error cache and return input as-is. */
#define IF_TRUE_RETURN_INPUT_AS_IS(condition, x) \
    if (condition) { PyErr_Clear(); return Py_INCREF(x), (x); }

/* Check a condition, and if true clear error cache and return value. */
#define IF_TRUE_RETURN_VALUE(condition, x) \
    if (condition) { PyErr_Clear(); return (x); }

/*************
 * Functions *
 *************/

char* convert_string(PyObject *input);
bool case_insensitive_match(const char *s, const char *t);
#if PY_MAJOR_VERSION == 2
PyObject * convert_PyString_to_PyFloat_possible_long_literal(PyObject *s);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CONVENIENCE */