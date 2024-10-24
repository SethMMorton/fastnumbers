/*
 * This file contains the functions that directly interface with the Python interpreter.
 */
#include <exception>
#include <functional>
#include <limits>
#include <string>
#include <utility>

#include <Python.h>

#include "fastnumbers/argparse.hpp"
#include "fastnumbers/docstrings.hpp"
#include "fastnumbers/exception.hpp"
#include "fastnumbers/implementation.hpp"
#include "fastnumbers/selectors.hpp"

/**
 * \brief Function to handle the conversion of base to integers.
 *
 * \param pybase The base as a Python object
 * \param base The base as an integer
 * \throws fastnumbers_exception on invalid input
 */
static inline int assess_integer_base_input(PyObject* pybase) noexcept(false)
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
) noexcept(false)
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
static inline PyObject*
create_consider(const bool str_only, const bool num_only) noexcept
{
    if (str_only) {
        return Selectors::STRING_ONLY;
    } else if (num_only) {
        return Selectors::NUMBER_ONLY;
    } else {
        return Py_None;
    }
}

/**
 * \brief Make the value of map one of three possible values
 * \param mapval The value of map given on input
 * \return Either PyList_Type, Py_True, or Py_False
 */
static inline PyObject* normalize_map(PyObject* mapval) noexcept
{
    if (mapval == (PyObject*)&PyList_Type) {
        return mapval;
    } else if (PyObject_IsTrue(mapval)) {
        return Py_True;
    } else {
        return Py_False;
    }
}

/**
 * \brief Execute the conversion function as a one-off or as an iterable
 * \param input The input from Python-land
 * \param convert The function that converts our input to output
 * \param map If True or list execute as an iterable, otherwise as a one-off
 * \return The object to return to Python-land
 */
static PyObject* choose_execution_scheme(
    PyObject* input, std::function<PyObject*(PyObject*)> convert, const PyObject* map
) noexcept(false)
{
    if (map == Py_True) {
        return iter_iteration_impl(input, convert);
    } else if (map == (PyObject*)&PyList_Type) {
        return list_iteration_impl(input, convert);
    } else {
        return convert(input);
    }
}

/**
 * \brief Quickly convert to an int or float, depending on value, with error handling
 */
static PyObject* fastnumbers_try_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    bool coerce = true;
    bool denoise = false;
    bool allow_underscores = false;
    PyObject* map = Py_False;

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
                           "$map", false, &map,
                           "$denoise", true, &denoise,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        // Use a lambda instead of the convert function directly so that the
        // Implementation object stays in memory even if we return an iterator.
        Implementation impl(UserType::REAL);
        impl.set_fail_action(on_fail);
        impl.set_type_error_action(on_type_error);
        impl.set_inf_action(inf);
        impl.set_nan_action(nan);
        impl.set_coerce(coerce);
        impl.set_denoise(denoise);
        impl.set_underscores_allowed(allow_underscores);
        auto convert = [impl = std::move(impl)](PyObject* x) -> PyObject* {
            return impl.convert(x);
        };
        return choose_execution_scheme(input, convert, normalize_map(map));
    });
}

/**
 * \brief Quickly convert to a float, with error handling
 */
static PyObject* fastnumbers_try_float(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    bool allow_underscores = false;
    PyObject* map = Py_False;

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
                           "$map", false, &map,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        // Use a lambda instead of the convert function directly so that the
        // Implementation object stays in memory even if we return an iterator.
        Implementation impl(UserType::FLOAT);
        impl.set_fail_action(on_fail);
        impl.set_type_error_action(on_type_error);
        impl.set_inf_action(inf);
        impl.set_nan_action(nan);
        impl.set_underscores_allowed(allow_underscores);
        auto convert = [impl = std::move(impl)](PyObject* x) -> PyObject* {
            return impl.convert(x);
        };
        return choose_execution_scheme(input, convert, normalize_map(map));
    });
}

/**
 * \brief Quickly convert to an int, with error handling
 */
