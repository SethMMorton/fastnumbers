#pragma once

#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <system_error>
#include <type_traits>
#include <vector>

#include "fastnumbers/third_party/fast_float.h"

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
constexpr int8_t DIGIT_TABLE[]
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

constexpr int8_t DIGIT_TABLE_ARBITRARY_BASE[]
    = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  -1, -1,
        -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, -1, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

/// Selector for the type of data a string can contain
enum class StringType {
    INVALID, ///< Contains an invalid number
    INTEGER, ///< Contains an integer
    FLOAT, ///< Contains a float
    INTLIKE_FLOAT, ///< Contains an integer-like float
};

/**
 * \class StringChecker
 * \brief Assess the type of number that is contained in a string
 */
class StringChecker {
public:
    /**
     * \brief Check if a string could be converted to some numeric type
     *
     * Assumes no sign or whitespace.
     *
     * \param str The string to parse, assumed to be non-NULL
     * \param end The end of the string being checked
     * \param base The base to assume when checking an integer, set to 10
     *             unless you know it *must* be an integer.
     */
    StringChecker(const char* str, const char* end, int base) noexcept;

    // Default copy/move/assign/destruct
    StringChecker(const StringChecker&) = default;
    StringChecker(StringChecker&&) = default;
    StringChecker& operator=(const StringChecker&) = default;
    ~StringChecker() = default;

    /// Return the contained type of the string
    StringType get_type() const { return m_contained_type; }

    /// Is the contained string an invalid number?
    bool is_invalid() const { return get_type() == StringType::INVALID; }

    /// Is the contained string an integer?
    bool is_integer() const { return get_type() == StringType::INTEGER; }

    /// Is the contained string a float?
    bool is_float() const { return get_type() == StringType::FLOAT; }

    /// Is the contained string an int-like float?
    bool is_intlike_float() const { return get_type() == StringType::INTLIKE_FLOAT; }

    /// The start of the integer component of the contained number.
    const char* integer_start() const { return m_integer_start; }

    /// The end of the integer component of the contained number.
    const char* integer_end() const { return m_decimal_start; }

    /// The length of the integer component of the contained number.
    uint32_t integer_length() const
    {
        return std::max(static_cast<uint32_t>(integer_end() - integer_start()), 0U);
    }

    /// The number of zeros that trail the integer part of the contained number.
    uint32_t integer_trailing_zeros() const { return m_int_trailing_zeros; }

    /// The start of the decimal component of the contained number.
    const char* decimal_start() const
    {
        return (m_decimal_start == m_decimal_end) ? m_decimal_start
                                                  : (m_decimal_start + 1);
    }

    /// The end of the decimal component of the contained number.
    const char* decimal_end() const { return m_decimal_end; }

    /// The length of the decimal component of the contained number.
    uint32_t decimal_length() const
    {
        return std::max(static_cast<uint32_t>(decimal_end() - decimal_start()), 0U);
    }

    /// The number of zeros that trail the decimal part of the contained number.
    uint32_t decimal_trailing_zeros() const { return m_dec_trailing_zeros; }

    /// Was any decimal data found in the string?
    bool has_decimal_data() const { return m_decimal_start != m_decimal_end; }

    /// The value of the exponent of the nubmer.
    uint32_t exponent_value() const { return m_expon; }

    /// Is the exponent negative?
    bool is_exponent_negative() const { return m_exp_negative; }

    /// The total length of the integer plus decimal components.
    uint32_t digit_length() const { return integer_length() + decimal_length(); }

    /// The decimal length after removing trailing zeros.
    uint32_t truncated_decimal_length() const
    {
        return std::max(decimal_length() - decimal_trailing_zeros(), 0U);
    }

    /// The exponent after taking into account the decimal digits.
    uint32_t adjusted_exponent_value() const
    {
        if (is_exponent_negative()) {
            return exponent_value();
        } else {
            return std::max(exponent_value() - truncated_decimal_length(), 0U);
        }
    }

