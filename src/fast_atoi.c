/*
 * Simple and fast atoi (ascii to int) function.
 *
 * Based on fast_atof.
 * 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 * Error checking added by Seth M. Morton, July 30, 2014 
 */
#include "fast_conversions.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

// int fast_atoi(char *c, bool *error) {
//     int res = 0, n = 1;

//     /* Skip leading white space, if any. */
 
//     while (white_space(*c)) { c += 1; }

//     /* Handle negative/positive signs */
    
//     if (*c == '-') { n=-1; c += 1; }
//     else if (*c == '+') { c += 1; }
    
//     /* Convert each character to numbers. */
    
//     while (*c >= '0' && *c <= '9')
//         res = res * 10 + *c++ - '0';

//     /* If the next character is not whitespace or null character, it is an error. */
//     *error = (!white_space(*c) && *c != '\0' && *c != '\n') ? true : false;

//     /* Return with sign */
//     return res * n;
// }

long fast_atoi (char *p, bool *error)
{
    int sign;
    long value;
 
    /* Skip leading white space, if any. */
 
    while (white_space(*p)) { p += 1; }
 
    /* Get sign, if any. */

    sign = 1;
    if (*p == '-') {
        sign = -1;
        p += 1;
    } else if (*p == '+') {
        p += 1;
    }
 
    /* Get digits, if any. */
 
    for (value = 0; valid_digit(*p); p += 1) {
        value = value * 10 + (*p - '0');
    }
 
    /* If the next character is not whitespace or null character, it is an error. */
    *error = (!white_space(*p) && *p != '\0' && *p != '\n') ? true : false;

    /* Return signed result. */
 
    return sign * value;
}