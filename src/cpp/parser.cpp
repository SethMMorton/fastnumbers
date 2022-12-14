#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/buffer.hpp"
#include "fastnumbers/parser/character.hpp"
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
    end -= 1;
    while (start < end && is_whitespace(*end)) {
        end -= 1;
    }
    end += 1;
}

CharacterParser::CharacterParser(
    const char* str,
    const std::size_t len,
    const UserOptions& options,
    const bool explict_base_allowed
)
    : Parser(ParserType::CHARACTER, options, explict_base_allowed)
    , m_start(nullptr)
    , m_end_orig(nullptr)
    , m_str_len(0)
{
    // Skip if this character array points to nothing
    if (str == nullptr) {
        set_as_unknown_parser();
        return;
    }

    // Store the start and end point of the character array
    m_start = str;
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

long CharacterParser::as_int()
{
    return as_type<long>(is_likely_int, int_might_overflow, parse_int);
}

double CharacterParser::as_float()
{
    return as_type<double>(is_likely_float, float_might_overflow, parse_float);
}

PyObject* CharacterParser::as_pyint()
{
    reset_error();

    // If the number is negative, include the sign as part of the string
    const std::size_t offset = std::size_t(is_negative());
    const char* start = m_start - offset;

    // Attempt to pre-detect if the string is a valid integer.
    // it is cheaper to do this than to let Python do that and construct an exception.
    // Need to account for underscores if they exist and we are allowing them.
    Buffer buffer;
    const char* my_end = m_end_orig;
    const bool is_integer = check_string_for_number(
        buffer,
        start,
        my_end,
        offset,
        [&](const char* start, const char* end) -> bool {
            return string_contains_int(start, end, options().get_base());
        }
    );
    if (!is_integer) {
        encountered_conversion_error();
        return nullptr;
    }

    // Parse and record the location where parsing ended (including trailing whitespace)
    char* their_end = nullptr;
    PyObject* retval = PyLong_FromString(start, &their_end, options().get_base());

    // Check the parsed end against the original end - if they don't match
    // there was a parsing error.
    if (their_end != my_end && retval != nullptr) {
        encountered_conversion_error();
        return nullptr;
    }

    // Return the value without checking python's error state - let Cython handle it
    return retval;
}

PyObject* CharacterParser::as_pyfloat()
{
    reset_error();

    // If the number is negative, include the sign as part of the string
    const std::size_t offset = std::size_t(is_negative());
    const char* start = m_start - offset;

    // Attempt to pre-detect if the string is a valid integer.
    // it is cheaper to do this than to let Python do that and construct an exception.
    // Need to account for underscores if they exist and we are allowing them.
    double retval = -1.0;
    char* their_end = nullptr;
    const char* my_end = end();
    Buffer buffer;
    if (!check_string_for_number(buffer, start, my_end, offset, string_contains_float)) {
        encountered_conversion_error();
        return nullptr;
    }
    strip_trailing_whitespace(start, my_end);

    // Parse and record the location where parsing ended (excluding trailing whitespace)
    retval = PyOS_string_to_double(start, &their_end, nullptr);

    // If an exception was raised propagate it. If the full string was not parsed, raise
    // an error also.
    if (retval == -1.0 && PyErr_Occurred()) {
        return nullptr;
    } else if (their_end != my_end) {
        encountered_conversion_error();
        return nullptr;
    }

    // Return the value as a python object
    return PyFloat_FromDouble(retval);
}

bool CharacterParser::is_infinity() const
{
    return quick_detect_infinity(m_start, m_str_len);
}

bool CharacterParser::is_nan() const
{
    return quick_detect_nan(m_start, m_str_len);
}

bool CharacterParser::is_float() const
{
    if (string_contains_float(m_start, end())) {
        return true;
    } else if (has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores();
        return string_contains_float(buffer.start(), buffer.end());
    } else {
        return false;
    }
}

bool CharacterParser::is_int() const
{
    if (string_contains_int(m_start, end(), options().get_base())) {
        return true;
    } else if (has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores(!options().is_default_base());
        return string_contains_int(buffer.start(), buffer.end(), options().get_base());
    } else {
        return false;
    }
}

bool CharacterParser::is_intlike() const
{
    if (string_contains_intlike_float(m_start, end())) {
        return true;
    } else if (has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores();
        return string_contains_intlike_float(buffer.start(), buffer.end());
    } else {
        return false;
    }
}

template <
    typename T,
    typename CheckFunction,
    typename OverflowCheckFunction,
    typename ConvertFunction>
T CharacterParser::as_type(
    CheckFunction check_function,
    OverflowCheckFunction overflow_check_function,
    ConvertFunction convert_function
)
{
    reset_error();

    // Do a quick check if the input likely contains the desired type
    if (check_function(m_start, m_str_len)) {

        // If the number will be too long report an overflow error
        if (overflow_check_function(m_start, m_str_len)) {
            if (has_invalid_underscores()) {
                encountered_conversion_error();
            } else {
                encountered_potential_overflow_error();
            }
            return static_cast<T>(-1);
        }

        // Convert to the desired type - handle the special case
        // of underscores in the string by removing them and trying again
        bool error = false;
        T result = convert_function(m_start, end(), error);
        if (!error) {
            return sign() * result;
        } else if (has_valid_underscores()) {
            Buffer buffer(m_start, m_str_len);
            buffer.remove_valid_underscores();
            result = convert_function(buffer.start(), buffer.end(), error);
            if (!error) {
                return sign() * result;
            }
        }
    }

    // Any non-success above ends up here, record as failure
    encountered_conversion_error();
    return static_cast<T>(-1);
}

template <typename Function>
bool CharacterParser::check_string_for_number(
    Buffer& buffer,
    const char*& start,
    const char*& original_end,
    const std::size_t offset,
    Function string_contains_number
)
{
    if (!string_contains_number(m_start, end())) {
        // If it does not contain a number, see if it is because of underscores.
        // If so, remove them and try again.
        // Otherwise, it does not contain a number.
        if (has_valid_underscores()) {
            const std::size_t len = static_cast<std::size_t>(original_end - start);
            buffer.copy(start, len);
            buffer.remove_valid_underscores(options().get_base() != 10);
            const char* b_start = buffer.start() + offset;
            const char* b_end = buffer.end();
            strip_trailing_whitespace(b_start, b_end);
            if (!string_contains_number(b_start, b_end)) {
                return false;
            }
            start = buffer.start();
            original_end = buffer.end();
        } else {
            return false;
        }
    }

    // If here, the string for sure contains the number
    return true;
}

void Buffer::remove_valid_underscores(const bool based)
{
    const char* new_end = end();
    ::remove_valid_underscores(start(), new_end, based);
    m_len = static_cast<std::size_t>(new_end - start());
}
