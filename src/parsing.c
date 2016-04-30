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