    /// The total length of the entire number.
    uint32_t total_length() const
    {
        return std::max(static_cast<uint32_t>(m_total_end - integer_start()), 0U);
    }

    /// The length of the start of the decimal component to the end of the number.
    uint32_t decimal_and_exponent_length() const
    {
        return std::max(static_cast<uint32_t>(m_total_end - decimal_start()), 0U);
    }

private:
    /// Set the contained type.
    void set_type(StringType val) { m_contained_type = val; }

    /// Set the integer start.
    void set_integer_start(const char* val) { m_integer_start = val; }

    /// Set the decimal start.
    void set_decimal_start(const char* val) { m_decimal_start = val; }

    /// Set the decimal end.
    void set_decimal_end(const char* val) { m_decimal_end = val; }

    /// Set the total end.
    void set_total_end(const char* val) { m_total_end = val; }

    /// Set the exponent value.
    void set_exponent(const uint32_t val) { m_expon = val; }

    /// Set whether or not the exponent is negative.
    void set_exponent_negative(const bool val) { m_exp_negative = val; }

    /// Set the number of trailing zeros on the integer part.
    void set_int_trailing_zeros(const uint32_t val) { m_int_trailing_zeros = val; }

    /// Set the number of trailing zeros on the decimal part.
    void set_dec_trailing_zeros(const uint32_t val) { m_dec_trailing_zeros = val; }

    /// The start of the integer component of the contained number.
    const char* m_integer_start;

    /// The start of the decimal component of the contained number.
    const char* m_decimal_start;

    /// The end of the decimal component of the contained number.
    const char* m_decimal_end;

    /// The end of the contained number.
    const char* m_total_end;

    /// The value of the exponent of the nubmer.
    uint32_t m_expon;

    /// Whether or not the exponent is negative.
    bool m_exp_negative;

    /// The number of zeros that trail the integer part of the contained number.
    uint32_t m_int_trailing_zeros;

    /// The number of zeros that trail the decimal part of the contained number.
    uint32_t m_dec_trailing_zeros;

    /// The contained type of the string.
    StringType m_contained_type;
};

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
void remove_valid_underscores(char* str, const char*& end, const bool based) noexcept;

/**
 * \brief Lowercase a character - does no error checking
 */
constexpr inline char lowercase(const char c) noexcept
{
    // The ASCII standard was quite clever... upper- and lower-case
    // letters only differ from each other by the 32 bit, otherwise
    // they are identical. ORing the 32-bit forces lowercase.
    return static_cast<char>(c | 32);
}

/**
 * \brief Determine if a character is whitespace
 */
constexpr inline bool is_whitespace(const char c) noexcept
{
    // Using a table was determined through performance testing to be
    // many times faster than std::isspace, and twice as fast as using
    // a switch statement.
    return WHITESPACE_TABLE[static_cast<uint8_t>(c)];
}

/**
 * \brief Advance a string's pointer while whitespace is found
 */
constexpr inline void consume_whitespace(const char*& str, const char* end) noexcept
{
    while (str != end && is_whitespace(*str)) {
        str += 1;
    }
}

/**
 * \brief Convert a character to a digit, returns -1 on failure.
 */
template <typename T>
constexpr inline T to_digit(const char c) noexcept
{
    // Using a table was determined through performance testing to be
    // three times faster than c - '0' or a switch statement.
    return static_cast<T>(DIGIT_TABLE[static_cast<uint8_t>(c)]);
}

/**
 * \brief Convert a character to a digit from an arbitrary base, returns -1 on failure.
 */
template <typename T>
constexpr inline T to_digit(const char c, const int base) noexcept
{
    // Using a table was determined through performance testing to be
    // three times faster than c - '0' or a switch statement.
    const int8_t value = DIGIT_TABLE_ARBITRARY_BASE[static_cast<uint8_t>(c)];
    return static_cast<T>(value < base ? value : -1);
}

/**
 * \brief Determine if a character represents a digit
 */
constexpr inline bool is_valid_digit(const char c) noexcept
{
    // Using a table was determined through performance testing to be
    // faster than std::isdigit or a switch statement.
    return to_digit<int8_t>(c) >= 0;
}

