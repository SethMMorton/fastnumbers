#pragma once

#include <cmath>
#include <map>
#include <type_traits>
#include <utility>
#include <variant>

#include <Python.h>

#include "fastnumbers/compatibility.hpp"
#include "fastnumbers/exception.hpp"
#include "fastnumbers/extractor.hpp"
#include "fastnumbers/helpers.hpp"
#include "fastnumbers/introspection.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/selectors.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class CTypeExtractor
 * \brief Extract the requested C numeric type from a Python object
 */
template <typename T>
class CTypeExtractor {
public:
    /// Constructor
    CTypeExtractor(const UserOptions& options) noexcept
        : m_inf()
        , m_nan()
        , m_fail()
        , m_overflow()
        , m_type_error()
        , m_options(options)
        , m_buffer()
    { }

    // Copy and assignment are disallowed
    CTypeExtractor(const CTypeExtractor&) = delete;
    CTypeExtractor(CTypeExtractor&&) = delete;
    CTypeExtractor& operator=(const CTypeExtractor&) = delete;

    // Default destructor
    ~CTypeExtractor() = default;

    /**
     * \brief Return a C number in the requested type
     * \param input The Python object from which to extract the number
     * \return The C number in the template type specified
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    T extract_c_number(PyObject* input) noexcept(false)
    {
        // Get the payload no matter which parser was returned
        RawPayload<T> payload;
        std::visit(
            [&payload](const auto& parser) {
                parser.as_number(payload);
            },
            extract_parser(input, m_buffer, m_options)
        );

        // Function to pass-through a valid value, handling the special
        // case of the value being NaN or INF and requiring a replacement.
        auto handle_value = [&](const T value) -> T {
            if constexpr (std::is_floating_point_v<T>) {
                const bool replace_nan = !std::holds_alternative<std::monostate>(m_nan);
                const bool replace_inf = !std::holds_alternative<std::monostate>(m_inf);
                if (std::isnan(value) && replace_nan) {
                    return replace_value(ReplaceType::NAN_, input);
                } else if (std::isinf(value) && replace_inf) {
                    return replace_value(ReplaceType::INF_, input);
                }
            }
            return value;
        };

        // Function to call the appropriate error handler if an error occured.
        auto handle_error = [this, input](const ErrorType err) -> T {
            if (err == ErrorType::BAD_VALUE) {
                return replace_value(ReplaceType::FAIL_, input);
            } else if (err == ErrorType::OVERFLOW_) {
                return replace_value(ReplaceType::OVERFLOW_, input);
            } else {
                return replace_value(ReplaceType::TYPE_ERROR_, input);
            }
        };

        // Based perform different logic depending on what was contained in the payload
        return std::visit(overloaded { handle_value, handle_error }, payload);
    }

    /**
     * \brief Define if the value needs to be replaced if NaN would be returned
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_nan_replacement(PyObject* replacement) noexcept(false)
    {
        add_replacement_to_mapping(ReplaceType::NAN_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced if INF would be returned
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_inf_replacement(PyObject* replacement) noexcept(false)
    {
        add_replacement_to_mapping(ReplaceType::INF_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when a parsing error occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_fail_replacement(PyObject* replacement) noexcept(false)
    {
        add_replacement_to_mapping(ReplaceType::FAIL_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when an overflow occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_overflow_replacement(PyObject* replacement) noexcept(false)
    {
        add_replacement_to_mapping(ReplaceType::OVERFLOW_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when a type error occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_type_error_replacement(PyObject* replacement) noexcept(false)
    {
        add_replacement_to_mapping(ReplaceType::TYPE_ERROR_, replacement);
    }

private:
    /// Represent reasons to replace a value
    enum class ReplaceType {
        INF_,
        NAN_,
        FAIL_,
        OVERFLOW_,
        TYPE_ERROR_,
    };

    /// The value (or Python callable to generate a value) to use on replacement
    using ReplaceValue = std::variant<std::monostate, T, PyObject*>;

    /// Potential replacement for infinity
    ReplaceValue m_inf;

    /// Potential replacement for NaN
    ReplaceValue m_nan;

    /// Potential replacement for invalid values
    ReplaceValue m_fail;

    /// Potential replacement for overflows
    ReplaceValue m_overflow;

    /// Potential replacement for invalid types
    ReplaceValue m_type_error;

    /// Store string representations of the replacement types
    const std::map<ReplaceType, const char*> m_replace_repr {
        { ReplaceType::INF_, "inf" },
        { ReplaceType::NAN_, "nan" },
        { ReplaceType::FAIL_, "on_fail" },
        { ReplaceType::OVERFLOW_, "on_overflow" },
        { ReplaceType::TYPE_ERROR_, "on_type_error" },
    };

    /// Hold the evaluator options
    UserOptions m_options;

    /// A buffer into which to store text data
    Buffer m_buffer;

private:
    /// Return the object that corresponds to the user's requested key -
    /// the return is a reference so it can be edited
    ReplaceValue& get_value(ReplaceType key) noexcept
    {
        switch (key) {
        case ReplaceType::INF_:
            return m_inf;
        case ReplaceType::NAN_:
            return m_nan;
        case ReplaceType::FAIL_:
            return m_fail;
        case ReplaceType::OVERFLOW_:
            return m_overflow;
        default: // ReplaceType::TYPE_ERROR_:
            return m_type_error;
        }
    }

    /// Return the object that corresponds to the user's requested key -
    /// the return is a reference so it can be edited
    const ReplaceValue& get_value(ReplaceType key) const noexcept
    {
        switch (key) {
        case ReplaceType::INF_:
            return m_inf;
        case ReplaceType::NAN_:
            return m_nan;
        case ReplaceType::FAIL_:
            return m_fail;
        case ReplaceType::OVERFLOW_:
            return m_overflow;
        default: // ReplaceType::TYPE_ERROR_:
            return m_type_error;
        }
    }

    /**
     * \brief Replace the given input in the user-specified method
     * \param key The key to use to look up the appropriate replacement method
     * \param input The Python object that triggered the need for a replacement
     * \return The C number after replacement
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    T replace_value(ReplaceType key, PyObject* input) const noexcept(false)
    {
        // Function to raise a Python exception on error
        auto raise_exception = [input, key](std::monostate) -> T {
            if (key == ReplaceType::FAIL_) {
                PyErr_Format(
                    PyExc_ValueError,
                    "Cannot convert %.200R to C type '%s'",
                    input,
                    type_name<T>()
                );
            } else if (key == ReplaceType::OVERFLOW_) {
                PyErr_Format(
                    PyExc_OverflowError,
                    "Cannot convert %.200R to C type '%s' without overflowing",
                    input,
                    type_name<T>()
                );
            } else { // "on_type_error", "nan" and "inf" omitted by construction
                PyObject* type_name = PyType_GetName(Py_TYPE(input));
                PyErr_Format(
                    PyExc_TypeError,
                    "The value %.200R has type %.200R which cannot be converted to "
                    "a numeric value",
                    input,
                    type_name
                );
                Py_DECREF(type_name);
            }
            throw exception_is_set();
        };

        // A safe and clean way to perform different actions on a std::variant based
        // on the stored type is to use std::visit, which will execute the correct
        // logic based on the overload that best matches the type currently stored
        // in the variant. What each action does is annotated above.
        return std::visit(
            overloaded {
                [](const T arg) -> T {
                    return arg;
                },
                [this, input, key](PyObject* arg) -> T {
                    return call_python_convert_result(arg, input, key);
                },
                raise_exception,
            },
            get_value(key)
        );
    }

    /**
     * \brief Save this replacement method for the given error type
     * \param key The key under which to store the replacement method
     * \param replacement The Python object representing the replacement method
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void
    add_replacement_to_mapping(ReplaceType key, PyObject* replacement) noexcept(false)
    {
        // If we are supposed to raise an error for this type of failure
        // we don't store anything in our replacements mapping.
        if (replacement == Selectors::RAISE || replacement == Selectors::ALLOWED) {
            return;
        }

        // If the input is a callable just store the callable in the mapping.
        if (PyCallable_Check(replacement)) {
            get_value(key) = replacement;
            return;
        }

        // Function to raise the appropraite exception if an error occured.
        auto handle_error = [this, key, replacement](const ErrorType err) {
            if (err == ErrorType::TYPE_ERROR) {
                PyObject* type_name = PyType_GetName(Py_TYPE(replacement));
                PyErr_Format(
                    PyExc_TypeError,
                    "The default value of %.200R given to option '%s' has type "
                    "%.200R which cannot be converted to a numeric value",
                    replacement,
                    m_replace_repr.at(key),
                    type_name
                );
                Py_DECREF(type_name);
            } else if (err == ErrorType::OVERFLOW_) {
                PyErr_Format(
                    PyExc_OverflowError,
                    "The default value of %.200R given to option '%s' cannot "
                    "be converted to C type '%s' without overflowing",
                    replacement,
                    m_replace_repr.at(key),
                    type_name<T>()
                );
            } else {
                PyErr_Format(
                    PyExc_ValueError,
                    "The default value of %.200R given to option '%s' cannot "
                    "be converted to C type '%s'",
                    replacement,
                    m_replace_repr.at(key),
                    type_name<T>()
                );
            }
            throw exception_is_set();
        };

        // Function to set a value to the appropriate data member.
        auto handle_value = [this, key](const T value) {
            get_value(key) = value;
        };

        // For anything else, assume it was a number and convert to a value
        // of the appropriate type and store in the mapping.
        const NumericParser parser(replacement, m_options);
        std::visit(overloaded { handle_value, handle_error }, parser.as_number<T>());
    }

    /**
     * \brief Call a python callable that will return a replacement value
     *        for a given input
     * \param callable The Python callable object
     * \param input The Python object to be given to the callalbe
     * \param key The key describing which callable is being invoked
     * \return A C-type of what was returned from the callable
     * \throws exception_is_set
     */
    T call_python_convert_result(
        PyObject* callable, PyObject* input, const ReplaceType key
    ) const
    {
        // Call a Python function
        PyObject* retval = PyObject_CallFunctionObjArgs(callable, input, nullptr);

        // On exception, no need to define our our own message,
        // I'm sure Python's is just fine.
        if (retval == nullptr) {
            throw exception_is_set();
        }

        // Function to raise an exception on conversion error, then decrease
        // the reference count of the Python object returned from the callable.
        auto handle_call_value_error = [&](const ErrorType err) -> T {
            if (err == ErrorType::TYPE_ERROR) {
                PyObject* type_name = PyType_GetName(Py_TYPE(input));
                PyErr_Format(
                    PyExc_TypeError,
                    "Callable passed to '%s' with input %.200R returned the "
                    "value %.200R that has type %.200R which cannot be "
                    "converted to a numeric value",
                    m_replace_repr.at(key),
                    input,
                    retval,
                    type_name
                );
                Py_DECREF(type_name);
            } else if (err == ErrorType::OVERFLOW_) {
                PyErr_Format(
                    PyExc_OverflowError,
                    "Callable passed to '%s' with input %.200R returned the "
                    "value %.200R that cannot be converted to C type '%s' "
                    "without overflowing",
                    m_replace_repr.at(key),
                    input,
                    retval,
                    type_name<T>()
                );
            } else {
                PyErr_Format(
                    PyExc_ValueError,
                    "Callable passed to '%s' with input %.200R returned the "
                    "value %.200R that cannot be converted to C type '%s'",
                    m_replace_repr.at(key),
                    input,
                    retval,
                    type_name<T>()
                );
            }
            Py_DECREF(retval);
            throw exception_is_set();
        };

        // If there was no error calling the function, attempt to extract the
        // C type, but check for errors here too. Make sure we decrease the
        // reference count of what was returned by the the callable.
        const NumericParser parser(retval, m_options);
        return std::visit(
            overloaded {
                [retval](const T call_value) -> T {
                    Py_DECREF(retval);
                    return call_value;
                },
                handle_call_value_error,
            },
            parser.as_number<T>()
        );
    }
};
