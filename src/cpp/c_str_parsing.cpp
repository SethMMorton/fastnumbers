/******************/
/****** NOTE ******/
/******************/
// All functions in this file assume whitespace has been trimmed
// from both sides of the string, and that the sign has been removed.

#include "fastnumbers/c_str_parsing.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

/// Convert a character to an integer in the desired type
template <typename T> static inline T ascii2int(const char c)
{
    return static_cast<T>(c - '0');
}

/// A function that accepts an argument and does nothing
static inline void do_nothing(const char) { }

/// Map to the approprate scaling factor for an exponent value
constexpr long double POWER_OF_TEN_SCALING_FACTOR[] = {
    1E0L,   1E1L,   1E2L,   1E3L,   1E4L,   1E5L,   1E6L,   1E7L,   1E8L,   1E9L,
    1E10L,  1E11L,  1E12L,  1E13L,  1E14L,  1E15L,  1E16L,  1E17L,  1E18L,  1E19L,
    1E20L,  1E21L,  1E22L,  1E23L,  1E24L,  1E25L,  1E26L,  1E27L,  1E28L,  1E29L,
    1E30L,  1E31L,  1E32L,  1E33L,  1E34L,  1E35L,  1E36L,  1E37L,  1E38L,  1E39L,
    1E40L,  1E41L,  1E42L,  1E43L,  1E44L,  1E45L,  1E46L,  1E47L,  1E48L,  1E49L,
    1E50L,  1E51L,  1E52L,  1E53L,  1E54L,  1E55L,  1E56L,  1E57L,  1E58L,  1E59L,
    1E60L,  1E61L,  1E62L,  1E63L,  1E64L,  1E65L,  1E66L,  1E67L,  1E68L,  1E69L,
    1E70L,  1E71L,  1E72L,  1E73L,  1E74L,  1E75L,  1E76L,  1E77L,  1E78L,  1E79L,
    1E80L,  1E81L,  1E82L,  1E83L,  1E84L,  1E85L,  1E86L,  1E87L,  1E88L,  1E89L,
    1E90L,  1E91L,  1E92L,  1E93L,  1E94L,  1E95L,  1E96L,  1E97L,  1E98L,  1E99L,
    1E100L, 1E101L, 1E102L, 1E103L, 1E104L, 1E105L, 1E106L, 1E107L, 1E108L, 1E109L,
    1E110L, 1E111L, 1E112L, 1E113L, 1E114L, 1E115L, 1E116L, 1E117L, 1E118L, 1E119L,
    1E120L, 1E121L, 1E122L, 1E123L, 1E124L, 1E125L, 1E126L, 1E127L, 1E128L, 1E129L,
    1E130L, 1E131L, 1E132L, 1E133L, 1E134L, 1E135L, 1E136L, 1E137L, 1E138L, 1E139L,
    1E140L, 1E141L, 1E142L, 1E143L, 1E144L, 1E145L, 1E146L, 1E147L, 1E148L, 1E149L,
};
constexpr int32_t LEN_EXP_ARRAY
    = sizeof(POWER_OF_TEN_SCALING_FACTOR) / sizeof(POWER_OF_TEN_SCALING_FACTOR[0]);
constexpr int32_t MAX_EXP_VALUE = LEN_EXP_ARRAY - 1;

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
static inline bool neg_exp_ok(const char* str, std::size_t len);
static inline bool pos_exp_ok(const char* str, std::size_t len);
static inline uint32_t number_trailing_zeros(const char* start, const char* end);
static inline int detect_base(const char* str, const char* end);
static inline bool is_valid_digit_arbitrary_base(const char c, const int base);

// END FORWARD DECLARATIONS, BEGIN DEFINITITONS

/*********************/
/* EXPOSED FUNCTIONS */
/*********************/

bool string_contains_int(const char* str, const char* end, int base)
{
    if (base == 0) {
        base = detect_base(str, end);
    }

    /* If base 10, take fast route. */
    if (base == 10) {
        return parse_integer_components(str) && str == end;
    } else if (base == -1) {
        return false;
    } else {
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
        return valid && str == end;
    }
}

bool string_contains_float(const char* str, const char* end)
{
    bool valid = parse_integer_components(str);
    valid = parse_decimal_components(str) || valid;
    if (valid) {
        valid = parse_exponent_components(str);
    }
    return valid && str == end;
}

bool string_contains_intlike_float(const char* str, const char* end)
{
    /* Before decimal. Keep track of number of digits read. */
    const char* int_start = str;
    bool valid = parse_integer_components(str);

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

    /* Exponential part of float. Parse the magnitude. */
    uint32_t expon = 0;
    bool exp_negative = false;
    if (valid) {
        valid = parse_exponent_components(
            str,
            [&exp_negative](const char) {
                exp_negative = true;
            },
            [&expon](const char c) {
                expon *= 10;
                expon += ascii2int<int32_t>(c);
            }
        );
    }

    if (!valid || str != end) {
        return false;
    } else {
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
            return expon <= int_trailing_zeros && dec_length == dec_trailing_zeros;
        } else {
            return expon >= (dec_length - dec_trailing_zeros);
        }
    }
}

