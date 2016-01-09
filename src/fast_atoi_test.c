/* Scan a string and determine if it is a Python integer */
#include "parsing.h"
#include "fast_conversions.h"

bool fast_atoi_test (const char *str, size_t expected_len)
{
    register bool valid = false;
    consume_white_space(&str);
    consume_sign(&str); 
    while (is_valid_digit(str)) { str += 1; valid = true; }
    consume_python2_long_literal_lL(&str);
    return valid && trailing_characters_are_vaild_and_nul_terminated(&str);
}
