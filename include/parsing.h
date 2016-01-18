#ifndef __PARSING
#define __PARSING

#include <Python.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ascii2int(c) ((int) (*(c) - '0'))
#define ascii2uint(c) ((unsigned) (*(c) - '0'))
#define ascii2long(c) ((long) (*(c) - '0'))
#define ascii2ulong(c) ((unsigned long) (*(c) - '0'))

#define is_white_space(c) (*(c) == ' ' || *(c) == '\t')
#define is_valid_digit(c) (*(c) >= '0' && *(c) <= '9')
#define is_zero(c) (*(c) == '0')
#define is_null(c) (*(c) == '\0')
#define is_decimal(c) (*(c) == '.')
#define is_l_or_L(c) (*(c) == 'l' || *(c) == 'L')
#define is_e_or_E(c) (*(c) == 'e' || *(c) == 'E')
#define is_n_or_N(c) (*(c) == 'n' || *(c) == 'N')
#define is_i_or_I(c) (*(c) == 'i' || *(c) == 'I')
#define is_negative_sign(c) (*(c) == '-')
#define is_positive_sign(c) (*(c) == '+')
#define is_sign(c) (is_negative_sign(c) || is_positive_sign(c))
#define is_non_integer_character(c) (is_decimal(c) || is_e_or_E(c))

#if PY_MAJOR_VERSION == 2
#define consume_python2_long_literal_lL(str) (is_l_or_L(str) && ++(str))
#else
#define consume_python2_long_literal_lL(str) false
#endif
#define consume_white_space(str) while (is_white_space(str)) ++(str)
#define consume_sign(str) (is_sign(str) && ++(str))
#define consume_decimal(str) (is_decimal(str) && ++(str))
#define consume_exponent_prefix(str) (is_e_or_E(str) && ++(str))
#define consume_sign_and_is_negative(str) (is_negative_sign(str) ? \
	                                      (consume_sign(str) && true) : \
	                                      (consume_sign(str) && false))

bool
case_insensitive_match(const char *s, const char *t);

bool
trailing_characters_are_vaild_and_nul_terminated(const char **str);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PARSING */
