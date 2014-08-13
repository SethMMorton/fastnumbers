.. _changelog:

Changelog
---------

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
