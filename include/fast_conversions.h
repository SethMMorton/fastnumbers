#ifndef __FAST_CONVERSIONS
#define __FAST_CONVERSIONS

#include <Python.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

long fast_atoi(const char *str, bool *error, bool *overflow);
double fast_atof (const char *str, bool *error, bool *overflow);
bool fast_atof_test (const char *str, const bool allow_inf, const bool allow_nan);
bool fast_atoi_test (const char *str);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __FAST_CONVERSIONS */
