# Development Collateral

This file contains some files useful for development.

- `astyle.cfg` - Configuration settings for [astyle](http://astyle.sourceforge.net/).
- `bump.py` - Execute `bumpversion` then post-processes the CHANGELOG to handle corner-cases
  that `bumpversion` cannot. Requires [`bump2version`](https://github.com/c4urself/bump2version),
  which is the maintained fork of [`bumpversion`](https://github.com/peritus/bumpversion).
  Not intended to be called directly, but rather by `tox -e bump`.
- `ci-install-python.bash` - Code to install Python on Windows in Travis-CI.
- `ci-update-path.bash` - Sourcing this file in Travis-CI will ensure Python is
  on the PATH in Windows.
- `ci-wheel-deploy.bash` - Code to install `cibuildwheel` and deploy with `twine`
  on Travis-CI.
- `clean.py` - This file cleans most files that are created during development.
  Run in the project home directory.
  Not intended to be called directly, but rather by `tox -e clean`.
- `patch_doctest.py` -
  The [doctest](https://docs.python.org/3/library/doctest.html) module
  from the Python standard library has an interesting limitation
  whereby it cannot run tests on extension modules. This file
  makes a local copy of `doctest.py` and patches it to be able to run
  on extension modules.
- `requirements.in` - Our direct requirements to run tests.
- `requirements.txt` - All pinned requirements to run tests.
- `run_coverage.sh` - A small shell one-liner placed into a file that
  will compile `fastnumbers`, run all tests, then run `lcov` and
  `genhtml` to create an HTML coverage report. Only tested on OSX.
