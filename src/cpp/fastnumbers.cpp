#include <exception>

#include <Python.h>

#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/docstrings.hpp"
#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/extractor.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/user_options.hpp"
#include "fastnumbers/version.hpp"

// Initiallize static objects
PyObject* Resolver::POS_INFINITY = nullptr;
PyObject* Resolver::NEG_INFINITY = nullptr;
PyObject* Resolver::POS_NAN = nullptr;
PyObject* Resolver::NEG_NAN = nullptr;
PyObject* Resolver::ALLOWED = nullptr;
PyObject* Resolver::DISALLOWED = nullptr;
PyObject* Resolver::INPUT = nullptr;
PyObject* Resolver::RAISE = nullptr;
PyObject* Resolver::STRING_ONLY = nullptr;
PyObject* Resolver::NUMBER_ONLY = nullptr;

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
void assess_integer_base_input(PyObject* pybase, int& base)
{
    Py_ssize_t longbase = 0;

    // Default to INT_MIN
    if (pybase == nullptr) {
        base = INT_MIN;
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
void handle_fail_backwards_compatibility(
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
        on_fail = Resolver::RAISE;
    }
    // fastnumbers default
    if (on_fail == nullptr) {
        on_fail = Resolver::INPUT;
    }
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
    options.set_nan_allowed_str(allow_nan);
    options.set_inf_allowed_str(allow_inf);
    options.set_nan_allowed_num(true);
    options.set_inf_allowed_num(true);
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
    PyObject* inf = Resolver::ALLOWED;
    PyObject* nan = Resolver::ALLOWED;
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

    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    UserOptions options;
    options.set_coerce(coerce);
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::REAL);

    Resolver resolver(input, options);
    resolver.set_inf_action(inf);
    resolver.set_nan_action(nan);
    resolver.set_fail_action(on_fail);
    return resolver.resolve(payload);
}

/* Quickly convert to a float, depending on value. */
static PyObject* fastnumbers_fast_float(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = Resolver::ALLOWED;
    PyObject* nan = Resolver::ALLOWED;
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

    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    UserOptions options;
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::FLOAT);

    Resolver resolver(input, options);
    resolver.set_inf_action(inf);
    resolver.set_nan_action(nan);
    resolver.set_fail_action(on_fail);
    return resolver.resolve(payload);
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

    int base = INT_MIN;
    try {
        assess_integer_base_input(pybase, base);
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    UserOptions options;
    options.set_base(base);
    options.set_unicode_allowed(options.is_default_base());
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::INT);

    Resolver resolver(input, options);
    resolver.set_fail_action(on_fail);
    return resolver.resolve(payload);
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

    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    UserOptions options;
    options.set_underscores_allowed(allow_underscores);

    const Payload payload = collect_payload(input, options, UserType::FORCEINT);

    Resolver resolver(input, options);
    resolver.set_fail_action(on_fail);
    return resolver.resolve(payload);
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
    try {
        assess_integer_base_input(pybase, base);
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
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
    options.set_nan_allowed_str(allow_nan);
    options.set_inf_allowed_str(allow_inf);
    options.set_nan_allowed_num(true);
    options.set_inf_allowed_num(true);
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
    try {
        assess_integer_base_input(pybase, base);
    } catch (fastnumbers_exception& e) {
        return e.raise_value_error();
    }

    UserOptions options;
    options.set_base(base);
    options.set_unicode_allowed(false);
    options.set_underscores_allowed(true);

    const Payload payload = collect_payload(input, options, UserType::INT);

    Resolver resolver(input, options);
    return resolver.resolve(payload);
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

    Resolver resolver(input, options);
    return resolver.resolve(payload);
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

    Resolver resolver(input, options);
    return resolver.resolve(payload);
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
    Resolver::ALLOWED = PyObject_New(PyObject, &PyBaseObject_Type);
    Resolver::DISALLOWED = PyObject_New(PyObject, &PyBaseObject_Type);
    Resolver::INPUT = PyObject_New(PyObject, &PyBaseObject_Type);
    Resolver::RAISE = PyObject_New(PyObject, &PyBaseObject_Type);
    Resolver::STRING_ONLY = PyObject_New(PyObject, &PyBaseObject_Type);
    Resolver::NUMBER_ONLY = PyObject_New(PyObject, &PyBaseObject_Type);
    PyModule_AddObject(m, "ALLOWED", Resolver::ALLOWED);
    PyModule_AddObject(m, "DISALLOWED", Resolver::DISALLOWED);
    PyModule_AddObject(m, "INPUT", Resolver::INPUT);
    PyModule_AddObject(m, "RAISE", Resolver::RAISE);
    PyModule_AddObject(m, "STRING_ONLY", Resolver::STRING_ONLY);
    PyModule_AddObject(m, "NUMBER_ONLY", Resolver::NUMBER_ONLY);

    // Constants cached for internal use
    PyObject* pos_inf_str = PyBytes_FromString("+infinity");
    PyObject* neg_inf_str = PyBytes_FromString("-infinity");
    PyObject* pos_nan_str = PyBytes_FromString("+nan");
    PyObject* neg_nan_str = PyBytes_FromString("-nan");
    Resolver::POS_INFINITY = PyFloat_FromString(pos_inf_str);
    Resolver::NEG_INFINITY = PyFloat_FromString(neg_inf_str);
    Resolver::POS_NAN = PyFloat_FromString(pos_nan_str);
    Resolver::NEG_NAN = PyFloat_FromString(neg_nan_str);
    Py_DecRef(pos_inf_str);
    Py_DecRef(neg_inf_str);
    Py_DecRef(pos_nan_str);
    Py_DecRef(neg_nan_str);

    return m;
}
