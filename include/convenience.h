#ifndef CONVENIENCE
#define CONVENIENCE

/**********
 * MACROS *
 **********/

/* Check for numeric types. */
#if PY_MAJOR_VERSION >= 3
#define ANYNUM(x) PyLong_Check(x) || PyFloat_Check(x)
#else
#define ANYNUM(x) PyInt_Check(x) || PyLong_Check(x) || PyFloat_Check(x)
#endif

/* Turn a number to an int */
#if PY_MAJOR_VERSION >= 3
#define PYNUM_ASINT(x) PyNumber_Long(x)
#else
#define PYNUM_ASINT(x) PyNumber_Int(x)
#endif

/* Different return conditions for numeric types. */
#define IF_ANYNUM_RETURN_AS_IS(x) if (ANYNUM(x)) return Py_BuildValue("O", (x));
#define IF_ANYNUM_RETURN_FLOAT(x) if (ANYNUM(x)) return PyNumber_Float(x);
#define IF_ANYNUM_RETURN_INT(x)   if (ANYNUM(x)) return PYNUM_ASINT(x);

/* Turn a python string into a float. */
#if PY_MAJOR_VERSION >= 3
#define PYFLOAT_FROM_PYSTRING(x) PyFloat_FromString(x)
#else
#define PYFLOAT_FROM_PYSTRING(x) PyFloat_FromString((x), NULL)
#endif

/* Turn a character string into an int. */
#if PY_MAJOR_VERSION >= 3
#define PYINT_FROM_STRING(x) PyLong_FromString((x), NULL, 10)
#else
#define PYINT_FROM_STRING(x) PyInt_FromString((x), NULL, 10)
#endif

/* Convert PyObject to a string, or raise an error. */
#define CONVERT_TO_STRING_OR_RAISE(in, out) \
    out = convert_string(in); if (out == NULL) return NULL;

/* Return an error unless the variable is a string. */
#define RETURN_IF_STRING_OR_RAISE(x) \
    if (PyUnicode_Check(x) || PyBytes_Check(x)) { \
        PyErr_Clear(); \
        return Py_BuildValue("O", (x)); \
    } \
    else return NULL;

/* Check a condition, and if true return an error unless the variable is a string. */
#define IF_TRUE_RETURN_IF_STRING_OR_RAISE(condition, x) \
    if (condition) { RETURN_IF_STRING_OR_RAISE(x) }

/* Check a condition, and if true, clear error cache and return input as-is. */
#define IF_TRUE_RETURN_INPUT_AS_IS(condition, x) \
    if (condition) { PyErr_Clear(); return Py_BuildValue("O", (x)); }

/* Check a condition, and if true clear error cache and return value. */
#define IF_TRUE_RETURN_VALUE(condition, x) \
    if (condition) { PyErr_Clear(); return (x); }

/*************
 * Functions *
 *************/

char* convert_string(PyObject *input);

#endif /* CONVENIENCE */