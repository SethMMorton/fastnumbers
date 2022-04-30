#include <Python.h>

#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/docstrings.hpp"
#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/version.hpp"

// Infinity and NaN can be cached at startup
static PyObject* POS_INFINITY;
static PyObject* NEG_INFINITY;
static PyObject* POS_NAN;
static PyObject* NEG_NAN;

/// Increment the refcount of a non-null object, then return the object
PyObject* increment_reference(PyObject* obj)
{
    Py_IncRef(obj);
    return obj;
}

/**
 * \brief Function to handle the conversion of base to integers.
 *
 * \param pybase The base as a Python object
 * \param base The base as an integer
 * \returns 0 is success, 1 is failure.
 */
int assess_integer_base_input(PyObject* pybase, int& base)
{
    Py_ssize_t longbase = 0;

    // Default to INT_MIN
    if (pybase == nullptr) {
        base = INT_MIN;
        return 0;
    }

    // Convert to int and check for overflow
    longbase = PyNumber_AsSsize_t(pybase, nullptr);
    if (longbase == -1 && PyErr_Occurred()) {
        return 1;
    }

    // Ensure valid integer in valid range
    if ((longbase != 0 && longbase < 2) || longbase > 36) {
        PyErr_SetString(PyExc_ValueError, "int() base must be >= 2 and <= 36");
        return 1;
    }
    base = static_cast<int>(longbase);
    return 0;
}

/**
 * \brief If key is defined, move the value to on_fail.
 *
 * If both were defined, error.
 *
 * \param on_fail
 * \param key
 * \returns 0 is success, 1 is failure.
 */
int handle_key_backwards_compatibility(PyObject*& on_fail, PyObject*& key)
{
    if (key != nullptr) {
        if (on_fail != nullptr) {
            PyErr_SetString(PyExc_ValueError, "Cannot set both on_fail and key");
            return 1;
        }
        on_fail = key;
        key = nullptr;
    }
    return 0;
}

/// Return the best return value for when an error occurs
PyObject* failure_return_value(PyObject* obj, bool raise_on_invalid, PyObject* default_)
{
    if (raise_on_invalid) {
        return nullptr;
    } else if (default_ != nullptr) {
        return default_;
    } else {
        return obj;
    }
}

/// Prepare and raise the appropriate exception given an action type
PyObject*
raise_appropriate_exception(PyObject* obj, const ActionType atype, Evaluator& evaluator)
{
    switch (atype) {
    case ActionType::ERROR_BAD_TYPE_INT:
        // Raise an exception due passing an invalid type to convert to an integer
        PyErr_Format(
            PyExc_TypeError,
            "int() argument must be a string, a bytes-like object or a number, not "
            "'%s'",
            Py_TYPE(obj)->tp_name
        );
        break;

    case ActionType::ERROR_BAD_TYPE_FLOAT:
        // Raise an exception due passing an invalid type to convert to a float
        PyErr_Format(
            PyExc_TypeError,
            "float() argument must be a string or a number, not '%s'",
            Py_TYPE(obj)->tp_name
        );
        break;

    case ActionType::ERROR_ILLEGAL_EXPLICIT_BASE: // TODO - duplciate
        // Raise an exception due to useing an explict integer base where it shouldn't
        PyErr_SetString(
            PyExc_TypeError, "int() can't convert non-string with explicit base"
        );
        break;

    case ActionType::ERROR_INVALID_INT:
        // Raise an exception due to an invalid integer
        PyErr_Format(
            PyExc_ValueError,
            "invalid literal for int() with base %d: %.200R",
            evaluator.get_base(),
            obj
        );
        break;

    case ActionType::ERROR_INVALID_FLOAT:
        // Raise an exception due to an invalid float
        PyErr_Format(PyExc_ValueError, "could not convert string to float: %.200R", obj);
        break;

    case ActionType::ERROR_INVALID_BASE: // TODO - duplciate
        // Raise an exception due to an invalid base for integer conversion
        PyErr_SetString(
            PyExc_TypeError, "int() can't convert non-string with explicit base"
        );
        break;

    case ActionType::ERROR_INFINITY_TO_INT:
        // Raise an exception due to attempting to convert infininty to an integer
        PyErr_SetString(PyExc_OverflowError, "cannot convert float infinity to integer");
        break;

    case ActionType::ERROR_NAN_TO_INT:
        // Raise an exception due to attempting to convert NaN to an integer
        PyErr_SetString(PyExc_ValueError, "cannot convert float NaN to integer");

    default:
        Py_UNREACHABLE();
    }

    return nullptr;
}

