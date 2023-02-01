# Release Checklist

- [ ] Get main to the appropriate code release state.
      [GitHub Actions](https://github.com/SethMMorton/fastnumbers/actions) must be passing:
      [![Build Status](https://github.com/SethMMorton/fastnumbers/workflows/Tests/badge.svg)](https://github.com/SethMMorton/fastnumbers/actions)

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

- [ ] Push the tag:

    ```bash
    git push --tags
    ```

- [ ] Check that the tagged [GitHub Actions build](https://github.com/SethMMorton/fastnumbers/actions) has
      deployed correctly to [PyPI](https://pypi.org/project/fastnumbers/#history).

- [ ] Check installation:

    ```bash
    python -m pip uninstall -y fastnumbers && python -m pip install -U fastnumbers
    ```
