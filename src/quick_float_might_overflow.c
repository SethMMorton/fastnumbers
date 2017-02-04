/*
 * This file contains a function to quickly guess if a float string might overflow.
 *
 * Author: Seth M. Morton
 *
 * February 2017
 */

#include <float.h>
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
    }

    /* If an exponent was found, ensure it is less than or equal to 255. */
    if (str != end) {  /* If not at end, we must have broken above. */
        register unsigned len2 = 0;
        str++;  /* Exponential. */
        (void) consume_sign(str);
        len2 = end - str;
        exp_ok = (len2 > 0 && len2 < 3) ||
                 (len2 == 3 && *(str + 1) >= '0' && *(str + 1) <= '2'
                            && *(str + 2) >= '0' && *(str + 2) <= '5'
                            && *(str + 3) >= '0' && *(str + 3) <= '5');
    }

    /* To be safe, we only allow up to three less than max double length. */
    return len >= DBL_DIG - 3 || !exp_ok;
}
