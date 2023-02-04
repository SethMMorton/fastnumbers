/*
 * This file contains the functions that directly interface with the Python interpreter.
 */
#include <exception>
#include <limits>
#include <stdexcept>
#include <string>

#include <Python.h>

#include "fastnumbers/argparse.hpp"
#include "fastnumbers/docstrings.hpp"
#include "fastnumbers/implementation.hpp"
#include "fastnumbers/parser/numeric.hpp"
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

/// Convert a char array to a Python exception
static inline PyObject* convert_exception(PyObject* obj, const char* msg)
{
    PyErr_Format(
        PyExc_SystemError,
        "fastnumbers with input '%.R' has thrown an unexpected C++ exception: %s",
        obj,
        msg
    );
    return nullptr;
}

/// Convert a C++ error to a Python exception
static inline PyObject* convert_exception(PyObject* obj, const std::exception& exc)
{
    return convert_exception(obj, exc.what());
}

/// Convert a string to a Python exception
static inline PyObject* convert_exception(PyObject* obj, const std::string& s)
{
    return convert_exception(obj, s.data());
}

/// Handle all exceptions from running fastnumbers logic.
/// This is a "function try block", hence the missing pair of braces.
static inline PyObject* handle_exceptions(PyObject* input)
try {
    throw;
} catch (const fastnumbers_exception& e) {
    return e.raise_value_error();
} catch (const std::exception& e) {
    return convert_exception(input, e);
} catch (const std::string& msg) {
    return convert_exception(input, msg);
} catch (const char* msg) {
    return convert_exception(input, msg);
} catch (...) {
    return convert_exception(input, "Unknown C++ exception");
}

/**
 * \brief Function to handle the conversion of base to integers.
 *
 * \param pybase The base as a Python object
 * \param base The base as an integer
 * \throws fastnumbers_exception on invalid input
 */