/// Convert the Payload of an Evaluator into a PyNumber
PyObject* convert_evaluator_payload(
    PyObject* obj,
    Evaluator& evaluator,
    const UserType ntype,
    PyObject* infinity,
    PyObject* nan,
    PyObject* return_object,
    PyObject* on_fail
)
{
    const Payload payload = evaluator.as_type(ntype);

    // Level 1: If the payload contains an actual number,
    //          convert to PyObject directly
    switch (payload.payload_type()) {
    case PayloadType::LONG:
        return PyLong_FromLong(payload.to_long());

    case PayloadType::DOUBLE:
        return PyFloat_FromDouble(payload.to_double());

    case PayloadType::DOUBLE_TO_LONG:
        return PyLong_FromDouble(payload.to_double());

    case PayloadType::PYOBJECT: {
        PyObject* retval = payload.to_pyobject();
        if (retval == nullptr) {
            if (return_object == nullptr) {
                return nullptr;
            }

            PyErr_Clear();

            if (on_fail != nullptr) {
                return PyObject_CallFunctionObjArgs(on_fail, return_object, nullptr);
            } else {
                return increment_reference(return_object);
            }
        }
        return increment_reference(retval);
    }

    // Level 2: We need to instruct Cython as to what action to take
    case PayloadType::ACTION: {
        const ActionType atype = payload.get_action();

        switch (atype) {
        // Return the appropriate value for when infinity is found
        case ActionType::INF_ACTION:
            return increment_reference(infinity == nullptr ? POS_INFINITY : infinity);

        // Return the appropriate value for when negative infinity is found
        case ActionType::NEG_INF_ACTION:
            return increment_reference(infinity == nullptr ? NEG_INFINITY : infinity);

        // Return the appropriate value for when NaN is found
        case ActionType::NAN_ACTION:
            return increment_reference(nan == nullptr ? POS_NAN : nan);

        // Return the appropriate value for when negative NaN is found
        case ActionType::NEG_NAN_ACTION:
            return increment_reference(nan == nullptr ? NEG_NAN : nan);

        // Raise an exception due passing an invalid type to convert to
        // an integer or float, or if using an explicit integer base
        // where it shouldn't be used
        case ActionType::ERROR_BAD_TYPE_INT:
        case ActionType::ERROR_BAD_TYPE_FLOAT:
        case ActionType::ERROR_ILLEGAL_EXPLICIT_BASE:
            return raise_appropriate_exception(obj, atype, evaluator);

        default:
            // Raise an exception if that is what the user has asked for, otherwise
            // transform the input via a function, otherwise return the input as-is
            if (return_object == nullptr) {
                return raise_appropriate_exception(obj, atype, evaluator);
            }

            PyErr_Clear();

            if (on_fail != nullptr) {
                return PyObject_CallFunctionObjArgs(on_fail, return_object, nullptr);
            } else {
                return increment_reference(return_object);
            }
        }
    }
    }

    Py_UNREACHABLE();
}

/// Check if an arbitrary PyObject is a PyNumber.
PyObject* object_is_number(
    PyObject* obj,
    const UserType type,
    int base,
    bool allow_nan,
    bool allow_inf,
    bool str_only,
    bool num_only,
    bool allow_underscores
)
{

    // Create the evaluator and populate with the appropriate options
    Evaluator evaluator(obj);
    evaluator.set_base(base);
    evaluator.set_nan_allowed(allow_nan);
    evaluator.set_inf_allowed(allow_inf);
    evaluator.set_underscores_allowed(allow_underscores);

    // If the user explictly asked to disallow some types, check that here.
    switch (evaluator.parser_type()) {
    case ParserType::NUMERIC:
        if (str_only) {
            Py_RETURN_FALSE;
        }
        evaluator.set_nan_allowed(true);
        evaluator.set_inf_allowed(true);
        break;
    case ParserType::UNICODE:
    case ParserType::CHARACTER:
        if (num_only) {
            Py_RETURN_FALSE;
        }
        break;
    default:
        Py_RETURN_FALSE;
    }

    // Evaluate the type!
    return PyBool_FromLong(evaluator.is_type(type));
}

