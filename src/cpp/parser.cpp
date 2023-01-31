#include "fastnumbers/buffer.hpp"
#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/character.hpp"
#include "fastnumbers/user_options.hpp"
#include <Python.h>
#include <limits>

/**
 * \brief Remove whitespace at the end of a string
 *
 * To do this successfully, move the end pointer
 * back one to not be on the '\0' character, move
 * backwards, then push the end pointer up one
 * as if there were still a '\0' character.
 *
 * \param start The beginning of the string to strip
 * \param end The end of the string to strip - updated in-place
 */
inline void strip_trailing_whitespace(const char* start, const char*& end)
{
    while (start < end && is_whitespace(*(end - 1))) {
        end -= 1;
    }
}

/// Convert a 64-int with the appropriate function depending on the compiler's long size
inline PyObject* pyobject_from_int64(const int64_t value)
{
    if constexpr (std::numeric_limits<long>::max() == std::numeric_limits<int64_t>::max()) {
        return PyLong_FromLong(static_cast<long>(value));
    } else {
        return PyLong_FromLongLong(static_cast<long long>(value));
    };
}

CharacterParser::CharacterParser(
    const char* str,
    const std::size_t len,
    const UserOptions& options,
    const bool explict_base_allowed
)
    : Parser(ParserType::CHARACTER, options, explict_base_allowed)
    , m_start(nullptr)
    , m_start_orig(nullptr)
    , m_end_orig(nullptr)
    , m_str_len(0)
{
    // Skip if this character array points to nothing
    if (str == nullptr) {
        set_as_unknown_parser();
        return;
    }

    // Store the start and end point of the character array
    m_start = m_start_orig = str;
    const char* end = m_end_orig = str + len;

    // Strip leading whitespace
    while (is_whitespace(*m_start)) {
        m_start += 1;
    }

    // Strip trailing whitespace.
    strip_trailing_whitespace(m_start, end);

    // Remove the sign if present and remember what it represents
    if (*m_start == '+') {
        m_start += 1;
    } else if (*m_start == '-') {
        m_start += 1;
        set_negative();
    }

    // Calculate the length of the string after accounting for
    // whitespace and signs
    m_str_len = static_cast<std::size_t>(end - m_start);
}

PyObject* CharacterParser::as_pyint()
{
    reset_error();

    // We use the fast path method even if the result overflows,
    // so that we can determine if the integer was at least valid.
    // If it was valid but overflowed, we use Python's parser, otherwise
    // return an error.
    // The only thing special handling we need is underscores.
    bool error;
    bool overflow;
    int64_t result = parse_int(m_start, end(), options().get_base(), error, overflow);
    if (error && has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores(options().get_base() != 10);
        result = parse_int(
            buffer.start(), buffer.end(), options().get_base(), error, overflow
        );
    }
    if (error) {
        encountered_conversion_error();
        return nullptr;
    }
    if (!overflow) {
        return pyobject_from_int64(sign() * result);
    }

    // Parse and record the location where parsing ended (including trailing whitespace)
    char* their_end = nullptr;
    PyObject* retval = PyLong_FromString(m_start_orig, &their_end, options().get_base());

    // Check the parsed end against the original end - if they don't match
    // there was a parsing error.
    if (their_end != m_end_orig && retval != nullptr) {
        encountered_conversion_error();
        return nullptr;
    }

    // Return the value without checking python's error state
    return retval;
}

double CharacterParser::as_double()
{
    reset_error();

    // The C++ parser is robust enough to handle any double properly,
    // so there is no need to fall back on Python's parser.
    // The only thing special handling we need is underscores.
    bool error;
    double result = parse_float(m_start, end(), error);
    if (error && has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores();
        result = parse_float(buffer.start(), buffer.end(), error);
    }
    if (error) {
        encountered_conversion_error();
        return -1.0;
    }
    return sign() * result;
}

PyObject* CharacterParser::as_pyfloat()
{
    const double result = as_double();
    return errored() ? nullptr : PyFloat_FromDouble(result);
}

PyObject* CharacterParser::as_pyfloat(const bool force_int, const bool coerce)
{
    const double result = as_double();

    // Fail fast on error
    if (errored()) {
        return nullptr;
    }

    // force_int takes precidence, and coerce conditionally returns as an integer
    if (force_int) {
        return PyLong_FromDouble(result);
    } else if (coerce) {
        return Parser::float_is_intlike(result) ? PyLong_FromDouble(result)
                                                : PyFloat_FromDouble(result);
    } else {
        return PyFloat_FromDouble(result);
    }
}

NumberFlags CharacterParser::get_number_type() const
{
    // If this value is cached, use that instead of re-calculating
    if (Parser::get_number_type() != static_cast<NumberFlags>(NumberType::UNSET)) {
        return Parser::get_number_type();
    }

    // If the string contains an infinity or NaN then we don't need to do any
    // other fancy processing and can return now.
    if (quick_detect_infinity(m_start, m_str_len)) {
        return flag_wrap(NumberType::Float | NumberType::Infinity);

    } else if (quick_detect_nan(m_start, m_str_len)) {
        return flag_wrap(NumberType::Float | NumberType::NaN);
    }

    // If the string contains a numeric representation,
    // report which representation type is contained.
    int value = string_contains_what(m_start, end(), options().get_base());

    // If it still looks like there is no numeric representation in the string,
    // check to see if it it contains underscores, and if so remove them and
    // try a numeric representation again. No need to check for infinity and
    // NaN here because those are not allowed to contain underscores.
    if (value == 0 && has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores(!options().is_default_base());
        value = string_contains_what(buffer.start(), buffer.end(), options().get_base());
    }

    // Map integer values to numeric flag values
    static constexpr NumberFlags type_mapping[] = {
        /* 0 */ NumberType::INVALID,
        /* 1 */ flag_wrap(NumberType::Integer),
        /* 2 */ flag_wrap(NumberType::Float),
        /* 3 */ flag_wrap(NumberType::Float | NumberType::IntLike),
    };

    // Return the found type
    return type_mapping[value];
}
