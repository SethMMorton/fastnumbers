/* Scan a string and determine if it is a Python integer */
#include "parsing.h"
#include "fast_conversions.h"

bool
string_contains_integer (const char *str)
{
    register bool valid = false;
    consume_white_space(str);
    (void) consume_sign(str); 
    while (is_valid_digit(str)) { str++; valid = true; }
    (void) consume_python2_long_literal_lL(str);
    return valid && trailing_characters_are_vaild_and_nul_terminated(&str);
}
