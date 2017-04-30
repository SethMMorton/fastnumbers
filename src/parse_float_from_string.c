/* See if a string contains a python float, and return the contained double. */
/* It is assumed that leading whitespace has already been removed. */
#include <Python.h>
#include <limits.h>
#include <float.h>
#include "parsing.h"
#include "quick_detection.h"
#include "pstdint.h"

/* Ensure 64 bits are handled. */
#ifndef INT64_MAX
#error "fastnumbers requires that your compiler support 64 bit integers, but it appears that this compiler does not"
#endif

static long double
power_of_ten_scaling_factor(const int expon);

static long double
apply_power_of_ten_scaling(const long double value, const int expon);

double
parse_float_from_string (const char *str, const char *end, bool *error)
{
    register uint64_t intvalue = 0UL;
    register bool valid = false;
    register int16_t decimal_expon = 0;
    register int16_t expon = 0;
    register const char starts_with_sign = (char) is_sign(str);
    register int8_t sign = (starts_with_sign && is_negative_sign(str)) ? -1L : 1L;
    *error = true;

    /* If we had started with a sign, increment the pointer by one. */

    str += starts_with_sign;

    /* Otherwise parse as an actual number. */

    while (is_valid_digit(str)) {
        intvalue *= 10UL;
        intvalue += ascii2ulong(str);
        valid = true;
        str++;
        consume_single_underscore_before_digit_36_and_above(str);
    }

    /* If long literal, quit here. */

    if (consume_python2_long_literal_lL(str)) {
        *error = !valid || str != end;
        return (long double) sign * (long double) intvalue;
    }

    /* Parse decimal part. */

    if (is_decimal(str)) {
        str++;
        while (is_valid_digit(str)) {
            intvalue *= 10UL;
            intvalue += ascii2ulong(str);
            valid = true;
            str++;
            consume_single_underscore_before_digit_36_and_above(str);
            decimal_expon++;
        }
        decimal_expon = -decimal_expon;
    }
 
    /* Parse exponential part. */

    if (is_e_or_E(str) && valid) {
        const int8_t exp_sign = (++str && consume_sign_and_is_negative(str)) ? -1 : 1;
        valid = false;
        while (is_valid_digit(str)) {
            expon *= 10;
            expon += ascii2int(str);
            valid = true;
            str++;
            consume_single_underscore_before_digit_36_and_above(str);
        }
        expon *= exp_sign;
    }

    *error = !valid || str != end;
    return (long double) sign *
           apply_power_of_ten_scaling((long double) intvalue, decimal_expon + expon);
}

long double
apply_power_of_ten_scaling(const long double value, const int expon)
{
    const long double scale = power_of_ten_scaling_factor(abs(expon));
    return expon < 0 ? value / scale : value * scale;
}

long double
power_of_ten_scaling_factor(const int expon) {
    /* Calculates the exponential scaling factor with hard-coded values. */
    switch(expon) {
    case 0:    return 1E0L;
    case 1:    return 1E1L;
    case 2:    return 1E2L;
    case 3:    return 1E3L;
    case 4:    return 1E4L;
    case 5:    return 1E5L;
    case 6:    return 1E6L;
    case 7:    return 1E7L;
    case 8:    return 1E8L;
    case 9:    return 1E9L;
    case 10:   return 1E10L;
    case 11:   return 1E11L;
    case 12:   return 1E12L;
    case 13:   return 1E13L;
    case 14:   return 1E14L;
    case 15:   return 1E15L;
    case 16:   return 1E16L;
    case 17:   return 1E17L;
    case 18:   return 1E18L;
    case 19:   return 1E19L;
    case 20:   return 1E20L;
    case 21:   return 1E21L;
    case 22:   return 1E22L;
    case 23:   return 1E23L;
    case 24:   return 1E24L;
    case 25:   return 1E25L;
    case 26:   return 1E26L;
    case 27:   return 1E27L;
    case 28:   return 1E28L;
    case 29:   return 1E29L;
    case 30:   return 1E30L;
    case 31:   return 1E31L;
    case 32:   return 1E32L;
    case 33:   return 1E33L;
    case 34:   return 1E34L;
    case 35:   return 1E35L;
    case 36:   return 1E36L;
    case 37:   return 1E37L;
    case 38:   return 1E38L;
    case 39:   return 1E39L;
    case 40:   return 1E40L;
    case 41:   return 1E41L;
    case 42:   return 1E42L;
    case 43:   return 1E43L;
    case 44:   return 1E44L;
    case 45:   return 1E45L;
    case 46:   return 1E46L;
    case 47:   return 1E47L;
    case 48:   return 1E48L;
    case 49:   return 1E49L;
    case 50:   return 1E50L;
    case 51:   return 1E51L;
    case 52:   return 1E52L;
    case 53:   return 1E53L;
    case 54:   return 1E54L;
    case 55:   return 1E55L;
    case 56:   return 1E56L;
    case 57:   return 1E57L;
    case 58:   return 1E58L;
    case 59:   return 1E59L;
    case 60:   return 1E60L;
    case 61:   return 1E61L;
    case 62:   return 1E62L;
    case 63:   return 1E63L;
    case 64:   return 1E64L;
    case 65:   return 1E65L;
    case 66:   return 1E66L;
    case 67:   return 1E67L;
    case 68:   return 1E68L;
    case 69:   return 1E69L;
    case 70:   return 1E70L;
    case 71:   return 1E71L;
    case 72:   return 1E72L;
    case 73:   return 1E73L;
    case 74:   return 1E74L;
    case 75:   return 1E75L;
    case 76:   return 1E76L;
    case 77:   return 1E77L;
    case 78:   return 1E78L;
    case 79:   return 1E79L;
    case 80:   return 1E80L;
    case 81:   return 1E81L;
    case 82:   return 1E82L;
    case 83:   return 1E83L;
    case 84:   return 1E84L;
    case 85:   return 1E85L;
    case 86:   return 1E86L;
    case 87:   return 1E87L;
    case 88:   return 1E88L;
    case 89:   return 1E89L;
    case 90:   return 1E90L;
    case 91:   return 1E91L;
    case 92:   return 1E92L;
    case 93:   return 1E93L;
    case 94:   return 1E94L;
    case 95:   return 1E95L;
    case 96:   return 1E96L;
    case 97:   return 1E97L;
    case 98:   return 1E98L;
    case 99:   return 1E99L;
    case 100:   return 1E100L;
    case 101:   return 1E101L;
    case 102:   return 1E102L;
    case 103:   return 1E103L;
    case 104:   return 1E104L;
    case 105:   return 1E105L;
    case 106:   return 1E106L;
    case 107:   return 1E107L;
    case 108:   return 1E108L;
    case 109:   return 1E109L;
    case 110:   return 1E110L;
    /* We should never see anything larger than 99. */
    /* This should never be reached. */
    default: return 1E308L;
    }
}
