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

    /* If an exponent was found, ensure it is within chosen range. */
    if (str != end) {  /* If not at end, we must have broken above. */
        register bool negative = is_negative_sign(++str); /* First remove exponential. */
        register unsigned len2 = 0;
        (void) consume_sign(str);
        len2 = (unsigned) (end - str);
        /* Negative exponential can handle up to 98 (22 on MSVC). */
        if (negative)
            exp_ok = (len2 > 0 && len2 < 2) ||
#ifdef _MSC_VER
                     (len2 == 2 && (*str <= '1' ||
                                        (*str == '2' && *(str + 1) <= '2')
#else
                     (len2 == 2 && (*str <= '8' ||
                                        (*str == '9' && *(str + 1) <= '8')
#endif
                                    )
                      );
        /* Positive exponential can handle up to 99 (22 on MSVC). */
        else
#ifdef _MSC_VER
            exp_ok = (len2 > 0 && len2 < 2) ||
                     (len2 == 2 && (*str <= '1' ||
                                        (*str == '2' && *(str + 1) <= '2')
                                    )
                      );
#else
            exp_ok = len2 > 0 && len2 <= 2;
#endif
    }

#ifdef _MSC_VER
    /* To be safe, we only allow up to five less than max double length. */
    return len >= DBL_DIG - 5 || !exp_ok;
#else
    /* To be safe, we only allow up to three less than max double length. */
    return len >= DBL_DIG - 3 || !exp_ok;
#endif
}