static PyObject* fastnumbers_try_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    PyObject* pybase = nullptr;
    bool allow_underscores = false;
    PyObject* map = Py_False;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("try_int", args, len_args, kwnames,
                           "x", false,  &input,
                           "$on_fail", false, &on_fail,
                           "$on_type_error", false, &on_type_error,
                           "$base", false, &pybase,
                           "$allow_underscores", true, &allow_underscores,
                           "$map", false, &map,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        // Use a lambda instead of the convert function directly so that the
        // Implementation object stays in memory even if we return an iterator.
        Implementation impl(UserType::INT, assess_integer_base_input(pybase));
        impl.set_fail_action(on_fail);
        impl.set_type_error_action(on_type_error);
        impl.set_unicode_allowed(); // determine from base
        impl.set_underscores_allowed(allow_underscores);
        auto convert = [impl = std::move(impl)](PyObject* x) -> PyObject* {
            return impl.convert(x);
        };
        return choose_execution_scheme(input, convert, normalize_map(map));
    });
}

/**
 * \brief Quickly convert to an int (even if input is float), with error handling
 */
static PyObject* fastnumbers_try_forceint(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* on_fail = Selectors::INPUT;
    PyObject* on_type_error = Selectors::RAISE;
    bool allow_underscores = false;
    bool denoise = false;
    PyObject* map = Py_False;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("try_forceint", args, len_args, kwnames,
                           "x", false,  &input,
                           "$on_fail", false, &on_fail,
                           "$on_type_error", false, &on_type_error,
                           "$allow_underscores", true, &allow_underscores,
                           "$map", false, &map,
                           "$denoise", true, &denoise,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        // Use a lambda instead of the convert function directly so that the
        // Implementation object stays in memory even if we return an iterator.
        Implementation impl(UserType::FORCEINT);
        impl.set_fail_action(on_fail);
        impl.set_type_error_action(on_type_error);
        impl.set_denoise(denoise);
        impl.set_underscores_allowed(allow_underscores);
        auto convert = [impl = std::move(impl)](PyObject* x) -> PyObject* {
            return impl.convert(x);
        };
        return choose_execution_scheme(input, convert, normalize_map(map));
    });
}

/**
 * \brief Like try_*, but return in a memory buffer
 */
static PyObject* fastnumbers_array(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* output = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    PyObject* on_fail = Selectors::RAISE;
    PyObject* on_overflow = Selectors::RAISE;
    PyObject* on_type_error = Selectors::RAISE;
    PyObject* pybase = nullptr;
    bool allow_underscores = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("array", args, len_args, kwnames,
                           "input", false,  &input,
                           "output", false, &output,
                           "$inf", false, &inf,
                           "$nan", false, &nan,
                           "$on_fail", false, &on_fail,
                           "$on_overflow", false, &on_overflow,
                           "$on_type_error", false, &on_type_error,
                           "$base", false, &pybase,
                           "$allow_underscores", true, &allow_underscores,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        array_impl(
            input,
            output,
            inf,
            nan,
            on_fail,
            on_overflow,
            on_type_error,
            allow_underscores,
            assess_integer_base_input(pybase)
        );

        // No return value, need to return None
        Py_RETURN_NONE;
    });
}

/**
 * \brief Quickly determine if the input is a real.
 */
static PyObject* fastnumbers_check_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* consider = Py_None;
    PyObject* inf = Selectors::NUMBER_ONLY;
    PyObject* nan = Selectors::NUMBER_ONLY;
    bool allow_underscores = false;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::REAL);
        impl.set_inf_allowed(inf);
        impl.set_nan_allowed(nan);
        impl.set_consider(consider);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

/**
 * \brief Quickly determine if the input is a float.
 */
static PyObject* fastnumbers_check_float(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* consider = Py_None;
    PyObject* inf = Selectors::NUMBER_ONLY;
    PyObject* nan = Selectors::NUMBER_ONLY;
    int strict = false;
    bool allow_underscores = false;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::FLOAT);
        impl.set_inf_allowed(inf);
        impl.set_nan_allowed(nan);
        impl.set_consider(consider);
        impl.set_strict(strict);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

/**
 * \brief Quickly determine if the input is an int.
 */
static PyObject* fastnumbers_check_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* consider = Py_None;
    PyObject* pybase = nullptr;
    bool allow_underscores = false;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::INT, assess_integer_base_input(pybase));
        impl.set_consider(consider);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

/**
 * \brief Quickly determine if the input is an int or int-like float.
 */
static PyObject* fastnumbers_check_intlike(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::INTLIKE);
        impl.set_consider(consider);
        impl.set_coerce(true);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

