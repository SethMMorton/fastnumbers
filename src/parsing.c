#include "parsing.h"

/* Macros to convert character to lower.  Only needed for Python 2.6. */
#if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION == 6
#include <ctype.h>
#define Py_TOLOWER(c) tolower(c)
#endif

bool
case_insensitive_match(const char *s, const char *t)
{
    while (*t && (Py_TOLOWER(*s) == *t)) {
        s++;
        t++;
    }
    return !(*t);
}

#if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION == 6
#undef Py_TOLOWER
#endif

bool
trailing_characters_are_vaild_and_nul_terminated(const char **str)
{
    consume_white_space(*str);
    return is_null(*str);
}


bool
precheck_input_may_be_int(const char **str)
{
    consume_white_space(*str);
    return is_valid_digit(*str) || is_sign(*str);
}

bool
precheck_input_may_be_float(const char **str)
{
    consume_white_space(*str);
    return is_valid_digit(*str) || is_sign(*str) ||
           is_decimal(*str) || is_n_or_N(*str) || is_i_or_I(*str);
}