/// Ensure the type is allowed, otherwise return None
PyObject* validate_query_type(PyObject* result, PyObject* allowed_types)
{
    if (allowed_types != nullptr and !PySequence_Contains(allowed_types, result)) {
        Py_RETURN_NONE;
    }
    return increment_reference(result);
}

// Quickly convert to an int or float, depending on value
static PyObject* fastnumbers_fast_real(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = nullptr;
    PyObject* nan = nullptr;
    int raise_on_invalid = false;
    int coerce = true;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x",   "default", "raise_on_invalid",  "on_fail", "inf",
            "nan", "coerce",  "allow_underscores", "key",     nullptr };
    static const char* format = "O|O$pOOOppO:fast_real";

    // Read the function argument
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &default_value,
            &raise_on_invalid,
            &on_fail,
            &inf,
            &nan,
            &coerce,
            &allow_underscores,
            &key
        )) {
        return nullptr;
    }
    if (handle_key_backwards_compatibility(on_fail, key)) {
        return nullptr;
    }

    Evaluator evaluator(input);
    evaluator.set_coerce(coerce);
    evaluator.set_underscores_allowed(allow_underscores);
    PyObject* on_invalid = failure_return_value(input, raise_on_invalid, default_value);
    return convert_evaluator_payload(
        input, evaluator, UserType::REAL, inf, nan, on_invalid, on_fail
    );
}

/* Quickly convert to a float, depending on value. */
static PyObject* fastnumbers_fast_float(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = nullptr;
    PyObject* nan = nullptr;
    int raise_on_invalid = false;
    int allow_underscores = true;
    static const char* keywords[] = { "x",    "default", "raise_on_invalid",  "on_fail",
                                      "inf",  "nan",     "allow_underscores", "key",
                                      nullptr };
    static const char* format = "O|O$pOOOpO:fast_float";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &default_value,
            &raise_on_invalid,
            &on_fail,
            &inf,
            &nan,
            &allow_underscores,
            &key
        )) {
        return nullptr;
    }
    if (handle_key_backwards_compatibility(on_fail, key)) {
        return nullptr;
    }

    Evaluator evaluator(input);
    evaluator.set_underscores_allowed(allow_underscores);
    PyObject* on_invalid = failure_return_value(input, raise_on_invalid, default_value);
    return convert_evaluator_payload(
        input, evaluator, UserType::FLOAT, inf, nan, on_invalid, on_fail
    );
}

/* Quickly convert to an int, depending on value. */
static PyObject* fastnumbers_fast_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* pybase = nullptr;
    int raise_on_invalid = false;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x",   "default", "raise_on_invalid", "on_fail", "base", "allow_underscores",
            "key", nullptr

          };
    static const char* format = "O|O$pOOpO:fast_int";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &default_value,
            &raise_on_invalid,
            &on_fail,
            &pybase,
            &allow_underscores,
            &key
        )) {
        return nullptr;
    }
    if (handle_key_backwards_compatibility(on_fail, key)) {
        return nullptr;
    }

    int base = INT_MIN;
    if (assess_integer_base_input(pybase, base)) {
        return nullptr;
    }

    Evaluator evaluator(input);
    evaluator.set_underscores_allowed(allow_underscores);
    evaluator.set_base(base);
    evaluator.set_unicode_allowed(evaluator.is_default_base());
    PyObject* on_invalid = failure_return_value(input, raise_on_invalid, default_value);
    return convert_evaluator_payload(
        input, evaluator, UserType::INT, nullptr, nullptr, on_invalid, on_fail
    );
}

/* Safely convert to an int (even if in a string and as a float). */
static PyObject*
fastnumbers_fast_forceint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    int raise_on_invalid = false;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x",   "default", "raise_on_invalid", "on_fail", "allow_underscores",
            "key", nullptr

          };
    static const char* format = "O|O$pOpO:fast_forceint";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &default_value,
            &raise_on_invalid,
            &on_fail,
            &allow_underscores,
            &key
        )) {
        return nullptr;
    }
    if (handle_key_backwards_compatibility(on_fail, key)) {
        return nullptr;
    }

    Evaluator evaluator(input);
    evaluator.set_underscores_allowed(allow_underscores);
    PyObject* on_invalid = failure_return_value(input, raise_on_invalid, default_value);
    return convert_evaluator_payload(
        input, evaluator, UserType::FORCEINT, nullptr, nullptr, on_invalid, on_fail
    );
}

