import hypothesis

# This disables the "too slow" hypothesis heath check globally.
# For some reason it thinks that the text/binary generation is too
# slow then causes the tests to fail.
hypothesis.settings.register_profile(
    "slow-tests", suppress_health_check=[hypothesis.HealthCheck.too_slow]
)
