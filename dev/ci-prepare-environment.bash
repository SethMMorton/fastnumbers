#! /bin/bash

# Fail on first non-successful command
set -e
# set -x  # turn on for debugging

# Grab Python from the web then install.
# Handles using a cached version if available
# Parameters:
#  os: "osx" or "windows"
#  version: Python version to install
function install_python {
    local os="${1?}"
    local ver="${2?}"
    if [ "${os}" = osx ]
    then
        echo "Installing Python version ${ver} using pyenv..."
        wget https://github.com/praekeltfoundation/travis-pyenv/releases/download/0.4.0/setup-pyenv.sh
        # shellcheck disable=SC1091
        PYENV_VERSION="${ver}" PYENV_VERSION_STRING="Python ${ver}" source setup-pyenv.sh
    else
        echo "Installing Python version ${ver} using choco..."
        local shortver="${ver%.*}"  # Remove patch component of version
        shortver="${shortver//./}"  # Remove dot
        if [ -f "/c/Python${shortver}/python" ]
        then
            echo "Cached python found at /c/Python${shortver}! Skipping!"
        else
            choco install python --version "${ver}"
        fi
    fi
}

# Perform special processing for MacOS and Windows. None for Linux.
case "${TRAVIS_OS_NAME}" in
    osx|windows) install_python "${TRAVIS_OS_NAME}" "${PYTHON_VERSION?}";;
esac
