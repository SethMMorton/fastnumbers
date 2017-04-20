/* See if a string contains a python integer, and return the contained long. */
/* It is assumed that leading whitespace has already been removed. */
#include "parsing.h"

long
parse_integer_from_string (const char *str, const char *end, bool *error)
{
    register long value = 0L;
    register bool valid = false;
    register const char starts_with_sign = (char) is_sign(str);
    register long sign = (starts_with_sign && is_negative_sign(str)) ? -1L : 1L;
    *error = true;

    /* If we had started with a sign, increment the pointer by one. */

    str += starts_with_sign;

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

