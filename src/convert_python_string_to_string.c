/*
 * Conversion from PyUnicode/PyBytes type to a string (character array).
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include <Python.h>
#include <string.h>
#include "string_handling.h"
#include "parsing.h"

/* The unicode handling methods in the C API essentially completely
 * changed starting with Python 3.3. The method that fastnumbers uses
 * to get a character array from the PyUnicode object is very different
 * between Python < 3.3 and Python >= 3.3. The older method is
 * much slower because it involves first converting to PyBytes type,
 * while the newer method can get the character array directly.
 */

/* PRE-PYTHON 3.3. */
#if (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 3) || PY_MAJOR_VERSION == 2

static const char*
PyUnicode_as_ascii_string(PyObject *obj, Py_ssize_t *len, PyObject **tobj, char **tchar)
{
    /* Attempt to convert to a PyBytes object encoded as
     * ASCII, and if possible extract the character array from
     * that object. The PyBytes object must stay alive till
     * we are done with the string to avoid the garbage collector
     * de-allocating our character array and causing a seg fault;
     * to do this we return the PyBytes object so it can be de-referenced
     * at a later time.
     *
     * If we cannot convert to ASCII, try to convert non-ASCII decimal
     * and whitespace characters and store in a character array.
     * We need to keep the character array alive because it was create dynamically.
     */
    *tobj = PyUnicode_AsASCIIString(obj);
    if (*tobj == NULL) {
        PyErr_Clear();
        *tchar = (char *) PyMem_MALLOC(PyUnicode_GET_SIZE(obj) + 1);
        if (*tchar == NULL) {
            PyErr_NoMemory();
            return NULL;
        }
        if (PyUnicode_EncodeDecimal(PyUnicode_AS_UNICODE(obj),
                                    PyUnicode_GET_SIZE(obj),
                                    *tchar, NULL))
        {
            PyErr_Clear();
            return NULL;
        }
        *len = strlen(*tchar);
        {
            /* If any characters are out of range, wimp out.
             * Shouldn't have to do this...
             */
            int i = 0;
            for (i = 0; i < *len; ++i) {
                if ((*tchar)[i] <= 0)
                    return NULL;
            }
        }
        return *tchar;
    }
    else {
        *len = PyBytes_GET_SIZE(*tobj);
        return PyBytes_AS_STRING(*tobj);
    }
}

/* PYTHON 3.3 AND AFTER. */
#else

static const char*
PyUnicode_as_ascii_string(PyObject *obj, Py_ssize_t *len, PyObject **tobj, char **tchar)
{
    /* If the input is not ASCII, return NULL.
     * Otherwise we return the character array.
     * The temporary object will stay alive till no longer
     * needed to avoid seg-faulting when accessing the character array.
     * The temporary character array is only used in Python 2.
     *
     */
     if (PyUnicode_IS_ASCII(obj)) {
        return PyUnicode_AsUTF8AndSize(obj, len);
     }
     else {
        *tobj = _PyUnicode_TransformDecimalAndSpaceToASCII(obj);
        if (*tobj == NULL || !PyUnicode_IS_ASCII(*tobj)) {
            PyErr_Clear();
            return NULL;
        }
        return PyUnicode_AsUTF8AndSize(*tobj, len);
     }
}

#endif


/*
 * Try to convert the Python object to bytes (i.e. char*).
 * Possibly convert unicode to bytes object first.
 * If the string contains nul characters, return "\0".
 */
const char*
convert_PyString_to_str(PyObject *input, const char** end, PyObject **temp_object, char **temp_char, Py_buffer *view)
{
    /* A reference to Python object can to be given to the
     * caller if it was created to ensure the character array is not
     * destroyed.
     */
    *temp_object = NULL;

    /* If the input was in unicode format, extract as ASCII if we can.
     */
    if (PyUnicode_Check(input)) {
        Py_ssize_t len = 0;
        const char* str = PyUnicode_as_ascii_string(input, &len, temp_object, temp_char);
        if (str == NULL) return NULL;
        strip_whitespace(str, *end, len);
        return str;
    }

    /* If the input is already bytes, then just extract the
     * underlying data and return.
     */
    else if (PyBytes_Check(input)) {
        const char *str = PyBytes_AS_STRING(input);
        const Py_ssize_t len = PyBytes_GET_SIZE(input);
        strip_whitespace(str, *end, len);
        return str;
    }

    /* Same for byte array.
     */
    else if (PyByteArray_Check(input)) {
        const char *str = PyByteArray_AS_STRING(input);
        const Py_ssize_t len = PyByteArray_GET_SIZE(input);
        strip_whitespace(str, *end, len);
        return str;
    }

    /* For a buffer, retrieve the bytes object then convert to bytes.
     */
    else if (PyObject_GetBuffer(input, view, PyBUF_SIMPLE) == 0) {
        /* Copy to NUL-terminated buffer. */
        *temp_object = PyBytes_FromStringAndSize((const char *) view->buf,
                                                  view->len);
        if (*temp_object == NULL) {
            PyBuffer_Release(view);
            return NULL;
        }
        {
            const char *str = PyBytes_AS_STRING(*temp_object);
            strip_whitespace(str, *end, view->len);
            return str;
        }
    }

    /* Return NULL if the data type was invalid. */
    else {
        PyErr_Clear();  // Protect against errors from PyObject_GetBuffer.
        return NULL;
    }
}
