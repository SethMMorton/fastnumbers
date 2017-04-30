/* Scan a string and determine if it is a Python float */
/* It is assumed that leading whitespace has already been removed. */
#include "parsing.h"
#include "quick_detection.h"

bool
string_contains_float(const char *str,
                      const char *end,
                      const bool allow_inf,
                      const bool allow_nan)
{
    register bool valid = false;
    register const char starts_with_sign = (char) is_sign(str);

    /* Shorten length by one if it starts with sign. */
    register const size_t len = end - str - starts_with_sign;

    /* If we had started with a sign, increment the pointer by one. */

    str += starts_with_sign;
 
    /* Are we possibly dealing with infinity or NAN? */

    if (quick_detect_infinity(str, len))
        return allow_inf;
    if (quick_detect_nan(str, len))
        return allow_nan;

    /* Check if it is a float. */

    while (is_valid_digit(str)) {
        str++;
        consume_single_underscore_before_digit_36_and_above(str);
        valid = true;
    }

    if (!consume_python2_long_literal_lL(str)) {

        if (is_decimal(str)) {  /* After decimal digits */
            str++;
            while (is_valid_digit(str)) {
                str++;
                consume_single_underscore_before_digit_36_and_above(str);
                valid = true;
            }
        }

        if (is_e_or_E(str) && valid) {  /* Exponent */
            valid = false;
            str++;
            (void) consume_sign(str);         
            while (is_valid_digit(str)) {
                str++;
                consume_single_underscore_before_digit_36_and_above(str);
                valid = true;
            }
        }

    }

    return valid && str == end;
}
