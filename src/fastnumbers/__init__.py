from .fastnumbers import (
    ALLOWED,
    DISALLOWED,
    INPUT,
    NUMBER_ONLY,
    RAISE,
    STRING_ONLY,
    __version__,
    array as _array,
    check_float,
    check_int,
    check_intlike,
    check_real,
    fast_float,
    fast_forceint,
    fast_int,
    fast_real,
    float,
    int,
    isfloat,
    isint,
    isintlike,
    isreal,
    map_try_float,
    map_try_forceint,
    map_try_int,
    map_try_real,
    query_type,
    real,
    try_float,
    try_forceint,
    try_int,
    try_real,
)

try:
    import numpy
except ImportError:
    has_numpy = False
else:
    has_numpy = True
    _allowed_dtypes = {
        numpy.int8,
        numpy.uint8,
        numpy.int16,
        numpy.uint16,
        numpy.int32,
        numpy.uint32,
        numpy.int64,
        numpy.uint64,
        numpy.float32,
        numpy.float64,
    }


def try_array(input, output=None, *, dtype=None, **kwargs):
    """ """
    # If output is not provided, we construct a numpy array of the same length
    # as the input into which the C++ function can populate the output.
    if output is None:
        return_output = True

        # Construct a numpy ndarray of the appropriate length and
        # dtype to contain the output.
        if not has_numpy:
            raise RuntimeError(
                "To use fastnumbers.try_array without an explict "
                "output requires numpy to also be installed"
            )
        try:
            length = len(input)
        except TypeError:
            input = list(input)
            length = len(input)
        output = numpy.empty(length, dtype=dtype or numpy.float64)
    else:
        return_output = False

        # Let's be conservative about what we feed to the C++ code.
        try:
            if output.dtype.type not in _allowed_dtypes:
                raise TypeError(
                    "The only supported numpy dtypes for output are: "
                    + ", ".join(sorted([x.__name__ for x in _allowed_dtypes]))
                    + f" not {output.dtype.name}"
                )
        except AttributeError:
            if not hasattr(output, "typecode"):
                raise TypeError(
                    "Only numpy ndarray and array.array types for output are "
                    f"supported, not {type(output)}"
                )

    # Call the C++ extension
    _array(input, output, **kwargs)

    # If no output value was given on calling, we return the output as a return value.
    if return_output:
        return output


__all__ = [
    "ALLOWED",
    "DISALLOWED",
    "INPUT",
    "NUMBER_ONLY",
    "RAISE",
    "STRING_ONLY",
    "__version__",
    "check_float",
    "check_int",
    "check_intlike",
    "check_real",
    "fast_float",
    "fast_forceint",
    "fast_int",
    "fast_real",
    "float",
    "int",
    "isfloat",
    "isint",
    "isintlike",
    "isreal",
    "map_try_float",
    "map_try_forceint",
    "map_try_int",
    "map_try_real",
    "query_type",
    "real",
    "try_array",
    "try_float",
    "try_forceint",
    "try_int",
    "try_real",
]
