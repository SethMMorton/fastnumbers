#! /usr/bin/env python
"""Profile the performance of fastnumbers."""

from __future__ import annotations

import copy
import decimal
import gc
import math
import re
import sys
import timeit

import numpy as np

import fastnumbers

# If given an file path as an argument, cause print to write
# to stdout and it.
try:
    outloc = sys.argv[1]
except IndexError:
    pass
else:

    class Tee:
        """Redirect an output stream to both file and the stream."""

        def __init__(self, stream, filepath):
            """Initialize."""
            self.stream = stream
            self.fo = open(filepath, "w")  # noqa: SIM115, PTH123

        def write(self, data):
            """Write the data to the streams."""
            self.stream.write(data)
            self.stream.flush()
            self.fo.write(data)

        def flush(self):
            """Flush the buffer."""
            self.stream.flush()
            self.fo.flush()

        def close(self):
            """Close the open file object."""
            self.fo.close()

        def __del__(self):
            """Close the data on instance deletion."""
            self.close()

    sys.stdout = Tee(sys.stdout, outloc)


class Timer:
    """Class to time functions and make pretty tables of the output."""

    # This is a list of all the things we will time with an associated label.
    THINGS_TO_TIME = (
        ("not_a_number", "Non-number String"),
        ("-4", "Small Int String"),
        ("-41053", "Int String"),
        ("358924829458", "Medium Int String"),
        ("35892482945872302493947939485729", "Large Int String"),
        ("-4.1", "Small Float String"),
        ("-41053.543034e34", "Float String"),
        ("-41053.543028758302e256", "Large Float String"),
        (-41053, "Int"),
        (-41053.543028758302e100, "Float"),
    )

    def __init__(self, title):
        """Initialize."""
        print("### " + title)
        print()
        self.functions = []

    def add_function(self, func, label, *, setup="pass", iterable=False):
        """Add a function to be timed and compared."""
        self.functions.append((func, setup, label, iterable))

    def time_functions(self, *, repeat=5):
        """Time all the given functions against all input then display results."""
        # Collect the function labels to make the header of this table.
        # Show that the units are seconds for each.
        function_labels = [label + " (ms)" for _, _, label, _ in self.functions]

        # Construct the table strings, formatted in Markdown.
        # Store each line as a string element in a list.
        # This portion here is the table header only for now.
        table = Table()
        table.add_header("Input type", *function_labels)

        # For each value, time each function and collect the results.
        for value, value_label in self.THINGS_TO_TIME:
            row = []
            for func, setup, _, iterable in self.functions:
                if iterable:
                    setup += f"; iterable = [{value!r}] * 50"  # noqa: PLW2901
                    call = f"{func}(iterable)"
                else:
                    call = f"{func}({value!r})"
                try:
                    row.append(self._timeit(call, setup, repeat))
                except (ValueError, TypeError):
                    # We might send in some invalid input accidentally.
                    # Ignore those inputs.
                    break

            # Only add this row if the for loop quit without break.
            else:
                # Convert to milliseconds
                row = [(mean * 1000, stddev * 1000) for mean, stddev in row]
                # Make the lowest value bold.
                min_indx = min(enumerate(row), key=lambda x: x[1])[0]
                row = ["{:.3f} ± {:.3f}".format(*x) for x in row]
                row[min_indx] = self.bold(row[min_indx])
                table.add_row(value_label, *row)

        # Show the results in a table.
        print(str(table))
        print()
        gc.collect()

    @staticmethod
    def mean(x):
        """Compute the mean of the measured times."""
        return math.fsum(x) / len(x)

    @staticmethod
    def stddev(x):
        """Compute the standard deviation of the measured times."""
        mean = Timer.mean(x)
        sum_of_squares = math.fsum((v - mean) ** 2 for v in x)
        return math.sqrt(sum_of_squares / (len(x) - 1))

    @staticmethod
    def bold(x):
        """Make text bold."""
        return f"**{x}**"

    def _timeit(self, call, setup, repeat=5):
        """Perform the actual timing and return a formatted string of the runtime."""
        result = timeit.repeat(call, setup, number=100000, repeat=repeat)
        return self.mean(result), self.stddev(result)