/**
 * \brief Determine if a character represents a digit in an arbitrary base
 */
constexpr inline bool is_valid_digit(const char c, const int base) noexcept
{
    // Using a table was determined through performance testing to be
    // faster than std::isdigit or a switch statement.
    return to_digit<int8_t>(c, base) >= 0;
}

/**
 * \brief Advance a string's pointer while digits are found
 */
constexpr inline void consume_digits(const char*& str, const char* end) noexcept
{
    while (str != end && is_valid_digit(*str)) {
        str += 1;
    }
}

/**
 * \brief Advance a string's pointer while digits are found
 *        and attempt to read multiple digits at a time if possible
 */
inline void consume_digits(const char*& str, const std::size_t len) noexcept
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
constexpr inline bool is_sign(const char c) noexcept
{
    return c == '-' || c == '+';
}

/**
 * \brief Determine if a character a prefix for base 2, 8, or 16
 */
constexpr inline bool is_base_prefix(const char c) noexcept
{
    const char lowered = lowercase(c);
    return (lowered == 'x') || (lowered == 'o') || (lowered == 'b');
}

/**
 * \brief Determine if a character a prefix for a specific base 2, 8, or 16
 */
constexpr inline bool is_base_prefix(const char c, const int base) noexcept
{
    const char lowered = lowercase(c);
    return (base == 16 && (lowered == 'x')) || (base == 8 && (lowered == 'o'))
        || (base == 2 && (lowered == 'b'));
}

/**
 * \brief Determine if a string begins with a base prefix
 */
constexpr inline bool has_base_prefix(const char* str, const std::size_t len) noexcept
{
    return len > 2 && str[0] == '0' && is_base_prefix(str[1]);
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
constexpr inline bool
quick_detect_infinity(const char* str, const std::size_t len) noexcept
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
constexpr inline bool quick_detect_nan(const char* str, const std::size_t len) noexcept
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
constexpr inline bool is_likely_int(const char* str, std::size_t len) noexcept
{
    return len > 0 && is_valid_digit(*str);
}

/**
 * \brief Check the number of zeros at the end of a number
 *
 * \param str The string to check, assumed to be non-NULL
 * \param end The end of the string being checked
 * \return The number of zeros
 */
constexpr inline uint32_t
number_trailing_zeros(const char* start, const char* end) noexcept
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
constexpr inline int detect_base(const char* str, const char* end) noexcept
{
    if (str[0] == '-') // Skip leading negative sign
        str += 1;
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
    } else {
        /* "old" (C-style) octal literal illegal in 3.x. */
        if (number_trailing_zeros(str, end) == len) {
            return 10;
        } else {
            return -1;
        }
    }
}

/**
 * \brief Return the number of digits an integer type can safely parse without overflow
 */
template <typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
constexpr inline uint8_t overflow_cutoff() noexcept
{
    // len('std::numeric_limits<T>::max()') - 1 == return value
    constexpr uint64_t limit = static_cast<uint64_t>(std::numeric_limits<T>::max());
    if constexpr (limit == 18446744073709551615ULL) {
        return 19;
    } else if constexpr (limit == 9223372036854775807ULL) {
        return 18;
    } else if constexpr (limit == 2147483647ULL) {
        return 9;
    } else if constexpr (limit == 4294967295ULL) {
        return 9;
    } else if constexpr (limit == 32767ULL) {
        return 4;
    } else if constexpr (limit == 65535ULL) {
        return 4;
    } else if constexpr (limit == 127ULL) {
        return 2;
    } else if constexpr (limit == 255ULL) {
        return 2;
    } else {
        return 0; // be safe - should never be encountered
    }
}

/**
 * \brief Convert a string to an int type
 *
 * Assumes no whitespace, and only a single '-' is allowed.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 * \param base The base to assume when checking an integer, 0 is "guess".
 * \param error Flag to indicate if there was a parsing error
 * \param overflow Flag to indicate if the string was long enough to overflow
 * \param always_convert
 */
