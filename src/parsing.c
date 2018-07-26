/* Parse strings for numbers.
 *
 * Author: Seth M. Morton
 *
 * July 2018
 */
#include <Python.h>
#include "pstdint.h"
#include "parsing.h"


/* Forward declarations. */
static uint16_t
number_trailing_zeros(const char *start, const char *end);
static long double
power_of_ten_scaling_factor(const int expon);


/* Local convenience macros.
 * These each provide an abstraction over parsing components of a number.
 * The user can provide a "payload" of commands to execute for each
 * iteration of the parsing loop. It is recommended to put the payload
 * in a block.
 */
#define parse_integer_macro(str, payload) \
    while (is_valid_digit(str)) { \
        payload; \
        (str) += 1; \
    }

#define parse_decimal_macro(str, payload) \
    if (*(str) == '.') { \
        (str) += 1; \
        parse_integer_macro(str, payload); \
    }

#define parse_exponent_macro(str, ref, negative_payload, payload) \
    if ((*(str) == 'e' || *(str) == 'E') && (str) != (ref)) { \
        (str) += 1; \
        if (is_sign(str)) { \
            if (*(str) == '-') { \
                negative_payload; \
            } \
            (str) += 1; \
        } \
        (ref) = (str); \
        while (is_valid_digit(str)) { \
            payload; \
            (str) += 1; \
        } \
    }

#define just_a_decimal_point(start, end, dstart, dend) \
    ((dend) - (dstart) == 1 && (start) == (dstart) && (end) == (dend))


bool
string_contains_int(const char *str, const char *end, int base)
{
    register Py_ssize_t len = 0;
    const char starts_with_sign = (char) is_sign(str);
    const char *reference = str;

    /* Account for sign. */
    str += starts_with_sign;
    /* For some reason, Python 2 allows space between the sign and digits. */
    consume_white_space_py2_only(str);

    len = (Py_ssize_t)(end - str);
    if (base == 0) {
        base = detect_base(str, len);
    }

    /* If base 10, take fast route. */
    if (base == 10) {
        reference = str;
        reference = str;
        parse_integer_macro(str, {});
        (void) consume_python2_long_literal_lL(str);
        return str == end && str != reference;
    }

    /* Skip leading characters for non-base 10 ints. */
    if (len > 1 && str[0] == '0' &&
            ((base == 16 && (str[1] == 'x' || str[1] == 'X')) ||
             (base == 8  && (str[1] == 'o' || str[1] == 'O')) ||
             (base == 2  && (str[1] == 'b' || str[1] == 'B')))) {
        str += 2;
    }

    /* The rest behaves as normal. */
    reference = str;
    while (is_valid_digit_arbitrary_base(*str, base)) {
        str += 1;
    }
#if PY_MAJOR_VERSION == 2
    if (base == 2 || base == 8 || base == 10 || base == 16) {
        (void) consume_python2_long_literal_lL(str);
    }
#endif
    return str == end && str != reference;
}


bool
string_contains_float(const char *str,
                      const char *end,
                      const bool allow_inf,
                      const bool allow_nan)
{
    const char starts_with_sign = (char) is_sign(str);
    const char *int_start, *decimal_start, *decimal_end;
    const char *reference = str;
    Py_ssize_t len = 0;

    /* Account for sign. */
    str += starts_with_sign;

    /* NAN or INF */
    len = end - str;
    if (quick_detect_infinity(str, len)) {
        return allow_inf;
    }
    if (quick_detect_nan(str, len)) {
        return allow_nan;
    }

    int_start = reference = str;
    parse_integer_macro(str, {});
    if (consume_python2_long_literal_lL(str)) {
        return str == end && str != reference;
    }
    decimal_start = str;
    parse_decimal_macro(str, {});
    decimal_end = str;
    parse_exponent_macro(str, reference, {}, {});
    return str == end && str != reference &&
           !just_a_decimal_point(int_start, end, decimal_start, decimal_end);
}


bool
string_contains_intlike_float(const char *str, const char *end)
{
    register int16_t expon = 0;
    uint16_t int_length = 0;
    uint16_t dec_length = 0;
    bool exp_negative = false;
    const char *int_start, *decimal_start, *decimal_end;
    const char starts_with_sign = (char) is_sign(str);
    const char *reference = str;
    str += starts_with_sign;

    /* Before decimal. Keep track of number of digits read. */
    int_start = reference = str;
    parse_integer_macro(str, {});
    int_length = (uint16_t)(str - int_start);
    if (consume_python2_long_literal_lL(str)) {
        return str == end && str != reference;
    }

    /* Decimal part of float. Keep track of number of digits read */
    /* as well as beginning and end locations. */
    decimal_start = str;
    parse_decimal_macro(str, {});
    decimal_end = str;
    if (decimal_start != decimal_end) {
        dec_length = (uint16_t)(decimal_end - decimal_start) - 1;
    }

    /* Exponential part of float. Parse the magnitude. */
    parse_exponent_macro(str, reference, { exp_negative = true; }, {
        expon *= 10;
        expon += ascii2int(str);
    });

    if (str != end || str == reference ||
            just_a_decimal_point(int_start, end, decimal_start, decimal_end)) {
        return false;
    }
    else {
        /* If we "move the decimal place" left or right depending on
         * exponent sign and magnitude, all digits after the decimal
         * must be zero.
         */
        const uint16_t int_trailing_zeros = number_trailing_zeros(
                                                int_start, decimal_start
                                            );
        const uint16_t dec_trailing_zeros =
            decimal_start == decimal_end
            ? 0
            : number_trailing_zeros(decimal_start + 1, decimal_end);

        if (exp_negative) {
            return expon <= int_trailing_zeros &&
                   dec_length == dec_trailing_zeros;
        }
        else {
            return expon >= (dec_length - dec_trailing_zeros);
        }
    }
}


