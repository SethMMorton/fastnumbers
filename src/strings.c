/*
 *  * Functions that will convert/evaluate a Python string.
 *
 * Author: Seth M. Morton
 *
 * July 2018
 */

#include <Python.h>
#include <string.h>
#include <limits.h>
#include "strings.h"
#include "numbers.h"
#include "options.h"
#include "objects.h"
#include "unicode_character.h"
#include "parsing.h"


#if PY_MAJOR_VERSION == 2
#define STRING_TYPE_CHECK(o) (PyBytes_Check(o) || PyUnicode_Check(o))
#else
#define STRING_TYPE_CHECK(o) (PyBytes_Check(o) || \
                              PyUnicode_Check(o) || \
                              PyByteArray_Check(o) \
                             )
#endif


/* Forward declarations */
static PyObject *
str_to_PyInt(const char *str, const char *end, const struct Options *options);
static PyObject *
str_to_PyFloat(const char *str, const char *end,
               const struct Options *options);


static PyObject *
str_to_PyInt_or_PyFloat(const char *str, const char *end,
                        const struct Options *options)
{
    PyObject *pyresult = NULL;
    /* If the input contains an integer, convert to int directly. */
    if (string_contains_int(str, end, 10)) {
        return str_to_PyInt(str, end, options);
    }

    /* If not an int, assume the input is a float. */
    pyresult = str_to_PyFloat(str, end, options);
    if (pyresult == NULL) {
        return NULL;
    }

    /* Coerce to int if needed. Don't do it for NAN or INF. */
    return (Options_Coerce_True(options)
            && string_contains_intlike_float(str, end)
            && !PyNumber_IsNAN(pyresult)
            && !PyNumber_IsINF(pyresult)
           )
           ? PyFloat_to_PyInt(pyresult, options)
           : pyresult;
}


static PyObject *
python_lib_str_to_PyFloat(const char *str, Py_ssize_t len, void *options)
{
    char *pend = NULL, *nend = (char *) str + len;
    double result = -10.0;
#if PY_MAJOR_VERSION == 2
    /* If this is a long literal, don't include the L. */
    if (*(nend - 1) == 'l' || *(nend - 1) == 'L') {
        pend = nend = (char *) nend - 1;
    }
#endif
    result = PyOS_string_to_double(str, &pend, NULL);
    if (pend == nend) {
        return PyFloat_FromDouble(result);
    }
    /* Clear error if we should not raise. */
    if (Options_Should_Raise((const struct Options *) options)) {
        PyErr_Clear();
    }
    else {
        SET_ERR_INVALID_FLOAT((const struct Options *) options);
    }
    return NULL;
}


PyObject *
str_to_PyFloat(const char *str, const char *end, const struct Options *options)
{
    /* Use some simple heuristics to determine if the the string
     * is likely a float - first and last characters must be digits.
     * Also quick detect NaN and INFINITY.
     */
    const char *start = str + (unsigned) is_sign(str);
    const unsigned len = (unsigned)(end - start);
    const unsigned real_len = (unsigned)(end - str);
    if (quick_detect_infinity(start, len)) {
        if (Options_Has_INF_Sub(options)) {
            return Options_Return_INF_Sub(options);
        }
        else {
            Py_RETURN_INF(*str == '-' ? -1.0 : 1.0);
        }
    }
    else if (quick_detect_nan(start, len)) {
        if (Options_Has_NaN_Sub(options)) {
            return Options_Return_NaN_Sub(options);
        }
        else {
            return PyFloat_from_NaN(*str == '-');
        }
    }
    else if (!is_likely_float(start, len)) {
        SET_ERR_INVALID_FLOAT(options);
        return NULL;
    }

    /* Perform the actual parse using either the "fast" parser
     * or Python's built-in parser. Pre-determine if the "fast"
     * method might overflow based on the length of the string,
     * and if it might stay on the safe side and go straight to
     * Python's built-in version, otherwise use the "fast" version.
     */
    else if (float_might_overflow(str, end)) {
        /* Building an exception takes a long time. The tiny
         * performance hit of checking that the input is a valid
         * float before converting to float is well worth it
         * compared to trying and failing and then waiting for the
         * exception to be created, only to clear it and move on.
         */
        if (!string_contains_float(str, end, true, true)) {
            SET_ERR_INVALID_FLOAT(options);
            return NULL;
        }
        return python_lib_str_to_PyFloat(str, real_len, (void *) options);
    }
    else {
        bool error = false;
        double result = parse_float(str, end, &error);
        if (error) {
            SET_ERR_INVALID_FLOAT(options);
            return NULL;
        }
        return PyFloat_FromDouble(result);
    }
}


