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

// FORWARD DECLARATIONS
static inline uint32_t number_trailing_zeros(const char* start, const char* end);
static inline int detect_base(const char* str, const char* end);
static inline bool is_valid_digit_arbitrary_base(const char c, const int base);

// END FORWARD DECLARATIONS, BEGIN DEFINITITONS

/*********************/
/* EXPOSED FUNCTIONS */
/*********************/

int string_contains_what(const char* str, const char* end, int base)
{
    const std::size_t len = static_cast<std::size_t>(end - str);

    // Define possible return values
    static constexpr int INVALID = 0;
    static constexpr int INTEGER = 1;
    static constexpr int FLOAT = 2;
    static constexpr int INTLIKE_FLOAT = 3;

    // If the base needs to be guessed, do so now and get it over with.
    if (base == 0) {
        base = detect_base(str, end);
    }

    // Negative bases are illegal. So is zero length.
    if (base < 0 || len == 0) {
        return INVALID;
    }

    // Special-case non-base 10.
    if (base != 10) {
        // Skip leading characters for non-base 10 ints.
        if (len > 1 && str[0] == '0' && is_base_prefix(str[1], base)) {
            str += 2;
        }

        // The rest behaves as normal.
        const char* digit_start = str;
        while (str != end && is_valid_digit_arbitrary_base(*str, base)) {
            str += 1;
        }
        return (str == end && str != digit_start) ? INTEGER : INVALID;
    }

    // Before decimal. Keep track of the start location.
    bool valid = false;
    int value = INVALID;
    const char* int_start = str;
    consume_digits(str, len);
    if (str != int_start) {
        valid = true;
        value = INTEGER;
    }

    // Decimal part of float. Keep track of number of digits read
    // as well as beginning and end locations.
    const char* decimal_start = str;
    uint32_t dec_length = 0;
    if (str != end && *str == '.') {
        str += 1;
        const char* dec_digits_start = str;
        consume_digits(str, static_cast<std::size_t>(end - str));
        dec_length = static_cast<uint32_t>(str - dec_digits_start);
        valid = valid || dec_length > 0;
    }
    const char* decimal_end = str;

    // If there are any decimal components then we are looking at a float.
    if (valid && decimal_end != decimal_start) {
        value = FLOAT;
    }

    // Exponential part of float. Parse the magnitude.
    uint32_t expon = 0;
    bool exp_negative = false;
    if (valid) {
        const char* exp_start = str;
        if (str != end && (*str == 'e' || *str == 'E')) {

            // Skip the 'e' and any sign that might be present.
            str += 1;
            if (str != end && is_sign(*str)) {
                exp_negative = *str == '-';
                str += 1;
            }

            // Parse the exponent as a digit.
            const char* exp_digit_start = str;
            int32_t this_char_as_digit = 0L;
            while (str != end && (this_char_as_digit = to_digit<int32_t>(*str)) >= 0) {
                expon = expon * 10L + this_char_as_digit;
                str += 1;
            }

            // If all we found was e.g. "e" or "e-" then it's not valid.
            valid = str != exp_digit_start;
        }
        const char* exp_end = str;

        // If we found an exponent AND it was parsed validly then this is a float.
        if (exp_end != exp_start && valid) {
            value = FLOAT;
        }
    }

    // If the parsing was not valid or we are not at the end of the string
    // then the string is invalid.
    // Othewise, do a check to see if it is an *intlike* float.
    if (!valid || str != end) {
        return INVALID;
    } else if (value == FLOAT) {
        // If we "move the decimal place" left or right depending on
        // exponent sign and magnitude, all digits after the decimal
        // must be zero.
        const unsigned int_trailing_zeros
            = number_trailing_zeros(int_start, decimal_start);
        const unsigned dec_trailing_zeros = decimal_start == decimal_end
            ? 0U
            : number_trailing_zeros(decimal_start + 1, decimal_end);

        if (exp_negative) {
            if (expon <= int_trailing_zeros && dec_length == dec_trailing_zeros) {
                value = INTLIKE_FLOAT;
            }
        } else {
            if (expon >= (dec_length - dec_trailing_zeros)) {
                value = INTLIKE_FLOAT;
            }
        }
    }
    return value;
}

