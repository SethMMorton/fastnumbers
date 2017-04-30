/*
 * This file contains a function to quickly guess if a float string might overflow.
 *
 * Author: Seth M. Morton
 *
 * February 2017
 */

#include "quick_detection.h"

bool
float_might_overflow(const char *str, const char *end)
{
    register unsigned len = 0;
    register bool exp_ok = true;

    /* Scan string, collecting the count of digits. */
    while (str < end) {
        len += (unsigned) is_valid_digit(str);
        if (is_e_or_E(str))
            break;  /* Don't count digits after exponent. */
        str++;
        consume_single_underscore_before_digit_36_and_above(str);
    }

    /* If an exponent was found, ensure it is within chosen range. */
    if (str != end) {  /* If not at end, we must have broken above. */
        register bool negative = is_negative_sign(++str); /* First remove exponential. */
        register unsigned len2 = 0;
        (void) consume_sign(str);
        len2 = (unsigned) (end - str);
        exp_ok = negative ? neg_exp_might_overflow(len2, str) : pos_exp_might_overflow(len2, str);
    }

    /* If there are too many digits or the exponent is not OK, it might overflow. */
    return len > FN_DBL_DIG || !exp_ok;
}
