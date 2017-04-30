#ifndef __FN_PARSING
#define __FN_PARSING

#include <Python.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* All the awesome MACROS */

/* Convert char to int/long etc. */
#define ascii2int(c) ((int) (*(c) - '0'))
#define ascii2uint(c) ((unsigned) (*(c) - '0'))
#define ascii2long(c) ((long) (*(c) - '0'))
#define ascii2ulong(c) ((unsigned long) (*(c) - '0'))

/* ID characters. */
#define is_white_space(c) (*(c) == ' ' || (*(c) >= '\t' && *(c) <= '\r'))
#define is_valid_digit(c) (*(c) >= '0' && *(c) <= '9')
#define is_zero(c) (*(c) == '0')
#define is_null(c) (*(c) == '\0')
#define is_decimal(c) (*(c) == '.')
#define is_l_or_L(c) (*(c) == 'l' || *(c) == 'L')
#define is_e_or_E(c) (*(c) == 'e' || *(c) == 'E')
#define is_n_or_N(c) (*(c) == 'n' || *(c) == 'N')
#define is_a_or_A(c) (*(c) == 'a' || *(c) == 'A')
#define is_i_or_I(c) (*(c) == 'i' || *(c) == 'I')
#define is_f_or_F(c) (*(c) == 'f' || *(c) == 'F')
#define is_t_or_T(c) (*(c) == 't' || *(c) == 'T')
#define is_y_or_Y(c) (*(c) == 'y' || *(c) == 'Y')
#define is_negative_sign(c) (*(c) == '-')
#define is_positive_sign(c) (*(c) == '+')
#define is_sign(c) (is_negative_sign(c) || is_positive_sign(c))
#define is_non_integer_character(c) (is_decimal(c) || is_e_or_E(c))
#define is_underscore(c) (*(c) == '_')

/* Consume characters based on ID. */
#if PY_MAJOR_VERSION == 2
#define consume_python2_long_literal_lL(str) (is_l_or_L(str) && ++(str))
#else
#define consume_python2_long_literal_lL(str) false
#endif
#define consume_white_space(str) while (is_white_space(str)) ++(str)
#if PY_MAJOR_VERSION == 2
/* For some reason, Python 2 allows space between the sign and the digits. */
#define consume_white_space_py2_only(str) consume_white_space(str)
#else
#define consume_white_space_py2_only(str) do {} while (0)
#endif
#define consume_non_white_space(str) while (!is_white_space(str)) ++(str)
#define consume_sign(str) (is_sign(str) && ++(str))
#define consume_decimal(str) (is_decimal(str) && ++(str))
#define consume_exponent_prefix(str) (is_e_or_E(str) && ++(str))
#define consume_sign_and_is_negative(str) (is_negative_sign(str) ? \
	                                      (consume_sign(str) && true) : \
	                                      (consume_sign(str) && false))
/* Underscores only valid for 3.6 or above. */
#if PY_MAJOR_VERSION == 2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 6)
#define consume_single_underscore_before_digit_36_and_above(str) do {} while (0)
#else
#define consume_single_underscore_before_digit_36_and_above(str) \
    ((is_underscore(str) && is_valid_digit(str + 1)) && ++(str))
#endif

/* A rather bold MACRO to strip whitespace from both ends. */
#define strip_whitespace(start, end, length) \
do { \
    (end) = (start) + (size_t) (length) - 1; /* Length includes NUL char. */ \
    consume_white_space(start); \
    while (is_white_space(end) && (start) != (end)) --(end); \
    end += 1;  /* End on the space after the non-whitespace. */ \
} while(0)

/* Helper function declarations. */

bool
case_insensitive_match(const char *s, const char *t);

bool
trailing_characters_are_vaild_and_nul_terminated(const char **str);

bool
precheck_input_may_be_int(const char **str);

bool
precheck_input_may_be_float(const char **str);

/* These are the "fast conversion and checking" function declarations. */

long
parse_integer_from_string(const char *str, const char *end, bool *error);

double
parse_float_from_string(const char *str, const char *end, bool *error);

bool
string_contains_float(const char *str, const char *end,
                      const bool allow_inf, const bool allow_nan);

bool
string_contains_intlike_float(const char *str, const char *end);

bool
string_contains_integer(const char *str, const char *end);

bool
string_contains_integer_arbitrary_base(const char *str, const char *end, const int base);

bool
string_contains_non_overflowing_float(const char *str, const char *end);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_PARSING */
