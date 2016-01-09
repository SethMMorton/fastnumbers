#ifndef __PARSING
#define __PARSING

#include <Python.h>
#include "fn_bool.h"

inline static int ascii2int(const char *c) {
    return *c - '0';
}
inline static unsigned ascii2uint(const char *c) {
    return *c - '0';
}
inline static long ascii2long(const char *c) {
    return *c - '0';
}
inline static unsigned long ascii2ulong(const char *c) {
    return *c - '0';
}

inline static bool is_white_space(const char *c)
{
    return *c == ' ' || *c == '\t';
}
inline static bool is_valid_digit(const char *c)
{
    return *c >= '0' && *c <= '9';
}
inline static bool is_null(const char *c)
{
    return *c == '\0';
}
inline static bool is_decimal(const char *c)
{
    return *c == '.';
}
inline static bool is_l_or_L(const char *c)
{
    return *c == 'l' || *c == 'L';
}
inline static bool is_e_or_E(const char *c)
{
    return *c == 'e' || *c == 'E';
}
inline static bool is_n_or_N(const char *c)
{
    return *c == 'n' || *c == 'N';
}
inline static bool is_i_or_I(const char *c)
{
    return *c == 'i' || *c == 'I';
}
inline static bool is_negative_sign(const char *c)
{
    return *c == '-';
}
inline static bool is_positive_sign(const char *c)
{
    return *c == '+';
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
inline static bool consume_sign_and_is_negative(const char **str)
{
    switch (**str) {
        case '+': {
            (*str) += 1;
            return false;
        }
        case '-': {
            (*str) += 1;
            return true;
        }
        default:
            return false;
    }
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
