#! /bin/bash

rm -rf doctest.py* build/ dist/ *.egg-info .pytest_cache/ .hypothesis/ .tox/
find . -type d -name __pycache__ -delete
find . -type f -name "*.pyc" -delete