/**
 * \brief Quickly determine the type
 */
static PyObject* fastnumbers_query_type(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* allowed_types = nullptr;
    bool coerce = false;
    int allow_inf = false;
    int allow_nan = false;
    bool allow_underscores = false;

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

    // Convert old-style arguments to new-style
    const PyObject* inf = allow_inf ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;
    const PyObject* nan = allow_nan ? Selectors::ALLOWED : Selectors::NUMBER_ONLY;

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::REAL); // type doesn't matter, choose REAL
        impl.set_inf_allowed(inf);
        impl.set_nan_allowed(nan);
        impl.set_coerce(coerce);
        impl.set_allowed_types(allowed_types);
        impl.set_underscores_allowed(allow_underscores);
        return impl.query_type(input);
    });
}

/**
 * \brief Drop-in replacement for float
 */
static PyObject*
fastnumbers_float(PyObject* self, PyObject* const* args, Py_ssize_t len_args) noexcept
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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::FLOAT);
        impl.set_unicode_allowed(false);
        impl.set_underscores_allowed(true);
        return impl.convert(input);
    });
}

/**
 * \brief Drop-in replacement for int
 */
static PyObject* fastnumbers_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::INT, assess_integer_base_input(pybase));
        impl.set_unicode_allowed(false);
        impl.set_underscores_allowed(true);
        return impl.convert(input);
    });
}

/**
 * \brief Behaves like float or int, but returns correct type
 */
static PyObject* fastnumbers_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    bool coerce = true;
    bool denoise = false;

    // Read the function arguments
    FN_PREPARE_ARGPARSER;
    // clang-format off
    if (fn_parse_arguments("real", args, len_args, kwnames,
                           "|", false,  &input,
                           "$coerce", true, &coerce,
                           "$denoise", true, &denoise,
                           nullptr, false, nullptr
        )) return nullptr;
    // clang-format on

    // No arguments returns 0.0 or 0 depending on the state of coerce
    if (input == nullptr) {
        return coerce ? PyLong_FromLong(0) : PyFloat_FromDouble(0.0);
    }

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::REAL);
        impl.set_coerce(coerce);
        impl.set_denoise(denoise);
        impl.set_unicode_allowed(false);
        impl.set_underscores_allowed(true);
        return impl.convert(input);
    });
}

/// Deprecated, use try_real
static PyObject* fastnumbers_fast_real(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    int raise_on_invalid = false;
    bool coerce = true;
    bool allow_underscores = true;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );

        Implementation impl(UserType::REAL);
        impl.set_fail_action(on_fail);
        impl.set_inf_action(inf);
        impl.set_nan_action(nan);
        impl.set_coerce(coerce);
        impl.set_underscores_allowed(allow_underscores);
        return impl.convert(input);
    });
}

/// Deprecated, use try_float
static PyObject* fastnumbers_fast_float(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* inf = Selectors::ALLOWED;
    PyObject* nan = Selectors::ALLOWED;
    int raise_on_invalid = false;
    bool allow_underscores = true;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );

        Implementation impl(UserType::FLOAT);
        impl.set_fail_action(on_fail);
        impl.set_inf_action(inf);
        impl.set_nan_action(nan);
        impl.set_underscores_allowed(allow_underscores);
        return impl.convert(input);
    });
}

/// Deprecated, use try_int
static PyObject* fastnumbers_fast_int(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    PyObject* pybase = nullptr;
    int raise_on_invalid = false;
    bool allow_underscores = true;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );

        Implementation impl(UserType::INT, assess_integer_base_input(pybase));
        impl.set_fail_action(on_fail);
        impl.set_unicode_allowed(); // determine from base
        impl.set_underscores_allowed(allow_underscores);
        return impl.convert(input);
    });
}

/// Deprecated, use try_forceint
static PyObject* fastnumbers_fast_forceint(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* default_value = nullptr;
    PyObject* on_fail = nullptr;
    PyObject* key = nullptr;
    int raise_on_invalid = false;
    bool allow_underscores = true;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        handle_fail_backwards_compatibility(
            on_fail, key, default_value, raise_on_invalid
        );

        Implementation impl(UserType::FORCEINT);
        impl.set_fail_action(on_fail);
        impl.set_underscores_allowed(allow_underscores);
        return impl.convert(input);
    });
}

