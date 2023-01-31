#pragma once

// Based on:
// https://github.com/numpy/numpy/blob/2ce11750cbdba4d81fa8b3b8856c5b40aee75d45/numpy/core/src/common/npy_argparse.h
// but edited to fastnumbers needs.

#include <Python.h>

/*
 * This file defines macros to help with keyword argument parsing.
 * This solves two issues as of now:
 *   1. Pythons C-API PyArg_* keyword argument parsers are slow, due to
 *      not caching the strings they use.
 *   2. It allows the use of METH_ARGPARSE (and `tp_vectorcall`)
 *      when available in Python, which removes a large chunk of overhead.
 *
 * Internally CPython achieves similar things by using a code generator
 * argument clinic.
 */

#define FN_MAX_KWARGS 15

typedef struct {
    int npositional;
    int nargs;
    int npositional_only;
    int nrequired;
    /* Null terminated list of keyword argument name strings */
    PyObject* kw_strings[FN_MAX_KWARGS + 1];
} _FNArgParserCache;

/*
 * The sole purpose of this macro is to hide the argument parsing cache.
 * Since this cache must be static, this also removes a source of error.
 */
#define FN_PREPARE_ARGPARSER static _FNArgParserCache __argparse_cache = { -1 }

/**
 * Macro to help with argument parsing.
 *
 * The pattern for using this macro is by defining the method as:
 *
 * @code
 * static PyObject *
 * my_method(PyObject *self,
 *           PyObject *const *args, Py_ssize_t len_args, PyObject *kwnames)
 * {
 *     FN_PREPARE_ARGPARSER;
 *
 *     PyObject *argument1, *argument3;
 *     bool argument2 = false;
 *     if (fn_parse_arguments("method", args, len_args, kwnames,
 *                "argument1", false, &argument1,
 *                "|argument2", true, &argument2,
 *                "$argument3", false, &argument3,
 *                nullptr, false, nullptr) < 0) {
 *          return nullptr;
 *      }
 * }
 * @endcode
 *
 * The `FN_PREPARE_ARGPARSER` macro sets up a static cache variable necessary
 * to hold data for speeding up the parsing. `fn_parse_arguments` must be
 * used in cunjunction with the macro defined in the same scope.
 * (No two `fn_parse_arguments` may share a single `FN_PREPARE_ARGPARSER`.)
 *
 * '|' means an optional parameter, and '$' is keyword-only.
 *
 * @param funcname
 * @param args Python passed args (METH_FASTCALL)
 * @param len_args Number of arguments (not flagged)
 * @param kwnames Tuple as passed by METH_FASTCALL or nullptr.
 * @param ... List of arguments must be param1_name, param2_converter,
 *            *param1_outvalue, param2_name, ..., nullptr, nullptr, nullptr.
 *            Where name is ``char *``, is a ``bool`` indicating if the return
 *            is a boolean or not, and ``outvalue`` is the
 *            ``PyObject *`` or ``bool` borrowed reference.
 *
 * @return Returns 0 on success and -1 on failure.
 */
int _fn_parse_arguments(
    const char* funcname,
    /* cache_ptr is a nullptr initialized persistent storage for data */
    _FNArgParserCache* cache_ptr,
    PyObject* const* args,
    Py_ssize_t len_args,
    PyObject* kwnames,
    /* va_list is nullptr, false, nullptr terminated: name, as_bool, value */
    ...
);

#define fn_parse_arguments(funcname, args, len_args, kwnames, ...)                      \
    _fn_parse_arguments(                                                                \
        funcname, &__argparse_cache, args, len_args, kwnames, __VA_ARGS__               \
    )
