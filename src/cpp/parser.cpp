#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/buffer.hpp"
#include "fastnumbers/parser/character.hpp"

CharacterParser::CharacterParser(const char* str, const std::size_t len)
    : SignedParser(ParserType::CHARACTER)
    , m_start(nullptr)
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
    const char* end = str + len;

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
    return Parser::as_int();
}

double CharacterParser::as_float()
{
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
            return -1L;
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
    return Parser::as_float();
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
