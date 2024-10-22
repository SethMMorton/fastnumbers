import os

def get_version(
    root: os.PathLike[str] | str = ".",
    version_scheme: str = ...,
    local_scheme: str = ...,
    relative_to: os.PathLike[str] | str | None = ...,
) -> str: ...
