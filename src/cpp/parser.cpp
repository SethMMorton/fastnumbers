#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/buffer.hpp"
#include "fastnumbers/parser/character.hpp"

CharacterParser::CharacterParser(
    const char* str, const std::size_t len, const bool explict_base_allowed
)
    : SignedParser(ParserType::CHARACTER, explict_base_allowed)
    , m_start(nullptr)
    , m_end_orig(nullptr)
    , m_str_len(0)
    , m_underscore_allowed(true)
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
    // To do this successfully, move the end pointer
    // back one to not be on the '\0' character, move
    // backwards, then push the end pointer up one
    // as if there were still a '\0' character.
    end -= 1;
    while (m_start < end && is_whitespace(*end)) {
        end -= 1;
    }
    end += 1;

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
    reset_error();

    // Convert the data to an integer correctly based on the contained type
    // We do not intend to use the Parser to convert numeric objects, so
    // to enforce this we do not even implement that here.
    if (is_likely_int(m_start, m_str_len)) {
        if (int_might_overflow(m_start, m_str_len)) {
            if (has_invalid_underscores()) {
                encountered_conversion_error();
            } else {
                encountered_potential_overflow_error();
            }
            return -1L;
        }
        bool error = false;
        long result = parse_int(m_start, end(), error);
        if (!error) {
            return sign() * result;
        } else if (has_valid_underscores()) {
            Buffer buffer(m_start, m_str_len);
            buffer.remove_valid_underscores();
            result = parse_int(buffer.start(), buffer.end(), error);
            if (!error) {
                return sign() * result;
            }
        }
    }

    // Any non-success above ends up here, record as failure
    encountered_conversion_error();
    return -1L;
}

double CharacterParser::as_float()
{
    reset_error();

    // Convert the data to an integer correctly based on the contained type
    // We do not intend to use the Parser to convert numeric objects, so
    // to enforce this we do not even implement that here.
    if (is_likely_float(m_start, m_str_len)) {
        if (float_might_overflow(m_start, m_str_len)) {
            if (has_invalid_underscores()) {
                encountered_conversion_error();
            } else {
                encountered_potential_overflow_error();
            }
            return -1.0;
        }
        bool error = false;
        double result = parse_float(m_start, end(), error);
        if (!error) {
            return sign() * result;
        } else if (has_valid_underscores()) {
            Buffer buffer(m_start, m_str_len);
            buffer.remove_valid_underscores();
            result = parse_float(buffer.start(), buffer.end(), error);
            if (!error) {
                return sign() * result;
            }
        }
    }

    // Any non-success above ends up here, record as failure
    encountered_conversion_error();
    return -1.0;
}

PyObject* CharacterParser::as_pyint()
{
    reset_error();

    // If the number is negative, include the sign as part of the string
    const char* start = is_negative() ? m_start - 1 : m_start;

    // Parse and record the location where parsing ended (including trailing whitespace)
    char* their_end = nullptr;
    PyObject* retval = PyLong_FromString(start, &their_end, get_base());

    // Check the parsed end against the original end - if they don't match
    // there was a parsing error.
    if (their_end != m_end_orig && retval != nullptr) {
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
    const char* start = is_negative() ? m_start - 1 : m_start;
    const std::size_t str_len = is_negative() ? m_str_len + 1 : m_str_len;

    // Parse and record the location where parsing ended (excluding trailing whitespace)
    // handle possible underscores in the string (because Python won't do it for us)
    double retval = -1.0;
    char* their_end = nullptr;
    const char* my_end = end();
    if (has_valid_underscores()) {
        Buffer buffer(start, str_len);
        buffer.remove_valid_underscores();
        my_end = buffer.end();
        retval = PyOS_string_to_double(buffer.start(), &their_end, nullptr);
    } else {
        retval = PyOS_string_to_double(start, &their_end, nullptr);
    }

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
    if (string_contains_int(m_start, end(), get_base())) {
        return true;
    } else if (has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores(!is_default_base());
        return string_contains_int(buffer.start(), buffer.end(), get_base());
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

void Buffer::remove_valid_underscores(const bool based)
{
    const char* new_end = end();
    ::remove_valid_underscores(start(), new_end, based);
    m_len = static_cast<std::size_t>(new_end - start());
}
