#pragma once

#include "fastnumbers/third_party/fast_float.h"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <limits>

/// Table of what characters are classified as whitespace
constexpr bool WHITESPACE_TABLE[]
    = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/// Table of what characters are classified as digits
constexpr int DIGIT_TABLE[]
    = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

/**
 * \brief Convert a string to a long type
 *
 * Assumes no sign or whitespace. No overflow checking is performed.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 * \param base The base to assume when checking an integer, 0 is "guess".
 * \param error Flag to indicate if there was a parsing error
 * \param overflow Flag to indicate if the string was long enough to overflow
 */
long parse_int(const char* str, const char* end, int base, bool& error, bool& overflow);

/**
 * \brief Convert a string to a double type
 *
 * Assumes no sign or whitespace. No overflow checking is performed.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 * \param error Flag to indicate if there was a parsing error
 */
double parse_float(const char* str, const char* end, bool& error);

/**
 * \brief Check if a string could be converted to some numeric type
 *
 * Assumes no sign or whitespace.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 * \param base The base to assume when checking an integer, set to 10
 *             unless you know it *must* be an integer.
 * \return 0 - invalid
 *         1 - integer
 *         2 - float
 *         3 - "intlike" float
 */
int string_contains_what(const char* str, const char* end, int base);

/**
 * \brief Remove underscores in a numeric-representing string
 *
 * Assumes the input can ba modified.
 *
 * In a generic number, valid underscores are between two digits.
 * A "based" number is necessarily an int, and in those cases the
 * definition of a valid underscore is a bit less well-defined.
 *
 * \param str The string from which to remove underscores
 * \param end Reference to the end of the string - after processing will
 *            point to the new end of the string
 * \param based Whether or not the string contains a non-base-10 integer
 */
void remove_valid_underscores(char* str, const char*& end, const bool based);

/**
 * \brief Maximum number of digits we allow in an integer for conversion
 *
 * The number of digits in an int that fastnumbers is willing
 * to attempt to convert itself. This is entirely based on the size
 * of a this compiler's long, since the long is what Python uses to
 * represent an int under the hood - use one less than the maximum
 * length of a long.
 *
 * 64-bit int max == 9223372036854775807; len('9223372036854775807') - 1 == 18
 * 32-bit int max == 2147483647; len('2147483647') - 1 == 9
 */
constexpr long FN_MAX_INT_LEN
    = std::numeric_limits<unsigned long>::max() == 9223372036854775807 ? 18 : 9;

/**
 * \brief Lowercase a character - does no error checking
 */
constexpr inline char lowercase(const char c)
{
    // The ASCII standard was quite clever... upper- and lower-case
    // letters only differ from each other by the 32 bit, otherwise
    // they are identical. ORing the 32-bit forces lowercase.
    return static_cast<char>(c | 32);
}

/**
 * \brief Determine if a character is whitespace
 */
constexpr inline bool is_whitespace(const char c)
{
    // Using a table was determined through performance testing to be
    // many times faster than std::isspace, and twice as fast as using
    // a switch statement.
    return WHITESPACE_TABLE[static_cast<uint8_t>(c)];
}

/**
 * \brief Advance a string's pointer while whitespace is found
 */
constexpr inline void consume_whitespace(const char*& str, const char* end)
{
    while (str != end && is_whitespace(*str)) {
        str += 1;
    }
}

/**
 * \brief Convert a character to a digit, returns -1 on failure.
 */
template <typename T>
constexpr inline T to_digit(const char c)
{
    // Using a table was determined through performance testing to be
    // three times faster than c - '0' or a switch statement.
    return static_cast<T>(DIGIT_TABLE[static_cast<uint8_t>(c)]);
}

/**
 * \brief Determine if a character represents a digit
 */
constexpr inline bool is_valid_digit(const char c)
{
    // Using a table was determined through performance testing to be
    // faster than std::isdigit or a switch statement.
    return to_digit<int>(c) >= 0;
}

/**
 * \brief Advance a string's pointer while digits are found
 */
constexpr inline void consume_digits(const char*& str, const char* end)
{
    while (str != end && is_valid_digit(*str)) {
        str += 1;
    }
}

/**
 * \brief Advance a string's pointer while digits are found
 *        and attempt to read multiple digits at a time if possible
 */
inline void consume_digits(const char*& str, const std::size_t len)
{
    // Attempt to read eight characters at a time to determine
    // if they are digits. Loop over the character array in steps
    // of eight. Stop processing if not all eight characters are digits.
    const std::size_t number_of_eights = len / 8;
    for (std::size_t i = 0; i < number_of_eights; ++i) {
        if (fast_float::is_made_of_eight_digits_fast(str)) {
            str += 8;
        } else {
            break;
        }
    }

    // Read the remainder of digits one-at-a-time.
    consume_digits(str, str + len);
}

/**
 * \brief Determine if a character is '-' or '+'
 */