int64_t
parse_int(const char* str, const char* end, int base, bool& error, bool& overflow)
{
    const std::size_t len = static_cast<std::size_t>(end - str);

    // If the base needs to be guessed, do so now and get it over with.
    if (base == 0) {
        base = detect_base(str, end);
    }

    // Negative bases are illegal. So is zero-length
    if (base < 0 || len == 0) {
        overflow = false;
        error = true;
        return -1;
    }

    // Use std::from_chars for all but base-10.
    if (base != 10) {
        // Skip leading characters for non-base 10 ints.
        if (len > 1 && str[0] == '0' && is_base_prefix(str[1], base)) {
            str += 2;
        }

        // Use a very fast and accurate string to integer parser
        // that will report back if there was an overflow (which
        // we propagete back to the user).
        int64_t value;
        const std::from_chars_result res = std::from_chars(str, end, value, base);
        error = res.ptr != end || res.ec == std::errc::invalid_argument;
        overflow = res.ec == std::errc::result_out_of_range;
        return value;
    }

    // We use our own method for base-10 because we can omit some overflow
    // checking and get faster results.
    //
    // We just assume overflow if the length of the string is over a certain value.
    // The number of digits chosen for overflow is 18 - this is one less than the
    // maximumlength of a 64-bit integer.
    //
    // 64-bit int max == 9223372036854775807; len('9223372036854775807') - 1 == 18
    overflow = len > 18;

    // If an overflow is going to happen, just evaluate that this looks like
    // an integer. Otherwise, actually calculate the value contained in the string.
    int64_t value = 0L;
    if (overflow) {
        consume_digits(str, len);
    } else {
        // Attempt to read eight characters at a time and parse as digits.
        // Loop over the character array in steps of eight. Stop processing
        // if not all eight characters are digits.
        const std::size_t number_of_eights = len / 8;
        for (std::size_t i = 0; i < number_of_eights; ++i) {
            if (fast_float::is_made_of_eight_digits_fast(str)) {
                value = value * 100000000 + fast_float::parse_eight_digits_unrolled(str);
                str += 8;
            } else {
                break;
            }
        }

        // Convert digits the remaining digits one-at-a-time.
        int64_t this_char_as_digit = 0L;
        while (str != end && (this_char_as_digit = to_digit<int64_t>(*str)) >= 0) {
            value = value * 10L + this_char_as_digit;
            str += 1;
        }
    }
    error = str != end;
    return value;
}

double parse_float(const char* str, const char* end, bool& error)
{
    // parse_float is not supposed to accept signed values, but from_chars
    // will accept negative signs. To prevent accidental success on e.g. "+-3.14"
    // we short-cicuit on a leading negative sign.
    if (str != end && *str == '-') {
        error = true;
        return -1.0;
    }

    // Use a very fast and accurate string to double parser
    double value;
    const fast_float::from_chars_result res = fast_float::from_chars(str, end, value);
    error = !(res.ptr == end && res.ec == std::errc());
    return value;
}

void remove_valid_underscores(char* str, const char*& end, const bool based)
{
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
        if (is_sign(*str)) {
            i += 1;
        }
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
        else if ((len - i) > 2 && str[i] == '0' && str[i + 1] == '_' && is_base_prefix(str[i + 2])) {
            i += 3;
        }

        // Now search for simpler valid underscores.
        // use base 36 as the base because it is the most inclusive.
        for (; i < len; i++) {
            if (str[i] == '_' && i > 0 && i < len - 1
                && is_valid_digit_arbitrary_base(str[i - 1], 36)
                && is_valid_digit_arbitrary_base(str[i + 1], 36)) {
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

/**************************/
/* IMPLEMENTATION DETAILS */
/**************************/

/**
 * \brief Check the number of zeros at the end of a number
 *
 * \param str The string to check, assumed to be non-NULL
 * \param end The end of the string being checked
 * \return The number of zeros
 */
uint32_t number_trailing_zeros(const char* start, const char* end)
{
    uint32_t n = 0;
    for (end = end - 1; end >= start; --end) {
        if (*end == '0') {
            n += 1;
        } else {
            break;
        }
    }
    return n;
}

/**
 * \brief Auto-detect the base of the given integer string
 *
 * \param str The string to check, assumed to be non-NULL
 * \param end The end of the string being checked
 * \return The dectected base, possibly 2, 8, 10, 16, or -1 on error
 */
int detect_base(const char* str, const char* end)
{
    const std::size_t len = static_cast<std::size_t>(end - str);
    if (str[0] != '0' || len == 1) {
        return 10;
    }

    const char lowered = lowercase(str[1]);
    if (lowered == 'x') {
        return 16;
    } else if (lowered == 'o') {
        return 8;
    } else if (lowered == 'b') {
        return 2;
    } else
        /* "old" (C-style) octal literal illegal in 3.x. */
        if (number_trailing_zeros(str, end) == len) {
            return 10;
        } else {
            return -1;
        }
}

/**
 * \brief Determine if a character represents a digit in a given base
 *
 * \param c The character to analyze
 * \param base The base to use for digit determination
 */
bool is_valid_digit_arbitrary_base(const char c, const int base)
{
    if (base < 10) {
        return c >= '0' && c <= (static_cast<int>('0') + base);
    } else {
        const char lowered = lowercase(c);
        const char offset = static_cast<char>(base) - 10;
        return is_valid_digit(c) || (lowered >= 'a' && lowered <= 'a' + offset);
    }
}