/* We need to take some extra care on Python 2 for the str2long conversion. */
#if PY_MAJOR_VERSION == 2
static PyObject *
python_lib_str_to_PyInt(const char *str, char **pend, const int base)
{
    PyObject *num = PyLong_FromString((char *) str, pend, (int) base);
    PyObject *num_swap = NULL;
    if (num == NULL) {
        return NULL;
    }
    /* Convert to int from long if possible. */
    num_swap = num;
    num = PyNumber_Int(num_swap);
    Py_XDECREF(num_swap);
    return num;
}
#else
#define python_lib_str_to_PyInt(str, pend, base) \
    PyLong_FromString((char *) (str), (pend), (int) (base))
#endif


static PyObject *
handle_possible_conversion_error(const char *end, char *pend,
                                 PyObject *val, const struct Options *options)
{
    /* If the expected end matches the parsed end, it was a success.
     * This function includes trailing whitespace in "end" definition
     * so we must do the same. */
    consume_white_space(end);
    /* If an error occurred, clear exception (if needed) and return NULL. */
    if (val == NULL || pend != end) {
        if (pend != end && Options_Should_Raise(options))
#if PY_MAJOR_VERSION == 2
            PyErr_SetString(PyExc_ValueError,
                            "null byte in argument for int()");
#else
            SET_ERR_INVALID_INT(options);
#endif
        if (!Options_Should_Raise(options)) {
            PyErr_Clear();
        }
        Py_XDECREF(val);  /* Probably redundant. */
        val = NULL;  /* Probably redundant. */
    }
    return val;
}


PyObject *
str_to_PyInt(const char *str, const char *end, const struct Options *options)
{
    /* Use some simple heuristics to determine if the the string
     * is likely an int - first and last characters must be digits.
     */
    const char *start = str + (unsigned) is_sign(str);
    Py_ssize_t len = 0;

    /* Python 2 allows space between the sign and digits. */
    consume_white_space_py2_only(start);

    len = end - start;
    if (!is_likely_int(start, len)) {
        SET_ERR_INVALID_INT(options);
        return NULL;
    }

    /* Perform the actual parse using either the "fast" parser
     * or Python's built-in parser. Pre-determine if the "fast"
     * method might overflow based on the length of the string,
     * and if it might stay on the safe side and go straight to
     * Python's built-in version, otherwise use the "fast" version.
     */
    else if (int_might_overflow(start, end)) {
        PyObject *num = NULL;
        char *pend = "\0";
        /* Building an exception takes a long time. The tiny
         * performance hit of checking that the input is a valid
         * integer before converting to integer is well worth it
         * compared to trying and failing and then waiting for the
         * exception to be created, only to clear it and move on.
         */
        if (!string_contains_int(str, end, 10)) {
            SET_ERR_INVALID_INT(options);
            return NULL;
        }
        num = python_lib_str_to_PyInt(str, &pend, 10);
        return handle_possible_conversion_error(end, pend, num, options);
    }
    else {
        bool error = false;
        long result = parse_int(str, end, &error);
        if (error) {
            SET_ERR_INVALID_INT(options);
            return NULL;
        }
        return long_to_PyInt(result);
    }
}