/// Deprecated, use check_real
static PyObject* fastnumbers_isreal(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_inf = false;
    int allow_nan = false;
    bool allow_underscores = true;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::REAL);
        impl.set_inf_allowed(inf);
        impl.set_nan_allowed(nan);
        impl.set_consider(consider);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

/// Deprecated, use check_float
static PyObject* fastnumbers_isfloat(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    int str_only = false;
    int num_only = false;
    int allow_inf = false;
    int allow_nan = false;
    bool allow_underscores = true;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::FLOAT);
        impl.set_inf_allowed(inf);
        impl.set_nan_allowed(nan);
        impl.set_consider(consider);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

/// Deprecated, use check_int
static PyObject* fastnumbers_isint(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
{
    PyObject* input = nullptr;
    PyObject* pybase = nullptr;
    int str_only = false;
    int num_only = false;
    bool allow_underscores = true;

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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        const int base = assess_integer_base_input(pybase);
        Implementation impl(UserType::INT, base);
        impl.set_consider(consider);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

/// Deprecated, use check_intlike
static PyObject* fastnumbers_isintlike(
    PyObject* self, PyObject* const* args, Py_ssize_t len_args, PyObject* kwnames
) noexcept
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

    // Execute main logic in an exception handler to convert C++ exceptions
    return ExceptionHandler(input).run([&]() -> PyObject* {
        Implementation impl(UserType::INTLIKE);
        impl.set_consider(consider);
        impl.set_coerce(true);
        impl.set_underscores_allowed(allow_underscores);
        return impl.check(input);
    });
}

// Define the methods contained in this module
static PyMethodDef FastnumbersMethods[] = {
    { "try_real",
      (PyCFunction)fastnumbers_try_real,
      METH_FASTCALL | METH_KEYWORDS,
      try_real__doc__ },
    { "try_float",
      (PyCFunction)fastnumbers_try_float,
      METH_FASTCALL | METH_KEYWORDS,
      try_float__doc__ },
    { "try_int",
      (PyCFunction)fastnumbers_try_int,
      METH_FASTCALL | METH_KEYWORDS,
      try_int__doc__ },
    { "try_forceint",
      (PyCFunction)fastnumbers_try_forceint,
      METH_FASTCALL | METH_KEYWORDS,
      try_forceint__doc__ },
    { "array",
      (PyCFunction)fastnumbers_array,
      METH_FASTCALL | METH_KEYWORDS,
      "C-implementation of try_array" },
    { "check_real",
      (PyCFunction)fastnumbers_check_real,
      METH_FASTCALL | METH_KEYWORDS,
      check_real__doc__ },
    { "check_float",
      (PyCFunction)fastnumbers_check_float,
      METH_FASTCALL | METH_KEYWORDS,
      check_float__doc__ },
    { "check_int",
      (PyCFunction)fastnumbers_check_int,
      METH_FASTCALL | METH_KEYWORDS,
      check_int__doc__ },
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
    { "fast_real",
      (PyCFunction)fastnumbers_fast_real,
      METH_FASTCALL | METH_KEYWORDS,
      fast_real__doc__ },
    { "fast_float",
      (PyCFunction)fastnumbers_fast_float,
      METH_FASTCALL | METH_KEYWORDS,
      fast_float__doc__ },
    { "fast_int",
      (PyCFunction)fastnumbers_fast_int,
      METH_FASTCALL | METH_KEYWORDS,
      fast_int__doc__ },
    { "fast_forceint",
      (PyCFunction)fastnumbers_fast_forceint,
      METH_FASTCALL | METH_KEYWORDS,
      fast_forceint__doc__ },
    { "isreal",
      (PyCFunction)fastnumbers_isreal,
      METH_FASTCALL | METH_KEYWORDS,
      isreal__doc__ },
    { "isfloat",
      (PyCFunction)fastnumbers_isfloat,
      METH_FASTCALL | METH_KEYWORDS,
      isfloat__doc__ },
    { "isint",
      (PyCFunction)fastnumbers_isint,
      METH_FASTCALL | METH_KEYWORDS,
      isint__doc__ },
    { "isintlike",
      (PyCFunction)fastnumbers_isintlike,
      METH_FASTCALL | METH_KEYWORDS,
      isintlike__doc__ },
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
