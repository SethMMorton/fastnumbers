.. _changelog:

Changelog
---------

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