static PyObject *
str_to_PyInt_forced(const char *str, const char *end,
                    const struct Options *options)
{
    /* Convert the input to an int or float. */
    PyObject *pyresult = str_to_PyInt_or_PyFloat(str, end, options);
    if (pyresult == NULL) {
        return NULL;
    }

    /* If a float was returned, convert to an int. Otherwise return as-is. */
    return PyFloat_Check(pyresult)
           ? PyFloat_to_PyInt(pyresult, options)
           : pyresult;
}


/* The unicode handling methods in the C API essentially completely
 * changed starting with Python 3.3. However, with clever use of
 * typedef and the pre-processor we can make it seem like they are the
 * same. Run through the unicode and replace unicode decimals with
 * ASCII decimals, and replace weird whitespace with ASCII whitespace.
 * The returned char * WILL need to be freed at a later time.
 * If NULL is returned then a some sort of memory error occurred or
 * the data is invalid - it depends on the value of error.
 * If an error occurred, you will not need to free the result.
 */
static char *
PyUnicode_as_ascii_string(PyObject *obj, Py_ssize_t *len, bool *error)
{
#if PY_MAJOR_VERSION == 2
#define kind 0  /* Just to have a symbol defined below. */
    const uchar *data = PyUnicode_AS_UNICODE(obj);  /* Raw data */
#else
    const int kind = PyUnicode_KIND(obj);  /* Unicode storage format. */
    const void *data = PyUnicode_DATA(obj);  /* Raw data */
#endif
    char *ascii = NULL;
    *error = false;

#if PY_MAJOR_VERSION == 2
    *len = PyUnicode_GET_SIZE(obj);
#else
    *len = PyUnicode_GET_LENGTH(obj);
#endif

    /* It is assumed PyUnicode_Check has already been called. */
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 3
    if (PyUnicode_READY(obj)) {  /* If true, then not OK for conversion. */
        return NULL;
    }
#endif

    /* Allocate space for the new string. */
    if ((ascii = calloc(*len + 1, sizeof(char))) == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    else {
        register Py_ssize_t i;
        register long n;
        register uchar c;
        register const Py_ssize_t size = *len;

        /* Convert each character. If a character is out of range then
         * quit and set the error flag.
         */
        for (i = 0; i < size; i++) {
            c = UREAD(kind, data, i);
            if (c < 127) {
                ascii[i] = (char) c;
            }
            else if ((n = Py_UNICODE_TODECIMAL(c)) > -1) {
                ascii[i] = '0' + (char) n;
            }
            else if (Py_UNICODE_ISSPACE(c)) {
                ascii[i] = ' ';
            }
            else {
                free(ascii);
                *error = true;
                return NULL;
            }
        }
        return ascii;
    }
}


#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 6)
/* In a generic number, valid underscores are between two digits.
 * A "based" number is necessarily an int, and in those cases the
 * definition of a valid underscore is a bit less well-defined.
 * If NULL is returned an unrecoverable memory error occurred.
 */
static const char *
remove_valid_underscores(char *str, const char **end, char **buffer,
                         const Py_ssize_t len, const bool based)
{
    register Py_ssize_t i, offset;

    /* If a buffer has not been created, do so now and copy data to it. */
    if (*buffer == NULL) {
        if ((*buffer = calloc(len + 1, sizeof(char))) == NULL) {
            PyErr_NoMemory();
            return NULL;
        }
        memcpy(*buffer, str, len);
        str = *buffer;
    }

    /* The method that will be used to remove underscores is to
     * travers the character array, and when a valid underscore
     * is found all characters will be shifted one to the left in
     * order to remove that underscore. Extra characters at the
     * end of the character array will be overwritten with \0.
     */

    /* For non-based strings, parsing is "simple" -
     * a valid underscore is surrounded by two numbers.
     */
    if (!based) {
        for (i = offset = 0; i < len; i++) {
            if (str[i] == '_' && i > 0 && i < len - 1 &&
                    is_valid_digit_char(str[i - 1]) &&
                    is_valid_digit_char(str[i + 1])) {
                offset += 1;
                continue;
            }
            if (offset) {
                str[i - offset] = str[i];
            }
        }
    }

    /* For based strings we must incorporate some state at the
     * beginning of the string before the more simple "surrounded
     * by two numbers" algorithm kicks in.
     */
    else {
        i = offset = 0;
        if (is_sign(str)) {
            i += 1;
        }
        /* Skip leading characters for non-base 10 ints. */
        if ((len - i) > 1 && str[i] == '0' &&
                (((str[i + 1] == 'x' || str[i + 1] == 'X')) ||
                 ((str[i + 1] == 'o' || str[i + 1] == 'O')) ||
                 ((str[i + 1] == 'b' || str[i + 1] == 'B')))) {
            /* An underscore after the prefix is allowed, e.g. 0x_d4. */
            if ((len - i > 2) && str[i + 2] == '_') {
                i += 3;
                offset += 1;
            }
            else {
                i += 2;
            }
        }
        /* No underscore in the base selector, e.g. 0_b0 is invalid. */
        else if ((len - i) > 2 && str[i] == '0' && str[i + 1] == '_' &&
                 (((str[i + 2] == 'x' || str[i + 2] == 'X')) ||
                  ((str[i + 2] == 'o' || str[i + 2] == 'O')) ||
                  ((str[i + 2] == 'b' || str[i + 2] == 'B')))) {
            i += 3;
        }

        /* Now search for simpler valid underscores.
         * use hex as the base because it is the most inclusive.
         */
        for (; i < len; i++) {
            if (str[i] == '_' && i > 0 && i < len - 1 &&
                    is_valid_digit_arbitrary_base(str[i - 1], 16) &&
                    is_valid_digit_arbitrary_base(str[i + 1], 16)) {
                offset += 1;
                continue;
            }
            if (offset) {
                str[i - offset] = str[i];
            }
        }
    }

    /* Update the end position. */
    *end = str + (i - offset);

    /* Fill the trailing data with nul characters. */
    for (i = len - offset; i < len; i++) {
        str[i] = '\0';
    }
    return str;
}
#endif /* Python >= 3.6 */


/* Try to convert the Python object to bytes (i.e. char*).
 * Possibly convert unicode to bytes object first.
 * If the string contains nul characters, return "\0".
 */
static const char *
convert_PyString_to_str(PyObject *input, const char **end,
                        char **buffer, bool *must_raise, const bool based)
{
    const char *str = NULL;
    Py_ssize_t len = 0;
    Py_buffer view = {NULL, NULL}; /* Reference to a buffer object */
    *buffer = NULL;
    *must_raise = false;

    /* If the input was in unicode format, extract as ASCII if we can. */
    if (PyUnicode_Check(input)) {
#if PY_MAJOR_VERSION >= 3
        /* Unicode in ASCII form is stored like bytes! */
        if (PyUnicode_IS_READY(input) && PyUnicode_IS_COMPACT_ASCII(input)) {
            str = (const char *) PyUnicode_1BYTE_DATA(input);
            len = PyUnicode_GET_LENGTH(input);
        }
        else
#endif
        {
            bool has_error = false;
            *buffer = PyUnicode_as_ascii_string(input, &len, &has_error);
            if (has_error) {
                return NULL;  /* Buffer is always NULL if here. */
            }
            if (*buffer == NULL) {
                *must_raise = true;
                return NULL;
            }
            str = *buffer;
        }
    }

    /* If the input is already bytes, then just extract the
     * underlying data and return.
     */
    else if (PyBytes_Check(input)) {
        str = PyBytes_AS_STRING(input);
        len = PyBytes_GET_SIZE(input);
    }

    /* Same for byte array. */
    else if (PyByteArray_Check(input)) {
        str = PyByteArray_AS_STRING(input);
        len = PyByteArray_GET_SIZE(input);
    }

    /* For a buffer, retrieve the bytes object then convert to bytes. */
    else if (PyObject_CheckBuffer(input) &&
             PyObject_GetBuffer(input, &view, PyBUF_SIMPLE) == 0) {
        /* This buffer could be a memoryview slice. If this is the case, the
         * nul termination of the string wil be past the given length, creating
         * unexpected parsing results. Rather than complicate the parsing and
         * adding more operations for a low probability event, a copy of the
         * slice will be made here and it will be implicitly nul terminated
         * by the use of calloc.
         */
        if ((*buffer = calloc(view.len + 1, sizeof(char))) == NULL) {
            PyErr_NoMemory();
            *must_raise = true;
            return NULL;
        }
        memcpy(*buffer, (char *) view.buf, view.len);
        str = *buffer;
        len = view.len;
        /* All we care about is the underlying buffer data, not the obj
         * which was allocated when we created the buffer. For this reason
         * it is safe to release the buffer here.
         */
        PyBuffer_Release(&view);
    }

    /* Return NULL now the data type was invalid. */
    if (str == NULL) {
        return NULL;
    }

    /* Pre-process the input, first by stripping whitespace. */
    strip_whitespace(str, *end, len);
    len = *end - str;

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 6)
    /* Remove all "valid" underscores from the
     * string to simplify downstream parsing.
     */
    if (len > 0 && memchr(str, '_', len)) {
        if ((str = remove_valid_underscores((char *)str,
                                            end, buffer,
                                            len, based)) == NULL) {
            *must_raise = true;
            free(*buffer);
            return NULL;
        }
    }
#endif

    return str;
}


