#ifndef FN_OPTIONS
#define FN_OPTIONS

#include <Python.h>
#include <limits.h>

/* This struct holds all the user options.
 * Makes adding new future options easier to manage.
 */
struct Options {
    PyObject* retval;
    PyObject* input;
    PyObject* key;
    PyObject* handle_inf;
    PyObject* handle_nan;
    PyObject* coerce;
    PyObject* num_only;
    PyObject* str_only;
    int base;
};

/* Convenience for initializing. */
#define init_Options_convert { .retval = NULL,     \
                               .input = NULL,      \
                               .key = NULL,        \
                               .handle_inf = NULL, \
                               .handle_nan = NULL, \
                               .coerce = Py_True,  \
                               .num_only = NULL,   \
                               .str_only = NULL,   \
                               .base = INT_MIN,    \
                               }
#define init_Options_check { .retval = NULL,         \
                             .input = NULL,          \
                             .key = NULL,            \
                             .handle_inf = Py_False, \
                             .handle_nan = Py_False, \
                             .coerce = NULL,         \
                             .num_only = Py_False,   \
                             .str_only = Py_False,   \
                             .base = INT_MIN,        \
                             }

/* Some query MACROs. Each expects a pointer. */
#define Options_Coerce_True(o) PyObject_IsTrue((o)->coerce)
#define Options_Has_NaN_Sub(o) (o)->handle_nan != NULL
#define Options_Has_INF_Sub(o) (o)->handle_inf != NULL
#define Options_Return_NaN_Sub(o) (Py_INCREF((o)->handle_nan), (o)->handle_nan)
#define Options_Return_INF_Sub(o) (Py_INCREF((o)->handle_inf), (o)->handle_inf)
#define Options_Should_Raise(o) ((o)->retval == NULL)
#define Options_Default_Base(o) ((o)->base == INT_MIN)

/* MACRO to return the correct result based on user-input.
 * Expects the Options struct as a pointer.
 */
#define Options_Return_Correct_Result_On_Error(o)                       \
    (Options_Should_Raise(o) ?                                          \
        NULL :                                                          \
        ((o)->key != NULL ?                                             \
            PyObject_CallFunctionObjArgs((o)->key, (o)->retval, NULL) : \
            (Py_INCREF((o)->retval), (o)->retval)                       \
         ))

/* MACRO to set the correct return value based on given input.
 * Expects the Options struct NOT as a pointer.
 */
#define Options_Set_Return_Value(o, input, default_value, raise) \
    (o).input = input; \
    (o).retval = PyObject_IsTrue(raise) \
               ? NULL                              \
               : (((o).key != NULL || default_value == NULL) ? input : default_value)

#endif /* FN_OPTIONS */