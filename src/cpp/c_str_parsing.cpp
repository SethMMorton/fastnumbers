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

/// Convert a character to an integer in the desired type
template <typename T>
static inline T ascii2int(const char c)
{
    return static_cast<T>(c - '0');
}

/// A function that accepts an argument and does nothing
static inline void do_nothing(const char) { }

// FORWARD DECLARATIONS
template <typename Function>
static inline bool parse_integer_components(const char*& str, Function callback);
static inline bool parse_integer_components(const char*& str);
template <typename Function>
static inline bool parse_decimal_components(const char*& str, Function callback);
static inline bool parse_decimal_components(const char*& str);
template <typename NFunction, typename Function>
static inline bool
parse_exponent_components(const char*& str, NFunction sign_callback, Function callback);
static inline bool parse_exponent_components(const char*& str);
static inline uint32_t number_trailing_zeros(const char* start, const char* end);
static inline int detect_base(const char* str, const char* end);
static inline bool is_valid_digit_arbitrary_base(const char c, const int base);

// END FORWARD DECLARATIONS, BEGIN DEFINITITONS

/*********************/
/* EXPOSED FUNCTIONS */
/*********************/

int string_contains_what(const char* str, const char* end, int base)
{
    /* Define possible return values */
    static constexpr int INVALID = 0;
    static constexpr int INTEGER = 1;
    static constexpr int FLOAT = 2;
    static constexpr int INTLIKE_FLOAT = 3;

    if (base == 0) {
        base = detect_base(str, end);
    }

    /* Special-case non-base-10 integer detection */
    if (base < 0) {
        return INVALID;
    } else if (base != 10) {
        const std::size_t len = static_cast<std::size_t>(end - str);

        /* Skip leading characters for non-base 10 ints. */
        if (len > 1 && str[0] == '0' && is_base_prefix(str[1], base)) {
            str += 2;
        }

        /* The rest behaves as normal. */
        bool valid = false;
        while (is_valid_digit_arbitrary_base(*str, base)) {
            str += 1;
            valid = true;
        }
        return (valid && str == end) ? INTEGER : INVALID;
    }

    /* Before decimal. Keep track of number of digits read. */
    int value = INVALID;
    const char* int_start = str;
    bool valid = parse_integer_components(str);
    if (valid) {
        value = INTEGER;
    }

    /* Decimal part of float. Keep track of number of digits read */
    /* as well as beginning and end locations. */
    const char* decimal_start = str;
    uint32_t dec_length = 0;
    valid = parse_decimal_components(
                str,
                [&dec_length](const char) {
                    dec_length += 1;
                }
            )
        || valid;
    const char* decimal_end = str;
    if (valid && decimal_end != decimal_start) {
        value = FLOAT;
    }

    /* Exponential part of float. Parse the magnitude. */
    uint32_t expon = 0;
    bool exp_negative = false;
    if (valid) {
        const char* exp_start = str;
        valid = parse_exponent_components(
            str,
            [&exp_negative](const char) {
                exp_negative = true;
            },
            [&expon](const char c) {
                expon *= 10;
                expon += ascii2int<uint32_t>(c);
            }
        );
        const char* exp_end = str;
        if (valid && exp_end != exp_start) {
            value = FLOAT;
        }
    }

    /* If the parsing was not valid or we are not at the end of the string
     * then the string is invalid.
     * Othewise, do a check to see if it is an *intlike* float.
     */
    if (!valid || str != end) {
        return INVALID;
    } else if (value == FLOAT) {
        /* If we "move the decimal place" left or right depending on
         * exponent sign and magnitude, all digits after the decimal
         * must be zero.
         */
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

long parse_int(const char* str, const char* end, int base, bool& error, bool& overflow)
{
    const std::size_t len = static_cast<std::size_t>(end - str);

    // If the base needs to be guessed, do so now and get it over with.
    if (base == 0) {
        base = detect_base(str, end);
    }

    // Negative bases are illegal.
    if (base < 0) {
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
        long value;
        const std::from_chars_result res = std::from_chars(str, end, value, base);
        error = res.ptr != end || res.ec == std::errc::invalid_argument;
        overflow = res.ec == std::errc::result_out_of_range;
        return value;
    }

    // We use our own method for base-10 because we can omit some overflow
    // checking and get faster results.
    // We just assume overflow if the length of the string is over a certain value.
    overflow = len > FN_MAX_INT_LEN;

    // Convert digits.
    long value = 0L;
    bool valid = parse_integer_components(str, [&value](const char c) {
        value *= 10L;
        value += ascii2int<long>(c);
    });
    error = !valid || str != end;
    return value;
}

double parse_float(const char* str, const char* end, bool& error)
{
    // parse_float is not supposed to accept signed values, but from_chars
    // will accept negative signs. To prevent accidental success on e.g. "+-3.14"
    // we short-cicuit on a leading negative sign.
    if (*str == '-') {
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
        // use hex as the base because it is the most inclusive.
        for (; i < len; i++) {
            if (str[i] == '_' && i > 0 && i < len - 1
                && is_valid_digit_arbitrary_base(str[i - 1], 16)
                && is_valid_digit_arbitrary_base(str[i + 1], 16)) {
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
 * \brief Scan a string while integer components are found
 *
 * A user-defined callback function can be provided to perform
 * an operation when a digit is found.
 *
 * \param str The string to inspect. Assumed to be non-NULL
 * \param callback A function to call for each digit character.
 *                 Must accept a single char and return void.
 *
 * \returns true if at least one character was read as a digit
 */
template <typename Function>
bool parse_integer_components(const char*& str, Function callback)
{
    const char* start = str;
    while (is_valid_digit(*str)) {
        callback(*str);
        str += 1;
    }
    return str != start; // return whether or not a valid digit was found
}

bool parse_integer_components(const char*& str)
{
    return parse_integer_components(str, do_nothing);
}

/**
 * \brief Scan a string for decimal parts of a float
 *
 * It is basically the same as parse_integer_components except
 * that it allows a single '.' at the front of the string.
 *
 * A user-defined callback function can be provided to perform
 * an operation when a digit is found.
 *
 * \param str The string to inspect. Assumed to be non-NULL
 * \param callback A function to call for each digit character.
 *                 Must accept a single char and return void.
 *
 * \returns true if at least one character was read as a digit
 */
template <typename Function>
bool parse_decimal_components(const char*& str, Function callback)
{
    if (*str == '.') {
        str += 1;
        return parse_integer_components(str, callback);
    }
    return false;
}

bool parse_decimal_components(const char*& str)
{
    return parse_decimal_components(str, do_nothing);
}

/**
 * \brief Scan a string for exponents parts of a float
 *
 * Require that the string starts with 'e' or 'E', then
 * require that an (optionally signed) string of digits follows.
 *
 * A user-defined callback function can be provided to perform
 * an operation when a sign is found.
 *
 * A user-defined callback function can be provided to perform
 * an operation when a digit is found.
 *
 * \param str The string to inspect. Assumed to be non-NULL
 * \param callback A function to call for each digit character.
 *                 Must accept a single char and return void.
 *
 * \returns true if at least one character was read as a digit OR
 *          if nothing was read
 */
template <typename NFunction, typename Function>
bool parse_exponent_components(
    const char*& str, NFunction sign_callback, Function callback
)
{
    if (*str == 'e' || *str == 'E') {
        str += 1;
        if (is_sign(*str)) {
            sign_callback(*str);
            str += 1;
        }
        return parse_integer_components(str, callback);
    }
    return true;
}

bool parse_exponent_components(const char*& str)
{
    return parse_exponent_components(str, do_nothing, do_nothing);
}

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
    } else if (str[1] == 'x' || str[1] == 'X') {
        return 16;
    } else if (str[1] == 'o' || str[1] == 'O') {
        return 8;
    } else if (str[1] == 'b' || str[1] == 'B') {
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
        const char offset = static_cast<char>(base) - 10;
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'a' + offset)
            || (c >= 'A' && c <= 'A' + offset);
    }
}
