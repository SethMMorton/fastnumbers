/* Scan a string and determine if it is a Python float */
#include <float.h>
#include "parsing.h"
#include "fast_conversions.h"

bool
string_contains_non_overflowing_float (const char *str)
{
    register bool valid = false;
    register unsigned ndigits = 0;

    consume_white_space(str);
    (void) consume_sign(str); 
 
    /* Are we possibly dealing with infinity or NAN? */

    if (is_n_or_N(str) || is_i_or_I(str)) {
        
        if (case_insensitive_match(str, "inf")) {
            str += 3;
            if (case_insensitive_match(str, "inity"))
                str += 5;
            return trailing_characters_are_vaild_and_nul_terminated(&str);
        }

        else if (case_insensitive_match(str, "nan")) {
            str += 3;
            return trailing_characters_are_vaild_and_nul_terminated(&str);
        }

    }

    /* Check if it is a float. */
    ndigits = 0;
    while (is_valid_digit(str)) { str++; ndigits++; valid = true; }

    if (!consume_python2_long_literal_lL(str)) {

        if (is_decimal(str)) {  /* After decimal digits */
            str++;
            while (is_valid_digit(str)) { str++; ndigits++; valid = true; }
        }

        if (is_e_or_E(str) && valid) {  /* Exponent */
            valid = false;
            str++;
            (void) consume_sign(str);         
            while (is_valid_digit(str)) { str++; valid = true; }
        }

    }

    return valid && ndigits < DBL_DIG &&
           trailing_characters_are_vaild_and_nul_terminated(&str);
}
