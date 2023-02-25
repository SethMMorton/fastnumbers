#pragma once

#include <functional>
#include <stdexcept>

/// Custom exception class to tell the handler to just return NULL
class exception_is_set : public std::runtime_error {
public:
    exception_is_set()
        : std::runtime_error("")
    { }
};

/// Custom exception class for fastnumbers
class fastnumbers_exception : public std::runtime_error {
public:
    fastnumbers_exception(const char* message)
        : std::runtime_error(message)
    { }

    /// Set a ValueError if a non-empty message was given.
    PyObject* raise_value_error() const
    {
        if (what()[0] != '\0') {
            PyErr_SetString(PyExc_ValueError, what());
        }
        return nullptr;
    }
};

/**
 * \class ExceptionHandler
 * \brief Call a function and handle all exceptions
 */
class ExceptionHandler {
public:
    /// Construct with the original input sent to the function
    explicit ExceptionHandler(PyObject* input) noexcept
        : m_input(input)
    { }

    // Cannot copy
    ExceptionHandler(const ExceptionHandler&) = delete;
    ExceptionHandler(ExceptionHandler&&) = delete;
    ExceptionHandler& operator=(const ExceptionHandler&) = delete;

    // Default destroy
    ~ExceptionHandler() = default;

    /// Handle all exceptions from running fastnumbers logic.
    /// This is a "function try block", hence the missing pair of braces.
    PyObject* run(std::function<PyObject*()> func) noexcept(false)
    try {
        return func();
    } catch (const exception_is_set&) {
        return nullptr;
    } catch (const fastnumbers_exception& e) {
        return e.raise_value_error();
    } catch (const std::exception& e) {
        // This will ideally never get coverage, and that's OK
        PyErr_Format(
            PyExc_SystemError,
            "fastnumbers with input '%.200R' has thrown an unexpected C++ exception: %s",
            m_input,
            e.what()
        );
        return nullptr;
    }

private:
    /// The orignal input value given to the function
    PyObject* m_input;
};
