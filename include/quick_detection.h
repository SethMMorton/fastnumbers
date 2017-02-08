#ifndef __FN_QUICK_DETECTION
#define __FN_QUICK_DETECTION

#include <limits.h>
#include "parsing.h"

#ifdef __cplusplus
extern "C" {
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

/* Guess if an int will overflow.
 * Base the value on the size of this system's long.
 */
#if LONG_MAX == 0xffffffffffffffff
#define MAX_INT_LEN 18
#else
#define MAX_INT_LEN 9
#endif
#define int_might_overflow(start, end) ((end) - (start) - (size_t) is_sign(start)) > MAX_INT_LEN

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

/* Guess if a float will overflow. */
bool
float_might_overflow(const char *start, const char *end);

/* Quickly detect if a string is a float. */
#if PY_MAJOR_VERSION == 2
#define is_likely_float(start, end) \
    ((end) - (start) > 0 && \
        (is_valid_digit(start) || \
            (is_decimal(start) && is_valid_digit((start) + 1))) && \
        (is_valid_digit((end) - 1) || \
            ((is_decimal((end) - 1) || is_l_or_L((end) - 1)) && \
             (end) - (start) > 1   && \
             is_valid_digit((end) - 2) \
             ) \
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

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_QUICK_DETECTION */
