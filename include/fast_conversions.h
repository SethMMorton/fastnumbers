#ifndef FAST_CONVERSIONS
#define FAST_CONVERSIONS

#include <Python.h>
#include "fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

long fast_atoi(const char *c, bool *error, bool *overflow);
double fast_atof (const char *p, bool *error, bool *overflow);
bool fast_atof_test (const char *p, const bool allow_inf, const bool allow_nan);
bool fast_atoi_test (const char *p);
long fast_utoi(const Py_UNICODE *c);
double fast_utof (const Py_UNICODE *p);
bool fast_utof_test (const Py_UNICODE *p);
bool fast_utoi_test (const Py_UNICODE *p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FAST_CONVERSIONS */