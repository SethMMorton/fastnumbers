#! /bin/bash

# This works on OSX. Not tested otherwise.
CFLAGS="-coverage" python setup.py build_ext --inplace && \
    python -m pytest && \
    lcov --capture --directory . --output-file lcov.info && \
    genhtml lcov.info --output-directory coverage-html
