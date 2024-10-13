import glob
import os
import sys

from setuptools import Extension, setup

# Compilation arguments are platform-dependent
if sys.platform == "win32":
    compile_args = [
        "/std:c++17",
        "/W4",  # Equivalent to -Wall -Wpedantic on GCC/Clang
        "/wd4100",  # don't warn about unreferenced formal parameters
        "/wd4127",  # don't warn about constant conditional expressions
    ]
    if "FN_DEBUG" in os.environ or "FN_COV" in os.environ:
        compile_args.append("/Od")
        compile_args.append("/Z7")
else:
    compile_args = [
        "-std=c++17",
        "-Wall",
        "-Weffc++",
        "-Wpedantic",
    ]
    if sys.platform == "darwin":
        compile_args.append("-mmacosx-version-min=10.13")
    if "FN_DEBUG" in os.environ or "FN_COV" in os.environ:
        compile_args.append("-Og")
        compile_args.append("-g")


ext = [
    Extension(
        "fastnumbers.fastnumbers",
        sorted(glob.glob("src/cpp/*.cpp")),
        include_dirs=[os.path.abspath(os.path.join("include"))],
        extra_compile_args=compile_args,
        extra_link_args=["-lm"],
    )
]

# Define how to build the extension module.
# All other data is in the pyproject.toml file.
setup(ext_modules=ext)
