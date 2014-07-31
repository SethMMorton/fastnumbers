/*
 * Fast implementation of atoi (ascii to integer) function.
 *
 * Adapted from http://arekzb.wordpress.com/2008/05/04/atoi-or-not-to-atoi/
 * Originally written by Arek Bochinski, May 4, 2008
 * Whitespace and error checking added by Seth M. Morton, July 30, 2014 
 */
#include "fast_conversions.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')

int fast_atoi(char *c, bool *error) {
    int res = 0, n = 1;

    /* Skip leading white space, if any. */
 
    while (white_space(*c)) { *c += 1; }

    /* Handle negative signs */
    
    if (*c == '-') { n=-1; *c += 1; }
    
    /* Convert each character to numbers. */
    
    while (*c >= '0' && *c <= '9')
        res = res * 10 + *c++ - '0';

    /* If the next character is not whitespace or null character, it is an error. */
    *error = (!white_space(*c) || *c != '\n') ? true : false;

    /* Return with sign */
    return res * n;
}