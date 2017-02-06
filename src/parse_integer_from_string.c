/* See if a string contains a python integer, and return the contained long. */
/* It is assumed that leading whitespace has already been removed. */
#include "parsing.h"

long
parse_integer_from_string (const char *str, const char *end, bool *error)
{
    register long value = 0L;
    register bool valid = false;
    register long sign = 1L;
    *error = true;

    sign = consume_sign_and_is_negative(str) ? -1L : 1L;

    /* Convert digits, if any. Check for overflow. */

    while (is_valid_digit(str)) {
        value *= 10L;
        value += ascii2long(str);
        valid = true;
        str++;
    }

    (void) consume_python2_long_literal_lL(str);
    *error = !valid || str != end;
    return sign * value;

}

