#ifndef FN_BOOL
#define FN_BOOL

#ifdef __cplusplus
extern "C" {
#else

/* If using library in C, use stdbool.h if available */
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else

#ifndef __GNUC__
/* _Bool builtin type is included in GCC */
/* ISO C Standard: 5.2.5 An object declared as 
type _Bool is large enough to store 
the values 0 and 1. */
/* We choose 8 bit to match C++ */
/* It must also promote to integer */
typedef int8_t _Bool;
#endif /* __GNUC__ */

/* ISO C Standard: 7.16 Boolean type */
#ifndef bool
#define bool _Bool
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined 1
#endif

#endif /* __STDC_VERSION__ >= 199901L */

#endif /* __cplusplus */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FN_BOOL */