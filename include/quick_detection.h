#ifndef __FN_QUICK_DETECTION
#define __FN_QUICK_DETECTION

#include "parsing.h"
#include <float.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/*************
 * CONSTANTS *
 *************/

/* Define the number of digits in an int that fastnumbers is willing
 * to attempt to convert itself. This is entirely based on the size
 * of a this compilers long, since the long is what Python uses to
 * represent an int under the hood.
 */
#if ULONG_MAX == 0xffffffffffffffff
/* 64-bit int max == 9223372036854775807; len('9223372036854775807') - 1 == 18 */
#define FN_MAX_INT_LEN 18
#else
/* 32-bit int max == 2147483647; len('2147483647') - 1 == 9 */
#define FN_MAX_INT_LEN 9
#endif

/* Define the number of digits in a float that fastnumbers is willing
 * to attempt to convert itself. For Clang and GNUC, it is known
 * that DBL_DIG - 4 works. It is known that DBL_DIG - 6 works for MSVC.
 * To be safe, any compiler not known during fastnumbers testing will
 * use DBL_DIG - 6.
 * Also define the largest and smallest exponent that
 * fastnumbers is willing attempt to convert itself. For Clang and GNUC,
 * it is known that larger numbers work than MSVC.
 * To be safe, any compiler not known during fastnumbers testing will
 * use the smaller numbers.
 */
#if defined(__clang__) || defined(__GNUC__)
#define FN_DBL_DIG DBL_DIG - 4
#define FN_MAX_EXP 99
#define FN_MIN_EXP -98
#else
#define FN_DBL_DIG DBL_DIG - 6
#define FN_MAX_EXP 22
#define FN_MIN_EXP -22
#endif

/****************
 * HACKY MACROS *
 ****************/

/* Quickly detect if an exponent will overflow.
 */
#if defined(__clang__) || defined(__GNUC__)
#define neg_exp_might_overflow(len, str) \
    ((len) == 1) || \
    ((len) == 2 && (*(str) <= '8' || \
                      (*(str) == '9' && *((str) + 1) <= '8') \
                   ) \
     )
#define pos_exp_might_overflow(len, str) (len) > 0 && (len) <= 2
#else
/* Same for both positive and negative. */
#define __exp_might_overflow(len, str) \
    ((len) == 1) || \
    ((len) == 2 && (*(str) <= '1' || \
                      (*(str) == '2' && *((str) + 1) <= '2') \
                   ) \
     )
#define neg_exp_might_overflow(len, str) __exp_might_overflow((len), (str))
#define pos_exp_might_overflow(len, str) __exp_might_overflow((len), (str))
#endif

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

/* Quickly detect if a string is a float. Python2 has to handle a long literal. */
#if PY_MAJOR_VERSION == 2
#define is_likely_float(start, end) \
    ((end) - (start) > 0 && \
        ((is_valid_digit(start) || \
            (is_decimal(start) && is_valid_digit((start) + 1))) && \
         (is_valid_digit((end) - 1) || \
            ((is_decimal((end) - 1) || is_l_or_L((end) - 1)) && \
             (end) - (start) > 1   && \
             is_valid_digit((end) - 2) \
             ) \
         )) || \
    ((end) - (start) == 3 && \
        ((is_i_or_I(start) || is_n_or_N(start)) && \
         (is_f_or_F((end) - 1) || is_n_or_N((end) - 1))) \
     ) || \
    ((end) - (start) == 8 && \
        (is_i_or_I(start) && is_y_or_Y((end) - 1)) \
     ) \
    )
#else
#define is_likely_float(start, end) \
    ((end) - (start) > 0 && \
        ((is_valid_digit(start) || \
            (is_decimal(start) && is_valid_digit((start) + 1))) && \
         (is_valid_digit((end) - 1) || \
            (is_decimal((end) - 1) && \
             (end) - (start) > 1   && \
             is_valid_digit((end) - 2) \
             ) \
         )) || \
    ((end) - (start) == 3 && \
        ((is_i_or_I(start) || is_n_or_N(start)) && \
         (is_f_or_F((end) - 1) || is_n_or_N((end) - 1))) \
     ) || \
    ((end) - (start) == 8 && \
        (is_i_or_I(start) && is_y_or_Y((end) - 1)) \
     ) \
    )
#endif

/* Guess if an int will overflow. */
#define int_might_overflow(start, end) ((end) - (start) - (size_t) is_sign(start)) > FN_MAX_INT_LEN

/****************
 * DECLARATIONS *
 ****************/

/* Guess if a float will overflow. */
bool
float_might_overflow(const char *start, const char *end);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_QUICK_DETECTION */
