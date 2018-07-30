#ifndef __FN_PARSING
#define __FN_PARSING

#include <Python.h>
#include <float.h>
#include <limits.h>
#include "pstdint.h"
#include "fn_bool.h"

/* Ensure 64 bits are handled. */
#ifndef INT64_MAX
#error "fastnumbers requires that your compiler support 64 bit integers, but it appears that this compiler does not"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Parsing helpers. */

/* Convert char to int/long etc. */
#define ascii2int(c) ((int) (*(c) - '0'))
#define ascii2uint(c) ((unsigned) (*(c) - '0'))
#define ascii2long(c) ((long) (*(c) - '0'))
#define ascii2ulong(c) ((unsigned long) (*(c) - '0'))

/* ID characters. */
#define is_white_space(c) (*(c) == ' ' || (*(c) >= '\t' && *(c) <= '\r'))
#define is_valid_digit(c) (*(c) >= '0' && *(c) <= '9')
#define is_valid_digit_char(c) ((c) >= '0' && (c) <= '9')
#define is_sign(c) (*(c) == '-' || *(c) == '+')

/* Consume characters based on ID. */
#if PY_MAJOR_VERSION == 2
#define consume_python2_long_literal_lL(str) \
    ((*(str) == 'l' || *(str) == 'L') && ++(str))
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

/* A rather bold macro to strip whitespace from both ends. */
#define strip_whitespace(start, end, length) \
    do { \
        (end) = (start) + (size_t) (length) - 1; /* Length includes NUL char. */ \
        consume_white_space(start); \
        while (is_white_space(end) && (start) != (end)) --(end); \
        end += 1;  /* End on the space after the non-whitespace. */ \
    } while(0)


/* Overflow detection */

/* Define the number of digits in an int that fastnumbers is willing
 * to attempt to convert itself. This is entirely based on the size
 * of a this compilers long, since the long is what Python uses to
 * represent an int under the hood - use one less than the maximum
 * length of a long.
 */
#if ULONG_MAX == UINT64_MAX
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
#define neg_exp_ok(len, str) \
    ((len) == 1) || \
    ((len) == 2 && (*(str) <= '8' || \
                    (*(str) == '9' && *((str) + 1) <= '8')))
#define pos_exp_ok(len, str) (len) > 0 && (len) <= 2
#else
#define FN_DBL_DIG DBL_DIG - 6
#define FN_MAX_EXP 22
#define FN_MIN_EXP -22
#define __exp_ok(len, str) \
    ((len) == 1) || \
    ((len) == 2 && (*(str) <= '1' || \
                    (*(str) == '2' && *((str) + 1) <= '2')))
#define neg_exp_ok(len, str) __exp_ok((len), (str))
#define pos_exp_ok(len, str) __exp_ok((len), (str))
#endif

/* Quickly detect INFINITY and NAN. */
#define _quick_detect_nf(start, len) \
    ((len) == 3 && \
     ((start)[1] == 'n' || (start)[1] == 'N') && \
     ((start)[2] == 'f' || (start)[2] == 'F'))
#define _quick_detect_nfinity(start, len) \
    ((len) == 8 && \
     ((start)[1] == 'n' || (start)[1] == 'N') && \
     ((start)[2] == 'f' || (start)[2] == 'F') && \
     ((start)[3] == 'i' || (start)[3] == 'I') && \
     ((start)[4] == 'n' || (start)[4] == 'N') && \
     ((start)[5] == 'i' || (start)[5] == 'I') && \
     ((start)[6] == 't' || (start)[6] == 'T') && \
     ((start)[7] == 'y' || (start)[7] == 'Y'))
#define quick_detect_infinity(start, len) \
    (((start)[0] == 'i' || (start)[0] == 'I') && \
     (_quick_detect_nf(start, len) || _quick_detect_nfinity(start, len)))
#define quick_detect_nan(start, len) \
    (((start)[0] == 'n' || (start)[0] == 'N') && \
     (len) == 3 && \
     ((start)[1] == 'a' || (start)[1] == 'A') && \
     ((start)[2] == 'n' || (start)[2] == 'N'))

/* Quickly detect if a string is an integer or float. */
#define is_likely_int(start, len) ((len) > 0 && is_valid_digit(start))
#define is_likely_float(start, len) \
    ((len) > 0 && \
     (is_valid_digit(start) || \
      (*(start) == '.' && is_valid_digit((start) + 1))) \
    )

/* Guess if an int or float will overflow. */
#define int_might_overflow(start, end) ((end) - (start)) > FN_MAX_INT_LEN

bool
float_might_overflow(const char *start, const Py_ssize_t len);


/* Declarations. */

bool
is_valid_digit_arbitrary_base(const char c, const int base);

long
parse_int(const char *str, const char *end, bool *error);

double
parse_float(const char *str, const char *end, bool *error);

bool
string_contains_float(const char *str, const char *end,
                      const bool allow_inf, const bool allow_nan);

bool
string_contains_intlike_float(const char *str, const char *end);

bool
string_contains_int(const char *str, const char *end, const int base);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FN_PARSING */
