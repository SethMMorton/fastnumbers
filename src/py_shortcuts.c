#include <stdlib.h>
#include <limits.h>
#include "py_shortcuts.h"
#include "fast_conversions.h"


bool double_is_intlike(const double val)
{
    if (val < LLONG_MAX && val > LLONG_MIN)
        return val == (long long) val;
    /* If the above won't work, convert to float type and return the result of "is_integer". */
    PyObject *pyval = PyFloat_FromDouble(val);
    if (pyval == NULL) return PyErr_Clear(), false;
    PyObject *intlike = PyObject_CallMethod(pyval, "is_integer", NULL);
    Py_DECREF(pyval);
    if (intlike == NULL) return PyErr_Clear(), false;
    const bool is_intlike = PyObject_IsTrue(intlike);
    Py_DECREF(intlike);
    return is_intlike;
}


bool PyFloat_is_Intlike(PyObject *obj)
{
    const double dval = PyFloat_AS_DOUBLE(obj);
    if (dval < LLONG_MAX && dval > LLONG_MIN)
        return dval == (long long) dval;
    PyObject *py_is_intlike = PyObject_CallMethod(obj, "is_integer", NULL);
    if (py_is_intlike == NULL) return PyErr_Clear(), false;
    const bool is_intlike = PyObject_IsTrue(py_is_intlike);
    Py_DECREF(py_is_intlike);
    return is_intlike;
}


inline static bool unicode_result_error(const double d, const long i, const PyNumberType type)
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
}


PyObject* PyNumber_to_PyNumber(PyObject *pynum, const PyNumberType type,
                               PyObject *inf_sub, PyObject *nan_sub)
{
    switch (type) {
    case REAL:
    case FLOAT:
        if (nan_sub != NULL && PyFloat_CheckExact(pynum) && Py_IS_NAN(PyFloat_AS_DOUBLE(pynum)))
            return Py_INCREF(nan_sub), nan_sub;
        if (inf_sub != NULL && PyFloat_CheckExact(pynum) && Py_IS_INFINITY(PyFloat_AS_DOUBLE(pynum)))
            return Py_INCREF(inf_sub), inf_sub;
        return type == REAL ? (Py_INCREF(pynum), pynum) : PyNumber_Float(pynum);
    default:
    {
        if (PyFloat_CheckExact(pynum)) { /* Watch out for un-intable numbers. */
            const double d = PyFloat_AS_DOUBLE(pynum);
            if (Py_IS_NAN(d) || Py_IS_INFINITY(d))
                return NULL;
        }
        return PyNumber_ToInt(pynum);
    }
    }
}


PyObject* PyUnicode_to_PyNumber(const Py_UCS4 uni, const PyNumberType type)
{
    const double dresult = Py_UNICODE_TONUMERIC(uni);
    const long iresult = Py_UNICODE_TODIGIT(uni);
    if (unicode_result_error(dresult, iresult, type))
        return NULL;
    switch (type) {
    case REAL:
        return iresult > -1 ? long_to_PyInt(iresult) : PyFloat_FromDouble(dresult);
    case FLOAT:
        return PyFloat_FromDouble(dresult);
    case INT:
        return long_to_PyInt(iresult);
    default: /* FORCEINT */
    {
        if (iresult > -1)
            return long_to_PyInt(iresult);
        PyObject *pyfloat = PyFloat_FromDouble(dresult);
        if (pyfloat == NULL) return NULL;
        PyObject *pyint = PyNumber_ToInt(pyfloat);
        Py_DECREF(pyfloat);
        return pyint;
    }
    }
}


static PyObject* convert_PyFloat_to_PyInt(PyObject * fobj)
{
    if (fobj == NULL) return NULL;
    PyObject *tmp = PyNumber_to_PyNumber(fobj, INT, NULL, NULL);
    Py_DECREF(fobj);
    return tmp;
}



PyObject* str_to_PyNumber(const char* str, const PyNumberType type,
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
            pyresult = overflow ? str_to_PyInt((char*) str) : long_to_PyInt(result);        
        break;
    }
    default: /* FORCEINT*/
    {
        pyresult = str_to_PyNumber(str, REAL, inf_sub, nan_sub);
        if (pyresult != NULL && PyFloat_CheckExact(pyresult))
            pyresult = convert_PyFloat_to_PyInt(pyresult);
    }
    }
    return pyresult;
}


inline static PyObject* PyUnicode_is_a_number(PyObject *obj, const PyNumberType type)
{
    switch (type) {
    case REAL:
    case FLOAT:
        return PyBool_from_bool(PyUnicode_is_float(obj));
    case INT:
        return PyBool_from_bool(PyUnicode_is_int(obj));
    case FORCEINT:
    case INTLIKE:
        return PyBool_from_bool(PyUnicode_is_intlike(obj));
    }
}

PyObject* PyString_is_a_number(PyObject *obj, const PyNumberType type,
                               PyObject *allow_inf, PyObject *allow_nan)
{
    PyObject *bytes = NULL;
    const char *str = convert_PyString_to_str(obj, &bytes);
    if (str == NULL)
        return PyUnicode_is_a_number(obj, type);
    }
    bool result = false;
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


PyObject* PyObject_to_PyNumber(PyObject *obj, const PyNumberType type,
                               PyObject *inf_sub, PyObject *nan_sub)
{
    if (PyNumber_Check(obj))
        return PyNumber_to_PyNumber(obj, type, inf_sub, nan_sub);

    PyObject *bytes = NULL;
    const char *str = convert_PyString_to_str(obj, &bytes);
    if (str != NULL) {
        PyObject *pyreturn = str_to_PyNumber(str, type, inf_sub, nan_sub);
        Py_XDECREF(bytes);
        return pyreturn;
    }

    const Py_UCS4 uni = convert_PyUnicode_to_unicode_char(obj);
    if (uni != NULL_UNI)
        return PyUnicode_to_PyNumber(uni, type);

    return Py_None;
}
