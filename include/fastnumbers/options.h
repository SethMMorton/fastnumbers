#ifndef FN_OPTIONS
#define FN_OPTIONS

#include <Python.h>
#include <limits.h>
#include "fastnumbers/fn_bool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Selector for the type of number to check/convert. */
typedef enum PyNumberType { REAL, FLOAT, INT, INTLIKE, FORCEINT } PyNumberType;

/* This struct holds all the user options.
 * Makes adding new future options easier to manage.
 */
typedef struct Options {
    PyObject *retval;
    PyObject *input;
    PyObject *key;
    PyObject *handle_inf;
    PyObject *handle_nan;
    int coerce;             /* use int instead of bool because   */
    int num_only;           /* PyArg_ParseTupleAndKeywords       */
    int str_only;           /* does not support 'bool' type with */
    int allow_underscores;  /* the "p" converter.                */
    bool allow_uni;
    int base;
} Options;

/* Convenience for initializing.
 * Older MSVC does not like designated initializers.
 */
#define init_Options_convert {         \
        /*.retval =*/ NULL,            \
        /*.input =*/ NULL,             \
        /*.key =*/ NULL,               \
        /*.handle_inf =*/ NULL,        \
        /*.handle_nan =*/ NULL,        \
        /*.coerce =*/ true,            \
        /*.num_only =*/ false,         \
        /*.str_only =*/ false,         \
        /* allow_underscores =*/ true, \
        /*.allow_uni =*/ true,         \
        /*.base =*/ INT_MIN,           \
    }
#define init_Options_check {           \
        /*.retval =*/ Py_None,         \
        /*.input =*/ NULL,             \
        /*.key =*/ NULL,               \
        /*.handle_inf =*/ Py_False,    \
        /*.handle_nan =*/ Py_False,    \
        /*.coerce =*/ true,            \
        /*.num_only =*/ false,         \
        /*.str_only =*/ false,         \
        /* allow_underscores =*/ true, \
        /*.allow_uni =*/ true,         \
        /*.base =*/ INT_MIN,           \
    }

/* Some query MACROs. Each expects a pointer. */
#define Options_Coerce_True(o) ((o)->coerce)
#define Options_Has_NaN_Sub(o) ((o)->handle_nan != NULL)
#define Options_Has_INF_Sub(o) ((o)->handle_inf != NULL)
#define Options_Return_NaN_Sub(o) (Py_INCREF((o)->handle_nan), (o)->handle_nan)
#define Options_Return_INF_Sub(o) (Py_INCREF((o)->handle_inf), (o)->handle_inf)
#define Options_Should_Raise(o) ((o)->retval == NULL)
#define Options_Default_Base(o) ((o)->base == INT_MIN)
#define Options_Allow_UnicodeCharacter(o) (o)->allow_uni
#define Options_Allow_Infinity(o) PyObject_IsTrue((o)->handle_inf)
#define Options_Allow_NAN(o) PyObject_IsTrue((o)->handle_nan)
#define Options_Allow_Underscores(o) ((o)->allow_underscores)
#define Options_String_Only(o) ((o)->str_only)
#define Options_Number_Only(o) ((o)->num_only)

/* Set allow unicode. */
#define Options_Set_Disallow_UnicodeCharacter(o) \
    do {                        \
        (o)->retval = NULL;     \
        (o)->allow_uni = false; \
    } while (0)

/* MACRO to return the correct result based on user-input.
 * Expects the Options struct as a pointer.
 */
#define Options_Return_Correct_Result_On_Error(o)                 \
    (Options_Should_Raise(o) ?                                    \
     NULL :                                                       \
     ((o)->key != NULL ?                                          \
      PyObject_CallFunctionObjArgs((o)->key, (o)->retval, NULL) : \
      (Py_INCREF((o)->retval), (o)->retval)                       \
     ))

/* MACRO to set the correct return value based on given input.
 * Expects the Options struct NOT as a pointer.
 */
#define Options_Set_Return_Value(o, input, default_value, raise) \
    (o).input = (input); \
    (o).retval = (raise) \
                 ? NULL \
                 : (((o).key != NULL || default_value == NULL) ? input : default_value)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FN_OPTIONS */