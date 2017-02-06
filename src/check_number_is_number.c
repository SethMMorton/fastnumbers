/*
 * Functions that will assess if a number is the right type.
 *
 * Author: Seth M. Morton
 *
 * January 2017
 */

#include "number_handling.h"

bool
PyNumber_is_type(PyObject *obj, const PyNumberType type)
{
    register bool result = false;
    switch (type) {
    case REAL:
        result = true; break;
    case FLOAT:
        result = PyFloat_Check(obj); break;
    case INT:
        result = PyNumber_IsInt(obj); break;
    case INTLIKE:
    case FORCEINT:
        result = PyNumber_IsInt(obj) || PyFloat_is_Intlike(obj); break;
    }
    return result;
}
