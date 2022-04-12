#include "fastnumbers/parser.hpp"
#include "fastnumbers/c_str_parsing.hpp"

void Parser::set_input(PyObject* obj)
{
    // Clear any previous stored data
    reset();

    // Only save this if it is non-null
    if (obj != nullptr) {

        // If here, we are sure we are using the numeric parser
        m_ptype = ParserType::NUMERIC;

        // Store whether the number is a float or integer
        if (PyFloat_Check(obj)) {
            m_number_type = NumberType::FLOAT;
        } else if (PyLong_Check(obj)) {
            m_number_type = NumberType::INT;
        } else if (NumericMethodsAnalyzer(obj).is_numeric()) {
            m_number_type = NumberType::SPECIAL_NUMERIC;
        }

        // Store this object
        m_obj = obj;
        Py_IncRef(m_obj);
    }
}

void Parser::set_input(const Py_UCS4 uchar, const bool negative)
{
    // Clear any previous stored data
    reset();

    // We are using the unicode parser
    m_ptype = ParserType::UNICODE;

    // Extract the actual numeric data from the character,
    // and use this to evaluate if it is a float or integer
    m_numeric_uchar = Py_UNICODE_TONUMERIC(uchar);
    m_digit_uchar = Py_UNICODE_TODIGIT(uchar);
    if (m_digit_uchar > -1) {
        m_number_type = NumberType::INT;
    } else if (m_numeric_uchar > -1.0) {
        m_number_type = NumberType::FLOAT;
    }

    // Store character and the sign
    m_uchar = uchar;
    m_negative = negative;
}

void Parser::set_input(const char* str, const std::size_t len)
{
    // Clear any previous stored data
    reset();

    // Skip if this character array points to nothing
    if (str == nullptr) {
        return;
    }

    // We are using the character parser
    m_ptype = ParserType::CHARACTER;

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
    while (is_whitespace(*end) && m_start < end) {
        end -= 1;
    }
    end += 1;

    // Remove the sign if present and remember what it represents
    if (*m_start == '+') {
        m_start += 1;
    } else if (*m_start == '-') {
        m_start += 1;
        m_negative = true;
    }

    // Calculate the length of the string after accounting for
    // whitespace and signs
    m_str_len = static_cast<std::size_t>(end - m_start);
}

long Parser::as_int()
{
    // Reset any pre-existing error code
    unset_error_code();

    // Convert the data to an integer correctly based on the contained type
    // We do not intend to use the Parser to convert numeric objects, so
    // to enforce this we do not even implement that here.
    switch (m_ptype) {
    case ParserType::CHARACTER: {
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
    } break;

    case ParserType::UNICODE:
        if (m_number_type == NumberType::INT) {
            return sign() * m_digit_uchar;
        }
        break;

    case ParserType::NUMERIC:
    case ParserType::UNKNOWN:
    default:
        break;
    }

    // Any non-success above ends up here, record as failure
    encountered_conversion_error();
    return -1L;
}

double Parser::as_float()
{
    // Reset any pre-existing error code
    unset_error_code();

    // Convert the data to an integer correctly based on the contained type
    // We do not intend to use the Parser to convert numeric objects, so
    // to enforce this we do not even implement that here.
    switch (m_ptype) {
    case ParserType::CHARACTER: {
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
    } break;

    case ParserType::UNICODE:
        switch (m_number_type) {
        case NumberType::INT:
            return static_cast<double>(sign() * m_digit_uchar);
        case NumberType::FLOAT:
            return sign() * m_numeric_uchar;
        case NumberType::NOT_FLOAT_OR_INT:
        case NumberType::SPECIAL_NUMERIC:
        default:
            break;
        }
        break;

    case ParserType::NUMERIC:
    case ParserType::UNKNOWN:
    default:
        break;
    }

    // Any non-success above ends up here, record as failure
    encountered_conversion_error();
    return -1.0;
}

