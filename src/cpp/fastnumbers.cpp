/*
 * This file contains the functions that directly interface with the Python interpreter.
 */
#include <limits>
#include <stdexcept>

#include <Python.h>

#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/docstrings.hpp"
#include "fastnumbers/implementation.hpp"
#include "fastnumbers/selectors.hpp"
#include "fastnumbers/user_options.hpp"
#include "fastnumbers/version.hpp"

/// Custom exception class for fastnumbers
class fastnumbers_exception : public std::runtime_error {
public:
    fastnumbers_exception(const char* message)
        : std::runtime_error(message)
    { }

    /// Set a ValueError if a non-empty message was given.
    PyObject* raise_value_error() const
    {
        if (what()[0] != '\0') {
            PyErr_SetString(PyExc_ValueError, what());
        }
        return nullptr;
    }
};

/**
 * \brief Function to handle the conversion of base to integers.
 *
 * \param pybase The base as a Python object
 * \param base The base as an integer
 * \throws fastnumbers_exception on invalid input
 */
static inline void assess_integer_base_input(PyObject* pybase, int& base)
{
    Py_ssize_t longbase = 0;

    // Default to std::numeric_limits<int>::min()
    if (pybase == nullptr) {
        base = std::numeric_limits<int>::min();
        return;
    }

    // Convert to int and check for overflow
    longbase = PyNumber_AsSsize_t(pybase, nullptr);
    if (longbase == -1 && PyErr_Occurred()) {
        throw fastnumbers_exception("");
    }

    // Ensure valid integer in valid range
    if ((longbase != 0 && longbase < 2) || longbase > 36) {
        throw fastnumbers_exception("int() base must be >= 2 and <= 36");
    }
    base = static_cast<int>(longbase);
}

/**
 * \brief Resolve all possible backwards-compatible values for on_fail.
 *
 * If both were defined, error.
 *
 * \param on_fail
 * \param key
 * \param default_value
 * \param raise_on_invalid
 * \throws fastnumbers_exception on invalid input
 */
static inline void handle_fail_backwards_compatibility(
    PyObject*& on_fail, PyObject*& key, PyObject*& default_value, int raise_on_invalid
)
{
    if (key != nullptr) {
        if (on_fail != nullptr) {
            throw fastnumbers_exception("Cannot set both on_fail and key");
        }
        on_fail = key;
        key = nullptr;
    }
    if (default_value != nullptr) {
        if (on_fail != nullptr) {
            throw fastnumbers_exception("Cannot set both on_fail and default");
        }
        on_fail = default_value;
        default_value = nullptr;
    }
    if (raise_on_invalid) {
        if (on_fail != nullptr) {
            throw fastnumbers_exception("Cannot set both on_fail and raise_on_invalid");
        }
        on_fail = Selectors::RAISE;
    }
    // fastnumbers default
    if (on_fail == nullptr) {
        on_fail = Selectors::INPUT;
    }
}

/**
 * \brief Create the consider selector from booleans
 *
 * For backwards compatiblity purposes
 *
 * \param str_only
 * \param num_only
 *
 * \return Object containing the correct selector value, or nullptr.
 */
static inline PyObject* create_consider(const bool str_only, const bool num_only)
{
    if (str_only) {
        return Selectors::STRING_ONLY;
    } else if (num_only) {
        return Selectors::NUMBER_ONLY;
    } else {
        return nullptr;
    }
}

/**
 * \brief Quickly convert to an int or float, depending on value, with error handling
 */
static PyObject* fastnumbers_fast_real(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
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

    // Convert old-style arguments to new-style
    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    // Execute
    return float_conv_impl(
        input, on_fail, inf, nan, UserType::REAL, allow_underscores, coerce
    );
}

/**
 * \brief Quickly convert to a float, with error handling
 */
static PyObject* fastnumbers_fast_float(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    int raise_on_invalid = false;
    int allow_underscores = true;
    static const char* keywords[] = { "x",    "default", "raise_on_invalid",  "on_fail",
                                      "inf",  "nan",     "allow_underscores", "key",
                                      nullptr };
    static const char* format = "O|O$pOOOpO:fast_float";

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    // Execute
    return float_conv_impl(input, on_fail, inf, nan, UserType::FLOAT, allow_underscores);
}

