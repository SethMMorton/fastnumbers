#! /usr/bin/env bash

# Error on command that fails, and add debugging
set -ex

# Configure the environment
python -m pip install -U pip
python -m pip install -U wheel twine cibuildwheel==1.0.0

# Install and execute the builder.
export CIBW_SKIP="cp27-*"
python -m cibuildwheel --output-dir wheelhouse

# Release the built wheels. Upload to testing PyPI if there is no tag,
# otherwise upload to the real thing.
# shellcheck disable=SC2086
if [ -z $TRAVIS_TAG ]
then
    echo python -m twine upload \
        --user SethMMorton \
        --skip-existing \
        --repository-url https://test.pypi.org/legacy/ \
        wheelhouse/*.whl
else
    echo python -m twine upload \
        --user SethMMorton \
        --skip-existing \
        wheelhouse/*.whl
fi