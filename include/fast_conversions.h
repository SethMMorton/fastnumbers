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
int fast_atoi(char *c, bool *error);
double fast_atof (char *p, bool *error);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FAST_CONVERSIONS */