bool Parser::not_float_or_int() const
{
    return !is_real();
}

bool Parser::is_finite() const
{
    switch (m_ptype) {
    case ParserType::CHARACTER:
        return is_real() && !(is_infinity() || is_nan());
    case ParserType::UNICODE:
        return is_real();
    case ParserType::NUMERIC:
        return is_int() || (is_float() && std::isfinite(PyFloat_AS_DOUBLE(m_obj)));
    case ParserType::UNKNOWN:
    default:
        return false;
    }
}

bool Parser::is_infinity() const
{
    switch (m_ptype) {
    case ParserType::CHARACTER:
        return quick_detect_infinity(m_start, m_str_len);
    case ParserType::NUMERIC:
        return is_float() && std::isinf(PyFloat_AS_DOUBLE(m_obj));
    case ParserType::UNICODE:
    case ParserType::UNKNOWN:
    default:
        return false;
    }
}

bool Parser::is_nan() const
{
    switch (m_ptype) {
    case ParserType::CHARACTER:
        return quick_detect_nan(m_start, m_str_len);
    case ParserType::NUMERIC:
        return is_float() && std::isnan(PyFloat_AS_DOUBLE(m_obj));
    case ParserType::UNICODE:
    case ParserType::UNKNOWN:
    default:
        return false;
    }
}

bool Parser::is_real() const
{
    if (m_ptype == ParserType::CHARACTER) {
        return is_float();
    } else {
        return m_number_type != NumberType::NOT_FLOAT_OR_INT
            && m_number_type != NumberType::SPECIAL_NUMERIC;
    }
}

bool Parser::is_float() const
{
    switch (m_ptype) {
    case ParserType::CHARACTER:
        if (string_contains_float(m_start, end())) {
            return true;
        } else if (has_valid_underscores()) {
            Buffer buffer(m_start, m_str_len);
            buffer.remove_valid_underscores();
            return string_contains_float(buffer.start(), buffer.end());
        } else {
            return false;
        }
    case ParserType::UNICODE:
        return m_number_type != NumberType::NOT_FLOAT_OR_INT
            && m_number_type != NumberType::SPECIAL_NUMERIC;
    case ParserType::NUMERIC:
        return m_number_type == NumberType::FLOAT;
    case ParserType::UNKNOWN:
    default:
        return false;
    }
}

bool Parser::is_int() const
{
    if (m_ptype == ParserType::CHARACTER) {
        if (string_contains_int(m_start, end(), m_base)) {
            return true;
        } else if (has_valid_underscores()) {
            Buffer buffer(m_start, m_str_len);
            buffer.remove_valid_underscores(!is_default_base());
            return string_contains_int(buffer.start(), buffer.end(), m_base);
        } else {
            return false;
        }
    } else {
        return m_number_type == NumberType::INT;
    }
}

bool Parser::is_intlike() const
{
    if (m_ptype == ParserType::CHARACTER) {
        if (string_contains_intlike_float(m_start, end())) {
            return true;
        } else if (has_valid_underscores()) {
            Buffer buffer(m_start, m_str_len);
            buffer.remove_valid_underscores();
            return string_contains_intlike_float(buffer.start(), buffer.end());
        } else {
            return false;
        }
    } else {
        switch (m_number_type) {
        case NumberType::INT:
            return true;
        case NumberType::FLOAT: {
            const double d = m_ptype == ParserType::UNICODE ? m_numeric_uchar
                                                            : PyFloat_AS_DOUBLE(m_obj);
            return float_is_intlike(d);
        }
        case NumberType::NOT_FLOAT_OR_INT:
        case NumberType::SPECIAL_NUMERIC:
        default:
            return false;
        }
    }
}

void Buffer::remove_valid_underscores(const bool based)
{
    const char* new_end = end();
    ::remove_valid_underscores(start(), new_end, based);
    m_len = static_cast<std::size_t>(new_end - start());
}
