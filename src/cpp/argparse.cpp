// Based on:
// https://github.com/numpy/numpy/blob/2ce11750cbdba4d81fa8b3b8856c5b40aee75d45/numpy/core/src/common/npy_argparse.c
// but edited to fastnumbers needs.

#include "fastnumbers/argparse.hpp"
#include <Python.h>

typedef int convert(PyObject*, void*);

/**
 * Internal function to initialize keyword argument parsing.
 *
 * This does a few simple jobs:
 *
 * * Check the input for consistency to find coding errors, for example
 *   a parameter not marked with | after one marked with | (optional).
 * 2. Find the number of positional-only arguments, the number of
 *    total, required, and keyword arguments.
 * 3. Intern all keyword arguments strings to allow fast, identity based
 *    parsing and avoid string creation overhead on each call.
 *
 * @param funcname Name of the function, mainly used for errors.
 * @param cache A cache object stored statically in the parsing function
 * @param va_orig Argument list to npy_parse_arguments
 * @return 0 on success, -1 on failure
 */
static int
initialize_keywords(const char* funcname, _FNArgParserCache* cache, va_list va_orig)
{
    va_list va;
    int nargs = 0;
    int nkwargs = 0;
    int npositional_only = 0;
    int nrequired = 0;
    int npositional = 0;
    char state = '\0';

    va_copy(va, va_orig);
    while (1) {
        /* Count length first: */
        char* name = va_arg(va, char*);
        bool as_bool = va_arg(va, int);
        void* data = va_arg(va, void*);

        /* Check if this is the sentinel */
        if ((name == NULL) && (!as_bool) && (data == NULL)) {
            break;
        }

        if (name == NULL) {
            PyErr_Format(
                PyExc_SystemError,
                "fastnumbers internal error: name is NULL in %s() at "
                "argument %d.",
                funcname,
                nargs
            );
            va_end(va);
            return -1;
        }
        if (data == NULL) {
            PyErr_Format(
                PyExc_SystemError,
                "fastnumbers internal error: data is NULL in %s() at "
                "argument %d.",
                funcname,
                nargs
            );
            va_end(va);
            return -1;
        }

        nargs += 1;
        if (*name == '|') {
            if (state == '$') {
                PyErr_Format(
                    PyExc_SystemError,
                    "fastnumbers internal error: positional argument `|` "
                    "after keyword only `$` one to %s() at argument %d.",
                    funcname,
                    nargs
                );
                va_end(va);
                return -1;
            }
            state = '|';
            name++; /* advance to actual name. */
            npositional += 1;
        } else if (*name == '$') {
            state = '$';
            name++; /* advance to actual name. */
        } else {
            if (state != '\0') {
                PyErr_Format(
                    PyExc_SystemError,
                    "fastnumbers internal error: non-required argument after "
                    "required | or $ one to %s() at argument %d.",
                    funcname,
                    nargs
                );
                va_end(va);
                return -1;
            }

            nrequired += 1;
            npositional += 1;
        }

        if (*name == '\0') {
            /* Empty string signals positional only */
            if (state == '$') {
                PyErr_Format(
                    PyExc_SystemError,
                    "fastnumbers internal error: non-kwarg marked with or $ "
                    "to %s() at argument %d.",
                    funcname,
                    nargs
                );
                va_end(va);
                return -1;
            }
            npositional_only += 1;
        } else {
            nkwargs += 1;
        }
    }
    va_end(va);

    if (npositional == -1) {
        npositional = nargs;
    }

    if (nargs > FN_MAX_KWARGS) {
        PyErr_Format(
            PyExc_SystemError,
            "fastnumbers internal error: function %s() has %d arguments, but "
            "the maximum is currently limited to %d for easier parsing; "
            "it can be increased by modifying `FN_MAX_KWARGS`.",
            funcname,
            nargs,
            FN_MAX_KWARGS
        );
        return -1;
    }

    /*
     * Do any necessary string allocation and interning,
     * creating a caching object.
     */
    cache->nargs = nargs;
    cache->npositional_only = npositional_only;
    cache->npositional = npositional;
    cache->nrequired = nrequired;

    /* NULL kw_strings for easier cleanup (and NULL termination) */
    memset(cache->kw_strings, 0, sizeof(PyObject*) * (nkwargs + 1));

    va_copy(va, va_orig);
    for (int i = 0; i < nargs; i++) {
        /* Advance through non-kwargs, which do not require setup. */
        char* name = va_arg(va, char*);
        va_arg(va, int);
        va_arg(va, void*);

        if (*name == '|' || *name == '$') {
            name++; /* ignore | and $ */
        }
        if (i >= npositional_only) {
            int i_kwarg = i - npositional_only;
            cache->kw_strings[i_kwarg] = PyUnicode_InternFromString(name);
            if (cache->kw_strings[i_kwarg] == NULL) {
                va_end(va);
                goto error;
            }
        }
    }

    va_end(va);
    return 0;

error:
    for (int i = 0; i < nkwargs; i++) {
        Py_XDECREF(cache->kw_strings[i]);
    }
    cache->npositional = -1; /* not initialized */
    return -1;
}

