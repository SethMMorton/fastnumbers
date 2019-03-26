Unreleased
---

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
