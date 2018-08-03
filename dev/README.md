# Development Collateral

This file contains some files useful for development.

- `astyle.cfg` - Configuration settings for [astyle](http://astyle.sourceforge.net/).
- `build.cmd` - Used to build and run tests on Windows. Shamelessly
  stolen from https://packaging.python.org/appveyor/#support-scripts.
- `clean.sh` - This file cleans most files that are created during development.
  Run in the project home directory.
- `fmt.sh` - This file executes [astyle](http://astyle.sourceforge.net/) then [black](https://black.readthedocs.io/en/stable/).
- `patch_doctest.py` -
  The [doctest](https://docs.python.org/3/library/doctest.html) module
  from the Python standard library has an interesting limitation
  whereby it cannot run tests on extension modules. This file
  makes a local copy of `doctest.py` and patches it to be able to run
  on extension modules.
- `run_coverage.sh` - A small shell one-liner placed into a file that
  will compile `fastnumbers`, run all tests, then run `lcov` and
  `genhtml` to create an HTML coverage report. Only tested on OSX.
