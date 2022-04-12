#include "fastnumbers/parser.hpp"
#include "fastnumbers/c_str_parsing.hpp"

void Parser::set_input(PyObject* obj)
{
    // Clear any previous stored data
    reset();

    // Only save this if it is non-null
    if (obj != nullptr) {

        // If here, we are sure we are using the numeric parser
        ptype = ParserType::NUMERIC;

        // Store whether the number is a float or integer
        if (PyFloat_Check(obj)) {
            number_type = NumberType::FLOAT;
        } else if (PyLong_Check(obj)) {
            number_type = NumberType::INT;
        } else if (NumericMethodsAnalyzer(obj).is_numeric()) {
            number_type = NumberType::SPECIAL_NUMERIC;
        }

        // Store this object
        this->obj = obj;
        Py_IncRef(this->obj);
    }
}

void Parser::set_input(const Py_UCS4 uchar, const bool negative)
{
    // Clear any previous stored data
    reset();

    // We are using the unicode parser
    ptype = ParserType::UNICODE;

    // Extract the actual numeric data from the character,
    // and use this to evaluate if it is a float or integer
    numeric_uchar = Py_UNICODE_TONUMERIC(uchar);
    digit_uchar = Py_UNICODE_TODIGIT(uchar);
    if (digit_uchar > -1) {
        number_type = NumberType::INT;
    } else if (numeric_uchar > -1.0) {
        number_type = NumberType::FLOAT;
    }

    // Store character and the sign
    this->uchar = uchar;
    this->negative = negative;
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
    ptype = ParserType::CHARACTER;

    // Store the start and end point of the character array
    start = str;
    const char* end = str + len;

    // Strip leading whitespace
    while (is_whitespace(*start)) {
        start += 1;
    }

    // Strip trailing whitespace.
    // To do this successfully, move the end pointer
    // back one to not be on the '\0' character, move
    // backwards, then push the end pointer up one
    // as if there were still a '\0' character.
    end -= 1;
    while (is_whitespace(*end) and start < end) {
        end -= 1;
    }
    end += 1;

    // Remove the sign if present and remember what it represents
    if (*start == '+') {
        start += 1;
    } else if (*start == '-') {
        start += 1;
        negative = true;
    }

    // Calculate the length of the string after accounting for
    // whitespace and signs
    str_len = end - start;
}

