/******************/
/****** NOTE ******/
/******************/
// All functions in this file assume whitespace has been trimmed
// from both sides of the string, and that the sign has been removed.

#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/third_party/fast_float.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstring>

/*********************/
/* EXPOSED FUNCTIONS */
/*********************/

StringChecker::StringChecker(const char* str, const char* end, int base) noexcept
    : m_integer_start(nullptr)
    , m_decimal_start(nullptr)
    , m_decimal_end(nullptr)
    , m_total_end(nullptr)
    , m_expon(0U)
    , m_exp_negative(false)
    , m_int_trailing_zeros(0U)
    , m_dec_trailing_zeros(0U)
    , m_contained_type(StringType::INVALID)
{
    const std::size_t len = static_cast<std::size_t>(end - str);

    // If the base needs to be guessed, do so now and get it over with.
    if (base == 0) {
        base = detect_base(str, end);
    }

    // Negative bases are illegal. So is zero length.
    if (base < 0 || len == 0) {
        set_integer_start(str);
        set_decimal_start(str);
        set_decimal_end(str);
        set_total_end(str);
        return;
    }

    // Special-case non-base 10.
    if (base != 10) {
        // Skip leading characters for non-base 10 ints.
        if (len > 1 && str[0] == '0' && is_base_prefix(str[1], base)) {
            str += 2;
        }

        // The rest behaves as normal.
        set_integer_start(str);
        while (str != end && is_valid_digit(*str, base)) {
            str += 1;
        }
        set_decimal_start(str);
        set_decimal_end(str);
        set_total_end(str);
        set_type(
            (str == end && str != integer_start()) ? StringType::INTEGER
                                                   : StringType::INVALID
        );
        return;
    }

    // Before decimal. Keep track of the start location.
    bool valid = false;
    set_integer_start(str);
    consume_digits(str, len);
    if (str != integer_start()) {
        valid = true;
        set_type(StringType::INTEGER);
    }

    // Decimal part of float. Keep track of number of digits read
    // as well as beginning and end locations.
    set_decimal_start(str);
    uint32_t dec_length = 0;
    if (str != end && *str == '.') {
        str += 1;
        const char* dec_digits_start = str;
        consume_digits(str, static_cast<std::size_t>(end - str));
        dec_length = static_cast<uint32_t>(str - dec_digits_start);
        valid = valid || dec_length > 0;
    }
    set_decimal_end(str);

    // If there are any decimal components then we are looking at a float.
    if (valid && has_decimal_data()) {
        set_type(StringType::FLOAT);
    }

    // Exponential part of float. Parse the magnitude.
    if (valid) {
        const char* exp_start = str;
        if (str != end && (*str == 'e' || *str == 'E')) {

            // Skip the 'e' and any sign that might be present.
            str += 1;
            if (str != end && is_sign(*str)) {
                set_exponent_negative(*str == '-');
                str += 1;
            }

            // Parse the exponent as a digit.
            const char* exp_digit_start = str;
            int32_t this_char_as_digit = 0L;
            while (str != end && (this_char_as_digit = to_digit<int32_t>(*str)) >= 0) {
                set_exponent(exponent_value() * 10UL + this_char_as_digit);
                str += 1;
            }

            // If all we found was e.g. "e" or "e-" then it's not valid.
            valid = str != exp_digit_start;
        }
        const char* exp_end = str;

        // If we found an exponent AND it was parsed validly then this is a float.
        if (exp_end != exp_start && valid) {
            set_type(StringType::FLOAT);
        }
    }
    set_total_end(str);

    // If the parsing was not valid or we are not at the end of the string
    // then the string is invalid.
    // Othewise, do a check to see if it is an *intlike* float.
    if (!valid || str != end) {
        set_type(StringType::INVALID);
    } else if (is_float()) {
        // If we "move the decimal place" left or right depending on
        // exponent sign and magnitude, all digits after the decimal
        // must be zero.
        set_int_trailing_zeros(number_trailing_zeros(integer_start(), integer_end()));
        set_dec_trailing_zeros(number_trailing_zeros(decimal_start(), decimal_end()));
        const uint32_t expval = exponent_value();
        if (is_exponent_negative()) {
            if (expval <= integer_trailing_zeros() && truncated_decimal_length() == 0) {
                set_type(StringType::INTLIKE_FLOAT);
            }
        } else {
            if (expval >= truncated_decimal_length()) {
                set_type(StringType::INTLIKE_FLOAT);
            }
        }
    }
}

void remove_valid_underscores(char* str, const char*& end, const bool based) noexcept
{
    // Ignore a leading negative sign
    if (*str == '-') {
        str += 1;
    }

    const std::size_t len = static_cast<std::size_t>(end - str);
    std::size_t i, offset;

    // The method that will be used to remove underscores is to
    // traverse the character array, and when a valid underscore
    // is found all characters will be shifted one to the left in
    // order to remove that underscore. Extra characters at the
    // end of the character array will be overwritten with \0.

    // For non-based strings, parsing is "simple" -
    // a valid underscore is surrounded by two numbers.
    if (!based) {
        for (i = offset = 0; i < len; i++) {
            if (str[i] == '_' && i > 0 && i < len - 1 && is_valid_digit(str[i - 1])
                && is_valid_digit(str[i + 1])) {
                offset += 1;
                continue;
            }
            if (offset) {
                str[i - offset] = str[i];
            }
        }
    }

    // For based strings we must incorporate some state at the
    // beginning of the string before the more simple "surrounded
    // by two numbers" algorithm kicks in.
    else {
        i = offset = 0;

        // Skip leading characters for non-base 10 ints.
        if ((len - i) > 1 && str[i] == '0' && is_base_prefix(str[i + 1])) {
            // An underscore after the prefix is allowed, e.g. 0x_d4.
            if ((len - i > 2) && str[i + 2] == '_') {
                i += 3;
                offset += 1;
            } else {
                i += 2;
            }
        }
        // No underscore in the base selector, e.g. 0_b0 is invalid.
        else if ((len - i) > 2 && str[i] == '0' && str[i + 1] == '_'
                 && is_base_prefix(str[i + 2])) {
            i += 3;
        }

        // Now search for simpler valid underscores.
        // use base 36 as the base because it is the most inclusive.
        for (; i < len; i++) {
            if (str[i] == '_' && i > 0 && i < len - 1 && is_valid_digit(str[i - 1], 36)
                && is_valid_digit(str[i + 1], 36)) {
                offset += 1;
                continue;
            }
            if (offset) {
                str[i - offset] = str[i];
            }
        }
    }

    // Update the end position.
    end = str + (i - offset);

    // Fill the trailing data with nul characters.
    for (i = len - offset; i < len; i++) {
        str[i] = '\0';
    }
}
