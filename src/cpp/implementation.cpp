/*
 * This file contains the high-level implementations for the Python-exposed functions
 */
#include <limits>

#include <Python.h>

#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/extractor.hpp"
#include "fastnumbers/implementation.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/resolver.hpp"
#include "fastnumbers/selectors.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \brief Extract the return payload from a given python object
 *
 * \param obj The object from which to extract a payload
 * \param options User-specified options on how to extract
 * \param ntype The data type to extract
 *
 * \return The payload object containing the extracted data
 */
static inline Payload
collect_payload(PyObject* obj, const UserOptions& options, const UserType ntype)
{
    Buffer buffer;

    // The text extractor is responsible for taking a python object
    // and returning either a char array or single unicode character.
    // Depending on what would be returned, we pass the data to the
    // appropriate parser, and this parser is then passed to the evaluator
    // to decide how to convert the data into the appropriate payload.
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

/**
 * \brief Extract the contained numeric type from a given python object
 *
 * \param obj The object from which to determine numeric type
 * \param options User-specified options on how to determine type
 * \param consider Whether to consider string inputs, numeric inputs, or both
 *
 * \return The bitfield specifying the numeric type attributes in the given object
 */
static inline NumberFlags
collect_type(PyObject* obj, const UserOptions& options, const PyObject* consider)
{
    const bool num_only = consider == Selectors::NUMBER_ONLY;
    const bool str_only = consider == Selectors::STRING_ONLY;
    Buffer buffer;

    // The text extractor is responsible for taking a python object
    // and returning either a char array or single unicode character.
    // Depending on what would be returned, we pass the data to the
    // appropriate parser, and this parser is then passed to the evaluator
    // to decide how to convert the data into the appropriate payload.
    TextExtractor extractor(obj, buffer);
    if (num_only && (extractor.is_text() || extractor.is_unicode_character())) {
        return NumberType::INVALID;
    } else if (str_only && extractor.is_non_text()) {
        return NumberType::INVALID;
    } else if (extractor.is_text()) {
        CharacterParser cparser = extractor.text_parser(options);
        return Evaluator<CharacterParser>(obj, options, cparser).number_type();
    } else if (extractor.is_unicode_character()) {
        UnicodeParser uparser = extractor.unicode_char_parser(options);
        return Evaluator<UnicodeParser>(obj, options, uparser).number_type();
    } else {
        NumericParser nparser(obj, options);
        return Evaluator<NumericParser>(obj, options, nparser).number_type();
    }
}

/**
 * \brief Resolve the input into the appropriate return object
 *
 * \param options User-specified options on how to determine type
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param inf The value indicating what action to take when an infinity is encountered
 * \param nan The value indicating what action to take when an NaN is encountered
 * \param ntype The desired type of the converted object
 *
 * \return Converted python object or nullptr with appropriate error set
 */
static inline PyObject* do_resolve(
    const UserOptions& options,
    PyObject* input,
    PyObject* on_fail,
    PyObject* inf,
    PyObject* nan,
    const UserType ntype
)
{
    Resolver resolver(input, options);
    resolver.set_inf_action(inf);
    resolver.set_nan_action(nan);
    resolver.set_fail_action(on_fail);
    return resolver.resolve(collect_payload(input, options, ntype));
}

/**
 * \brief Resolve the input into the appropriate return object
 *
 * \param options User-specified options on how to determine type
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param on_type_error The value indicating what action to take on invalid type
 * \param inf The value indicating what action to take when an infinity is encountered
 * \param nan The value indicating what action to take when an NaN is encountered
 * \param ntype The desired type of the converted object
 *
 * \return Converted python object or nullptr with appropriate error set
 */
static inline PyObject* do_resolve(
    const UserOptions& options,
    PyObject* input,
    PyObject* on_fail,
    PyObject* on_type_error,
    PyObject* inf,
    PyObject* nan,
    const UserType ntype
)
{
    Resolver resolver(input, options);
    resolver.set_inf_action(inf);
    resolver.set_nan_action(nan);
    resolver.set_fail_action(on_fail);
    resolver.set_type_error_action(on_type_error);
    return resolver.resolve(collect_payload(input, options, ntype));
}

/**
 * \brief Resolve the input into the appropriate return object
 *
 * \param options User-specified options on how to determine type
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param ntype The desired type of the converted object
 *
 * \return Converted python object or nullptr with appropriate error set
 */
static inline PyObject* do_resolve(
    const UserOptions& options, PyObject* input, PyObject* on_fail, const UserType ntype
)
{
    Resolver resolver(input, options);
    resolver.set_fail_action(on_fail);
    return resolver.resolve(collect_payload(input, options, ntype));
}

/**
 * \brief Resolve the input into the appropriate return object
 *
 * \param options User-specified options on how to determine type
 * \param input The python object to consider for conversion
 * \param on_fail The value indicating what action to take on failure to convert
 * \param on_type_error The value indicating what action to take on invalid type
 * \param ntype The desired type of the converted object
 *
 * \return Converted python object or nullptr with appropriate error set
 */
static inline PyObject* do_resolve(
    const UserOptions& options,
    PyObject* input,
    PyObject* on_fail,
    PyObject* on_type_error,
    const UserType ntype
)
{
    Resolver resolver(input, options);
    resolver.set_fail_action(on_fail);
    resolver.set_type_error_action(on_type_error);
    return resolver.resolve(collect_payload(input, options, ntype));
}

// "Full" implementation for converting floats
PyObject* float_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    PyObject* inf,
    PyObject* nan,
    const UserType ntype,
    const bool allow_underscores,
    const bool coerce
)
{
    UserOptions options;
    options.set_coerce(coerce);
    options.set_underscores_allowed(allow_underscores);
    return do_resolve(options, input, on_fail, inf, nan, ntype);
}

