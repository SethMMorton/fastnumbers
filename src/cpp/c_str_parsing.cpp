/******************/
/****** NOTE ******/
/******************/
// All functions in this file assume whitespace has been trimmed
// from both sides of the string, and that the sign has been removed.

#include <cmath>
#include <cstdint>
#include <cstring>
#include "fastnumbers/c_str_parsing.hpp"


/// Convert a character to an integer in the desired type
template<typename T>
static inline T ascii2int(const char c) {
    return static_cast<T>(c - '0');
}


/// A function that accepts an argument and does nothing
static inline void do_nothing(const char x) {}


// FORWARD DECLARATIONS
template<typename Function>
static inline bool parse_integer_components(const char *&str, Function callback);
static inline bool parse_integer_components(const char *&str);
template<typename Function>
static inline bool parse_decimal_components(const char *&str, Function callback);
static inline bool parse_decimal_components(const char *&str);
template<typename NFunction, typename Function>
static inline bool parse_exponent_components(const char *&str, NFunction sign_callback, Function callback);
static inline bool parse_exponent_components(const char *&str);
static inline bool neg_exp_ok(const char * str, std::size_t len);
static inline bool pos_exp_ok(const char * str, std::size_t len);
static inline uint32_t number_trailing_zeros(const char *start, const char *end);
static inline int detect_base(const char *str, const char *end);
static inline bool is_valid_digit_arbitrary_base(const char c, const int base);
static inline long double power_of_ten_scaling_factor(const int expon);

// END FORWARD DECLARATIONS, BEGIN DEFINITITONS


/*********************/
/* EXPOSED FUNCTIONS */
/*********************/


bool string_contains_int(const char *str, const char *end, int base)
{
    if (base == 0) {
        base = detect_base(str, end);
    }

    /* If base 10, take fast route. */
    if (base == 10) {
        return parse_integer_components(str) and str == end;
    }
    else if (base == -1) {
        return false;
    }
    else {
        const std::size_t len = end - str;

        /* Skip leading characters for non-base 10 ints. */
        if (len > 1 and str[0] == '0' and
                ((base == 16 and (str[1] == 'x' or str[1] == 'X')) or
                 (base == 8  and (str[1] == 'o' or str[1] == 'O')) or
                 (base == 2  and (str[1] == 'b' or str[1] == 'B')))) {
            str += 2;
        }

        /* The rest behaves as normal. */
        bool valid = false;
        while (is_valid_digit_arbitrary_base(*str, base)) {
            str += 1;
            valid = true;
        }
        return valid and str == end;
    }
}


bool string_contains_float(const char *str, const char *end)
{
    bool valid = parse_integer_components(str);
    valid = parse_decimal_components(str) or valid;
    if (valid) {
        valid = parse_exponent_components(str);
    }
    return valid and str == end;
}


bool string_contains_intlike_float(const char *str, const char *end)
{
    /* Before decimal. Keep track of number of digits read. */
    const char *int_start = str;
    bool valid = parse_integer_components(str);

    /* Decimal part of float. Keep track of number of digits read */
    /* as well as beginning and end locations. */
    const char *decimal_start = str;
    uint32_t dec_length = 0;
    valid = parse_decimal_components(
        str,
        [&dec_length](const char c) { dec_length += 1; }
    ) or valid;
    const char *decimal_end = str;

    /* Exponential part of float. Parse the magnitude. */
    uint32_t expon = 0;
    bool exp_negative = false;
    if (valid) {
        valid = parse_exponent_components(
            str,
            [&exp_negative](const char c) { exp_negative = true; },
            [&expon](const char c) {
                expon *= 10;
                expon += ascii2int<int32_t>(c);
            }
        );
    }

    if (not valid or str != end) {
        return false;
    }
    else {
        /* If we "move the decimal place" left or right depending on
         * exponent sign and magnitude, all digits after the decimal
         * must be zero.
         */
        const unsigned int_trailing_zeros = number_trailing_zeros(
                                                int_start, decimal_start
                                            );
        const unsigned dec_trailing_zeros =
            decimal_start == decimal_end
            ? 0U
            : number_trailing_zeros(decimal_start + 1, decimal_end);

        if (exp_negative) {
            return expon <= int_trailing_zeros and
                   dec_length == dec_trailing_zeros;
        }
        else {
            return expon >= (dec_length - dec_trailing_zeros);
        }
    }
}


long parse_int(const char *str, const char *end, bool &error)
{
    long value = 0L;

    /* Convert digits, if any. */
    bool valid = parse_integer_components(
        str,
        [&value](const char c) {
            value *= 10L;
            value += ascii2int<long>(c);
        }
    );

    error = not valid or str != end;
    return value;
}


