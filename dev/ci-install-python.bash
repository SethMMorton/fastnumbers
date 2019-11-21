#! /bin/bash

# Fail on first non-successful command
set -e
# set -x  # turn on for debugging

# Perform special processing Windows. Otherwise do nothing.
if [ "${TRAVIS_OS_NAME}" = windows ]
then
    echo "Installing Python version ${PYTHON_VERSION?} using choco..."
    shortver="${PYTHON_VERSION%.*}"  # Remove patch component of version
    shortver="${shortver//./}"  # Remove dot
    if [ -f "/c/Python${shortver}/python" ]
    then
        echo "Cached python found at /c/Python${shortver}! Skipping!"
    else
        choco install python --version "${PYTHON_VERSION}"
    fi
fi
