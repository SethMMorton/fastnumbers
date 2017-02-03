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

/* Consume characters based on ID. */
#if PY_MAJOR_VERSION == 2
#define consume_python2_long_literal_lL(str) (is_l_or_L(str) && ++(str))
#else
#define consume_python2_long_literal_lL(str) false
#endif
#define consume_white_space(str) while (is_white_space(str)) ++(str)
#define consume_non_white_space(str) while (!is_white_space(str)) ++(str)
#define consume_sign(str) (is_sign(str) && ++(str))
#define consume_decimal(str) (is_decimal(str) && ++(str))
#define consume_exponent_prefix(str) (is_e_or_E(str) && ++(str))
#define consume_sign_and_is_negative(str) (is_negative_sign(str) ? \
	                                      (consume_sign(str) && true) : \
	                                      (consume_sign(str) && false))

/* A rather bold MACRO to strip whitespace from both ends. */
#define strip_whitespace(start, end, length) \
do { \
    (end) = (start) + (size_t) (length) - 1; /* Length includes NUL char. */ \
    consume_white_space(start); \
    while (is_white_space(end) && (start) != (end)) --(end); \
    end += 1;  /* End on the space after the non-whitespace. */ \
} while(0)

/* Quickly detect INFINITY and NAN.
 * Check length, first, and last characters, and only if they
 * match do we continue.
 */
#define quick_detect_infinity(start, len) \
    is_i_or_I(start) && \
        (((len) == 3 && is_f_or_F((start) + 2) && is_n_or_N((start) + 1)) \
      || ((len) == 8 && is_y_or_Y((start) + 7) && is_n_or_N((start) + 1) && \
                                                  is_f_or_F((start) + 2) && \
                                                  is_i_or_I((start) + 3) && \
                                                  is_n_or_N((start) + 4) && \
                                                  is_i_or_I((start) + 5) && \
                                                  is_t_or_T((start) + 6)))
#define quick_detect_nan(start, len) \
    is_n_or_N(start) && \
        ((len) == 3 && is_n_or_N((start) + 2) && is_a_or_A((start) + 1))

/* Quickly detect if a string is an integer. */
#define MAX_INT_LEN 18
#define int_might_overflow(start, end) ((end) - (start) - (size_t) is_sign(start)) > MAX_INT_LEN
#define int_start_is_OK
#if PY_MAJOR_VERSION == 2
#define is_likely_int(start, end) \
    ((end) - (start) > 0 && \
        (is_valid_digit(start) && \
            (is_valid_digit((end) - 1) || \
                (is_l_or_L((end) - 1)    && \
                 (end) - (start) > 1     && \
                 is_valid_digit((end) - 2)  \
                 ) \
             ) \
         ))
#else
#define is_likely_int(start, end) \
    ((end) - (start) > 0 && (is_valid_digit(start) && is_valid_digit((end) - 1)))
#endif

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
parse_float_from_string(const char *str, const char *end, bool *error, bool *overflow);

bool
string_contains_float(const char *str, const char *end,
                      const bool allow_inf, const bool allow_nan);

bool
string_contains_intlike_float(const char *str, const char *end);

bool
string_contains_integer(const char *str, const char *end);

bool
string_contains_non_overflowing_float(const char *str, const char *end);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_PARSING */
