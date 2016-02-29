#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "parsing.h"
#include "py_shortcuts.h"
#include "fast_conversions.h"


#ifdef _MSC_VER
#if _MSC_VER < 1800
/* Compensate for missing round in MSVC */
static double
round(double val) {    
    return (val > 0.0) ? floor(val + 0.5) : floor(val - 0.5); 
}
#endif
#endif


PyObject*
PyBool_from_bool_and_DECREF(const bool b, PyObject *obj)
{
    Py_XDECREF(obj);
    return PyBool_from_bool(b);
}


static bool
_PyFloat_is_Intlike(PyObject *obj) {
    PyObject *py_is_intlike = PyObject_CallMethod(obj, "is_integer", NULL);
    if (py_is_intlike == NULL)
        return PyErr_Clear(), false;
    else {            
        const bool is_intlike = PyObject_IsTrue(py_is_intlike);
        Py_DECREF(py_is_intlike);
        return is_intlike;
    }    
}


static bool
double_is_intlike(const double val)
{
    if (val < LONG_MAX && val > LONG_MIN)
        return val == round(val);
    else {
        PyObject *pyval = PyFloat_FromDouble(val);
        if (pyval == NULL)
            return PyErr_Clear(), false;
        return _PyFloat_is_Intlike(pyval);
    }
}


static bool
PyFloat_is_Intlike(PyObject *obj)
{
    const double dval = PyFloat_AS_DOUBLE(obj);
    if (dval < LONG_MAX && dval > LONG_MIN)
        return dval == round(dval);
    return _PyFloat_is_Intlike(obj);
}


static bool
unicode_result_error(const double d, const long i, const PyNumberType type)
{
    switch (type) {
    case REAL:
    case FLOAT:
        return d <= -1.0;
    case INT:
    case INTLIKE:
        return i <= -1;
    case FORCEINT:
        return i <= -1 && d <= -1.0;
    }
    return false;  /* Silence GCC */
}


static PyObject*
PyNumber_to_PyNumber(PyObject *pynum, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub)
{
    switch (type) {
    case REAL:
    case FLOAT:
        if (nan_sub != NULL &&
            PyFloat_CheckExact(pynum) &&
            Py_IS_NAN(PyFloat_AS_DOUBLE(pynum)))
            return Py_INCREF(nan_sub), nan_sub;
        if (inf_sub != NULL &&
            PyFloat_CheckExact(pynum) &&
            Py_IS_INFINITY(PyFloat_AS_DOUBLE(pynum)))
            return Py_INCREF(inf_sub), inf_sub;
        return type == REAL ? (Py_INCREF(pynum), pynum) : PyNumber_Float(pynum);
    case INT:
    case INTLIKE:
    case FORCEINT:
    {
        if (PyFloat_CheckExact(pynum)) { /* Watch out for un-intable numbers. */
            const double d = PyFloat_AS_DOUBLE(pynum);
            if (Py_IS_NAN(d) || Py_IS_INFINITY(d))
                return NULL;
        }
        return PyNumber_ToInt(pynum);
    }
    }
    return NULL;  /* Silence GCC */
}


static PyObject*
PyUnicode_to_PyNumber(const Py_UCS4 uni, const PyNumberType type)
{
    const double dresult = Py_UNICODE_TONUMERIC((Py_UNICODE) uni);
    const long iresult = Py_UNICODE_TODIGIT((Py_UNICODE) uni);
    if (unicode_result_error(dresult, iresult, type))
        return NULL;
    switch (type) {
    case REAL:
        return iresult > -1 ? long_to_PyInt(iresult)
                            : PyFloat_FromDouble(dresult);
    case FLOAT:
        return PyFloat_FromDouble(dresult);
    case INT:
        return long_to_PyInt(iresult);
    case INTLIKE:
    case FORCEINT:
        if (iresult > -1)
            return long_to_PyInt(iresult);
        else {
            PyObject *pyfloat = PyFloat_FromDouble(dresult);
            if (pyfloat == NULL)
                return NULL;
            else {
                PyObject *pyint = PyNumber_ToInt(pyfloat);
                Py_DECREF(pyfloat);
                return pyint;
            }
        }
    }
    return NULL;  /* Silence GCC */
}


static PyObject*
convert_PyFloat_to_PyInt(PyObject * fobj)
{
    if (fobj == NULL)
        return NULL;
    else {
        PyObject *tmp = PyNumber_to_PyNumber(fobj, INT, NULL, NULL);
        Py_DECREF(fobj);
        return tmp;
    }
}


