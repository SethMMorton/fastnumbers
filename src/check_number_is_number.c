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
    switch (type) {
    case REAL:
        return true;
    case FLOAT:
        return PyFloat_Check(obj);
    case INT:
        return PyNumber_IsInt(obj);
    case INTLIKE:
    case FORCEINT:
        return PyNumber_IsInt(obj) || PyFloat_is_Intlike(obj);
    }
    return false;  /* Silence GCC, will never be reached. */
}
