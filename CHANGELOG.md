Unreleased
---

[4.0.1] - 2023-02-05
---

### Fixed
- Removed a nasty memory leak that would cause every number returned
  from `fastnumbers` to never get freed.

[4.0.0] - 2023-02-01
---

### Added
- Added `check_real`, `check_float`, `check_int`, and `check_intlike`
 to provide a more flexible interface than `isreal`, `isfloat`, `isint`,
 and `isintlike`, respectively (Issues
 [#37](https://github.com/SethMMorton/fastnumbers/issues/37) and
 [#39](https://github.com/SethMMorton/fastnumbers/issues/39))
- Added `try_real`, `try_float`, `try_int`, and `try_forceint`
 to provide a more flexible interface than `fast_real`, `fast_float`, `fast_int`,
 and `fast_forceint`, respectively (Issues
 [#37](https://github.com/SethMMorton/fastnumbers/issues/37) and
 [#40](https://github.com/SethMMorton/fastnumbers/issues/40))
- Added Python 3.11 support

### Changed
- `query_type` now sets `allow_underscores` to `False` by default
- Complete rewrite into C++ (Issues [#45](https://github.com/SethMMorton/fastnumbers/issues/45),
  [#56](https://github.com/SethMMorton/fastnumbers/issues/56),
  [#55](https://github.com/SethMMorton/fastnumbers/issues/55), and
  [#35](https://github.com/SethMMorton/fastnumbers/issues/35))
- All functions now use use `METH_FASTCALL` to pass arguments from
  Python to C (Issue [#59](https://github.com/SethMMorton/fastnumbers/issues/59))
- Always parse integers using `int64_t` instead of using `long` to ensure
  we can always quickly parse at least 18 digits
- Parsing floats now always uses the fast and robust parser
  [`fast_float::from_chars()`](https://github.com/fastfloat/fast_float)
  (Issues [#57](https://github.com/SethMMorton/fastnumbers/issues/57) and
  [#28](https://github.com/SethMMorton/fastnumbers/issues/28))

### Deprecated
- `isreal`, `isfloat`, `isint`, and `isintlike` are deprecated in
  favor of `check_real`, `check_float`, `check_int`, and `check_intlike`,
  though they will never be removed from the API
- `fast_real`, `fast_float`, `fast_int`, and `fast_forceint` are deprecated in
  favor of `try_real`, `try_float`, `try_int`, and `try_forceint`,
  though they will never be removed from the API

### Removed
- Support for Python 3.6
- Removed the undocumented `dig`, `max_exp`, `min_exp`, and `max_int_len`
  module-level constants, as they are now meaningless

[3.2.1] - 2021-11-02
---

- No changes - needed a new release to fix a deployment issue

[3.2.0] - 2021-11-01
---

### Added
- Support for ARM wheels ([@odidev](https://github.com/odidev), issues #48, #49)
- Support for Python 3.10 (issue #50)
- Full coverage of mypy type-checking (issue #51)

### Removed
- Support for Python 3.5

[3.1.0] - 2020-11-21
---

### Added
- `query_type` function to determine what as type `fastnumbers` will interpret
  a given input

### Fixed
- Support for Python 3.9 (eliminate use of private Python C function that is now
  hidden in 3.9) (issue #43)

[3.0.0] - 2020-01-06
---

### Added
 - Support and tests for Python 3.8
 - Text to highlight that `fastnumbers` is not always faster that native Python
 - `on_fail` option that is identical to `key`, but has a more descriptive name
 - Windows testing to Travis-CI
 - Code quality checks to Travis-CI
 - Deployment from Travis-CI
 - RELEASING.md

### Changed
 - Cleaned up all test code so that it no longer includes unused code
   and also conformes to flake8/black
 - Near-complete re-write of the README, hopefully to make the functionality
   of `fastnumbers` clearer, to support better navigation, and to better
   highlight caveats.
 - Made all named options keyword-only except for `default`

### Deprecated
 - `key` function (it will forever remain allowed, but is "hidden" and cannot
   be given with `on_fail`)

### Fixed
 - Bug where the `coerce` option of `real()` was ignored
 - Improved testing reproducibility by pinning all test dependencies

### Removed
 - Support for Python 2.7 and Python 3.4
 - Appveyor service

[2.2.1] - 2019-03-25
---

### Fixed
 - Formatting docstring
 - Package metadata

[2.2.0] - 2019-03-24
---

### Changed
 - Add `allow_underscores` option to toggle whether or not underscores
   are allowed inside numbers on Python >= 3.6
 - Update CHANGELOG format to style from https://keepachangelog.com/
 - Build system now uses pip in stead of pipenv (issue #22)
 - Simplify tox.ini file

[2.1.1] - 2018-08-19
---

### Added
  - A high-level description of the fastnumbers algorithm
    in the documentation.

### Fixed
  - Compile error on FreeBSD where fastnumbers' `"string.h"`
    shadowed the system `"string.h"`.

[2.1.0] - 2018-08-03
---

### Changed
  - Speedup of conversions of ASCII-like unicode in Python 3.
  - Speedup of conversions for large integers and large floats -
    fastnumbers should now be at least as fast as built-in
    functions.
  - Restructure code base, hopefully improving simplicity.

### Fixed
  - Bug in converting unicode numbers on Python 3.7.

[2.0.5] - 2018-07-01
---

### Changed
  - Source files are sorted before compilation.

### Fixed
  - Bug in `fast_real` that resulted in an `OverflowError`
    when given very large int-like floats.

[2.0.4] - 2018-05-18
---

### Fixed
  - Install error on old versions of setuptools.

[2.0.3] - 2018-05-14
---

### Added
  - [`bumpversion`](https://github.com/c4urself/bump2version) infrastrucutre.

### Changed
  - Reorganized testing and development infrastructure.
  - Development dependencies are now defined using Pipfile.

[2.0.2] - 2017-11-11
---

### Added
  - Added testing for Python 3.7.

### Changed
  - Updated docstrings.
  - Improved timing documentation.

[2.0.1] - 2017-04-30
---

### Fixed
  - Bug in decimal digit limit on GCC.

[2.0.0] - 2017-04-30
---

### Added
  - Support for Python 3.6 underscores.
  - Drop-in replacements for the built-in `int()` and `float()` functions.
  - Appveyor testing to ensure no surprises on Windows.

### Changes
  - Incorporated unit tests from Python's testing library to ensure that any
    input that Python can handle will also be handled the same way by `fastnumbers`.
  - Revamped documentation.
  - Refactored internal mechanism for assessing overflow to be faster in the most
    common cases.

### Removed
  - Dropped support for Python 2.6.

[1.0.0] - 2016-04-23
---

### Changed
  - "coerce" in `fast_real` now applies to any input, not just numeric;
    the default is now *True* instead of *False*.
  - Now all ASCII whitespace characters are stripped by fastnumbers
  - Typechecking is now more forgiving
  - fastnumbers now checks for errors when converting between numeric types
  - Testing now includes Python 2.6.

### Fixed
  - Bug where very small numbers are not converted properly
  - Unicode handling on Windows.
  - Python2.6 on Windows.

### Removed
  - Removed `safe_*` functions (which were deprecated since version 0.3.0)

[0.7.4] - 2016-03-19
---

### Added
  - The `coerce` option to `fast_real`.

[0.7.3] - 2016-03-08
---

### Changed
  - Newline is now considered to be whitespace (for consistency with
    the builtin `float` and `int`).

[0.7.2] - 2016-03-07
---

### Fixed
  - Overflow bug in exponential parts of floats.

[0.7.1] - 2016-02-29
---

### Added
  - `key` function option to transform invalid input arguments.

### Fixed
  - Compilation bug with MSVC.

[0.7.0] - 2016-01-18
---

### Changed
  - Sped up functions by eliminating an unnecessary string copy.
  - Broke all functions into smaller components, eliminating
    a lot of duplication.
  - Improved documentation.

[0.6.2] - 2015-11-01
---

### Fixed
  - Bug that caused a SystemError exception to be raised
    on Python 3.5 if a very large int was passed to the "fast"
    functions.

[0.6.1] - 2015-10-29
---

### Added
  - `tox.ini`

### Changed
  - Sped up unit testing.

### Fixed
  - Segfault on Python 3.5 that seemed to be related to a
    change in the PyObject_CallMethod C function.

[0.6.0] - 2015-10-27
---

### Added
  - The `nan` and `inf` options to `fast_real` and `fast_float`.
    These options allow alternate return values in the case of *nan*
    or *inf*, respectively.

### Changed
  - Improved documentation.
  - Improved testing.

### Fixed
  - Fixed issue where giving a default of *None* would be ignored.
  
[0.5.2] - 2015-06-11
---

### Fixed
  - Compile error with Visual Studio compilers.

[0.5.1] - 2015-06-04
---

### Changed
  - Made handling of Infinity and NaN for `fast_int` and `fast_forceint`
    consistent with the `built-in` int function.

### Fixed
  - Solved rare segfault when parsing Unicode input.

[0.5.0] - 2015-05-12
---

### Added
  - Added `num_only` option for checker functions.

### Changed
  - Made `default` the first optional argument instead of `raise_on_invalid\`
    for conversion functions.

[0.4.0] - 2015-05-03
---

### Added
  - Support for conversion of single Unicode characters
    that represent numbers and digits.

[0.3.0] - 2015-04-23
---

### Changed
  - Updated all unit testing to use the `hypothesis` module,
    which results in better test coverage.
  - Updated the `fast_*` functions to check if an overflow
    loss of precision has occurred, and if so fall back on the
    more accurate number conversion method.

### Deprecated
  - In response to the above change, the `safe_*` functions
    are now deprecated, and internally now use the same code as
    the `fast_*` functions.

[0.2.0] - 2014-09-03
---

### Added
  - A `default` option to the conversion functions.

[0.1.4] - 2014-08-12
---

### Changed
  - The method to catch corner-cases like '.', '+', 'e', etc. has been
    reworked to be more general... case-by-case patches should no longer
    be needed.

### Fixed
  - Bug where '.' was incorrectly identified as a valid
    float/int and converted to 0. This bug only applied to the `fast_*`
    and `is*` functions.

[0.1.3] - 2014-08-12
---

### Fixed
  - Bug where 'e' and 'E' were incorrectly identified as a valid
    float/int and converted to 0. This bug only applied to the `fast_*`
    and `is*` functions.

[0.1.2] - 2014-08-12
---

### Fixed
  - Bug where '+' and '-' were incorrectly identified as a valid
    float/int and converted to 0. This bug only applied to the `fast_*`
    and `is*` functions.
  - Bug where `safe_forceint` did not handle `nan` correctly.

[0.1.1] - 2014-08-11
---

### Added
  -  Support for `inf` and `nan`

[0.1.0] - 2014-08-10
---

  - Initial release of `fastnumbers`

<!---Comparison links-->
[4.0.1]: https://github.com/SethMMorton/fastnumbers/compare/4.0.0...4.0.1
[4.0.0]: https://github.com/SethMMorton/fastnumbers/compare/3.2.1...4.0.0
[3.2.1]: https://github.com/SethMMorton/fastnumbers/compare/3.2.0...3.2.1
[3.2.0]: https://github.com/SethMMorton/fastnumbers/compare/3.1.0...3.2.0
[3.1.0]: https://github.com/SethMMorton/fastnumbers/compare/3.0.0...3.1.0
[3.0.0]: https://github.com/SethMMorton/fastnumbers/compare/2.2.1...3.0.0
[2.2.1]: https://github.com/SethMMorton/fastnumbers/compare/2.2.0...2.2.1
[2.2.0]: https://github.com/SethMMorton/fastnumbers/compare/2.1.1...2.2.0
[2.1.1]: https://github.com/SethMMorton/fastnumbers/compare/2.1.0...2.1.1
[2.1.0]: https://github.com/SethMMorton/fastnumbers/compare/2.0.5...2.1.0
[2.0.5]: https://github.com/SethMMorton/fastnumbers/compare/2.0.4...2.0.5
[2.0.4]: https://github.com/SethMMorton/fastnumbers/compare/2.0.3...2.0.4
[2.0.3]: https://github.com/SethMMorton/fastnumbers/compare/2.0.2...2.0.3
[2.0.2]: https://github.com/SethMMorton/fastnumbers/compare/2.0.1...2.0.2
[2.0.1]: https://github.com/SethMMorton/fastnumbers/compare/2.0.0...2.0.1
[2.0.0]: https://github.com/SethMMorton/fastnumbers/compare/1.0.0...2.0.0
[1.0.0]: https://github.com/SethMMorton/fastnumbers/compare/0.7.4...1.0.0
[0.7.4]: https://github.com/SethMMorton/fastnumbers/compare/0.7.3...0.7.4
[0.7.3]: https://github.com/SethMMorton/fastnumbers/compare/0.7.2...0.7.3
[0.7.2]: https://github.com/SethMMorton/fastnumbers/compare/0.7.1...0.7.2
[0.7.1]: https://github.com/SethMMorton/fastnumbers/compare/0.7.0...0.7.1
[0.7.0]: https://github.com/SethMMorton/fastnumbers/compare/0.6.2...0.7.0
[0.6.2]: https://github.com/SethMMorton/fastnumbers/compare/0.6.1...0.6.2
[0.6.1]: https://github.com/SethMMorton/fastnumbers/compare/0.6.0...0.6.1
[0.6.0]: https://github.com/SethMMorton/fastnumbers/compare/0.5.2...0.6.0
[0.5.2]: https://github.com/SethMMorton/fastnumbers/compare/0.5.1...0.5.2
[0.5.1]: https://github.com/SethMMorton/fastnumbers/compare/0.5.0...0.5.1
[0.5.0]: https://github.com/SethMMorton/fastnumbers/compare/0.4.0...0.5.0
[0.4.0]: https://github.com/SethMMorton/fastnumbers/compare/0.3.0...0.4.0
[0.3.0]: https://github.com/SethMMorton/fastnumbers/compare/0.2.0...0.3.0
[0.2.0]: https://github.com/SethMMorton/fastnumbers/compare/0.1.4...0.2.0
[0.1.4]: https://github.com/SethMMorton/fastnumbers/compare/0.1.3...0.1.4
[0.1.3]: https://github.com/SethMMorton/fastnumbers/compare/0.1.2...0.1.3
[0.1.2]: https://github.com/SethMMorton/fastnumbers/compare/0.1.1...0.1.2
[0.1.1]: https://github.com/SethMMorton/fastnumbers/compare/0.1.0...0.1.1
[0.1.0]: https://github.com/SethMMorton/fastnumbers/releases/tag/0.1.0