long Parser::as_int()
{
    // Reset any pre-existing error code
    unset_error_code();

    // Convert the data to an integer correctly based on the contained type
    // We do not intend to use the Parser to convert numeric objects, so
    // to enforce this we do not even implement that here.
    switch (ptype) {
    case ParserType::CHARACTER: {
        if (is_likely_int(start, str_len)) {
            if (int_might_overflow(start, str_len)) {
                if (has_invalid_underscores()) {
                    encountered_conversion_error();
                } else {
                    encountered_potential_overflow_error();
                }
                return -1L;
            }
            bool error = false;
            long result = parse_int(start, end(), error);
            if (not error) {
                return sign() * result;
            } else if (has_valid_underscores()) {
                Buffer buffer(start, str_len);
                buffer.remove_valid_underscores();
                result = parse_int(buffer.start(), buffer.end(), error);
                if (not error) {
                    return sign() * result;
                }
            }
        }
    } break;

    case ParserType::UNICODE:
        switch (number_type) {
        case NumberType::INT:
            return sign() * digit_uchar;
        default:
            break;
        }
        break;

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
    switch (ptype) {
    case ParserType::CHARACTER: {
        if (is_likely_float(start, str_len)) {
            if (float_might_overflow(start, str_len)) {
                if (has_invalid_underscores()) {
                    encountered_conversion_error();
                } else {
                    encountered_potential_overflow_error();
                }
                return -1L;
            }
            bool error = false;
            double result = parse_float(start, end(), error);
            if (not error) {
                return sign() * result;
            } else if (has_valid_underscores()) {
                Buffer buffer(start, str_len);
                buffer.remove_valid_underscores();
                result = parse_float(buffer.start(), buffer.end(), error);
                if (not error) {
                    return sign() * result;
                }
            }
        }
    } break;

    case ParserType::UNICODE:
        switch (number_type) {
        case NumberType::INT:
            return static_cast<double>(sign() * digit_uchar);
        case NumberType::FLOAT:
            return sign() * numeric_uchar;
        default:
            break;
        }
        break;

    default:
        break;
    }

    // Any non-success above ends up here, record as failure
    encountered_conversion_error();
    return -1.0;
}

bool Parser::not_float_or_int() const
{
    return not is_real();
}

bool Parser::is_finite() const
{
    switch (ptype) {
    case ParserType::CHARACTER:
        return is_real() and not(is_infinity() or is_nan());
    case ParserType::UNICODE:
        return is_real();
    case ParserType::NUMERIC:
        return is_int() or (is_float() and std::isfinite(PyFloat_AS_DOUBLE(obj)));
    default:
        return false;
    }
}

bool Parser::is_infinity() const
{
    switch (ptype) {
    case ParserType::CHARACTER:
        return quick_detect_infinity(start, str_len);
    case ParserType::NUMERIC:
        return is_float() and std::isinf(PyFloat_AS_DOUBLE(obj));
    default:
        return false;
    }
}

bool Parser::is_nan() const
{
    switch (ptype) {
    case ParserType::CHARACTER:
        return quick_detect_nan(start, str_len);
    case ParserType::NUMERIC:
        return is_float() and std::isnan(PyFloat_AS_DOUBLE(obj));
    default:
        return false;
    }
}

bool Parser::is_real() const
{
    switch (ptype) {
    case ParserType::CHARACTER:
        return is_float();
    default:
        return number_type != NumberType::NOT_FLOAT_OR_INT
            and number_type != NumberType::SPECIAL_NUMERIC;
    }
}

bool Parser::is_float() const
{
    switch (ptype) {
    case ParserType::CHARACTER:
        if (string_contains_float(start, end())) {
            return true;
        } else if (has_valid_underscores()) {
            Buffer buffer(start, str_len);
            buffer.remove_valid_underscores();
            return string_contains_float(buffer.start(), buffer.end());
        } else {
            return false;
        }
    case ParserType::UNICODE:
        return number_type != NumberType::NOT_FLOAT_OR_INT
            and number_type != NumberType::SPECIAL_NUMERIC;
    case ParserType::NUMERIC:
        return number_type == NumberType::FLOAT;
    default:
        return false;
    }
}

bool Parser::is_int() const
{
    switch (ptype) {
    case ParserType::CHARACTER:
        if (string_contains_int(start, end(), base)) {
            return true;
        } else if (has_valid_underscores()) {
            Buffer buffer(start, str_len);
            buffer.remove_valid_underscores(not is_default_base());
            return string_contains_int(buffer.start(), buffer.end(), base);
        } else {
            return false;
        }
    default:
        return number_type == NumberType::INT;
    }
}

bool Parser::is_intlike() const
{
    switch (ptype) {
    case ParserType::CHARACTER:
        if (string_contains_intlike_float(start, end())) {
            return true;
        } else if (has_valid_underscores()) {
            Buffer buffer(start, str_len);
            buffer.remove_valid_underscores();
            return string_contains_intlike_float(buffer.start(), buffer.end());
        } else {
            return false;
        }
    default:
        switch (number_type) {
        case NumberType::INT:
            return true;
        case NumberType::FLOAT: {
            const double d
                = ptype == ParserType::UNICODE ? numeric_uchar : PyFloat_AS_DOUBLE(obj);
            return float_is_intlike(d);
        }
        default:
            return false;
        }
    }
}

void Buffer::remove_valid_underscores(const bool based)
{
    const char* new_end = end();
    ::remove_valid_underscores(start(), new_end, based);
    len = new_end - start();
}