class Table(list):
    """List of strings that can be made into a Markdown table."""

    def add_row(self, *elements):
        """Insert a row into the table."""
        self.append(list(elements))

    def add_header(self, *elements):
        """Insert a row into the table."""
        self.add_row(*elements)

    def __str__(self):
        """Convert the table into a markdown string."""
        header = copy.deepcopy(self[0])
        rows = copy.deepcopy(self[1:])

        # We want to center each number on the ± character.
        # To do this, we have to partition each element on that character,
        # find the longest vales in each column, then pad so that all in the
        # column match that, then replace in the column.
        for i in range(1, len(rows[0])):
            column = [row[i] for row in rows]
            partitioned_column = [x.partition("±") for x in column]
            left_max = len(max(partitioned_column, key=lambda x: len(x[0]))[0])
            right_max = len(max(partitioned_column, key=lambda x: len(x[2]))[2])
            column = [
                (x[0].rjust(left_max), x[1], x[2].ljust(right_max))
                for x in partitioned_column
            ]
            for j, elem in enumerate(column):
                rows[j][i] = "".join(elem)

        # Now, we need to include the headers and pad each column.
        # We will pad left headers right for rows.
        for i in range(len(header)):
            longest = len(max([header[i], *[x[i] for x in rows]], key=len))
            header[i] = header[i].ljust(longest)
            for j, row in enumerate(rows):
                rows[j][i] = row[i].rjust(longest)

        # Finally, add the header/row spacer
        spacer = ["-" * (len(x) - 1) + ":" for x in header]

        # Format!
        out = [("| " + " | ".join(x) + " |") for x in [header, spacer, *rows]]
        return "\n".join(out)


def int_re(x, int_match=re.compile(r"[-+]?\d+$").match):
    """Simulate fast_int but with regular expressions."""
    try:
        return int(x) if int_match(x) else x
    except TypeError:
        return int(x)


def int_try(x):
    """Simulate fast_int but with try/except."""
    try:
        return int(x)
    except ValueError:
        return x


