/*
 * Stripped-down atof to determine if a string is a float.
 *
 * Based on fast_atof.
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Modified by Seth M. Morton, Aug 3, 2014 
 */

#include "fast_conversions.h"
#include "convenience.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')
 
bool fast_atof_test (const char *p, const bool allow_inf, const bool allow_nan)
{
    const char *s;
    bool is_nan = false;
    bool is_inf = false;

    /* Skip leading white space, if any. */
 
    while (white_space(*p)) { p += 1; }
 
    /* Get sign, if any. */
 
    if (*p == '-') { p += 1; }
    else if (*p == '+') { p += 1; }
 
    /* Are we possibly dealing with infinity or NAN? */

    if (*p == 'i' || *p == 'I' || *p == 'n' || *p == 'N') {
        
        /* Make a pointer copy so we can back up if needed. */

        s = p;

        /* Are we infinity? */

        if (case_insensitive_match(s, "inf")) {
            s += 3;
            if (case_insensitive_match(s, "inity"))
                s += 5;
            is_inf = true;
        }

        /* Are we NaN? */

        else if (case_insensitive_match(s, "nan")) {
            s += 3;
            is_nan = true;
        }

        /* Reset pointer. */

        p = s;

    }

    /* Otherwise this is might be an actual number. */

    else {

        /* Get digits before decimal point or exponent, if any. */
     
        while (valid_digit(*p)) { p += 1; }
     
        /* Get digits after decimal point, if any. */
     
        if (*p == '.') {
            p += 1;
            while (valid_digit(*p)) { p += 1; }
        }
     
        /* Handle exponent, if any. */
     
        if ((*p == 'e') || (*p == 'E')) {
     
            /* Get sign of exponent, if any. */
     
            p += 1;
            if (*p == '-') { p += 1; }
            else if (*p == '+') { p += 1; }
     
            /* Get digits of exponent, if any. */
     
            while (valid_digit(*p)) { p += 1; }
     
        }
 
    }

    /* Skip trailing white space, if any. */
 
    while (white_space(*p)) { p += 1; }

    /* Make sure that only '+' or '-' are flagged as an error. */
    /* If the next character is the null character, it is a float. */

    if (allow_inf && allow_nan)
        return *p == '\0' ? !(*(p-1) == '+' || *(p-1) == '-') : false;
    else if (allow_inf)
        return *p == '\0' ? !is_nan && !(*(p-1) == '+' || *(p-1) == '-') : false;
    else if (allow_nan)
        return *p == '\0' ? !is_inf && !(*(p-1) == '+' || *(p-1) == '-') : false;
    else
        return *p == '\0' ? !is_inf && !is_nan && !(*(p-1) == '+' || *(p-1) == '-') : false;

}