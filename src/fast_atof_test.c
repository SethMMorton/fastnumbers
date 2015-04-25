/*
 * Stripped-down atof to determine if a string is a float.
 *
 * Based on fast_atof.
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Modified by Seth M. Morton, Aug 3, 2014 
 */

#include <Python.h>
#include "fast_conversions.h"
#include "convenience.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

bool fast_atof_test (const char *p, const bool allow_inf, const bool allow_nan)
{
    const char *s;
    bool valid = false;
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
            valid = true;
        }

        /* Are we NaN? */

        else if (case_insensitive_match(s, "nan")) {
            s += 3;
            is_nan = true;
            valid = true;
        }

        /* Reset pointer. */

        p = s;

    }

    /* Otherwise this is might be an actual number. */

    else {

        /* Get digits before decimal point or exponent, if any. */
     
        while (valid_digit(*p)) { p += 1; valid = true; }
     
#if PY_MAJOR_VERSION == 2
        /* On Python 2, long literals are allowed and end in 'l'. */

        if (*p == 'l' || *p == 'L') { p += 1; }

        /* The following code is for floats, and can only be */
        /* valid if not a long literal, hence the else. */

        else {
#endif

            /* Get digits after decimal point, if any. */
         
            if (*p == '.') {
                p += 1;
                while (valid_digit(*p)) { p += 1; valid = true; }
            }
         
            /* Handle exponent, if any. */
         
            if (((*p == 'e') || (*p == 'E')) && valid) {
                valid = false;
         
                /* Get sign of exponent, if any. */
         
                p += 1;
                if (*p == '-') { p += 1; }
                else if (*p == '+') { p += 1; }
         
                /* Get digits of exponent, if any. */
         
                while (valid_digit(*p)) { p += 1; valid = true; }
         
            }

#if PY_MAJOR_VERSION == 2
        }
#endif
 
    }

    /* Skip trailing white space, if any. */
 
    while (white_space(*p)) { p += 1; }

    /* If the next character is the null character, it is a float. */
    /* Make sure we have at least seen one valid character. */

    if (allow_inf && allow_nan)
        return *p == '\0' ? valid : false;
    else if (allow_inf)
        return *p == '\0' ? !is_nan && valid : false;
    else if (allow_nan)
        return *p == '\0' ? !is_inf && valid : false;
    else
        return *p == '\0' ? !is_inf && !is_nan && valid : false;

}