static inline int assess_integer_base_input(PyObject* pybase)
{
    Py_ssize_t longbase = 0;

    // Default to std::numeric_limits<int>::min()
    if (pybase == nullptr) {
        return std::numeric_limits<int>::min();
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
    return static_cast<int>(longbase);
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
 * \brief Validate the selector has only a "yes, no, num, str" value
 * \param selector The python object to validate
 * \throws fastnumbers_exception if not one of the four valid values
 */
static inline void validate_allow_disallow_str_only_num_only(const PyObject* selector)
{
    const bool ok = selector == Selectors::ALLOWED || selector == Selectors::DISALLOWED
        || selector == Selectors::NUMBER_ONLY || selector == Selectors::STRING_ONLY;
    if (!ok) {
        throw fastnumbers_exception(
            "allowed values for 'inf' and 'nan' are fastnumbers.ALLOWED, "
            "fastnumbers.DISALLOWED, fastnumbers.NUMBER_ONLY, or "
            "fastnumbers.STRING_ONLY"
        );
    }
}

/**
 * \brief Validate the selector is not a "yes, no, num, str" value
 * \param selector The python object to validate
 * \throws fastnumbers_exception if one of the four valid values
 */
static inline void validate_not_allow_disallow_str_only_num_only(const PyObject* selector
)
{
    const bool bad = selector == Selectors::ALLOWED || selector == Selectors::DISALLOWED
        || selector == Selectors::NUMBER_ONLY || selector == Selectors::STRING_ONLY;
    if (bad) {
        throw fastnumbers_exception(
            "values for 'on_fail' and 'on_type_error' cannot be fastnumbers.ALLOWED, "
            "fastnumbers.DISALLOWED, fastnumbers.NUMBER_ONLY, or "
            "fastnumbers.STRING_ONLY"
        );
    }
}

/**
 * \brief Validate the selector is not "DISALLOWED"
 * \param selector The python object to validate
 * \throws fastnumbers_exception if the value is "DISALLOWED"
 */
static inline void validate_not_disallow(const PyObject* selector)
{
    const bool bad = selector == Selectors::DISALLOWED
        || selector == Selectors::STRING_ONLY || selector == Selectors::NUMBER_ONLY;
    if (bad) {
        throw fastnumbers_exception(
            "'inf' and 'nan' cannot be fastnumbers.DISALLOWED, "
            "fastnumbers.STRING_ONLY, or fastnumbers.NUMBER_ONLY."
        );
    }
}

/**
 * \brief Validate the selector has only a "num, str, None" value
 * \param selector The python object to validate
 * \throws fastnumbers_exception if not one of the three valid values
 */
static inline void validate_consider(const PyObject* selector)
{
    const bool ok = selector == Py_None || selector == Selectors::NUMBER_ONLY
        || selector == Selectors::STRING_ONLY;
    if (!ok) {
        throw fastnumbers_exception(
            "allowed values for 'consider' are None, fastnumbers.NUMBER_ONLY, or "
            "fastnumbers.STRING_ONLY"
        );
    }
}

/**
 * \brief Quickly convert to an int or float, depending on value, with error handling
 */
static PyObject* fastnumbers_try_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    int coerce = true;
    int allow_underscores = false;

    // Read the function argument
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("try_real", args, len_args, kwnames,
                           "x", false,  &input,
                           "$inf", false, &inf,
                           "$nan", false, &nan,
                           "$on_fail", false, &on_fail,
                           "$on_type_error", false, &on_type_error,
                           "$coerce", true, &coerce,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_not_disallow(inf);
        validate_not_disallow(nan);
        validate_not_allow_disallow_str_only_num_only(on_fail);
        validate_not_allow_disallow_str_only_num_only(on_type_error);
        return float_conv_impl(
            input,
            on_fail,
            on_type_error,
            inf,
            nan,
            UserType::REAL,
            allow_underscores,
            coerce
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly convert to an int or float, depending on value, with error handling
 */
static PyObject* fastnumbers_fast_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
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

    // Read the function argument
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("fast_real", args, len_args, kwnames,
                           "x", false,  &input,
                           "|default", false, &default_value,
                           "$raise_on_invalid", true, &raise_on_invalid,
                           "$on_fail", false, &on_fail,
                           "$inf", false, &inf,
                           "$nan", false, &nan,
                           "$coerce", true, &coerce,
                           "$allow_underscores", true, &allow_underscores,
                           "$key", false, &key,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
        return float_conv_impl(
            input, on_fail, inf, nan, UserType::REAL, allow_underscores, coerce
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly convert to a float, with error handling
 */
static PyObject* fastnumbers_try_float(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    int allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("try_float", args, len_args, kwnames,
                           "x", false,  &input,
                           "$inf", false, &inf,
                           "$nan", false, &nan,
                           "$on_fail", false, &on_fail,
                           "$on_type_error", false, &on_type_error,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_not_disallow(inf);
        validate_not_disallow(nan);
        validate_not_allow_disallow_str_only_num_only(on_fail);
        validate_not_allow_disallow_str_only_num_only(on_type_error);
        return float_conv_impl(
            input, on_fail, on_type_error, inf, nan, UserType::FLOAT, allow_underscores
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly convert to a float, with error handling
 */
static PyObject* fastnumbers_fast_float(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    int raise_on_invalid = false;
    int allow_underscores = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("fast_float", args, len_args, kwnames,
                           "x", false,  &input,
                           "|default", false, &default_value,
                           "$raise_on_invalid", true, &raise_on_invalid,
                           "$on_fail", false, &on_fail,
                           "$inf", false, &inf,
                           "$nan", false, &nan,
                           "$allow_underscores", true, &allow_underscores,
                           "$key", false, &key,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
        return float_conv_impl(
            input, on_fail, inf, nan, UserType::FLOAT, allow_underscores
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly convert to an int, with error handling
 */
static PyObject* fastnumbers_try_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    PyObject* pybase = nullptr;
    int allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("try_int", args, len_args, kwnames,
                           "x", false,  &input,
                           "$on_fail", false, &on_fail,
                           "$on_type_error", false, &on_type_error,
                           "$base", false, &pybase,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_not_allow_disallow_str_only_num_only(on_fail);
        validate_not_allow_disallow_str_only_num_only(on_type_error);
        const int base = assess_integer_base_input(pybase);
        return int_conv_impl(
            input, on_fail, on_type_error, UserType::INT, allow_underscores, base
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly convert to an int, with error handling
 */
static PyObject* fastnumbers_fast_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* pybase = nullptr;
    int raise_on_invalid = false;
    int allow_underscores = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("fast_int", args, len_args, kwnames,
                           "x", false,  &input,
                           "|default", false, &default_value,
                           "$raise_on_invalid", true, &raise_on_invalid,
                           "$on_fail", false, &on_fail,
                           "$base", false, &pybase,
                           "$allow_underscores", true, &allow_underscores,
                           "$key", false, &key,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
        const int base = assess_integer_base_input(pybase);
        return int_conv_impl(input, on_fail, UserType::INT, allow_underscores, base);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly convert to an int (even if input is float), with error handling
 */
static PyObject* fastnumbers_try_forceint(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    int allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("try_forceint", args, len_args, kwnames,
                           "x", false,  &input,
                           "$on_fail", false, &on_fail,
                           "$on_type_error", false, &on_type_error,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_not_allow_disallow_str_only_num_only(on_fail);
        validate_not_allow_disallow_str_only_num_only(on_type_error);
        return int_conv_impl(
            input, on_fail, on_type_error, UserType::FORCEINT, allow_underscores
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly convert to an int (even if input is float), with error handling
 */
static PyObject* fastnumbers_fast_forceint(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    int raise_on_invalid = false;
    int allow_underscores = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("fast_forceint", args, len_args, kwnames,
                           "x", false,  &input,
                           "|default", false, &default_value,
                           "$raise_on_invalid", true, &raise_on_invalid,
                           "$on_fail", false, &on_fail,
                           "$allow_underscores", true, &allow_underscores,
                           "$key", false, &key,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );
        return int_conv_impl(input, on_fail, UserType::FORCEINT, allow_underscores);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is a real.
 */
static PyObject* fastnumbers_check_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* consider = Py_None;
    PyObject* inf = Selectors::NUMBER_ONLY;
    PyObject* nan = Selectors::NUMBER_ONLY;
    int allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("check_real", args, len_args, kwnames,
                           "x", false,  &input,
                           "$inf", false, &inf,
                           "$nan", false, &nan,
                           "$consider", false, &consider,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_allow_disallow_str_only_num_only(inf);
        validate_allow_disallow_str_only_num_only(nan);
        validate_consider(consider);
        return float_check_impl(
            input, inf, nan, consider, UserType::REAL, allow_underscores
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is a real.
 */
static PyObject* fastnumbers_isreal(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_inf = false;
    int allow_nan = false;
    int allow_underscores = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("isreal", args, len_args, kwnames,
                           "x", false,  &input,
                           "$str_only", true, &str_only,
                           "$num_only", true, &num_only,
                           "$allow_inf", true, &allow_inf,
                           "$allow_nan", true, &allow_nan,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Convert old-style arguments to new-style
    const PyObject* consider = create_consider(str_only, num_only);
    const PyObject* inf = allow_inf ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;
    const PyObject* nan = allow_nan ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        return float_check_impl(
            input, inf, nan, consider, UserType::REAL, allow_underscores
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is a float.
 */
static PyObject* fastnumbers_check_float(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* consider = Py_None;
    PyObject* inf = Selectors::NUMBER_ONLY;
    PyObject* nan = Selectors::NUMBER_ONLY;
    int strict = false;
    int allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("check_float", args, len_args, kwnames,
                           "x", false,  &input,
                           "$inf", false, &inf,
                           "$nan", false, &nan,
                           "$consider", false, &consider,
                           "$strict", true, &strict,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_allow_disallow_str_only_num_only(inf);
        validate_allow_disallow_str_only_num_only(nan);
        validate_consider(consider);
        return float_check_impl(
            input, inf, nan, consider, UserType::FLOAT, allow_underscores, strict
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is a float.
 */
static PyObject* fastnumbers_isfloat(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_inf = false;
    int allow_nan = false;
    int allow_underscores = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("isfloat", args, len_args, kwnames,
                           "x", false,  &input,
                           "$str_only", true, &str_only,
                           "$num_only", true, &num_only,
                           "$allow_inf", true, &allow_inf,
                           "$allow_nan", true, &allow_nan,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Convert old-style arguments to new-style
    const PyObject* consider = create_consider(str_only, num_only);
    const PyObject* inf = allow_inf ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;
    const PyObject* nan = allow_nan ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        return float_check_impl(
            input, inf, nan, consider, UserType::FLOAT, allow_underscores
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is an int.
 */
static PyObject* fastnumbers_check_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* consider = Py_None;
    PyObject* pybase = nullptr;
    int allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("check_int", args, len_args, kwnames,
                           "x", false,  &input,
                           "$consider", false, &consider,
                           "$base", false, &pybase,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_consider(consider);
        const int base = assess_integer_base_input(pybase);
        return int_check_impl(input, consider, UserType::INT, allow_underscores, base);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is an int.
 */
static PyObject* fastnumbers_isint(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* pybase = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_underscores = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("isint", args, len_args, kwnames,
                           "x", false,  &input,
                           "$str_only", true, &str_only,
                           "$num_only", true, &num_only,
                           "$base", false, &pybase,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Convert old-style arguments to new-style
    const PyObject* consider = create_consider(str_only, num_only);

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        const int base = assess_integer_base_input(pybase);
        return int_check_impl(input, consider, UserType::INT, allow_underscores, base);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is an int or int-like float.
 */
static PyObject* fastnumbers_check_intlike(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* consider = Py_None;
    bool allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("check_intlike", args, len_args, kwnames,
                           "x", false,  &input,
                           "$consider", false, &consider,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        validate_consider(consider);
        return int_check_impl(input, consider, UserType::INTLIKE, allow_underscores);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine if the input is an int or int-like float.
 */
static PyObject* fastnumbers_isintlike(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    bool str_only = false;
    bool num_only = false;
    bool allow_underscores = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("isintlike", args, len_args, kwnames,
                           "x", false,  &input,
                           "$str_only", true, &str_only,
                           "$num_only", true, &num_only,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Convert old-style arguments to new-style
    const PyObject* consider = create_consider(str_only, num_only);

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        return int_check_impl(input, consider, UserType::INTLIKE, allow_underscores);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Quickly determine the type
 */
static PyObject* fastnumbers_query_type(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* allowed_types = nullptr;
    int coerce = false;
    int allow_inf = false;
    int allow_nan = false;
    int allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("query_type", args, len_args, kwnames,
                           "x", false,  &input,
                           "$allow_inf", true, &allow_inf,
                           "$allow_nan", true, &allow_nan,
                           "$coerce", true, &coerce,
                           "$allowed_types", false, &allowed_types,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

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

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        return type_query_impl(
            input, allowed_types, inf, nan, allow_underscores, coerce
        );
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Drop-in replacement for float
 */
static PyObject*
fastnumbers_float(PyObject* self, PyObject* const* args, Py_ssize_t len_args)
{
    PyObject* input = nullptr;

    // Read the function argument - do not accept it as a keyword argument
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("float", args, len_args, nullptr,
                           "|", false,  &input,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // No arguments returns 0.0
    if (input == nullptr) {
        return PyFloat_FromDouble(0.0);
    }

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        return float_conv_impl(input, UserType::FLOAT);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Drop-in replacement for int
 */
static PyObject* fastnumbers_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    PyObject* pybase = nullptr;

    // Read the function argument
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("int", args, len_args, kwnames,
                           "|", false,  &input,
                           "|base", false, &pybase,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // No arguments returns 0
    if (input == nullptr) {
        if (pybase != nullptr) {
            PyErr_SetString(PyExc_TypeError, "int() missing string argument");
            return nullptr;
        }
        return PyLong_FromLong(0);
    }

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        const int base = assess_integer_base_input(pybase);
        return int_conv_impl(input, UserType::INT, base);
    } catch (...) {
        return handle_exceptions(input);
    }
}

/**
 * \brief Behaves like float or int, but returns correct type
 */
static PyObject* fastnumbers_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
)
{
    PyObject* input = nullptr;
    int coerce = true;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("real", args, len_args, kwnames,
                           "|", false,  &input,
                           "$coerce", true, &coerce,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // No arguments returns 0.0 or 0 depending on the state of coerce
    if (input == nullptr) {
        return coerce ? PyLong_FromLong(0) : PyFloat_FromDouble(0.0);
    }

    // Execute main logic in a try-block to convert C++ exceptions
    try {
        return float_conv_impl(input, UserType::REAL, coerce);
    } catch (...) {
        return handle_exceptions(input);
    }
}

// Define the methods contained in this module
static PyMethodDef FastnumbersMethods[] = {
    { "try_real",
      (PyCFunction)fastnumbers_try_real,
      METH_FASTCALL | METH_KEYWORDS,
      try_real__doc__ },
    { "fast_real",
      (PyCFunction)fastnumbers_fast_real,
      METH_FASTCALL | METH_KEYWORDS,
      fast_real__doc__ },
    { "try_float",
      (PyCFunction)fastnumbers_try_float,
      METH_FASTCALL | METH_KEYWORDS,
      try_float__doc__ },
    { "fast_float",
      (PyCFunction)fastnumbers_fast_float,
      METH_FASTCALL | METH_KEYWORDS,
      fast_float__doc__ },
    { "try_int",
      (PyCFunction)fastnumbers_try_int,
      METH_FASTCALL | METH_KEYWORDS,
      try_int__doc__ },
    { "fast_int",
      (PyCFunction)fastnumbers_fast_int,
      METH_FASTCALL | METH_KEYWORDS,
      fast_int__doc__ },
    { "try_forceint",
      (PyCFunction)fastnumbers_try_forceint,
      METH_FASTCALL | METH_KEYWORDS,
      try_forceint__doc__ },
    { "fast_forceint",
      (PyCFunction)fastnumbers_fast_forceint,
      METH_FASTCALL | METH_KEYWORDS,
      fast_forceint__doc__ },
    { "check_real",
      (PyCFunction)fastnumbers_check_real,
      METH_FASTCALL | METH_KEYWORDS,
      check_real__doc__ },
    { "isreal",
      (PyCFunction)fastnumbers_isreal,
      METH_FASTCALL | METH_KEYWORDS,
      isreal__doc__ },
    { "check_float",
      (PyCFunction)fastnumbers_check_float,
      METH_FASTCALL | METH_KEYWORDS,
      check_float__doc__ },
    { "isfloat",
      (PyCFunction)fastnumbers_isfloat,
      METH_FASTCALL | METH_KEYWORDS,
      isfloat__doc__ },
    { "isint",
      (PyCFunction)fastnumbers_isint,
      METH_FASTCALL | METH_KEYWORDS,
      isint__doc__ },
    { "check_int",
      (PyCFunction)fastnumbers_check_int,
      METH_FASTCALL | METH_KEYWORDS,
      check_int__doc__ },
    { "isintlike",
      (PyCFunction)fastnumbers_isintlike,
      METH_FASTCALL | METH_KEYWORDS,
      isintlike__doc__ },
    { "check_intlike",
      (PyCFunction)fastnumbers_check_intlike,
      METH_FASTCALL | METH_KEYWORDS,
      check_intlike__doc__ },
    { "query_type",
      (PyCFunction)fastnumbers_query_type,
      METH_FASTCALL | METH_KEYWORDS,
      query_type__doc__ },
    { "int",
      (PyCFunction)fastnumbers_int,
      METH_FASTCALL | METH_KEYWORDS,
      fastnumbers_int__doc__ },
    { "float", (PyCFunction)fastnumbers_float, METH_FASTCALL, fastnumbers_float__doc__ },
    { "real",
      (PyCFunction)fastnumbers_real,
      METH_FASTCALL | METH_KEYWORDS,
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
PyObject* NumericParser::PYTHON_ZERO = nullptr;

// Actually create the module object itself
PyMODINIT_FUNC PyInit_fastnumbers()
{
    PyObject* m = PyModule_Create(&moduledef);
    if (m == nullptr) {
        return nullptr;
    }

    // Add module level constants.
    PyModule_AddStringConstant(m, "__version__", FASTNUMBERS_VERSION);

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
    NumericParser::PYTHON_ZERO = PyLong_FromLong(0L);

    return m;
}
