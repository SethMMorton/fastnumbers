/*
 * Simple and fast atof (ascii to float) function.
 *
 * - Executes about 5x faster than standard MSCRT library atof().
 * - An attractive alternative if the number of calls is in the millions.
 * - Assumes input is a proper integer, fraction, or scientific format.
 * - Matches library atof() to 15 digits (except at extreme exponents).
 *
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Error checking added by Seth M. Morton, July 30, 2014 
 */

#include "Python.h"
#include "fast_conversions.h"
#include "convenience.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')
 
double fast_atof (const char *p, bool *error)
{
    int frac = 0;
    unsigned int expon = 0;
    double sign = 1.0, value = 0.0, scale = 1.0;
    double pow10 = 10.0;
    const char *s;

    /* Skip leading white space, if any. */
 
    while (white_space(*p)) { p += 1; }
 
    /* Get sign, if any. */
 
    if (*p == '-') {
        sign = -1.0;
        p += 1;
    } else if (*p == '+') {
        p += 1;
    }
 
    /* Are we possibly dealing with infinity or NAN? */

    if (*p == 'i' || *p == 'I' || *p == 'n' || *p == 'N') {
        
        /* Make a pointer copy so we can back up if needed. */

        s = p;

        /* Are we infinity? */

        if (case_insensitive_match(s, "inf")) {
            s += 3;
            if (case_insensitive_match(s, "inity")) 
                s += 5;
            value = Py_HUGE_VAL;
        }

        /* Are we NaN? */

        else if (case_insensitive_match(s, "nan")) {
            s += 3;
            value = Py_NAN;
        }

        /* Reset pointer. */

        p = s;

    }

    /* Otherwise this is might be an actual number. */

    else {

        /* Get digits before decimal point or exponent, if any. */
     
        for (value = 0.0; valid_digit(*p); p += 1) {
            value = value * 10.0 + (*p - '0');
        }
     
        /* Get digits after decimal point, if any. */
     
        if (*p == '.') {
            p += 1;
            while (valid_digit(*p)) {
                value += (*p - '0') / pow10;
                pow10 *= 10.0;
                p += 1;
            }
        }
     
        /* Handle exponent, if any. */
     
        if ((*p == 'e') || (*p == 'E')) {
     
            /* Get sign of exponent, if any. */
     
            p += 1;
            if (*p == '-') {
                frac = 1;
                p += 1;
            } else if (*p == '+') {
                p += 1;
            }
     
            /* Get digits of exponent, if any. */
     
            for (expon = 0; valid_digit(*p); p += 1) {
                expon = expon * 10 + (*p - '0');
            }
            if (expon > 308) expon = 308;
     
            /* Calculate scaling factor. */
     
            while (expon >= 50) { scale *= 1E50; expon -= 50; }
            while (expon >=  8) { scale *= 1E8;  expon -=  8; }
            while (expon >   0) { scale *= 10.0; expon -=  1; }
        }
 
    }

    /* Skip trailing white space, if any. */
 
    while (white_space(*p)) { p += 1; }

    /* If the next character is not the null character, it is an error. */

    *error = *p != '\0' ? true : false;

    /* Return signed and scaled floating point result. */
 
    return sign * (frac ? (value / scale) : (value * scale));
}