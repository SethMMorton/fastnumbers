/* Scan a string and determine if it is a Python float */
/* It is assumed that leading whitespace has already been removed. */
#include "parsing.h"

static bool
between_chars_are_zero_or_decimal(const char* start, const char* end)
{
    register const char *c = NULL;
    for (c = start; c < end; c++) {
        if (!is_zero(c) && !is_decimal(c))
            return false;
    }
    return true;
}

bool
string_contains_intlike_float (const char *str, const char *end)
{
    register bool valid = false;
    register int pre_ndigits = 0;
    register int post_ndigits = 0;
    register int expon = 0;
    register int exp_sign = 0;
    register const char* pre_decimal_end = NULL;
    register const char *decimal_start = NULL;
    register const char *float_end = NULL;

    (void) consume_sign(str);
 
    /* Before decimal. Keep track of number of digits read. */

    pre_ndigits = 0;
    while (is_valid_digit(str)) {
        valid = true;
        pre_ndigits++;
        str++;
        consume_single_underscore_before_digit_36_and_above(str);
    }
    pre_decimal_end = str;

    /* If a long literal, stop here. */
    if (consume_python2_long_literal_lL(str))
        return valid && str == end;

    /* Decimal part of float. Keep track of number of digits read */
    /* as well as beginning and end locations. */

    post_ndigits = 0;
    decimal_start = str;
    if (is_decimal(str)) {  /* After decimal digits */
        str++;
        decimal_start = str;
        while (is_valid_digit(str)) {
            valid = true;
            post_ndigits++;
            str++;
            consume_single_underscore_before_digit_36_and_above(str);
        }
    }
    float_end = str;

    /* Exponential part of float. Parse the magnitude. */

    expon = 0;
    exp_sign = 0;
    if (is_e_or_E(str) && valid) {  /* Exponent */
        valid = false;
        str++;
        exp_sign = consume_sign_and_is_negative(str) ? -1 : 1;
        while (is_valid_digit(str)) {
            expon *= 10;
            expon += ascii2int(str);
            valid = true;
            str++;
            consume_single_underscore_before_digit_36_and_above(str);
        }
    }

    /* Move the decimal place left or right based on the exponential magnitude.
       Only zeros are allowed to follow the decimal place. */ 
    if (expon && post_ndigits) {
        if (exp_sign < 0 && expon >= pre_ndigits)
            valid = false;
        else if (exp_sign < 0)
            valid = between_chars_are_zero_or_decimal(pre_decimal_end - expon,
                                                      float_end);
        else if (expon >= post_ndigits)
            valid = true;
        else
            valid = between_chars_are_zero_or_decimal(decimal_start + expon,
                                                      float_end);
    }
    else if (post_ndigits) {
        valid = between_chars_are_zero_or_decimal(decimal_start, float_end);
    }
    else if (expon) {
        if (exp_sign < 0 && expon >= pre_ndigits)
            valid = false;
        else if (exp_sign < 0)
            valid = between_chars_are_zero_or_decimal(pre_decimal_end - expon,
                                                      pre_decimal_end);
        else
            valid = true;
    }

    return valid && str == end;
}
