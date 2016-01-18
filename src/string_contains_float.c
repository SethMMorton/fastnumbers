/* Scan a string and determine if it is a Python float */
#include "parsing.h"
#include "fast_conversions.h"

bool
string_contains_float (const char *str,
                       const bool allow_inf,
                       const bool allow_nan)
{
    register bool valid = false;

    consume_white_space(str);
    (void) consume_sign(str); 
 
    /* Are we possibly dealing with infinity or NAN? */

    if (is_n_or_N(str) || is_i_or_I(str)) {
        
        if (case_insensitive_match(str, "inf")) {
            str += 3;
            if (case_insensitive_match(str, "inity"))
                str += 5;
            return allow_inf &&
                   trailing_characters_are_vaild_and_nul_terminated(&str);
        }

        else if (case_insensitive_match(str, "nan")) {
            str += 3;
            return allow_nan &&
                   trailing_characters_are_vaild_and_nul_terminated(&str);
        }

    }

    /* Check if it is a float. */

    while (is_valid_digit(str)) { str++; valid = true; }

    if (!consume_python2_long_literal_lL(str)) {

        if (is_decimal(str)) {  /* After decimal digits */
            str++;
            while (is_valid_digit(str)) { str++; valid = true; }
        }

        if (is_e_or_E(str) && valid) {  /* Exponent */
            valid = false;
            str++;
            (void) consume_sign(str);         
            while (is_valid_digit(str)) { str++; valid = true; }
        }

    }

    return valid && trailing_characters_are_vaild_and_nul_terminated(&str);
}