/* Quickly determine if the input is a real. */
static PyObject* fastnumbers_isreal(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_inf = false;
    int allow_nan = false;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x",         "str_only",          "num_only", "allow_inf",
            "allow_nan", "allow_underscores", nullptr };
    static const char* format = "O|$ppppp:isreal";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &str_only,
            &num_only,
            &allow_inf,
            &allow_nan,
            &allow_underscores
        )) {
        return nullptr;
    }

    return object_is_number(
        input,
        UserType::REAL,
        INT_MIN,
        allow_nan,
        allow_inf,
        str_only,
        num_only,
        allow_underscores
    );
}

/* Quickly determine if the input is a float. */
static PyObject* fastnumbers_isfloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_inf = false;
    int allow_nan = false;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x",         "str_only",          "num_only", "allow_inf",
            "allow_nan", "allow_underscores", nullptr };
    static const char* format = "O|$ppppp:isfloat";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &str_only,
            &num_only,
            &allow_inf,
            &allow_nan,
            &allow_underscores
        )) {
        return nullptr;
    }

    return object_is_number(
        input,
        UserType::FLOAT,
        INT_MIN,
        allow_nan,
        allow_inf,
        str_only,
        num_only,
        allow_underscores
    );
}

/* Quickly determine if the input is an int. */
static PyObject* fastnumbers_isint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* pybase = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x", "str_only", "num_only", "base", "allow_underscores", nullptr };
    static const char* format = "O|$ppOp:isint";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &str_only,
            &num_only,
            &pybase,
            &allow_underscores
        )) {
        return nullptr;
    }

    int base = INT_MIN;
    if (assess_integer_base_input(pybase, base)) {
        return nullptr;
    }

    return object_is_number(
        input, UserType::INT, base, false, false, str_only, num_only, allow_underscores
    );
}

/* Quickly determine if the input is int-like. */
static PyObject* fastnumbers_isintlike(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x", "str_only", "num_only", "allow_underscores", nullptr };
    static const char* format = "O|$ppp:isintlike";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &str_only,
            &num_only,
            &allow_underscores
        )) {
        return nullptr;
    }

    return object_is_number(
        input,
        UserType::INTLIKE,
        INT_MIN,
        false,
        false,
        str_only,
        num_only,
        allow_underscores
    );
}

/* Quickly detect the type. */
static PyObject* fastnumbers_query_type(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* allowed_types = nullptr;
    int coerce = false;
    int allow_inf = false;
    int allow_nan = false;
    int allow_underscores = true;
    static const char* keywords[] = { "x",      "allow_inf",     "allow_nan",
                                      "coerce", "allowed_types", "allow_underscores",
                                      nullptr };
    static const char* format = "O|$pppOp:type";

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            format,
            const_cast<char**>(keywords),
            &input,
            &allow_inf,
            &allow_nan,
            &coerce,
            &allowed_types,
            &allow_underscores
        )) {
        return nullptr;
    }
    // Allowed types must be a non-empty sequence.
    if (allowed_types != nullptr) {
        if (!PySequence_Check(allowed_types)) {
            PyErr_Format(
                PyExc_TypeError, "allowed_type is not a sequence type: %R", allowed_types
            );
            return nullptr;
        }
        if (PySequence_Length(allowed_types) < 1) {
            PyErr_SetString(
                PyExc_ValueError, "allowed_type must not be an empty sequence"
            );
            return nullptr;
        }
    }

    // Create the evaluator and populate with the appropriate options
    Evaluator evaluator(input);
    evaluator.set_coerce(coerce);
    evaluator.set_underscores_allowed(allow_underscores);
    if (evaluator.parser_type() == ParserType::NUMERIC) {
        evaluator.set_nan_allowed(true);
        evaluator.set_inf_allowed(true);
    } else {
        evaluator.set_nan_allowed(allow_nan);
        evaluator.set_inf_allowed(allow_inf);
    }

    // Check that the found type allowed, and return
    if (evaluator.type_is_int()) {
        return validate_query_type((PyObject*)&PyLong_Type, allowed_types);
    } else if (evaluator.type_is_float()) {
        return validate_query_type((PyObject*)&PyFloat_Type, allowed_types);
    } else {
        return validate_query_type((PyObject*)Py_TYPE(input), allowed_types);
    }
}

