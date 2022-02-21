from cpython.ref cimport PyObject

from options cimport (
    Options,
    Options_Coerce_True,
    Options_Default_Base,
    Options_Number_Only,
    Options_String_Only,
    PyNumberType,
)


cdef extern from "fastnumbers/numbers.h":
    bint PyFloat_is_Intlike(PyObject *obj)


cdef extern from "fastnumbers/strings.h":
    PyObject * PyString_contains_type(PyObject *obj, const Options *options)
    PyObject * PyString_is_number(PyObject *obj, const PyNumberType type, const Options *options)


cdef extern from "fastnumbers/unicode_character.h":
    PyObject * PyUnicodeCharacter_contains_type(PyObject *obj)
    PyObject * PyUnicodeCharacter_is_number(PyObject *obj, const PyNumberType type)


cdef object_is_number(obj, const PyNumberType type, const Options *options):
    """Check if an arbitrary PyObject is a PyNumber."""

    # Already a number? Simple checks will work.
    if isinstance(obj, int):
        return not Options_String_Only(options) and type != PyNumberType.FLOAT
    elif isinstance(obj, float):
        if Options_String_Only(options):
            return False
        if type == PyNumberType.INTLIKE or type == PyNumberType.FORCEINT:
            return PyFloat_is_Intlike(<PyObject *> obj)
        elif type == PyNumberType.REAL or type == PyNumberType.FLOAT:
            return True
        else:
            return False

    # If we are requiring it to be a number then we must declare false now.
    elif Options_Number_Only(options):
        return False

    # Assume a string.
    cdef object pyresult
    pyresult = <object> PyString_is_number(<PyObject *> obj, type, options)
    if (pyresult is not None):
        return pyresult

    # Assume unicode.
    pyresult = <object> PyUnicodeCharacter_is_number(<PyObject *> obj, type)
    if (pyresult is not None):
        return pyresult

    # If we got here, the type was invalid so return False.
    return False


cdef check_potential_object_type(obj, const Options *options):
    """Return the type contained the object."""

    # Already a number? Just return the type directly.
    if isinstance(obj, int):
        return int
    elif isinstance(obj, float):
        if Options_Coerce_True(options) and PyFloat_is_Intlike(<PyObject *> obj):
            return int
        return float

    # Assume a string.
    cdef object pyresult
    pyresult = <object> PyString_contains_type(<PyObject *> obj, options)
    if (pyresult is not None):
        return pyresult

    # Assume unicode.
    pyresult = <object> PyUnicodeCharacter_contains_type(<PyObject *> obj)
    if (pyresult is not None):
        return pyresult

    # If we got here, the type was invalid.
    return type(obj)