static PyObject*
str_to_PyNumber(const char* str, const PyNumberType type,
                PyObject *inf_sub, PyObject *nan_sub)
{
    bool error = false, overflow = false;
    PyObject *pyresult = NULL;
    switch (type) {
    case REAL:
    {
        const PyNumberType t = string_contains_integer(str) ? INT : FLOAT;
        pyresult = str_to_PyNumber(str, t, inf_sub, nan_sub);
        if (pyresult != NULL && t == FLOAT && PyFloat_CheckExact(pyresult))
            if (string_contains_intlike_float(str))
                pyresult = convert_PyFloat_to_PyInt(pyresult);
        break;
    }
    case FLOAT:
    {
        double result = parse_float_from_string(str, &error, &overflow);
        if (!error) {
            if (overflow)
                result = strtod(str, NULL);
            if (inf_sub != NULL && Py_IS_INFINITY(result)) {
                pyresult = inf_sub;
                Py_INCREF(pyresult);
            }
            else if (nan_sub != NULL && Py_IS_NAN(result)) {
                pyresult = nan_sub;
                Py_INCREF(pyresult);
            }
            else
                pyresult = PyFloat_FromDouble(result);
        }
        break;
    }
    case INT:
    {
        long result = parse_integer_from_string(str, &error, &overflow);
        if (!error)
            pyresult = overflow ? str_to_PyInt((char*) str)
                                : long_to_PyInt(result);        
        break;
    }
    case INTLIKE:
    case FORCEINT:
    {
        pyresult = str_to_PyNumber(str, REAL, inf_sub, nan_sub);
        if (pyresult != NULL && PyFloat_CheckExact(pyresult))
            pyresult = convert_PyFloat_to_PyInt(pyresult);
    }
    }
    return pyresult;
}


static bool
PyUnicode_is_int(PyObject *obj)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (uni == NULL_UNI)
        return false;
    return Py_UNICODE_ISDIGIT((Py_UNICODE) uni);
}


static bool
PyUnicode_is_intlike(PyObject *obj)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (uni == NULL_UNI)
        return false;
    else if (Py_UNICODE_ISDIGIT((Py_UNICODE) uni))
        return true;
    else {
        const double val = Py_UNICODE_TONUMERIC((Py_UNICODE) uni);
        if (val <= -1.0)
            return false;  /* Lowerbound of values is > -1.0. */
        return double_is_intlike(val);
    }
}


static bool
PyUnicode_is_float(PyObject *obj)
{
    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (uni == NULL_UNI)
        return false;
    return Py_UNICODE_ISNUMERIC((Py_UNICODE) uni);
}


static PyObject*
PyUnicode_is_a_number(PyObject *obj, const PyNumberType type)
{
    switch (type) {
    case REAL:
    case FLOAT:
        return PyBool_from_bool(PyUnicode_is_float(obj));
    case INT:
        return PyBool_from_bool(PyUnicode_is_int(obj));
    case INTLIKE:
    case FORCEINT:
        return PyBool_from_bool(PyUnicode_is_intlike(obj));
    }
    return NULL;  /* Silence GCC */
}


bool
PyNumber_is_correct_type(PyObject *obj,
                         const PyNumberType type,
                         PyObject *str_only)
{
    if (PyObject_IsTrue(str_only)) return false;
    switch (type) {
    case REAL:
        return true;
    case FLOAT:
        return PyFloat_CheckExact(obj);
    case INT:
        return PyNumber_IsInt(obj);
    case INTLIKE:
    case FORCEINT:
        return PyNumber_IsInt(obj) || PyFloat_is_Intlike(obj);
    }
    return false;  /* Silence GCC */
}


PyObject*
PyString_is_a_number(PyObject *obj, const PyNumberType type,
                     PyObject *allow_inf, PyObject *allow_nan)
{
    bool result = false;
    PyObject *bytes = NULL;
    const char *str = convert_PyString_to_str(obj, &bytes);
    if (str == NULL)
        return PyUnicode_is_a_number(obj, type);
    else if (is_null(str))  /* Str contained null characters. */
        Py_RETURN_FALSE;
    switch (type) {
    case REAL:
    case FLOAT:
        result = string_contains_float(str, PyObject_IsTrue(allow_inf),
                                            PyObject_IsTrue(allow_nan));
        break;
    case INT:
        result = string_contains_integer(str);
        break;
    case FORCEINT:
    case INTLIKE:
        result = string_contains_intlike_float(str);
        break;
    }
    return PyBool_from_bool_and_DECREF(result, bytes);
}


PyObject*
PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                     PyObject *inf_sub, PyObject *nan_sub)
{
    PyObject *bytes = NULL;
    if (PyNumber_Check(obj))
        return PyNumber_to_PyNumber(obj, type, inf_sub, nan_sub);
    else {
        const char *str = convert_PyString_to_str(obj, &bytes);
        if (str != NULL) {
            PyObject *pyreturn = is_null(str)
                               ? NULL
                               : str_to_PyNumber(str, type, inf_sub, nan_sub);
            Py_XDECREF(bytes);
            return pyreturn;
        }
        else {
            const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
            if (uni != NULL_UNI)
                return PyUnicode_to_PyNumber(uni, type);
        }
    }

    return Py_None;
}