/**
 * \brief Quickly convert to an int, with error handling
 */
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

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    // Extract integer from python object for base
    int base = std::numeric_limits<int>::min();
    try {
        assess_integer_base_input(pybase, base);
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    // Execute
    return int_conv_impl(input, on_fail, UserType::INT, allow_underscores, base);
}

/**
 * \brief Quickly convert to an int (even if input is float), with error handling
 */
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

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    // Execute
    return int_conv_impl(input, on_fail, UserType::FORCEINT, allow_underscores);
}

/**
 * \brief Quickly determine if the input is a real.
 */
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

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    const PyObject* consider = create_consider(str_only, num_only);
    const PyObject* inf = allow_inf ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;
    const PyObject* nan = allow_nan ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;

    // Execute
    return float_check_impl(
        input, inf, nan, consider, UserType::REAL, allow_underscores
    );
}

/**
 * \brief Quickly determine if the input is a float.
 */
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

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    const PyObject* consider = create_consider(str_only, num_only);
    const PyObject* inf = allow_inf ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;
    const PyObject* nan = allow_nan ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;

    // Execute
    return float_check_impl(
        input, inf, nan, consider, UserType::FLOAT, allow_underscores
    );
}

/**
 * \brief Quickly determine if the input is an int.
 */
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

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    // Extract integer from python object for base
    int base = std::numeric_limits<int>::min();
    try {
        assess_integer_base_input(pybase, base);
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }
    const PyObject* consider = create_consider(str_only, num_only);

    // Execute
    return int_check_impl(input, consider, UserType::INT, allow_underscores, base);
}

/**
 * \brief Quickly determine if the input is an int or int-like float.
 */
static PyObject* fastnumbers_isintlike(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_underscores = true;
    static const char* keywords[]
        = { "x", "str_only", "num_only", "allow_underscores", nullptr };
    static const char* format = "O|$ppp:isintlike";

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    const PyObject* consider = create_consider(str_only, num_only);

    // Execute
    return int_check_impl(input, consider, UserType::INTLIKE, allow_underscores);
}

/**
 * \brief Quickly determine the type
 */
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

    // Read the function arguments
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

    // Convert old-style arguments to new-style
    const PyObject* inf = allow_inf ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;
    const PyObject* nan = allow_nan ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;

    // Execute
    return type_query_impl(input, allowed_types, inf, nan, allow_underscores, coerce);
}

/**
 * \brief Drop-in replacement for float
 */
static PyObject* fastnumbers_float(PyObject* self, PyObject* args)
{
    PyObject* input = nullptr;
    static const char* format = "|O:float";

    // Read the function argument - do not accept it as a keyword argument
    if (!PyArg_ParseTuple(args, format, &input)) {
        return nullptr;
    }

    // No arguments returns 0.0
    if (input == nullptr) {
        return PyFloat_FromDouble(0.0);
    }

    // Execute
    return float_conv_impl(input, UserType::FLOAT);
}

/**
 * \brief Drop-in replacement for int
 */
static PyObject* fastnumbers_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* pybase = nullptr;
    static const char* format = "|OO:int";

    // Do not accept number as a keyword argument
    static const char* keywords[] = { "", "base", nullptr };

    // Read the function argument
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, format, const_cast<char**>(keywords), &input, &pybase
        )) {
        return nullptr;
    }

    // No arguments returns 0
    if (input == nullptr) {
        if (pybase != nullptr) {
            PyErr_SetString(PyExc_TypeError, "int() missing string argument");
            return nullptr;
        }
        return PyLong_FromLong(0);
    }

    // Extract integer from python object for base
    int base = std::numeric_limits<int>::min();
    try {
        assess_integer_base_input(pybase, base);
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    // Execute
    return int_conv_impl(input, UserType::INT, base);
}

/**
 * \brief Behaves like float or int, but returns correct type
 */
static PyObject* fastnumbers_real(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    int coerce = true;
    static const char* format = "|O$p:real";

    // Do not accept number as a keyword argument
    static const char* keywords[] = { "", "coerce", nullptr };

    // Read the function arguments
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, format, const_cast<char**>(keywords), &input, &coerce
        )) {
        return nullptr;
    }

    // No arguments returns 0.0 or 0 depending on the state of coerce
    if (input == nullptr) {
        return coerce ? PyLong_FromLong(0) : PyFloat_FromDouble(0.0);
    }

    // Execute
    return float_conv_impl(input, UserType::REAL, coerce);
}

