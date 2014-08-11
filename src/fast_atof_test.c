/*
 * Stripped-down atof to determine if a string is a float.
 *
 * Based on fast_atof.
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Modified by Seth M. Morton, Aug 3, 2014 
 */
#include "fast_conversions.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')
 
bool fast_atof_test (char *p)
{
 
    /* Skip leading white space, if any. */
 
    while (white_space(*p)) { p += 1; }
 
    /* Get sign, if any. */
 
    if (*p == '-') { p += 1; }
    else if (*p == '+') { p += 1; }
 
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
 
    /* Skip trailing white space, if any. */
 
    while (white_space(*p)) { p += 1; }

    /* If the next character is the null character, it is a float. */

    return *p == '\0' ? true : false;

}