// "Fuller" implementation for converting floats
PyObject* float_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    PyObject* on_type_error,
    PyObject* inf,
    PyObject* nan,
    const UserType ntype,
    const bool allow_underscores,
    const bool coerce
)
{
    UserOptions options;
    options.set_coerce(coerce);
    options.set_underscores_allowed(allow_underscores);
    return do_resolve(options, input, on_fail, on_type_error, inf, nan, ntype);
}

// "Reduced" implementation for converting floats
PyObject* float_conv_impl(PyObject* input, const UserType ntype, const bool coerce)
{
    UserOptions options;
    options.set_coerce(coerce);
    options.set_unicode_allowed(false);
    options.set_underscores_allowed(true);
    return do_resolve(options, input, Selectors::RAISE, ntype);
}

// "Full" implementation for converting integers
PyObject* int_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    const UserType ntype,
    const bool allow_underscores,
    const int base
)
{
    UserOptions options;
    options.set_base(base);
    options.set_unicode_allowed(options.is_default_base());
    options.set_underscores_allowed(allow_underscores);
    return do_resolve(options, input, on_fail, ntype);
}

// "Fuller" implementation for converting integers
PyObject* int_conv_impl(
    PyObject* input,
    PyObject* on_fail,
    PyObject* on_type_error,
    const UserType ntype,
    const bool allow_underscores,
    const int base
)
{
    UserOptions options;
    options.set_base(base);
    options.set_unicode_allowed(options.is_default_base());
    options.set_underscores_allowed(allow_underscores);
    return do_resolve(options, input, on_fail, on_type_error, ntype);
}

// "Reduced" implementation for converting integers
PyObject* int_conv_impl(PyObject* input, const UserType ntype, const int base)
{
    UserOptions options;
    options.set_base(base);
    options.set_unicode_allowed(false);
    options.set_underscores_allowed(true);
    return do_resolve(options, input, Selectors::RAISE, ntype);
}

/**
 * \brief Evaluate the type contained by the number type bitflags
 *
 * \param flags The number type flags to evaluate
 * \param options User options informing how to interpret the flags
 * \param ok_float (out) The value can be interpeted as a float
 * \param ok_int (out) The value can be interpreted as an int
 * \param ok_intlike (out) The value can be interpreted as intlike
 */
