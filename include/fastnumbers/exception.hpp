#pragma once

#include <stdexcept>

/// Custom exception class to tell the handler to just return NULL
class exception_is_set : public std::runtime_error {
public:
    exception_is_set()
        : std::runtime_error("")
    { }
};