long parse_int(const char* str, const char* end, bool& error)
{
    long value = 0L;

    /* Convert digits, if any. */
    bool valid = parse_integer_components(str, [&value](const char c) {
        value *= 10L;
        value += ascii2int<long>(c);
    });

    error = !valid || str != end;
    return value;
}

double parse_float(const char* str, const char* end, bool& error)
{
    /* Parse integer part. */
    uint64_t intvalue = 0UL;
    bool valid = parse_integer_components(str, [&intvalue](const char c) {
        intvalue *= 10UL;
        intvalue += ascii2int<uint64_t>(c);
    });

    /* Parse decimal part. */
    uint32_t decimal_len = 0;
    valid = parse_decimal_components(
                str,
                [&intvalue, &decimal_len](const char c) {
                    intvalue *= 10UL;
                    intvalue += ascii2int<uint64_t>(c);
                    decimal_len += 1U;
                }
            )
        || valid;

    /* Parse exponential part. */
    int32_t expon = 0;
    if (valid) {
        int32_t exp_sign = 1;
        valid = parse_exponent_components(
            str,
            [&exp_sign](const char c) {
                if (c == '-') {
                    exp_sign = -1;
                }
            },
            [&expon](const char c) {
                expon *= 10;
                expon += ascii2int<int16_t>(c);
            }
        );
        expon *= exp_sign;
    }
    expon -= decimal_len; /* Adjust the exponent by the # of decimal places */

    error = !valid || str != end;
    if (expon < 0) {
        expon = std::abs(expon);
        return static_cast<double>(
            static_cast<long double>(intvalue)
            / POWER_OF_TEN_SCALING_FACTOR[std::min(expon, MAX_EXP_VALUE)]
        );
    } else {
        return static_cast<double>(
            static_cast<long double>(intvalue)
            * POWER_OF_TEN_SCALING_FACTOR[std::min(expon, MAX_EXP_VALUE)]
        );
    }
}

bool float_might_overflow(const char* start, const std::size_t len)
{
    // Locate the decimal place (if any).
    const char* decimal_loc = static_cast<const char*>(std::memchr(start, '.', len));
    const bool has_decimal = static_cast<bool>(decimal_loc);

    // Find the exponent (if any) in the input.
    // It is always after the exponent, usually close to the end, so
    // we will start from the back. No need to include the stop
    // location since that is either a decimal or the first digit
    // which cannot be 'e' or 'E'.
    const char* ptr = nullptr;
    const char* exp = nullptr;
    const char* stop = decimal_loc ? decimal_loc : start;
    for (ptr = start + len - 1; ptr > stop; ptr--) {
        if (*ptr == 'e' || *ptr == 'E') {
            exp = ptr;
            break;
        }
    }

    // If the number of pre-exponent digits is greater than the known
    // value it might overflow.
    const std::size_t decimal_offset = static_cast<std::size_t>(has_decimal);
    const std::size_t len_pre_exp = exp ? (exp - start) : len;
    if (len_pre_exp - decimal_offset > FN_DBL_DIG) {
        return true;
    }

    // If an exponent was found, ensure it is within chosen range.
    if (exp) {
        const bool neg = *(++exp) == '-'; // First remove 'e' or 'E'.
        std::size_t exp_len = len - (exp - start);
        if (is_sign(*exp)) {
            exp += 1;
            exp_len -= 1;
        }
        const bool exp_ok = neg ? neg_exp_ok(exp, exp_len) : pos_exp_ok(exp, exp_len);
        if (!exp_ok) {
            return true;
        }
    }

    // Won't overflow
    return false;
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

#ifndef FASTNUMBERS_WIDE_EXP_RANGE
/// Helper to check if an exponent number is in the allowed range
static inline bool _exp_ok(const char* str, std::size_t len)
{
    return len == 1 || (len == 2 && (*str <= '2' || (*str == '2' && *(str + 1) <= '2')));
}
#endif

/**
 * \brief Check if a negative exponent number is in the allowed range
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
bool neg_exp_ok(const char* str, std::size_t len)
{
#ifdef FASTNUMBERS_WIDE_EXP_RANGE
    return len == 1 || (len == 2 && (*str <= '8' || (*str == '9' && *(str + 1) <= '8')));
#else
    return _exp_ok(str, len);
#endif
}

/**
 * \brief Check if a positive exponent number is in the allowed range
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
bool pos_exp_ok(const char* str, std::size_t len)
{
#ifdef FASTNUMBERS_WIDE_EXP_RANGE
    return len > 0 && len <= 2;
#else
    return _exp_ok(str, len);
#endif
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
