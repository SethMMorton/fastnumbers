from __future__ import annotations

import hypothesis

# This disables the "too slow" hypothesis heath check globally.
# For some reason it thinks that the text/binary generation is too
# slow then causes the tests to fail.
hypothesis.settings.register_profile(
    "standard", suppress_health_check=[hypothesis.HealthCheck.too_slow]
)

# Explore less space in tests
hypothesis.settings.register_profile(
    "fast",
    suppress_health_check=[hypothesis.HealthCheck.too_slow],
    max_examples=10,
)

# Explore more space in tests
hypothesis.settings.register_profile(
    "thorough",
    suppress_health_check=[hypothesis.HealthCheck.too_slow],
    max_examples=500,
)


def base_n(
    num: int, b: int, numerals: str = "0123456789abcdefghijklmnopqrstuvwxyz"
) -> str:
    """
    Convert any integer to a Base-N string representation.
    Shamelessly stolen from http://stackoverflow.com/a/2267428/1399279
    """
    neg = num < 0
    num = abs(num)
    val = ((num == 0) and numerals[0]) or (
        base_n(num // b, b, numerals).lstrip(numerals[0]) + numerals[num % b]
    )
    return "-" + val if neg else val