// Define the methods contained in this module
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
    { "float", (PyCFunction)fastnumbers_float, METH_VARARGS, fastnumbers_float__doc__ },
    { "real",
      (PyCFunction)fastnumbers_real,
      METH_VARARGS | METH_KEYWORDS,
      fastnumbers_real__doc__ },
    { nullptr, nullptr, 0, nullptr } /* Sentinel */
};

// Define the module interface
static struct PyModuleDef moduledef = { PyModuleDef_HEAD_INIT,
                                        "fastnumbers",
                                        fastnumbers__doc__,
                                        -1,
                                        FastnumbersMethods,
                                        nullptr,
                                        nullptr,
                                        nullptr,
                                        nullptr };

// Initiallize static objects
PyObject* Selectors::POS_INFINITY = nullptr;
PyObject* Selectors::NEG_INFINITY = nullptr;
PyObject* Selectors::POS_NAN = nullptr;
PyObject* Selectors::NEG_NAN = nullptr;
PyObject* Selectors::ALLOWED = nullptr;
PyObject* Selectors::DISALLOWED = nullptr;
PyObject* Selectors::INPUT = nullptr;
PyObject* Selectors::RAISE = nullptr;
PyObject* Selectors::STRING_ONLY = nullptr;
PyObject* Selectors::NUMBER_ONLY = nullptr;

// Actually create the module object itself
PyMODINIT_FUNC PyInit_fastnumbers()
{
    PyObject* m = PyModule_Create(&moduledef);
    if (m == nullptr) {
        return nullptr;
    }

    // Add module level constants.
    PyModule_AddStringConstant(m, "__version__", FASTNUMBERS_VERSION);
    PyModule_AddIntConstant(m, "max_int_len", FN_MAX_INT_LEN);
    PyModule_AddIntConstant(m, "dig", FN_DBL_DIG);
    PyModule_AddIntConstant(m, "max_exp", FN_MAX_EXP);
    PyModule_AddIntConstant(m, "min_exp", FN_MIN_EXP);

    // Selectors
    Selectors::ALLOWED = PyObject_New(PyObject, &PyBaseObject_Type);
    Selectors::DISALLOWED = PyObject_New(PyObject, &PyBaseObject_Type);
    Selectors::INPUT = PyObject_New(PyObject, &PyBaseObject_Type);
    Selectors::RAISE = PyObject_New(PyObject, &PyBaseObject_Type);
    Selectors::STRING_ONLY = PyObject_New(PyObject, &PyBaseObject_Type);
    Selectors::NUMBER_ONLY = PyObject_New(PyObject, &PyBaseObject_Type);
    PyModule_AddObject(m, "ALLOWED", Selectors::ALLOWED);
    PyModule_AddObject(m, "DISALLOWED", Selectors::DISALLOWED);
    PyModule_AddObject(m, "INPUT", Selectors::INPUT);
    PyModule_AddObject(m, "RAISE", Selectors::RAISE);
    PyModule_AddObject(m, "STRING_ONLY", Selectors::STRING_ONLY);
    PyModule_AddObject(m, "NUMBER_ONLY", Selectors::NUMBER_ONLY);

    // Constants cached for internal use
    PyObject* pos_inf_str = PyBytes_FromString("+infinity");
    PyObject* neg_inf_str = PyBytes_FromString("-infinity");
    PyObject* pos_nan_str = PyBytes_FromString("+nan");
    PyObject* neg_nan_str = PyBytes_FromString("-nan");
    Selectors::POS_INFINITY = PyFloat_FromString(pos_inf_str);
    Selectors::NEG_INFINITY = PyFloat_FromString(neg_inf_str);
    Selectors::POS_NAN = PyFloat_FromString(pos_nan_str);
    Selectors::NEG_NAN = PyFloat_FromString(neg_nan_str);
    Py_DecRef(pos_inf_str);
    Py_DecRef(neg_inf_str);
    Py_DecRef(pos_nan_str);
    Py_DecRef(neg_nan_str);

    return m;
}