/* Drop-in replacement for int, float */
static PyObject* fastnumbers_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* pybase = nullptr;
    static const char* format = "|OO:int";

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
    /* Do not accept number as a keyword argument. */
    static const char* keywords[] = { "", "base", nullptr };
#else
    /* Do accept number as a keyword argument. */
    static const char* keywords[] = { "x", "base", nullptr };
#endif

    /* Read the function argument */
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, format, const_cast<char**>(keywords), &input, &pybase
        )) {
        return nullptr;
    }
    /* No arguments returns 0. */
    if (input == nullptr) {
        if (pybase != nullptr) {
            PyErr_SetString(PyExc_TypeError, "int() missing string argument");
            return nullptr;
        }
        return PyLong_FromLong(0);
    }

    int base = INT_MIN;
    if (assess_integer_base_input(pybase, base)) {
        return nullptr;
    }

    Evaluator evaluator(input);
    evaluator.set_base(base);
    evaluator.set_unicode_allowed(false);
    return convert_evaluator_payload(
        input, evaluator, UserType::INT, nullptr, nullptr, nullptr, nullptr
    );
}

static PyObject*
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
fastnumbers_float(PyObject* self, PyObject* args)
#else
fastnumbers_float(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    PyObject* input = nullptr;
    static const char* format = "|O:float";

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
    /* Read the function argument - do not accept it as a keyword argument. */
    if (!PyArg_ParseTuple(args, format, &input)) {
        return nullptr;
    }
#else
    static const char* keywords[] = { "x", nullptr };

    /* Read the function argument - accept it as a keyword argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, format, const_cast<char**>(keywords), &input
        )) {
        return nullptr;
    }
#endif

    /* No arguments returns 0.0. */
    if (input == nullptr) {
        return PyFloat_FromDouble(0.0);
    }
    Evaluator evaluator(input);
    evaluator.set_unicode_allowed(false);
    return convert_evaluator_payload(
        input, evaluator, UserType::FLOAT, nullptr, nullptr, nullptr, nullptr
    );
}

/* Behaves like float or int, but returns correct type. */
static PyObject* fastnumbers_real(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    int coerce = true;
    static const char* format = "|O$p:real";

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
    /* Do not accept number as a keyword argument. */
    static const char* keywords[] = { "", "coerce", nullptr };
#else
    /* Do accept number as a keyword argument. */
    static const char* keywords[] = { "x", "coerce", nullptr };
#endif

    /* Read the function argument. */
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, format, const_cast<char**>(keywords), &input, &coerce
        )) {
        return nullptr;
    }
    /* No arguments returns 0.0 or 0 depending on the state of coerce. */
    if (input == nullptr) {
        return coerce ? PyLong_FromLong(0) : PyFloat_FromDouble(0.0);
    }
    Evaluator evaluator(input);
    evaluator.set_coerce(coerce);
    evaluator.set_unicode_allowed(false);
    return convert_evaluator_payload(
        input, evaluator, UserType::REAL, nullptr, nullptr, nullptr, nullptr
    );
}

