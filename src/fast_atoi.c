/*
 * Simple and fast atoi (ascii to int) function.
 *
 * Based on fast_atof.
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Error checking added by Seth M. Morton, July 30, 2014 
 * Overflow checking added by Seth M. Morton, April 19, 2015
 */
#include <Python.h>
#include "fast_conversions.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

long fast_atoi (const char *p, bool *error, bool *overflow)
{
    long sign = 1L;
    long value = 0L, tmpval = 0L;
    bool valid = false;
    *overflow = false;
 
    /* Skip leading white space, if any. */
 
    while (white_space(*p)) { p += 1; }
 
    /* Get sign, if any. */

    if (*p == '-') {
        sign = -1L;
        p += 1;
    } else if (*p == '+') {
        p += 1;
    }
 
    /* Get digits, if any. Check for overflow. */
 
    value = 0L;
    while (valid_digit(*p)) {
        tmpval = (long) (*p - '0');
        *overflow = *overflow || ( value > ( LONG_MAX - tmpval ) / 10L );
        value = value * 10L + tmpval;
        valid = true;
        p += 1;
    }
 
#if PY_MAJOR_VERSION == 2
    /* On Python 2, long literals are allowed and end in 'l'. */

    if (*p == 'l' || *p == 'L') { p += 1; }
#endif

    /* Skip trailing white space, if any. */
 
    while (white_space(*p)) { p += 1; }

    /* If the next character is not the null character, it is an error. */
    /* Make sure we have at least seen one valid character. */

    *error = *p != '\0' ? true : !valid;

    /* Return signed result. */
 
    return sign * value;

}