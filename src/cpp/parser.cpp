#include <cstdint>
#include <limits>
#include <type_traits>

#include <Python.h>

#include "fastnumbers/buffer.hpp"
#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/character.hpp"
#include "fastnumbers/parser/numeric.hpp"
#include "fastnumbers/parser/unicode.hpp"
#include "fastnumbers/user_options.hpp"

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
    , m_start(str)
    , m_start_orig(str)
    , m_end_orig(str + len)
    , m_str_len(0)
{
    // Store the end point of the character array
    const char* end = m_end_orig;

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

    // We can only have a sign here if there are two consecutive signs.
    // Two or more signs is illegal - let's treat it as such.
    // Reset the start to before the first sign.
    // All parsers will treat this as illegal now.
    if (is_sign(*m_start)) {
        m_start -= 1;
        set_negative(false);
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
    // The only thing special handling we need is underscores or base prefixes
    // with negative signs that caused overflow.
    bool error;
    bool overflow;
    int64_t result = parse_int<int64_t>(
        signed_start(), end(), options().get_base(), error, overflow
    );
    const bool underscore_error = error && has_valid_underscores();
    const bool prefix_overflow = overflow && has_base_prefix(m_start, m_str_len);
    if (underscore_error || prefix_overflow) {
        Buffer buffer(signed_start(), signed_len());
        buffer.remove_valid_underscores(options().get_base() != 10);
        int base = options().get_base();
        if (base == 0) {
            base = detect_base(buffer.start(), buffer.end());
        }
        buffer.remove_base_prefix();
        result = parse_int<int64_t>(buffer.start(), buffer.end(), base, error, overflow);
    }
    if (error) {
        encountered_conversion_error();
        return nullptr;
    }
    if (!overflow) {
        return pyobject_from_int64(result);
    }

    // Parse and record the location where parsing ended (including trailing whitespace)
    // No need to do input validation with the second argument because we already know
    // the input is valid from above.
    PyObject* retval = PyLong_FromString(m_start_orig, nullptr, options().get_base());

    // Return the value without checking python's error state
    return retval;
}

PyObject* CharacterParser::as_pyfloat(const bool force_int, const bool coerce)
{
    const double result = as_number<double>();

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
