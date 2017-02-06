/* Scan a string and determine if it is a Python integer */
/* It is assumed that leading whitespace has already been removed. */
#include "parsing.h"

bool
string_contains_integer (const char *str, const char *end)
{
    register bool valid = false;
    (void) consume_sign(str);
    while (is_valid_digit(str)) {
        str++;
        valid = true;
    }
    (void) consume_python2_long_literal_lL(str);
    return valid && str == end;
}