static inline void resolve_types(
    const NumberFlags& flags,
    const UserOptions& options,
    bool& from_str,
    bool& ok_float,
    bool& ok_int,
    bool& ok_intlike
)
{
    // Build up the logic with individual "Boolean chunks"
    from_str = bool(flags & (NumberType::FromStr | NumberType::FromUni));
    const bool from_num = bool(flags & NumberType::FromNum);
    const bool no_inf_str = from_str && !options.allow_inf_str();
    const bool no_nan_str = from_str && !options.allow_nan_str();
    const bool no_inf_num = from_num && !options.allow_inf_num();
    const bool no_nan_num = from_num && !options.allow_nan_num();
    const bool no_inf = no_inf_str || no_inf_num;
    const bool no_nan = no_nan_str || no_nan_num;
    const bool bad_inf = no_inf && flags & NumberType::Infinity;
    const bool bad_nan = no_nan && flags & NumberType::NaN;

    // Set the final results as results of the booleans
    ok_float = flags & NumberType::Float && !(bad_inf || bad_nan);
    ok_int = bool(flags & NumberType::Integer);
    ok_intlike = options.allow_coerce() && flags & NumberType::IntLike;
}

// Implementation for checking floats
PyObject* float_check_impl(
    PyObject* input,
    const PyObject* inf,
    const PyObject* nan,
    const PyObject* consider,
    const UserType ntype,
    const bool allow_underscores,
    const bool strict
)
{
    UserOptions options;
    options.set_underscores_allowed(allow_underscores);
    options.set_inf_allowed(inf);
    options.set_nan_allowed(nan);

    const NumberFlags flags = collect_type(input, options, consider);

    bool from_str, ok_float, ok_int, ok_intlike;
    resolve_types(flags, options, from_str, ok_float, ok_int, ok_intlike);

    // We are alwasy OK with integers for REAL.
    // For FLOAT, we are OK only if not in strict mode.
    ok_int = ntype == UserType::REAL ? ok_int : (from_str && !strict && ok_int);

    if (ok_float || ok_int) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Implementation for checking integers
PyObject* int_check_impl(
    PyObject* input,
    const PyObject* consider,
    const UserType ntype,
    const bool allow_underscores,
    const int base
)
{
    UserOptions options;
    options.set_base(base);
    options.set_coerce(ntype == UserType::INTLIKE);
    options.set_underscores_allowed(allow_underscores);

    const NumberFlags flags = collect_type(input, options, consider);

    bool from_str, ok_float, ok_int, ok_intlike;
    resolve_types(flags, options, from_str, ok_float, ok_int, ok_intlike);

    // ok_intline never be true unless set_coerce was given true
    if (ok_int || ok_intlike) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Implementation for returning object type
PyObject* type_query_impl(
    PyObject* input,
    PyObject* allowed_types,
    const PyObject* inf,
    const PyObject* nan,
    const bool allow_underscores,
    const bool coerce
)
{
    UserOptions options;
    options.set_coerce(coerce);
    options.set_underscores_allowed(allow_underscores);
    options.set_inf_allowed(inf);
    options.set_nan_allowed(nan);

    const NumberFlags flags = collect_type(input, options, nullptr);

    bool from_str, ok_float, ok_int, ok_intlike;
    resolve_types(flags, options, from_str, ok_float, ok_int, ok_intlike);

    // If the input can be interpreted as a number, return that number
    // type. Otherwise, return the type of the input.
    PyObject* found_type = (ok_int || ok_intlike)
        ? (PyObject*)&PyLong_Type
        : (ok_float ? (PyObject*)&PyFloat_Type : (PyObject*)Py_TYPE(input));

    // If allowed types were given and the found type is not there, return None
    if (allowed_types != nullptr && !PySequence_Contains(allowed_types, found_type)) {
        Py_RETURN_NONE;
    }

    // Return the type of the input
    Py_IncRef(found_type);
    return found_type;
}