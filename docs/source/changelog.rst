.. _changelog:

Changelog
---------

03-19-2016 v. 0.7.4
'''''''''''''''''''

    - Added the "coerce" option to fast_real.

03-08-2016 v. 0.7.3
'''''''''''''''''''

    - Newline is now considered to be whitespace (for consistency with
      the builtin float and int).

03-07-2016 v. 0.7.2
'''''''''''''''''''

    - Fixed overflow bug in exponential parts of floats.

02-29-2016 v. 0.7.1
'''''''''''''''''''

    - Fixed compilation bug with MSVC.
    - Added “key” function to transform invalid input arguments.

01-18-2016 v. 0.7.0
'''''''''''''''''''

    - Broke all functions into smaller components, eliminating
      a lot of duplication.
    - Sped up functions by eliminating an unnecessary string copy.
    - Improved documentation.

11-01-2015 v. 0.6.2
'''''''''''''''''''

    - Fixed bug that caused a SystemError exception to be raised
      on Python 3.5 if a very large int was passed to the "fast"
      functions.

10-29-2015 v. 0.6.1
'''''''''''''''''''

    - Fixed segfault on Python 3.5 that seemed to be related to a
      change in the PyObject_CallMethod C function.
    - Sped up unit testing.
    - Added tox.ini.

10-27-2015 v. 0.6.0
'''''''''''''''''''

    - Fixed issue where giving a default of *None* would be ignored.
    - Added the "nan" and "inf" options to "fast_real" and "fast_float".
      These options allow alternate return values in the case of *nan*
      or *inf*, respectively.
    - Improved documentation.
    - Improved testing.
  
06-11-2015 v. 0.5.2
'''''''''''''''''''

    - Fixed compile error Visual Studio compilers.

06-04-2015 v. 0.5.1
'''''''''''''''''''

    - Solved rare segfault when parsing Unicode input.
    - Made handling of Infinity and NaN for ``fast_int`` and ``fast_forceint``
      consistent with the ``built-in`` int function.

05-12-2015 v. 0.5.0
'''''''''''''''''''

    - Made 'default' first optional argument instead of 'raise_on_invalid'
      for conversion functions.
    - Added 'num_only' option for checker functions.

05-03-2015 v. 0.4.0
'''''''''''''''''''

    - Added support for conversion of single Unicode characters
      that represent numbers and digits.

04-23-2015 v. 0.3.0
'''''''''''''''''''

    - Updated the ``fast_*`` functions to check if an overflow
      loss of precision has occurred, and if so fall back on the
      more accurate number conversion method.
    - In response to the above change, the ``safe_*`` functions
      are now deprecated, and internally now use the same code as
      the ``fast_*`` functions.
    - Updated all unit testing to use the ``hypothesis`` module,
      which results in better test coverage.

09-03-2014 v. 0.2.0
'''''''''''''''''''

    - Added a 'default' option to the conversion functions.

08-12-2014 v. 0.1.4
'''''''''''''''''''

    - Fixed bug where '.' was incorrectly identified as a valid
      float/int and converted to 0.  This bug only applied to the ``fast_*``
      and ``is*`` functions.
    - The method to catch corner-cases like '.', '+', 'e', etc. has been
      reworked to be more general... case-by-case patches should no longer
      be needed.

08-12-2014 v. 0.1.3
'''''''''''''''''''

    - Fixed bug where 'e' and 'E' were incorrectly identified as a valid
      float/int and converted to 0.  This bug only applied to the ``fast_*``
      and ``is*`` functions.

08-12-2014 v. 0.1.2
'''''''''''''''''''

    - Fixed bug where '+' and '-' were incorrectly identified as a valid
      float/int and converted to 0.  This bug only applied to the ``fast_*``
      and ``is*`` functions.
    - Fixed bug where 'safe_forceint' did not handle 'nan' correctly.

08-11-2014 v. 0.1.1
'''''''''''''''''''

    - 'fastnumbers' now understands 'inf' and 'nan'.

08-10-2014 v. 0.1.0
'''''''''''''''''''

    - Initial release of 'fastnumbers'.
