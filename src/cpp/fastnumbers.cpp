#include <Python.h>

#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/docstrings.hpp"
#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/extractor.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/user_options.hpp"
#include "fastnumbers/version.hpp"

// Infinity and NaN can be cached at startup
static PyObject* POS_INFINITY;
static PyObject* NEG_INFINITY;
static PyObject* POS_NAN;
static PyObject* NEG_NAN;

// Hacky way to allow access to these module-scoped variables outside this file
PyObject** FN_POS_INFINITY_PTR = &POS_INFINITY;
PyObject** FN_NEG_INFINITY_PTR = &NEG_INFINITY;
PyObject** FN_POS_NAN_PTR = &POS_NAN;
PyObject** FN_NEG_NAN_PTR = &NEG_NAN;

// Selectors for user options
static PyObject* fastnumbers_ALLOWED;
static PyObject* fastnumbers_DISALLOWED;
static PyObject* fastnumbers_INPUT;
static PyObject* fastnumbers_RAISE;
static PyObject* fastnumbers_STRING_ONLY;
static PyObject* fastnumbers_NUMBER_ONLY;

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

/// Extract the return payload from a given python object
Payload collect_payload(PyObject* obj, const UserOptions& options, const UserType ntype)
{
    Buffer buffer;
    TextExtractor extractor(obj, buffer);
    if (extractor.is_text()) {
        CharacterParser cparser = extractor.text_parser(options);
        return Evaluator<CharacterParser>(obj, options, cparser).as_type(ntype);
    } else if (extractor.is_unicode_character()) {
        UnicodeParser uparser = extractor.unicode_char_parser(options);
        return Evaluator<UnicodeParser>(obj, options, uparser).as_type(ntype);
    } else {
        NumericParser nparser(obj, options);
        return Evaluator<NumericParser>(obj, options, nparser).as_type(ntype);
    }
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
    // Store the user options in a common interface
    UserOptions options;
    options.set_base(base);
    options.set_nan_allowed(allow_nan);
    options.set_inf_allowed(allow_inf);
    options.set_underscores_allowed(allow_underscores);

    // Attempt to extract character data from the object
    Buffer buffer;
    TextExtractor extractor(obj, buffer);
    if (num_only && (extractor.is_text() || extractor.is_unicode_character())) {
        Py_RETURN_FALSE;
    } else if (str_only && extractor.is_non_text()) {
        Py_RETURN_FALSE;
    }

    // Create a parser and use it to evaluate the user request
    if (extractor.is_text()) {
        CharacterParser cparser = extractor.text_parser(options);
        return PyBool_FromLong(
            Evaluator<CharacterParser>(obj, options, cparser).is_type(type)
        );
    } else if (extractor.is_unicode_character()) {
        UnicodeParser uparser = extractor.unicode_char_parser(options);
        return PyBool_FromLong(
            Evaluator<UnicodeParser>(obj, options, uparser).is_type(type)
        );
    } else {
        NumericParser nparser(obj, options);
        return PyBool_FromLong(
            Evaluator<NumericParser>(obj, options, nparser).is_type(type)
        );
    }
}

/// Ensure the type is allowed, otherwise return None
PyObject* validate_query_type(PyObject* result, PyObject* allowed_types)
{
    if (allowed_types != nullptr && !PySequence_Contains(allowed_types, result)) {
        Py_RETURN_NONE;
    }
    Py_IncRef(result);
    return result;
}

/// Return the correct type to search for in the input
template <typename T>
PyObject* query_search_type(const T& evaluator, PyObject* input)
{
    return evaluator.type_is_int()
        ? (PyObject*)&PyLong_Type
        : (evaluator.type_is_float() ? (PyObject*)&PyFloat_Type
                                     : (PyObject*)Py_TYPE(input));
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

    UserOptions options;
    options.set_coerce(coerce);
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::REAL);
    return payload.resolve(
        input, options, inf, nan, default_value, on_fail, raise_on_invalid
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

    UserOptions options;
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::FLOAT);
    return payload.resolve(
        input, options, inf, nan, default_value, on_fail, raise_on_invalid
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

    UserOptions options;
    options.set_base(base);
    options.set_unicode_allowed(options.is_default_base());
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::INT);
    return payload.resolve(
        input, options, nullptr, nullptr, default_value, on_fail, raise_on_invalid
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

    UserOptions options;
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::FORCEINT);
    return payload.resolve(
        input, options, nullptr, nullptr, default_value, on_fail, raise_on_invalid
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

    // Store the user options in a common interface
    UserOptions options;
    options.set_coerce(coerce);
    options.set_nan_allowed(allow_nan);
    options.set_inf_allowed(allow_inf);
    options.set_underscores_allowed(allow_underscores);

    // Attempt to extract character data from the object
    Buffer buffer;
    TextExtractor extractor(input, buffer);

    // Create a parser and use it to evaluate the user request
    PyObject* search_type = nullptr;
    if (extractor.is_text()) {
        CharacterParser cparser = extractor.text_parser(options);
        search_type = query_search_type(
            Evaluator<CharacterParser>(input, options, cparser), input
        );
    } else if (extractor.is_unicode_character()) {
        UnicodeParser uparser = extractor.unicode_char_parser(options);
        search_type = query_search_type(
            Evaluator<UnicodeParser>(input, options, uparser), input
        );
    } else {
        NumericParser nparser(input, options);
        search_type = query_search_type(
            Evaluator<NumericParser>(input, options, nparser), input
        );
    }
    return validate_query_type(search_type, allowed_types);
}