constexpr inline bool is_sign(const char c)
{
    return c == '-' || c == '+';
}

/**
 * \brief Determine if a character a prefix for base 2, 8, or 16
 */
constexpr inline bool is_base_prefix(const char c)
{
    // The ASCII standard was quite clever... upper- and lower-case
    // letters only differ from each other by the 32 bit, otherwise
    // they are identical.
    // So, we can OR the 32 bit to force the character to be
    // lowercase and then just check against the lowercase characters.
    const char lowered = lowercase(c);
    return (lowered == 'x') || (lowered == 'o') || (lowered == 'b');
}

/**
 * \brief Determine if a character a prefix for a specific base 2, 8, or 16
 */
constexpr inline bool is_base_prefix(const char c, const int base)
{
    const char lowered = lowercase(c);
    return (base == 16 && (lowered == 'x')) || (base == 8 && (lowered == 'o'))
        || (base == 2 && (lowered == 'b'));
}

/**
 * \brief Detect if a string contains infinity
 *
 * This is done without loops in the hope of being optimized
 * by the compiler
 *
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
constexpr inline bool quick_detect_infinity(const char* str, const std::size_t len)
{
    // The ASCII standard was quite clever... upper- and lower-case
    // letters only differ from each other by the 32 bit, otherwise
    // they are identical. So, we can use the XOR operator against
    // lowercase "inf" and accumulate the results - if all bits are
    // zero or if only the 32 bit is one that means that the string
    // contained a case-insensitive INF.
    switch (len) {
    case 3: {
        const uint8_t accumulator = (str[0] ^ 'i') | (str[1] ^ 'n') | (str[2] ^ 'f');
        return accumulator == 0 || accumulator == 32;
    }
    case 8: {
        uint8_t accumulator = 0;
        accumulator |= (str[0] ^ 'i');
        accumulator |= (str[1] ^ 'n');
        accumulator |= (str[2] ^ 'f');
        accumulator |= (str[3] ^ 'i');
        accumulator |= (str[4] ^ 'n');
        accumulator |= (str[5] ^ 'i');
        accumulator |= (str[6] ^ 't');
        accumulator |= (str[7] ^ 'y');
        return accumulator == 0 || accumulator == 32;
    }
    default:
        return false;
    }
}

/**
 * \brief Detect if a string contains NaN
 *
 * This is done without loops in the hope of being optimized
 * by the compiler
 *
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
constexpr inline bool quick_detect_nan(const char* str, const std::size_t len)
{
    if (len != 3) {
        return false;
    }
    // The ASCII standard was quite clever... upper- and lower-case
    // letters only differ from each other by the 32 bit, otherwise
    // they are identical. So, we can use the XOR operator against
    // lowercase "nan" and accumulate the results - if all bits are
    // zero or if only the 32 bit is one that means that the string
    // contained a case-insensitive NaN.
    const uint8_t accumulator = (str[0] ^ 'n') | (str[1] ^ 'a') | (str[2] ^ 'n');
    return accumulator == 0 || accumulator == 32;
}

/**
 * \brief Detect if a string probably contains an integer
 *
 * This is not a 100% assurance, but it is a gross check that
 * makes sure the first character is a digit.
 *
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
constexpr inline bool is_likely_int(const char* str, std::size_t len)
{
    return len > 0 && is_valid_digit(*str);
}

// Define the number of digits in a float that fastnumbers is willing
// to attempt to convert itself. For Clang and GNUC, it is known
// that DBL_DIG - 4 works. It is known that DBL_DIG - 6 works for MSVC.
// To be safe, any compiler not known during fastnumbers testing will
// use DBL_DIG - 6.
// Also define the largest and smallest exponent that
// fastnumbers is willing attempt to convert itself. For Clang and GNUC,
// it is known that larger numbers work than MSVC.
// To be safe, any compiler not known during fastnumbers testing will
// use the smaller numbers.
#if defined(__clang__) || defined(__GNUC__)

/// Number of float digits fastnumbers will convert itself
constexpr long FN_DBL_DIG = std::numeric_limits<double>::digits10 - 4;

/// Most positive exponent fastumbers will convert itself
constexpr long FN_MAX_EXP = 99;

/// Most negative exponent fastumbers will convert itself
constexpr long FN_MIN_EXP = -98;

/// Allow the preprocessor to be able to dispatch on this decision
#define FASTNUMBERS_WIDE_EXP_RANGE

#else

/// Number of float digits fastnumbers will convert itself
constexpr long FN_DBL_DIG = std::numeric_limits<double>::digits10 - 6;

/// Most positive exponent fastumbers will convert itself
constexpr long FN_MAX_EXP = 19;

/// Most negative exponent fastumbers will convert itself
constexpr long FN_MIN_EXP = -19;

/// Allow the preprocessor to be able to dispatch on this decision
#undef FASTNUMBERS_WIDE_EXP_RANGE

#endif
