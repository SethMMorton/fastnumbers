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
#include "fastnumbers/introspection.hpp"
#include "fastnumbers/parser.hpp"
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
    CTypeExtractor(const UserOptions& options)
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
    T extract_c_number(PyObject* input)
    {
        T value;
        TextExtractor extractor(input, m_buffer);
        bool errored, overflow, type_error;

        // This function can grab the error state from the parser
        auto get_errors = [&errored, &overflow, &type_error](auto& parser) {
            errored = parser.errored();
            overflow = parser.overflow();
            type_error = parser.type_error();
        };

        // Depending on the found data type, create a different parser to get the number
        if (extractor.is_text()) {
            CharacterParser cparser = extractor.text_parser(m_options);
            value = cparser.as_number<T>();
            get_errors(cparser);
        } else if (extractor.is_unicode_character()) {
            UnicodeParser uparser = extractor.unicode_char_parser(m_options);
            value = uparser.as_number<T>();
            get_errors(uparser);
        } else {
            NumericParser nparser(input, m_options);
            value = nparser.as_number<T>();
            get_errors(nparser);
        }

        // Check if there were any error cases, and if so, attempt a value replacement.
        if (errored) {
            if (type_error) {
                return replace_value(ReplaceType::TYPE_ERROR_, input);
            } else if (overflow) {
                return replace_value(ReplaceType::OVERFLOW_, input);
            } else {
                return replace_value(ReplaceType::FAIL_, input);
            }
        }

        // For float types, attempt to replace NaN and INF if that is so desired.
        if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(value) && !std::holds_alternative<std::monostate>(m_nan)) {
                return replace_value(ReplaceType::NAN_, input);
            } else if (std::isinf(value) && !std::holds_alternative<std::monostate>(m_inf)) {
                return replace_value(ReplaceType::INF_, input);
            }
        }

        // If none of the above occured, just return the converted value.
        return value;
    }

    /**
     * \brief Define if the value needs to be replaced if NaN would be returned
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_nan_replacement(PyObject* replacement)
    {
        add_replacement_to_mapping(ReplaceType::NAN_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced if INF would be returned
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_inf_replacement(PyObject* replacement)
    {
        add_replacement_to_mapping(ReplaceType::INF_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when a parsing error occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_fail_replacement(PyObject* replacement)
    {
        add_replacement_to_mapping(ReplaceType::FAIL_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when an overflow occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_overflow_replacement(PyObject* replacement)
    {
        add_replacement_to_mapping(ReplaceType::OVERFLOW_, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when a type error occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_type_error_replacement(PyObject* replacement)
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
    ReplaceValue& get_value(ReplaceType key)
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
    T replace_value(ReplaceType key, PyObject* input)
    {
        // A safe and clean way to perform different actions on a std::variant based
        // on the stored type is to use std::visit, which will execute the correct
        // logic based on the overload that best matches the type currently stored
        // in the variant. What each action does is annotated below.
        // The logics are ordered in their anticipated most commonly used ordering
        // in practice, in case that somehow affects performance.
        return std::visit(
            overloaded {

                // If a raw value is stored, return it directly
                [](const T arg) -> T {
                    return arg;
                },

                // If no value is stored, then we should raise an error
                [input, key](std::monostate) -> T {
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
                            "The value %.200R has type %.200R which cannot be converted "
                            "to "
                            "a numeric value",
                            input,
                            type_name
                        );
                        Py_DECREF(type_name);
                    }
                    throw exception_is_set();
                },

                // If a Python object is stored, we assume it is a callable that will be
                // used to get the appropriate raw value to return.
                [input, key, this](PyObject* arg) -> T {
                    // Call a Python function
                    PyObject* retval = PyObject_CallFunctionObjArgs(arg, input, nullptr);

                    // On exception, no need to define our our own message,
                    // I'm sure Python's is just fine.
                    if (retval == nullptr) {
                        throw exception_is_set();
                    }

                    // If there was no error calling the function, attempt to extract the
                    // C type.
                    NumericParser parser(retval, m_options);
                    const T call_value = parser.as_number<T>();

                    // Check for more errors, and if we pass them then return the value.
                    if (parser.errored()) {
                        if (parser.type_error()) {
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
                        } else if (parser.overflow()) {
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
                    }
                    Py_DECREF(retval);
                    return call_value;
                },
            },

            // This is the actual value passed to std::visit that will be acted upon
            get_value(key)
        );
    }

    /**
     * \brief Save this replacement method for the given error type
     * \param key The key under which to store the replacement method
     * \param replacement The Python object representing the replacement method
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void add_replacement_to_mapping(ReplaceType key, PyObject* replacement)
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

        // For anything else, assume it was a number and convert to a value
        // of the appropriate type and store in the mapping.
        NumericParser parser(replacement, m_options);
        const T value = parser.as_number<T>();
        if (parser.errored()) {
            if (parser.type_error()) {
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
            } else if (parser.overflow()) {
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
        }

        // If the value is a legit C type, we store it.
        get_value(key) = value;
    }
};