long
parse_int(const char *str, const char *end, bool *error)
{
    register long value = 0L;
    const char starts_with_sign = (char) is_sign(str);
    bool negative = starts_with_sign && *str == '-';
    const char *reference = str;

    /* Account for sign. */
    str += starts_with_sign;
    /* For some reason, Python 2 allows space between the sign and digits. */
    consume_white_space_py2_only(str);

    /* Convert digits, if any. */
    reference = str;
    parse_integer_macro(str, {
        value *= 10L;
        value += ascii2long(str);
    });
    (void) consume_python2_long_literal_lL(str);

    *error = str != end || str == reference;
    return negative ? -value : value;
}


double
parse_float(const char *str, const char *end, bool *error)
{
    register uint64_t intvalue = 0UL;
    register int16_t expon = 0;
    int16_t decimal_len = 0;
    int16_t exp_sign = 1;
    const char *int_start, *decimal_start, *decimal_end;
    const char starts_with_sign = (char) is_sign(str);
    bool negative = starts_with_sign && *str == '-';
    const char *reference = str;
    str += starts_with_sign;

    /* Parse integer part. */
    int_start = reference = str;
    parse_integer_macro(str, {
        intvalue *= 10UL;
        intvalue += ascii2ulong(str);
    });

    /* If long literal, quit here. */
    if (consume_python2_long_literal_lL(str)) {
        *error = str != end || str == reference;
        return (long double)(negative ? -intvalue : intvalue);
    }

    /* Parse decimal part. */
    decimal_start = str;
    parse_decimal_macro(str, {
        intvalue *= 10UL;
        intvalue += ascii2ulong(str);
    });
    decimal_end = str;
    if (decimal_start != decimal_end) {
        decimal_len = decimal_end - decimal_start - 1;
    }

    /* Parse exponential part. */
    parse_exponent_macro(str, reference, { exp_sign = -1; }, {
        expon *= 10;
        expon += ascii2int(str);
    });
    expon *= exp_sign;
    expon -= decimal_len; /* Adjust the exponent by the # of decimal places */

    *error = str != end || str == reference ||
             just_a_decimal_point(int_start, end, decimal_start, decimal_end);
    if (negative) {
        return -(expon < 0
                 ? intvalue / power_of_ten_scaling_factor(abs(expon))
                 : intvalue * power_of_ten_scaling_factor(expon));
    }
    else {
        return expon < 0
               ? intvalue / power_of_ten_scaling_factor(abs(expon))
               : intvalue * power_of_ten_scaling_factor(expon);
    }
}


bool
float_might_overflow(const char *str, const char *end)
{
    register unsigned len = 0;

    /* Scan string, collecting the count of digits. */
    while (str < end) {
        len += (unsigned) is_valid_digit(str);
        if (*str == 'e' || *str == 'E') {
            break;  /* Don't count digits after exponent. */
        }
        str += 1;
    }
    if (len > FN_DBL_DIG) {
        return true;
    }

    /* If an exponent was found, ensure it is within chosen range. */
    if (str != end) {  /* If not at end, we must have broken above. */
        bool negative = *(++str) == '-'; /* First remove exponential. */
        len = 0;
        if (is_sign(str)) {
            str += 1;
        }
        len = (unsigned)(end - str);
        if (!(negative ? neg_exp_ok(len, str) : pos_exp_ok(len, str))) {
            return true;
        }
    }

    /* OK. */
    return false;
}


int
detect_base(const char *str, const Py_ssize_t len)
{
    const char starts_with_sign = (char) is_sign(str);
    str += starts_with_sign;
    consume_white_space_py2_only(str);

    if (str[0] != '0' || len == 1) {
        return 10;
    }
    else if (str[1] == 'x' || str[1] == 'X') {
        return 16;
    }
    else if (str[1] == 'o' || str[1] == 'O') {
        return 8;
    }
    else if (str[1] == 'b' || str[1] == 'B') {
        return 2;
    }
    else
        /* "old" (C-style) octal literal, still valid in
         * 2.x, although illegal in 3.x.
         */
#if PY_MAJOR_VERSION == 2
        return 8;
#else
        return 10;
#endif
}


bool
is_valid_digit_arbitrary_base(const char c, const int base)
{
    if (base < 10) {
        return c >= '0' && c <= ((int) '0' + base);
    }
    else {
        const char offset = (char) base - 10;
        return (c >= '0' && c <= '9') ||
               (c >= 'a' && c <= 'a' + offset) ||
               (c >= 'A' && c <= 'A' + offset);
    }
}


/* Given string bounds, count the number of zeros at the end. */
static uint16_t
number_trailing_zeros(const char *start, const char *end)
{
    register uint16_t n = 0;
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


/* Calculate the exponential scaling factor with hard-coded values. */
long double
power_of_ten_scaling_factor(const int expon) {
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
