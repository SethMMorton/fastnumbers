#! /bin/bash

# Fail on first non-successful command
set -e
# set -x  # turn on for debugging

# Perform special processing for MacOS and Windows. None for Linux.
case "${TRAVIS_OS_NAME}" in
    osx)
        echo "Installing Python version ${PYTHON_VERSION?} using pyenv..."
        wget https://github.com/praekeltfoundation/travis-pyenv/releases/download/0.4.0/setup-pyenv.sh
        # shellcheck disable=SC1091
        PYENV_VERSION="${PYTHON_VERSION}" PYENV_VERSION_STRING="Python ${PYTHON_VERSION}" source setup-pyenv.sh
        ;;
    windows)
        echo "Installing Python version ${PYTHON_VERSION?} using choco..."
        shortver="${PYTHON_VERSION%.*}"  # Remove patch component of version
        shortver="${shortver//./}"  # Remove dot
        if [ -f "/c/Python${shortver}/python" ]
        then
            echo "Cached python found at /c/Python${shortver}! Skipping!"
        else
            choco install python --version "${PYTHON_VERSION}"
        fi
        ;;
esac
