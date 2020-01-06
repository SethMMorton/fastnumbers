# Release Checklist

- [ ] Get master to the appropriate code release state.
      [Travis CI](https://travis-ci.com/SethMMorton/fastnumbers) must be passing:
      [![Build Status](https://travis-ci.com/SethMMorton/fastnumbers.svg?branch=master)](https://travis-ci.com/SethMMorton/fastnumbers)

- [ ] Ensure that the `CHANGELOG.md` includes the changes made since last release.
      Please follow the style outlined in https://keepachangelog.com/.
      All new entries should be added into the "Unreleased" section.

- [ ] Bump the version number. Specify either "major", "minor", or "patch":

    ```bash
    tox -e bump patch
    ```

    This will take care of updating the `CHANGELOG.md` with the correct
    release information.

- [ ] Push the bumped commit:

    ```bash
    git push
    ```

- [ ] Check that the [Travis CI build](https://travis-ci.com/SethMMorton/fastnumbers) has
      deployed correctly to [the test PyPI](https://test.pypi.org/project/fastnumbers/#history).

- [ ] Push the tag:

    ```bash
    git push --tags
    ```

- [ ] Check that the tagged [Travis CI build](https://travis-ci.com/SethMMorton/fastnumbers) has
      deployed correctly to [PyPI](https://pypi.org/project/fastnumbers/#history).

- [ ] Check installation:

    ```bash
    python -m pip uninstall -y fastnumbers && python -m pip install -U fastnumbers
    ```
