## `fastnumbers` profiling

The files in this folder represent the profiling results for
`fastnumbers` functionality. The results are separated by Python version.
You can see what is being profiled in `profile.py`.

Some general observations about the results:
- The older the Python version, the greater the benefit `fastnumbers` brings
  over the built-in functionality. This is because the CPython maintainers are
  continiously making Python faster.
- The conversion to floats is always faster than built-in functionality
  for Python <= 3.9. Starting with Python 3.10 some optimization was added
  that makes the conversion for small floats very fast.
- The conversion to integers is always faster than built-in functionality
  until the length of the input digit string is greater than 18 characters
  (the cut-off for 64-bit integers), at which point it is roughty tied to
  slightly slower than CPython.
- When numbers are provided (as opposed to strings), the Python built-ins
  are usually faster.