def float_re(x, float_match=re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$").match):
    """Simulate fast_float but with regular expressions."""
    try:
        return float(x) if float_match(x) else x
    except TypeError:
        return float(x)


def float_try(x):
    """Simulate fast_float but with try/except."""
    try:
        return float(x)
    except ValueError:
        return x


def real_re(
    x,
    int_match=re.compile(r"[-+]?\d+$").match,
    real_match=re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$").match,
):
    """Simulate fast_real but with regular expressions."""
    try:
        return int(x) if int_match(x) else float(x) if real_match(x) else x
    except TypeError:
        if type(x) in (float, int):
            return x
        raise


def real_try(x):
    """Simulate fast_real but with try/except."""
    try:
        a = float(x)
    except ValueError:
        return x
    else:
        b = int(a)
        return b if a == b else b


def forceint_re(
    x,
    int_match=re.compile(r"[-+]\d+$").match,
    float_match=re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$").match,
):
    """Simulate fast_forceint but with regular expressions."""
    try:
        return int(x) if int_match(x) else int(float(x)) if float_match(x) else x
    except TypeError:
        return int(x)


def forceint_try(x):
    """Simulate fast_forceint but with try/except."""
    try:
        return int(x)
    except ValueError:
        try:
            return int(float(x))
        except ValueError:
            return x


if sys.version_info >= (3, 9):

    def forceint_denoise(
        x, _decimal=decimal.Decimal, ceil=math.ceil, log10=math.log10, ulp=math.ulp
    ):
        """Noiselessly convert a float to an integer."""
        try:
            # Integer method
            int_val = int(x)
            if x == int_val:
                return int_val
            double_digits = ceil(log10(ulp(abs(x))))
            return round(int_val, -int(double_digits))
        except (TypeError, ValueError):
            # String method
            try:
                return int(_decimal(x))
            except decimal.InvalidOperation as e:
                raise TypeError from e

    def forceint_denoise_fn(x, try_forceint=fastnumbers.try_forceint):
        """Noiselessly convert a float to an integer using fastnumbers."""
        return try_forceint(x, denoise=True)


def check_int_re(x, int_match=re.compile(r"[-+]?\d+$").match):
    """Simulate check_int but with regular expressions."""
    t = type(x)
    return t is int if t in (float, int) else bool(int_match(x))


def check_int_try(x):
    """Simulate check_int but with try/except."""
    try:
        int(x)
    except ValueError:
        return False
    else:
        return type(x) is not float


def check_float_re(
    x, float_match=re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$").match
):
    """Simulate check_float but with regular expressions."""
    t = type(x)
    return t is float if t in (float, int) else bool(float_match(x))


def check_float_try(x):
    """Simulate check_float but with try/except."""
    try:
        float(x)
    except ValueError:
        return False
    else:
        return type(x) is not int


def check_real_re(x, real_match=re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$").match):
    """Simulate check_real but with regular expressions."""
    return type(x) in (float, int) or bool(real_match(x))


def check_real_try(x):
    """Simulate check_real but with try/except."""
    try:
        float(x)
    except ValueError:
        return False
    else:
        return True


def check_intlike_re(
    x,
    int_match=re.compile(r"[-+]?\d+$").match,
    float_match=re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?$").match,
):
    """Simulate check_intlike but with regular expressions."""
    try:
        return bool(int_match(x)) or (
            float(x).is_integer() if float_match(x) else False
        )
    except TypeError:
        return int(x) == x


def check_intlike_try(x):
    """Simulate check_intlike but with try/except."""
    try:
        a = int(x)
    except ValueError:
        try:
            a = float(x)
        except ValueError:
            return False
        else:
            return a.is_integer()
    else:
        return a == float(x)


def fn_listcomp(iterable, func=fastnumbers.try_float):
    """Execute the function in a list comprehension."""
    return [func(x) for x in iterable]


def fn_map(iterable, func=fastnumbers.try_float):
    """Execute the function with map."""
    return list(map(func, iterable))


def fn_map_option(iterable, func=fastnumbers.try_float):
    """Execute the function with the map option."""
    return func(iterable, map=list)


def fn_map_iter_option(iterable, func=fastnumbers.try_float):
    """Execute the function with the iter option."""
    return list(func(iterable, map=True))


def fn_then_array(iterable, func=fastnumbers.try_float):
    """Execute the function as an array."""
    return np.array(func(iterable, map=list), dtype=np.float64)


output = np.empty(50, dtype=np.float64)


def fn_into_array(iterable, func=fastnumbers.try_array, out=output):
    """Execute the function into an existing array."""
    func(iterable, out)


print(sys.version_info)
print()

timer = Timer("Timing comparison of `int` functions")
timer.add_function("int", "builtin")
timer.add_function("int", "fastnumbers", "from fastnumbers import int")
timer.time_functions(repeat=100)

timer = Timer("Timing comparison of `float` functions")
timer.add_function("float", "builtin")
timer.add_function("float", "fastnumbers", "from fastnumbers import float")
timer.time_functions(repeat=100)

timer = Timer("Timing comparison of `int` functions with error handling")
timer.add_function("int_try", "try/except", "from __main__ import int_try")
timer.add_function("int_re", "regex", "from __main__ import int_re")
timer.add_function("try_int", "fastnumbers", "from fastnumbers import try_int")
timer.time_functions()

timer = Timer("Timing comparison of `float` functions with error handling")
timer.add_function("float_try", "try/except", "from __main__ import float_try")
timer.add_function("float_re", "regex", "from __main__ import float_re")
timer.add_function("try_float", "fastnumbers", "from fastnumbers import try_float")
timer.time_functions()

timer = Timer(
    "Timing comparison of `float` (but coerce to `int` if possible) "
    "functions with error handling"
)
timer.add_function("real_try", "try/except", "from __main__ import real_try")
timer.add_function("real_re", "regex", "from __main__ import real_re")
timer.add_function("try_real", "fastnumbers", "from fastnumbers import try_real")
timer.time_functions()

timer = Timer("Timing comparison of forced `int` functions with error handling")
timer.add_function("forceint_try", "try/except", "from __main__ import forceint_try")
timer.add_function("forceint_re", "regex", "from __main__ import forceint_re")
timer.add_function(
    "try_forceint", "fastnumbers", "from fastnumbers import try_forceint"
)
timer.time_functions()

if sys.version_info >= (3, 9):
    timer = Timer(
        "Timing comparison of forced `int` functions with error handling "
        "and denoising"
    )
    timer.add_function(
        "forceint_denoise", "Python", "from __main__ import forceint_denoise"
    )
    timer.add_function(
        "forceint_denoise_fn", "fastnumbers", "from __main__ import forceint_denoise_fn"
    )
    timer.time_functions()

timer = Timer("Timing comparison to check if value can be converted to `int`")
timer.add_function("check_int_try", "try/except", "from __main__ import check_int_try")
timer.add_function("check_int_re", "regex", "from __main__ import check_int_re")
timer.add_function("check_int", "fastnumbers", "from fastnumbers import check_int")
timer.time_functions()

timer = Timer("Timing comparison to check if value can be converted to `float`")
timer.add_function(
    "check_float_try", "try/except", "from __main__ import check_float_try"
)
timer.add_function("check_float_re", "regex", "from __main__ import check_float_re")
timer.add_function("check_float", "fastnumbers", "from fastnumbers import check_float")
timer.time_functions()

timer = Timer(
    "Timing comparison to check if value can be converted to `float` or `int`"
)
timer.add_function(
    "check_real_try", "try/except", "from __main__ import check_real_try"
)
timer.add_function("check_real_re", "regex", "from __main__ import check_real_re")
timer.add_function("check_real", "fastnumbers", "from fastnumbers import check_real")
timer.time_functions()

timer = Timer("Timing comparison to check if value can be coerced losslessly to `int`")
timer.add_function(
    "check_intlike_try", "try/except", "from __main__ import check_intlike_try"
)
timer.add_function("check_intlike_re", "regex", "from __main__ import check_intlike_re")
timer.add_function(
    "check_intlike", "fastnumbers", "from fastnumbers import check_intlike"
)
timer.time_functions()


timer = Timer(
    "Timing comparison of `map` option vs. iterating over "
    "`try_float` for a 50 element list"
)
timer.add_function(
    "fn_listcomp",
    "[try_float(x) for x in iterable]",
    "from __main__ import fn_listcomp",
    iterable=True,
)
timer.add_function(
    "fn_map",
    "list(map(try_float, iterable))",
    "from __main__ import fn_map",
    iterable=True,
)
timer.add_function(
    "fn_map_iter_option",
    "list(try_float(iterable, map=True))",
    "from __main__ import fn_map_iter_option",
    iterable=True,
)
timer.add_function(
    "fn_map_option",
    "try_float(iterable, map=list)",
    "from __main__ import fn_map_option",
    iterable=True,
)
timer.time_functions()

timer = Timer(
    "Timing comparison of `try_array` vs. `map` option "
    "for a 50 element list to a numpy array"
)
timer.add_function(
    "fn_then_array",
    "np.array(try_float(iterable, map=True))",
    "from __main__ import fn_then_array; import numpy as np",
    iterable=True,
)
timer.add_function(
    "try_array",
    "try_array(iterable)",
    "from fastnumbers import try_array",
    iterable=True,
)
timer.add_function(
    "fn_into_array",
    "try_array(iterable, output)",
    "from __main__ import fn_into_array",
    iterable=True,
)
timer.time_functions()
