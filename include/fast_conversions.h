#ifndef __FAST_CONVERSIONS
#define __FAST_CONVERSIONS

#include <Python.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

long
parse_integer_from_string (const char *str, bool *error, bool *overflow);

double
parse_float_from_string (const char *str, bool *error, bool *overflow);

bool
string_contains_float (const char *str, const bool allow_inf, const bool allow_nan);

bool
string_contains_intlike_float (const char *str);

bool
string_contains_integer (const char *str);

bool
string_contains_non_overflowing_float (const char *str);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FAST_CONVERSIONS */
