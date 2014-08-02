#ifndef DOCSTRINGS
#define DOCSTRINGS

/*
 * Docstrings for the fastnumbers functions.
 *
 * Author: Seth M. Morton, August 2, 2014
 */


const char module_docstring[] = "Quickly convert strings to numbers.";


const char safe_real_docstring[] = 
"Convert input to an *int* or *float* if possible, return the input otherwise.\n"
"\n"
"Convert input to an *int* or *float* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str*, *int*, or *float*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def safe_real(input):\n"
"    ...     try:\n"
"    ...         a = float(input)\n"
"    ...     except ValueError:\n"
"    ...         return input\n"
"    ...     else:\n"
"    ...         b = int(a)\n"
"    ...         return b if a == b else b\n"
"    ...\n"
"\n"
"The main difference is that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data.\n"
"\n";


const char safe_float_docstring[] = 
"Convert input to a *float* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *float* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str* or *float*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def safe_float(input):\n"
"    ...    try:\n"
"    ...        return float(input)\n"
"    ...    except ValueError:\n"
"    ...        return input\n"
"    ...\n"
"\n"
"The main difference is that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data.\n"
"\n";


const char safe_int_docstring[] = 
"Convert input to a *int* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *int* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid string input, although\n"
"a TypeError error will be raised for types the *int* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str* or *int* (or *long* on Python2 if the input was *long*).\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def safe_int(input):\n"
"    ...    try:\n"
"    ...        return int(input)\n"
"    ...    except ValueError:\n"
"    ...        return input\n"
"    ...\n"
"\n"
"The main difference is that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data.\n"
"\n";


const char safe_forceint_docstring[] = 
"Convert input to an *int* if possible (even '46.52'), return the input otherwise.\n"
"\n"
"Convert input to an *int* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  If the input is a string of a *float*\n"
"(like '45.64'), the number will first be converted to a float, then to an\n"
"*int*.  The return value is guaranteed to be of type *str* or *int*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def safe_forceint(input):\n"
"    ...     try:\n"
"    ...         return int(input)\n"
"    ...     except ValueError:\n"
"    ...         try:\n"
"    ...             return int(float(input))\n"
"    ...         except ValueError:\n"
"    ...             return input\n"
"    ...\n"
"\n"
"The main difference is that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data.\n"
"\n";


const char fast_real_docstring[] = 
"Convert input to an *int* or *float* if possible, return the input otherwise.\n"
"\n"
"Convert input to an *int* or *float* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str*, *int*, or *float*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def safe_real(input):\n"
"    ...     try:\n"
"    ...         a = float(input)\n"
"    ...     except ValueError:\n"
"    ...         return input\n"
"    ...     else:\n"
"    ...         b = int(a)\n"
"    ...         return b if a == b else b\n"
"    ...\n"
"\n"
"The main differences are that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data, and a fast implementation of *atof* is used to\n"
"convert strings to a *float* which for large exponents may be inaccurate\n"
"in the 12th decimal place (you should be mindful of this when converting\n"
"very large or small numbers, especially if a given string integer is\n"
"larger than can be contained in the C-type 'long'.)"
"\n";


const char fast_float_docstring[] = 
"Convert a string to a *float* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *float* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str* or *float*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def fast_float(input):\n"
"    ...    try:\n"
"    ...        return float(input)\n"
"    ...    except ValueError:\n"
"    ...        return input\n"
"    ...\n"
"\n"
"The main differences are that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data, and a fast implementation of *atof* is used to\n"
"convert strings to a *float* which for large exponents may be inaccurate\n"
"in the 12th decimal place (you should be mindful of this when converting\n"
"very large or small numbers).\n"
"\n";


const char fast_int_docstring[] = 
"Convert a string to a *int* if possible, return the input otherwise.\n"
"\n"
"Convert input to a *int* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid string input, although\n"
"a TypeError error will be raised for types the *int* function also does\n"
"not accept, like a *list*.  The return value is guaranteed to be of\n"
"type *str*, or *int* (or *long* on Python2 if the input was *long*).\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def fast_int(input):\n"
"    ...    try:\n"
"    ...        return int(input)\n"
"    ...    except ValueError:\n"
"    ...        return input\n"
"    ...\n"
"\n"
"The main differences are that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data, and a fast implementation of *atoi* is used to\n"
"convert strings to an *int* which does not check for overflow/underflow\n"
"(you should be mindful of this when converting very large or small\n"
"numbers).\n"
"\n";


const char fast_forceint_docstring[] = 
"Convert input to an *int* if possible (even '46.52'), return the input otherwise.\n"
"\n"
"Convert input to an *int* if possible, return unchanged if not\n"
"possible; no ValueError will be raised for invalid input, although\n"
"a TypeError error will be raised for types the *float* function also does\n"
"not accept, like a *list*.  If the input is a string of a *float*\n"
"(like '45.64'), the number will first be converted to a float, then to an\n"
"*int*.  The return value is guaranteed to be of type *str* or *int*.\n"
"\n"
"It is roughly equivalent to\n"
"\n"
"    >>> def fast_forceint(input):\n"
"    ...     try:\n"
"    ...         return int(input)\n"
"    ...     except ValueError:\n"
"    ...         try:\n"
"    ...             return int(float(input))\n"
"    ...         except ValueError:\n"
"    ...             return input\n"
"    ...\n"
"\n"
"The main differences are that much of Python's exception mechanism is\n"
"bypassed so that this will be more efficient for attempted conversions\n"
"on large sets of data, and a fast implementation of *atof* is used to\n"
"convert strings to a *float* which for large exponents may be inaccurate\n"
"in the 12th decimal place (you should be mindful of this when converting\n"
"very large or small numbers, especially if a given string integer is\n"
"larger than can be contained in the C-type 'long'.)"
"\n";


#endif /* DOCSTRINGS */