/* This defines the methods contained in this module. */
static PyMethodDef FastnumbersMethods[] = {
    { "fast_real",
      (PyCFunction)fastnumbers_fast_real,
      METH_VARARGS | METH_KEYWORDS,
      fast_real__doc__ },
    { "fast_float",
      (PyCFunction)fastnumbers_fast_float,
      METH_VARARGS | METH_KEYWORDS,
      fast_float__doc__ },
    { "fast_int",
      (PyCFunction)fastnumbers_fast_int,
      METH_VARARGS | METH_KEYWORDS,
      fast_int__doc__ },
    { "fast_forceint",
      (PyCFunction)fastnumbers_fast_forceint,
      METH_VARARGS | METH_KEYWORDS,
      fast_forceint__doc__ },
    { "isreal",
      (PyCFunction)fastnumbers_isreal,
      METH_VARARGS | METH_KEYWORDS,
      isreal__doc__ },
    { "isfloat",
      (PyCFunction)fastnumbers_isfloat,
      METH_VARARGS | METH_KEYWORDS,
      isfloat__doc__ },
    { "isint",
      (PyCFunction)fastnumbers_isint,
      METH_VARARGS | METH_KEYWORDS,
      isint__doc__ },
    { "isintlike",
      (PyCFunction)fastnumbers_isintlike,
      METH_VARARGS | METH_KEYWORDS,
      isintlike__doc__ },
    { "query_type",
      (PyCFunction)fastnumbers_query_type,
      METH_VARARGS | METH_KEYWORDS,
      query_type__doc__ },
    { "int",
      (PyCFunction)fastnumbers_int,
      METH_VARARGS | METH_KEYWORDS,
      fastnumbers_int__doc__ },
    { "float",
      (PyCFunction)fastnumbers_float,
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 7)
      METH_VARARGS,
      fastnumbers_float__doc__
#else
      METH_VARARGS | METH_KEYWORDS,
      fastnumbers_float__doc__
#endif
    },
    { "real",
      (PyCFunction)fastnumbers_real,
      METH_VARARGS | METH_KEYWORDS,
      fastnumbers_real__doc__ },
    { nullptr, nullptr, 0, nullptr } /* Sentinel */
};

// We want a module-level variable that is the version.
static PyObject* fastnumbers__version__;

// Some constants that may be useful for debugging.
static PyObject* fastnumbers_FN_MAX_INT_LEN;
static PyObject* fastnumbers_FN_DBL_DIG;
static PyObject* fastnumbers_FN_MAX_EXP;
static PyObject* fastnumbers_FN_MIN_EXP;

/* Define the module interface. */
static struct PyModuleDef moduledef = { PyModuleDef_HEAD_INIT,
                                        "fastnumbers",
                                        fastnumbers__doc__,
                                        -1,
                                        FastnumbersMethods,
                                        nullptr,
                                        nullptr,
                                        nullptr,
                                        nullptr };

PyMODINIT_FUNC PyInit_fastnumbers()
{
    PyObject* m = PyModule_Create(&moduledef);
    if (m == nullptr) {
        return nullptr;
    }

    // Add module level constants.
    fastnumbers__version__ = PyUnicode_FromString(FASTNUMBERS_VERSION);
    fastnumbers_FN_MAX_INT_LEN = PyLong_FromLong(FN_MAX_INT_LEN);
    fastnumbers_FN_DBL_DIG = PyLong_FromLong(FN_DBL_DIG);
    fastnumbers_FN_MAX_EXP = PyLong_FromLong(FN_MAX_EXP);
    fastnumbers_FN_MIN_EXP = PyLong_FromLong(FN_MIN_EXP);
    Py_INCREF(fastnumbers__version__);
    Py_INCREF(fastnumbers_FN_MAX_INT_LEN);
    Py_INCREF(fastnumbers_FN_DBL_DIG);
    Py_INCREF(fastnumbers_FN_MAX_EXP);
    Py_INCREF(fastnumbers_FN_MIN_EXP);
    PyModule_AddObject(m, "__version__", fastnumbers__version__);
    PyModule_AddObject(m, "max_int_len", fastnumbers_FN_MAX_INT_LEN);
    PyModule_AddObject(m, "dig", fastnumbers_FN_DBL_DIG);
    PyModule_AddObject(m, "max_exp", fastnumbers_FN_MAX_EXP);
    PyModule_AddObject(m, "min_exp", fastnumbers_FN_MIN_EXP);

    // Constants cached for internal use
    PyObject* pos_inf_str = PyBytes_FromString("+infinity");
    PyObject* neg_inf_str = PyBytes_FromString("-infinity");
    PyObject* pos_nan_str = PyBytes_FromString("+nan");
    PyObject* neg_nan_str = PyBytes_FromString("-nan");
    POS_INFINITY = PyFloat_FromString(pos_inf_str);
    NEG_INFINITY = PyFloat_FromString(neg_inf_str);
    POS_NAN = PyFloat_FromString(pos_nan_str);
    NEG_NAN = PyFloat_FromString(neg_nan_str);
    Py_DecRef(pos_inf_str);
    Py_DecRef(neg_inf_str);
    Py_DecRef(pos_nan_str);
    Py_DecRef(neg_nan_str);

    return m;
}
