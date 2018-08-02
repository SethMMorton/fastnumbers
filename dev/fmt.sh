#! /bin/bash

find src include -type f | grep -E -v "(pstdint|docstrings|fn_bool).h" | xargs astyle --options=dev/astyle.cfg --suffix=none
black --exclude builtin_tests tests
