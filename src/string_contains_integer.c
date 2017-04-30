/* Scan a string and determine if it is a Python integer */
/* It is assumed that leading whitespace has already been removed. */
#include "parsing.h"

bool
string_contains_integer(const char *str, const char *end)
{
    register bool valid = false;
    (void) consume_sign(str);
    while (is_valid_digit(str)) {
        str++;
        consume_single_underscore_before_digit_36_and_above(str);
        valid = true;
    }
    (void) consume_python2_long_literal_lL(str);
    return valid && str == end;
}


static bool
is_valid_digit_arbitrary_base(const char c, const int base);


bool
string_contains_integer_arbitrary_base(const char *str, const char *end, const int base)
{
    register bool valid = false;
    register int actual_base = base;
    register size_t len = 0;

    (void) consume_sign(str);
    consume_white_space_py2_only(str);  /* For some reason, Python 2 allows space between the sign and the digits. */

    len = end - str;

    /* This if-block lifted from Python source code. */
    if (base == 0) {
        /* No base given. Deduce the base from the contents
         * of the string
         */
        if (str[0] != '0' || len == 1)
            return string_contains_integer(str, end);  /* Base-10 */
        else if (str[1] == 'x' || str[1] == 'X')
            actual_base = 16;
        else if (str[1] == 'o' || str[1] == 'O')
            actual_base = 8;
        else if (str[1] == 'b' || str[1] == 'B')
            actual_base = 2;
        else
            /* "old" (C-style) octal literal, still valid in
             * 2.x, although illegal in 3.x.
             */
#if PY_MAJOR_VERSION == 2
            actual_base = 8;
#else
            return false;
#endif
    }

    /* This if-block also lifted from Python source code.
     * Skips leading characters.
     */
    if (len > 1 && str[0] == '0' &&
        ((actual_base == 16 && (str[1] == 'x' || str[1] == 'X')) ||
         (actual_base == 8  && (str[1] == 'o' || str[1] == 'O')) ||
         (actual_base == 2  && (str[1] == 'b' || str[1] == 'B')))) {
        str += 2;
    }

    /* The rest behaves as normal. */
    while (is_valid_digit_arbitrary_base(*str, actual_base)) {
        str++;
        consume_single_underscore_before_digit_36_and_above(str);
        valid = true;
    }
#if PY_MAJOR_VERSION == 2
    if (actual_base == 2 || actual_base == 8 ||
        actual_base == 10 || actual_base == 16) {
        (void) consume_python2_long_literal_lL(str);
    }
#endif
    return valid && str == end;
}


bool
is_valid_digit_arbitrary_base(const char c, const int base)
{
    /* We know base 10 will not be given here. */
    if (base < 10)
        return c >= '0' && c <= ((int) '0' + base);
    else {
        char offset = (char) base - 10;
        return (c >= '0' && c <= '9') ||
               ((c >= 'a'|| c >= 'A') && (c < ('a' + offset) || c < ('A' + offset)));
    }
}
