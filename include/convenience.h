#ifndef __CONVENIENCE
#define __CONVENIENCE

#include <Python.h>
#include <string.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Used to determine if a float is so large it lost precision. */
extern const double maxsize;

/* Bad unicode. */
#define NULL_UNI ((Py_UCS4)-1)

/**********
 * MACROS *
 **********/

/* Convert character to lower.  Only needed for Python 2.6. */
#if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION == 6
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

/*************
 * Functions *
 *************/

void convert_string(PyObject *input, char **str, Py_UCS4 *uni, size_t *str_length);
PyObject * handle_error(PyObject *input,
                        PyObject *default_value,
                        const bool raise_on_invalid,
                        const bool bad_inf,
                        const bool bad_nan,
                        const char* str,
                        const Py_UCS4 uni);
bool case_insensitive_match(const char *s, const char *t);
#if PY_MAJOR_VERSION == 2
PyObject * convert_PyString_to_PyFloat_possible_long_literal(PyObject *s);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __CONVENIENCE */
