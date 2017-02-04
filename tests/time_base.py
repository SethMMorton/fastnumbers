from __future__ import print_function, division

# Std lib imports
from timeit import repeat


def mean(x):
    return sum(x) / len(x)


def comparison_timer(regex, try_, fast):
    """
    Run timing tests on multiple types of input, for multiple types of functions.
    """

    def time_input_against_all_functions(value, label):
        """Run the given input on all function types."""
        funcs = (regex, try_, fast)
        func_labels = ('re:', 'try:', 'fast:')
        fmt = '{func}({value!r})'
        for func_label, (func, setup) in zip(func_labels, funcs):
            print(label + ',', func_label, end=' ', flush=True)
            time_results = repeat(fmt.format(func=func, value=value), setup, repeat=10)
            time_results = mean(time_results)
            print(time_results, 'seconds')
        print()

    print('All timing results are the average of 10 runs.')
    print()
    time_input_against_all_functions('not_a_number', 'Non-number String')
    time_input_against_all_functions('-41053', 'Int String')
    time_input_against_all_functions('35892482945872302493947939485729', 'Large Int String')
    time_input_against_all_functions('-41053.543034e34', 'Float String')
    time_input_against_all_functions('-41053.543028758302e256', 'Large Float String')
    time_input_against_all_functions(-41053.543028758302e100, 'Float')
    time_input_against_all_functions(-41053, 'Int')
