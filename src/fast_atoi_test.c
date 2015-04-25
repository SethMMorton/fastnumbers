/*
 * Stripped-down atof to determine if a string is an int.
 *
 * Based on fast_atof.
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Modified by Seth M. Morton, Aug 3, 2014 
 */
#include <Python.h>
#include "fast_conversions.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

bool fast_atoi_test (const char *p)
{
    bool valid = false;

    /* Skip leading white space, if any. */
 
    while (white_space(*p)) { p += 1; }
 
    /* Get sign, if any. */
 
    if (*p == '-') { p += 1; }
    else if (*p == '+') { p += 1; }
 
    /* Get digits if any. */
 
    while (valid_digit(*p)) { p += 1; valid = true; }

#if PY_MAJOR_VERSION == 2
    /* On Python 2, long literals are allowed and end in 'l'. */

    if (*p == 'l' || *p == 'L') { p += 1; }
#endif

    /* Skip trailing white space, if any. */
 
    while (white_space(*p)) { p += 1; }

    /* If the next character is the null character, it is an int. */
    /* Make sure we have at least seen one valid character. */

    return *p == '\0' ? valid : false;

}