template <typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
inline T parse_int(
    const char* str,
    const char* end,
    int base,
    bool& error,
    bool& overflow,
    bool always_convert = false
) noexcept
{
    // Remember if we are negative.
    const bool is_negative = *str == '-';
    const std::size_t negative_offset = static_cast<std::size_t>(is_negative);
    str += negative_offset;

    // For unsigned values that are negative, quit now with an overflow error.
    if constexpr (std::is_unsigned_v<T>) {
        if (is_negative) {
            overflow = true;
            error = false;
            return static_cast<T>(0);
        }
    }

    // The length of the string will be useful below.
    const std::size_t len = static_cast<std::size_t>(end - str);

    // If the base needs to be guessed, do so now and get it over with.
    if (base == 0) {
        base = detect_base(str, end);
    }

    // Negative bases are illegal. So is zero-length.
    if (base < 0 || len == 0) {
        overflow = false;
        error = true;
        return static_cast<T>(0);
    }

    // We use our own method for base-10 because we can omit some overflow
    // checking and get faster results.
    //
    // We just assume overflow if the length of the string is over a certain value.
    overflow = len > static_cast<std::size_t>(overflow_cutoff<T>());

    // Use std::from_chars for all but base-10.
    if (base != 10 || (overflow && always_convert)) {
        // Skip leading characters for non-base 10 ints.
        // If we did not have to do that, replace a '-' if we had one.
        bool had_base_prefix = false;
        if (len > 1 && str[0] == '0' && is_base_prefix(str[1], base)) {
            str += 2;
            had_base_prefix = true;
        } else {
            str -= negative_offset;
        }

        // Use a very fast and accurate string to integer parser
        // that will report back if there was an overflow (which
        // we propagete back to the user).
        T value = static_cast<T>(0);
        std::from_chars_result res = std::from_chars(str, end, value, base);
        error = res.ptr != end || res.ec == std::errc::invalid_argument;
        overflow = res.ec == std::errc::result_out_of_range;
        if constexpr (std::is_signed_v<T>) {
            return had_base_prefix && is_negative ? -value : value;
        } else {
            return value;
        }
    }

    // If an overflow is going to happen, just evaluate that this looks like
    // an integer. Otherwise, actually calculate the value contained in the string.
    T value = static_cast<T>(0);
    if (overflow) {
        consume_digits(str, len);
    } else {
        // Attempt to read eight characters at a time and parse as digits.
        // Loop over the character array in steps of eight. Stop processing
        // if not all eight characters are digits.
        if constexpr (overflow_cutoff<T>() > 8) {
            const std::size_t number_of_eights = len / 8;
            for (std::size_t i = 0; i < number_of_eights; ++i) {
                if (fast_float::is_made_of_eight_digits_fast(str)) {
                    value = value * 100000000
                        + fast_float::parse_eight_digits_unrolled(str);
                    str += 8;
                } else {
                    break;
                }
            }
        }

        // Convert digits the remaining digits one-at-a-time.
        int8_t this_char_as_digit = 0;
        while (str != end && (this_char_as_digit = to_digit<int8_t>(*str)) >= 0) {
            value = value * 10 + this_char_as_digit;
            str += 1;
        }
    }
    error = str != end;
    if constexpr (std::is_signed_v<T>) {
        return is_negative ? -value : value;
    } else {
        return value;
    }
}

/**
 * \brief Convert a string to a double type
 *
 * Assumes no whitespace, and only a single '-' is allowed.
 * Overflows go to infinity. Underflows go to zero.
 *
 * \param str The string to parse, assumed to be non-NULL
 * \param end The end of the string being checked
 * \param error Flag to indicate if there was a parsing error
 */
template <
    typename T,
    typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
inline T parse_float(const char* str, const char* end, bool& error) noexcept
{
    // Use a very fast and accurate string-to-floating point parser
    T value;
    const fast_float::from_chars_result res = fast_float::from_chars(str, end, value);
    error = !(res.ptr == end && res.ec == std::errc());
    return value;
}