double parse_float(const char *str, const char *end, bool &error)
{
    /* Parse integer part. */
    uint64_t intvalue = 0UL;
    bool valid = parse_integer_components(
        str,
        [&intvalue](const char c) {
            intvalue *= 10UL;
            intvalue += ascii2int<uint64_t>(c);
        }
    );

    /* Parse decimal part. */
    uint16_t decimal_len = 0;
    valid = parse_decimal_components(
        str,
        [&intvalue, &decimal_len](const char c) {
            intvalue *= 10UL;
            intvalue += ascii2int<uint64_t>(c);
            decimal_len += 1U;
        }
    ) or valid;

    /* Parse exponential part. */
    int16_t expon = 0;
    if (valid) {
        int16_t exp_sign = 1;
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

    error = not valid or str != end;
    if (expon < 0) {
        return static_cast<long double>(intvalue) / power_of_ten_scaling_factor(std::abs(expon));
    }
    else {
        return static_cast<long double>(intvalue) * power_of_ten_scaling_factor(expon);
    }
}


bool float_might_overflow(const char *start, const std::size_t len)
{
    // Locate the decimal place (if any).
    const char *decimal_loc = static_cast<const char *>(std::memchr(start, '.', len));
    const bool has_decimal = static_cast<bool>(decimal_loc);

    // Find the exponent (if any) in the input.
    // It is always after the exponent, usually close to the end, so
    // we will start from the back. No need to include the stop
    // location since that is either a decimal or the first digit
    // which cannot be 'e' or 'E'.
    const char *ptr = NULL;
    const char *exp = NULL;
    const char *stop = decimal_loc ? decimal_loc : start;
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
        std::size_t exp_len = len - (exp - start) ;
        if (is_sign(*exp)) {
            exp += 1;
            exp_len -= 1;
        }
        const bool exp_ok = neg ? neg_exp_ok(exp, exp_len) : pos_exp_ok(exp, exp_len);
        if (not exp_ok) {
            return true;
        }
    }

    // Won't overflow
    return false;
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
template<typename Function>
bool parse_integer_components(const char *&str, Function callback) {
    const char* start = str;
    while (is_valid_digit(*str)) {
        callback(*str);
        str += 1;
    }
    return str != start;  // return whether or not a valid digit was found
}


bool parse_integer_components(const char *&str) {
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
template<typename Function>
bool parse_decimal_components(const char *&str, Function callback) {
    if (*str == '.') {
        str += 1;
        return parse_integer_components(str, callback);
    }
    return false;
}


bool parse_decimal_components(const char *&str) {
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
template<typename NFunction, typename Function>
bool parse_exponent_components(const char *&str, NFunction sign_callback, Function callback) {
    if (*str == 'e' or *str == 'E') {
        str += 1;
        if (is_sign(*str)) {
            sign_callback(*str);
            str += 1;
        }
        return parse_integer_components(str, callback);
    }
    return true;
}


bool parse_exponent_components(const char *&str) {
    return parse_exponent_components(str, do_nothing, do_nothing);
}


#if FASTNUMBERS_EXP_MAX_MIN == TWENTYTWO_TWENTYTWO
/// Helper to check if an exponent number is in the allowed range
static inline bool _exp_ok(const char * str, std::size_t len)
{
    return len == 1 or (
        len == 2 and (
            *str <= '2' or (
                *str == '2' and *(str + 1) <= '2'
            )
        )
    );
}
#endif

/**
 * \brief Check if a negative exponent number is in the allowed range
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
bool neg_exp_ok(const char * str, std::size_t len)
{
    #if FASTNUMBERS_EXP_MAX_MIN == NINTETYNINE_NINETYEIGHT
    return len == 1 or (
        len == 2 and (
            *str <= '8' or (
                *str == '9' and *(str + 1) <= '8'
            )
        )
    );
    #else
    return _exp_ok(str, len);
    #endif
}


/**
 * \brief Check if a positive exponent number is in the allowed range
 * \param str The string to check, assumed to be non-NULL
 * \param len The length of the string
 */
bool pos_exp_ok(const char * str, std::size_t len)
{
    #if FASTNUMBERS_EXP_MAX_MIN == NINTETYNINE_NINETYEIGHT
    return len > 0 and len <= 2;
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
uint32_t number_trailing_zeros(const char *start, const char *end)
{
    uint32_t n = 0;
    for (end = end - 1; end >= start; --end) {
        if (*end == '0') {
            n += 1;
        }
        else {
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
int detect_base(const char *str, const char *end)
{
    const std::size_t len = end - str;
    if (str[0] != '0' or len == 1) {
        return 10;
    }
    else if (str[1] == 'x' or str[1] == 'X') {
        return 16;
    }
    else if (str[1] == 'o' or str[1] == 'O') {
        return 8;
    }
    else if (str[1] == 'b' or str[1] == 'B') {
        return 2;
    }
    else
        /* "old" (C-style) octal literal illegal in 3.x. */
        if (number_trailing_zeros(str, end) == len) {
            return 10;
        }
        else {
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
        return c >= '0' and c <= (static_cast<int>('0') + base);
    }
    else {
        const char offset = static_cast<char>(base) - 10;
        return (c >= '0' and c <= '9') or
               (c >= 'a' and c <= 'a' + offset) or
               (c >= 'A' and c <= 'A' + offset);
    }
}


/**
 * \brief Return the approprate scaling factor for an exponent value
 */
long double power_of_ten_scaling_factor(const int expon) {
    switch (expon) {
    case 0:
        return 1E0L;
    case 1:
        return 1E1L;
    case 2:
        return 1E2L;
    case 3:
        return 1E3L;
    case 4:
        return 1E4L;
    case 5:
        return 1E5L;
    case 6:
        return 1E6L;
    case 7:
        return 1E7L;
    case 8:
        return 1E8L;
    case 9:
        return 1E9L;
    case 10:
        return 1E10L;
    case 11:
        return 1E11L;
    case 12:
        return 1E12L;
    case 13:
        return 1E13L;
    case 14:
        return 1E14L;
    case 15:
        return 1E15L;
    case 16:
        return 1E16L;
    case 17:
        return 1E17L;
    case 18:
        return 1E18L;
    case 19:
        return 1E19L;
    case 20:
        return 1E20L;
    case 21:
        return 1E21L;
    case 22:
        return 1E22L;
    case 23:
        return 1E23L;
    case 24:
        return 1E24L;
    case 25:
        return 1E25L;
    case 26:
        return 1E26L;
    case 27:
        return 1E27L;
    case 28:
        return 1E28L;
    case 29:
        return 1E29L;
    case 30:
        return 1E30L;
    case 31:
        return 1E31L;
    case 32:
        return 1E32L;
    case 33:
        return 1E33L;
    case 34:
        return 1E34L;
    case 35:
        return 1E35L;
    case 36:
        return 1E36L;
    case 37:
        return 1E37L;
    case 38:
        return 1E38L;
    case 39:
        return 1E39L;
    case 40:
        return 1E40L;
    case 41:
        return 1E41L;
    case 42:
        return 1E42L;
    case 43:
        return 1E43L;
    case 44:
        return 1E44L;
    case 45:
        return 1E45L;
    case 46:
        return 1E46L;
    case 47:
        return 1E47L;
    case 48:
        return 1E48L;
    case 49:
        return 1E49L;
    case 50:
        return 1E50L;
    case 51:
        return 1E51L;
    case 52:
        return 1E52L;
    case 53:
        return 1E53L;
    case 54:
        return 1E54L;
    case 55:
        return 1E55L;
    case 56:
        return 1E56L;
    case 57:
        return 1E57L;
    case 58:
        return 1E58L;
    case 59:
        return 1E59L;
    case 60:
        return 1E60L;
    case 61:
        return 1E61L;
    case 62:
        return 1E62L;
    case 63:
        return 1E63L;
    case 64:
        return 1E64L;
    case 65:
        return 1E65L;
    case 66:
        return 1E66L;
    case 67:
        return 1E67L;
    case 68:
        return 1E68L;
    case 69:
        return 1E69L;
    case 70:
        return 1E70L;
    case 71:
        return 1E71L;
    case 72:
        return 1E72L;
    case 73:
        return 1E73L;
    case 74:
        return 1E74L;
    case 75:
        return 1E75L;
    case 76:
        return 1E76L;
    case 77:
        return 1E77L;
    case 78:
        return 1E78L;
    case 79:
        return 1E79L;
    case 80:
        return 1E80L;
    case 81:
        return 1E81L;
    case 82:
        return 1E82L;
    case 83:
        return 1E83L;
    case 84:
        return 1E84L;
    case 85:
        return 1E85L;
    case 86:
        return 1E86L;
    case 87:
        return 1E87L;
    case 88:
        return 1E88L;
    case 89:
        return 1E89L;
    case 90:
        return 1E90L;
    case 91:
        return 1E91L;
    case 92:
        return 1E92L;
    case 93:
        return 1E93L;
    case 94:
        return 1E94L;
    case 95:
        return 1E95L;
    case 96:
        return 1E96L;
    case 97:
        return 1E97L;
    case 98:
        return 1E98L;
    case 99:
        return 1E99L;
    case 100:
        return 1E100L;
    case 101:
        return 1E101L;
    case 102:
        return 1E102L;
    case 103:
        return 1E103L;
    case 104:
        return 1E104L;
    case 105:
        return 1E105L;
    case 106:
        return 1E106L;
    case 107:
        return 1E107L;
    case 108:
        return 1E108L;
    case 109:
        return 1E109L;
    case 110:
        return 1E110L;
    /* We should never see anything larger than 99. */
    /* This should never be reached. */
    default:
        return 1E308L;
    }
}
