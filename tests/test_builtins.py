import sys

# Load test runner from correct python version.
if sys.version_info[0:2] == (2, 7):
    from builtin_tests.py27.builtin_test_float import *
    from builtin_tests.py27.builtin_test_int import *
elif sys.version_info[0:2] == (3, 3):
    from builtin_tests.py33.builtin_test_float import *
    from builtin_tests.py33.builtin_test_int import *
elif sys.version_info[0:2] == (3, 4):
    from builtin_tests.py34.builtin_test_float import *
    from builtin_tests.py34.builtin_test_int import *
elif sys.version_info[0:2] == (3, 5):
    from builtin_tests.py35.builtin_test_float import *
    from builtin_tests.py35.builtin_test_int import *
elif sys.version_info[0:2] == (3, 6):
    from builtin_tests.py36.builtin_test_float import *
    from builtin_tests.py36.builtin_test_int import *
else:
    from builtin_tests.py37.builtin_test_float import *
    from builtin_tests.py37.builtin_test_int import *