static int raise_incorrect_number_of_positional_args(
    const char* funcname, const _FNArgParserCache* cache, Py_ssize_t len_args
)
{
    if (cache->npositional == cache->nrequired) {
        PyErr_Format(
            PyExc_TypeError,
            "%s() takes %d positional arguments but %zd were given",
            funcname,
            cache->npositional,
            len_args
        );
    } else {
        PyErr_Format(
            PyExc_TypeError,
            "%s() takes from %d to %d positional arguments but "
            "%zd were given",
            funcname,
            cache->nrequired,
            cache->npositional,
            len_args
        );
    }
    return -1;
}

static void
raise_missing_argument(const char* funcname, const _FNArgParserCache* cache, int i)
{
    if (i < cache->npositional_only) {
        PyErr_Format(
            PyExc_TypeError, "%s() missing required positional argument %d", funcname, i
        );
    } else {
        PyObject* kw = cache->kw_strings[i - cache->npositional_only];
        PyErr_Format(
            PyExc_TypeError,
            "%s() missing required argument '%S' (pos %d)",
            funcname,
            kw,
            i
        );
    }
}

/**
 * Generic helper for argument parsing
 *
 * See macro version for an example pattern of how to use this function.
 *
 * @param funcname
 * @param cache
 * @param args Python passed args (METH_FASTCALL)
 * @param len_args
 * @param kwnames
 * @param ... List of arguments (see macro version).
 *
 * @return Returns 0 on success and -1 on failure.
 */
int _fn_parse_arguments(
    const char* funcname,
    /* cache_ptr is a NULL initialized persistent storage for data */
    _FNArgParserCache* cache,
    PyObject* const* args,
    Py_ssize_t len_args,
    PyObject* kwnames,
    /* ... is NULL, false, NULL terminated: name, as_bool, value */
    ...
)
{
    if (cache->npositional == -1) {
        va_list va;
        va_start(va, kwnames);

        int res = initialize_keywords(funcname, cache, va);
        va_end(va);
        if (res < 0) {
            return -1;
        }
    }

    if (len_args > cache->npositional) {
        return raise_incorrect_number_of_positional_args(funcname, cache, len_args);
    }

    /* NOTE: Could remove the limit but too many kwargs are slow anyway. */
    PyObject* all_arguments[FN_MAX_KWARGS];

    for (Py_ssize_t i = 0; i < len_args; i++) {
        all_arguments[i] = args[i];
    }

    /* Without kwargs, do not iterate all converters. */
    int max_nargs = (int)len_args;
    Py_ssize_t len_kwargs = 0;

    /* If there are any kwargs, first handle them */
    if (kwnames != NULL) {
        len_kwargs = PyTuple_GET_SIZE(kwnames);
        max_nargs = cache->nargs;

        for (int i = len_args; i < cache->nargs; i++) {
            all_arguments[i] = NULL;
        }

        for (Py_ssize_t i = 0; i < len_kwargs; i++) {
            PyObject* key = PyTuple_GET_ITEM(kwnames, i);
            PyObject* value = args[i + len_args];
            PyObject* const* name;

            /* Super-fast path, check identity: */
            for (name = cache->kw_strings; *name != NULL; name++) {
                if (*name == key) {
                    break;
                }
            }
            if (*name == NULL) {
                /* Slow fallback, if identity checks failed for some reason */
                for (name = cache->kw_strings; *name != NULL; name++) {
                    int eq = PyObject_RichCompareBool(*name, key, Py_EQ);
                    if (eq == -1) {
                        return -1;
                    } else if (eq) {
                        break;
                    }
                }
                if (*name == NULL) {
                    /* Invalid keyword argument. */
                    PyErr_Format(
                        PyExc_TypeError,
                        "%s() got an unexpected keyword argument '%S'",
                        funcname,
                        key
                    );
                    return -1;
                }
            }

            Py_ssize_t param_pos
                = ((name - cache->kw_strings) + cache->npositional_only);

            /* There could be an identical positional argument */
            if (all_arguments[param_pos] != NULL) {
                PyErr_Format(
                    PyExc_TypeError,
                    "argument for %s() given by name ('%S') and position "
                    "(position %zd)",
                    funcname,
                    key,
                    param_pos
                );
                return -1;
            }

            all_arguments[param_pos] = value;
        }
    }

    /*
     * There cannot be too many args, too many kwargs would find an
     * incorrect one above.
     */
    assert(len_args + len_kwargs <= cache->nargs);

    /* At this time `all_arguments` holds either NULLs or the objects */
    va_list va;
    va_start(va, kwnames);

    for (int i = 0; i < max_nargs; i++) {
        va_arg(va, char*);
        bool as_bool = va_arg(va, int);
        void* data = va_arg(va, void*);

        if (all_arguments[i] == NULL) {
            continue;
        }

        if (as_bool) {
            *((bool*)data) = PyObject_IsTrue(all_arguments[i]);
        } else {
            *((PyObject**)data) = all_arguments[i];
        }
    }

    /* Required arguments are typically not passed as keyword arguments */
    if (len_args < cache->nrequired) {
        /* (PyArg_* also does this after the actual parsing is finished) */
        if (max_nargs < cache->nrequired) {
            raise_missing_argument(funcname, cache, max_nargs);
            goto converting_failed;
        }
        for (int i = 0; i < cache->nrequired; i++) {
            if (all_arguments[i] == NULL) {
                raise_missing_argument(funcname, cache, i);
                goto converting_failed;
            }
        }
    }

    va_end(va);
    return 0;

converting_failed:
    va_end(va);
    return -1;
}
