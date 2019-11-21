#! /usr/bin/env bash

# Error on command that fails, and add debugging
set -ex

# Configure the environment
python -m pip install -U pip
python -m pip install twine

# Create the source distribution
python setup.py sdist --format=gztar

# Release the built wheels. Upload to testing PyPI if there is no tag,
# otherwise upload to the real thing.
# shellcheck disable=SC2086
if [ -z $TRAVIS_TAG ]
then
    python -m twine upload \
        --user SethMMorton \
        --skip-existing \
        --repository-url https://test.pypi.org/legacy/ \
        dist/fastnumbers*.tar.gz
else
    python -m twine upload \
        --user SethMMorton \
        --skip-existing \
        dist/fastnumbers*.tar.gz
fi
