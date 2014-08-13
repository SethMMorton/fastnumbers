/*
 * Stripped-down atof to determine if a string is an int.
 *
 * Based on fast_atof.
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Modified by Seth M. Morton, Aug 3, 2014 
 */
#include "fast_conversions.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')
#define invalid_char(c) (*(c) == '+' || *(c) == '-' || *(c) == 'e' || *(c) == 'E')

bool fast_atoi_test (const char *p)
{
 
    /* Skip leading white space, if any. */
 
    while (white_space(*p)) { p += 1; }
 
    /* Get sign, if any. */
 
    if (*p == '-') { p += 1; }
    else if (*p == '+') { p += 1; }
 
    /* Get digits if any. */
 
    while (valid_digit(*p)) { p += 1; }

 
    /* Skip trailing white space, if any. */
 
    while (white_space(*p)) { p += 1; }

    /* If the next character is the null character, it is an int. */
    /* Make sure that only '+' or '-' are flagged as an error. */

    return *p == '\0' ? !invalid_char(p-1) : false;

}