/* Convert numbers in strings. */
PyObject *
PyString_to_PyNumber(PyObject *obj, const PyNumberType type,
                     const struct Options *options)
{
    const char *end;
    PyObject *pyresult = Py_None;  /* None indicates TypeError, not ValueError. */
    bool needs_raise = false;
    char *buf = NULL;
    const char *str = convert_PyString_to_str(obj, &end, &buf, &needs_raise,
                                              !Options_Default_Base(options));
    if (needs_raise) {
        /* Never need to free buffer if needs_raise is true. */
        return NULL;
    }

    /* If we could extract the string, convert it! */
    if (str != NULL) {
        switch (type) {
        case REAL:
            pyresult = str_to_PyInt_or_PyFloat(str, end, options);
            break;
        case FLOAT:
            pyresult = str_to_PyFloat(str, end, options);
            break;
        case INT:
            /* To maintain compatibility with Python,
             * explicit base MUST be a string-like type.
             */
            if (!Options_Default_Base(options) && !STRING_TYPE_CHECK(obj))
            {
                SET_ILLEGAL_BASE_ERROR(options);
                pyresult = NULL;
            }
            else if (Options_Default_Base(options) || options->base == 10) {
                pyresult = str_to_PyInt(str, end, options);
            }
            else {
                char *pend = "\0";
                pyresult = python_lib_str_to_PyInt(str, &pend, options->base);
                pyresult = handle_possible_conversion_error(end, pend,
                                                            pyresult,
                                                            options);
            }
            break;
        case FORCEINT:
        case INTLIKE:
            pyresult = str_to_PyInt_forced(str, end, options);
            break;
        }
    }

    free(buf);
    return pyresult;
}


/* Detect numbers in strings. */
PyObject *
PyString_is_number(PyObject *obj, const PyNumberType type,
                   const struct Options *options)
{
    const char *end;
    const int base = Options_Default_Base(options) ? 10 : options->base;
    bool result = false, needs_raise = false;
    char *buf = NULL;
    const char *str = convert_PyString_to_str(obj, &end, &buf, &needs_raise,
                                              !Options_Default_Base(options));
    if (needs_raise) {
        /* Never need to free buffer if needs_raise is true. */
        return NULL;
    }

    if (str != NULL) {
        switch (type) {
        case REAL:
        case FLOAT:
            result = string_contains_float(str, end,
                                           Options_Allow_Infinity(options),
                                           Options_Allow_NAN(options));
            break;
        case INT:
            result = string_contains_int(str, end, base);
            break;
        case FORCEINT:
        case INTLIKE:
            result = string_contains_intlike_float(str, end);
            break;
        }
    }
    else {
        /* Not a string. Never need to free if str was NULL. */
        return Py_None;
    }

    free(buf);
    return PyBool_from_bool(result);
}
