#pragma once

#include <cstring>
#include <limits>

/**
 * \brief Convert a string to a long type
 *
 * Assumes no sign or whitespace. No overflow checking is performed.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 * \param error Flag to indicate if there was a parsing error
 */
long parse_int(const char* str, const char* end, bool& error);

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
 * \brief Check if a string could be converted to an integer
 *
 * Assumes no sign or whitespace.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 * \param base The base to assume when checking the integer
 */
bool string_contains_int(const char* str, const char* end, int base);

/**
 * \brief Check if a string could be converted to a float
 *
 * Assumes no sign or whitespace.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 */
bool string_contains_float(const char* str, const char* end);

/**
 * \brief Check if a string could be converted to a float that
 *        could be losslessly converted to an integer
 *
 * Assumes no sign or whitespace.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 */
bool string_contains_intlike_float(const char* str, const char* end);

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
 * \brief Determine if a character is whitespace
 */
inline bool is_whitespace(const char c)
{
    return c == ' ' || (c >= '\t' && c <= '\r');
}

/**
 * \brief Determine if a character represents a digit
 */
inline bool is_valid_digit(const char c)
{
    return c >= '0' && c <= '9';
}

/**
 * \brief Determine if a character is '-' or '+'
 */
inline bool is_sign(const char c)
{
    return c == '-' || c == '+';
}

/**
 * \brief Determine if a character a prefix for base 2, 8, or 16
 */
inline bool is_base_prefix(const char c)
{
    return (c == 'x' || c == 'X') || (c == 'o' || c == 'O') || (c == 'b' || c == 'B');
}

/**
 * \brief Determine if a character a prefix for a specific base 2, 8, or 16
 */
inline bool is_base_prefix(const char c, const int base)
{
    return (base == 16 && (c == 'x' || c == 'X'))
        || (base == 8 && (c == 'o' || c == 'O'))
        || (base == 2 && (c == 'b' || c == 'B'));
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
inline bool quick_detect_infinity(const char* str, const std::size_t len)
{
    switch (len) {
    case 3:
        return (str[0] == 'i' || str[0] == 'I') && (str[1] == 'n' || str[1] == 'N')
            && (str[2] == 'f' || str[2] == 'F');
    case 8:
        return (str[0] == 'i' || str[0] == 'I') && (str[1] == 'n' || str[1] == 'N')
            && (str[2] == 'f' || str[2] == 'F') && (str[3] == 'i' || str[3] == 'I')
            && (str[4] == 'n' || str[4] == 'N') && (str[5] == 'i' || str[5] == 'I')
            && (str[6] == 't' || str[6] == 'T') && (str[7] == 'y' || str[7] == 'Y');
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
inline bool quick_detect_nan(const char* str, const std::size_t len)
{
    return len == 3 && (str[0] == 'n' || str[0] == 'N')
        && (str[1] == 'a' || str[1] == 'A') && (str[2] == 'n' || str[2] == 'N');
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
inline bool is_likely_int(const char* str, std::size_t len)
{
    return len > 0 && is_valid_digit(*str);
}

/**
 * \brief Detect if a string probably contains a float
 *
 * This is not a 100% assurance, but it is a gross check that
 * makes sure the string starts with a digit or '.' followed
 * by a digit..
 *
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
inline bool is_likely_float(const char* str, std::size_t len)
{
    return len > 0
        && (is_valid_digit(*str)
            || (*str == '.' && len > 1 && is_valid_digit(*(str + 1))));
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

/**
 * \brief Guess if an int will overflow.
 *
 * \param start The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
inline bool int_might_overflow(const char*, const std::size_t len)
{
    return len > FN_MAX_INT_LEN;
}

/**
 * \brief Guess if a float will overflow.
 *
 * \param start The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
bool float_might_overflow(const char* start, const std::size_t len);
