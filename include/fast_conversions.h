#ifndef FAST_CONVERSIONS
#define FAST_CONVERSIONS

#ifdef __cplusplus
extern "C" {
#else
/* If using library in C, use stdbool.h if available */
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
typedef int bool;
const bool false = 0;
const bool true  = 1;
#endif
#endif

/* The actual declarations */
long fast_atoi(char *c, bool *error);
double fast_atof (char *p, bool *error);
bool fast_atof_test (char *p);
bool fast_atoi_test (char *p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FAST_CONVERSIONS */