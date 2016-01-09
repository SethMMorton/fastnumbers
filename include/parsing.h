#ifndef __PARSING
#define __PARSING

#include <Python.h>
#include "fn_bool.h"

inline static char current_char(const char *str)
{
    return *str;
}

inline static bool is_white_space(const char *c)
{
    return current_char(c) == ' ' || current_char(c) == '\t';
}
inline static bool is_valid_digit(const char *c)
{
    return current_char(c) >= '0' && current_char(c) <= '9';
}
inline static bool is_null(const char *c)
{
    return current_char(c) == '\0';
}
inline static bool is_decimal(const char *c)
{
    return current_char(c) == '.';
}
inline static bool is_l_or_L(const char *c)
{
    return current_char(c) == 'l' || current_char(c) == 'L';
}
inline static bool is_e_or_E(const char *c)
{
    return current_char(c) == 'e' || current_char(c) == 'E';
}
inline static bool is_n_or_N(const char *c)
{
    return current_char(c) == 'n' || current_char(c) == 'N';
}
inline static bool is_i_or_I(const char *c)
{
    return current_char(c) == 'i' || current_char(c) == 'I';
}
inline static bool is_negative_sign(const char *c)
{
    return current_char(c) == '-';
}
inline static bool is_positive_sign(const char *c)
{
    return current_char(c) == '+';
}
inline static bool is_sign(const char *c)
{
    return is_negative_sign(c) || is_positive_sign(c);
}
inline static bool is_non_integer_character(const char *c)
{
    return is_decimal(c) || is_e_or_E(c);
}

inline static bool consume_python2_long_literal_lL(const char **str)
{
#if PY_MAJOR_VERSION == 2
    if (is_l_or_L(*str)) {
        (*str) += 1;
        return true;
    }
    else {
        return false;
    }
#else
    return false;
#endif
}
inline static void consume_white_space(const char **str)
{
    while (is_white_space(*str)) (*str) += 1;
}
inline static void consume_sign(const char **str)
{
    if (is_sign(*str)) (*str) += 1;
}
inline static void consume_decimal(const char **str)
{
    if (is_decimal(*str)) (*str) += 1;
}
inline static void consume_exponent_prefix(const char **str)
{
    if (is_e_or_E(*str)) (*str) += 1;
}

inline static bool case_insensitive_match(const char *s, const char *t)
{
    while (*t && (Py_TOLOWER(*s) == *t)) {
        s++;
        t++;
    }
    return !(*t);
}

inline static bool trailing_characters_are_vaild_and_nul_terminated(const char **str)
{
    consume_white_space(str);
    return is_null(*str);
}

#endif /* __PARSING */