/* Drop-in replacement for int, float */
static PyObject* fastnumbers_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* pybase = nullptr;
    static const char* format = "|OO:int";

    /* Do not accept number as a keyword argument. */
    static const char* keywords[] = { "", "base", nullptr };

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

    UserOptions options;
    options.set_base(base);
    options.set_unicode_allowed(false);
    options.set_underscores_allowed(true);

    const Payload payload = collect_payload(input, options, UserType::INT);
    return payload.resolve(input, options, nullptr, nullptr, nullptr, nullptr, true);
}

static PyObject* fastnumbers_float(PyObject* self, PyObject* args)
{
    PyObject* input = nullptr;
    static const char* format = "|O:float";

    /* Read the function argument - do not accept it as a keyword argument. */
    if (!PyArg_ParseTuple(args, format, &input)) {
        return nullptr;
    }

    /* No arguments returns 0.0. */
    if (input == nullptr) {
        return PyFloat_FromDouble(0.0);
    }

    UserOptions options;
    options.set_unicode_allowed(false);
    options.set_underscores_allowed(true);

    const Payload payload = collect_payload(input, options, UserType::FLOAT);
    return payload.resolve(input, options, nullptr, nullptr, nullptr, nullptr, true);
}

/* Behaves like float or int, but returns correct type. */
static PyObject* fastnumbers_real(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    int coerce = true;
    static const char* format = "|O$p:real";

    /* Do not accept number as a keyword argument. */
    static const char* keywords[] = { "", "coerce", nullptr };

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

    UserOptions options;
    options.set_coerce(coerce);
    options.set_unicode_allowed(false);
    options.set_underscores_allowed(true);

    const Payload payload = collect_payload(input, options, UserType::REAL);
    return payload.resolve(input, options, nullptr, nullptr, nullptr, nullptr, true);
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
    { "float", (PyCFunction)fastnumbers_float, METH_VARARGS, fastnumbers_float__doc__ },
    { "real",
      (PyCFunction)fastnumbers_real,
      METH_VARARGS | METH_KEYWORDS,
      fastnumbers_real__doc__ },
    { nullptr, nullptr, 0, nullptr } /* Sentinel */
};

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
    PyModule_AddStringConstant(m, "__version__", FASTNUMBERS_VERSION);
    PyModule_AddIntConstant(m, "max_int_len", FN_MAX_INT_LEN);
    PyModule_AddIntConstant(m, "dig", FN_DBL_DIG);
    PyModule_AddIntConstant(m, "max_exp", FN_MAX_EXP);
    PyModule_AddIntConstant(m, "min_exp", FN_MIN_EXP);

    // Selectors
    fastnumbers_ALLOWED = PyObject_New(PyObject, &PyBaseObject_Type);
    fastnumbers_DISALLOWED = PyObject_New(PyObject, &PyBaseObject_Type);
    fastnumbers_INPUT = PyObject_New(PyObject, &PyBaseObject_Type);
    fastnumbers_RAISE = PyObject_New(PyObject, &PyBaseObject_Type);
    fastnumbers_STRING_ONLY = PyObject_New(PyObject, &PyBaseObject_Type);
    fastnumbers_NUMBER_ONLY = PyObject_New(PyObject, &PyBaseObject_Type);
    PyModule_AddObject(m, "ALLOWED", fastnumbers_ALLOWED);
    PyModule_AddObject(m, "DISALLOWED", fastnumbers_DISALLOWED);
    PyModule_AddObject(m, "INPUT", fastnumbers_INPUT);
    PyModule_AddObject(m, "RAISE", fastnumbers_RAISE);
    PyModule_AddObject(m, "STRING_ONLY", fastnumbers_STRING_ONLY);
    PyModule_AddObject(m, "NUMBER_ONLY", fastnumbers_NUMBER_ONLY);

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
