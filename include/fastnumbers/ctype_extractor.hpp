#pragma once

#include <cmath>
#include <cstring>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <Python.h>

#include "fastnumbers/exception.hpp"
#include "fastnumbers/extractor.hpp"
#include "fastnumbers/introspection.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/selectors.hpp"
#include "fastnumbers/user_options.hpp"

#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 11
// This function was introduced in Python 3.11, but is super-useful
// in obtaining the name of a type.
// The implementation was basically copied from the 3.11 source code,
// but adjustments were made to make it C++.
PyObject* PyType_GetName(PyTypeObject* type)
{
    auto _PyType_Name = [](PyTypeObject* type) -> const char* {
        assert(type->tp_name != nullptr);
        const char* s = std::strrchr(type->tp_name, '.');
        if (s == nullptr) {
            s = type->tp_name;
        } else {
            s++;
        }
        return s;
    };

    auto type_name = [_PyType_Name](PyTypeObject* type, void* context) -> PyObject* {
        if (type->tp_flags & Py_TPFLAGS_HEAPTYPE) {
            PyHeapTypeObject* et = (PyHeapTypeObject*)type;

            Py_INCREF(et->ht_name);
            return et->ht_name;
        } else {
            return PyUnicode_FromString(_PyType_Name(type));
        }
    };

    return type_name(type, nullptr);
}
#endif

/**
 * \class CTypeExtractor
 * \brief Extract the requested C numeric type from a Python object
 */
template <typename T>
class CTypeExtractor {
public:
    /// Constructor
    CTypeExtractor(const UserOptions& options)
        : m_replacements()
        , m_options(options)
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
        Buffer buffer;
        TextExtractor extractor(input, buffer);
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
                return replace_value(ReplaceType::TYPEERROR, input);
            } else if (overflow) {
                return replace_value(ReplaceType::OVERFLOW, input);
            } else {
                return replace_value(ReplaceType::FAIL, input);
            }
        }

        // For float types, attempt to replace NaN and INF if that is so desired.
        if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(value) && m_replacements.count(ReplaceType::NAN_) != 0) {
                return replace_value(ReplaceType::NAN_, input);
            } else if (std::isinf(value) && m_replacements.count(ReplaceType::INF) != 0) {
                return replace_value(ReplaceType::INF, input);
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
        add_replacement_to_mapping(ReplaceType::INF, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when a parsing error occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_fail_replacement(PyObject* replacement)
    {
        add_replacement_to_mapping(ReplaceType::FAIL, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when an overflow occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_overflow_replacement(PyObject* replacement)
    {
        add_replacement_to_mapping(ReplaceType::OVERFLOW, replacement);
    }

    /**
     * \brief Define if the value needs to be replaced when a type error occurs
     * \param replacement The Python object to use to replace the value
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    void set_type_error_replacement(PyObject* replacement)
    {
        add_replacement_to_mapping(ReplaceType::TYPEERROR, replacement);
    }

private:
    /// Represent reasons to replace a value
    enum class ReplaceType {
        INF,
        NAN_,
        FAIL,
        OVERFLOW,
        TYPEERROR,
    };

    /// Store values that could replace troublesome input
    std::unordered_map<ReplaceType, std::pair<T, PyObject*>> m_replacements;

    /// Store string representations of the replacement types
    const std::unordered_map<ReplaceType, const char*> m_replace_repr {
        { ReplaceType::INF, "inf" },
        { ReplaceType::NAN_, "nan" },
        { ReplaceType::FAIL, "on_fail" },
        { ReplaceType::OVERFLOW, "on_overflow" },
        { ReplaceType::TYPEERROR, "on_type_error" },
    };

    /// Hold the evaluator options
    UserOptions m_options;

private:
    /**
     * \brief Replace the given input in the user-specified method
     * \param key The key to use to look up the appropriate replacement method
     * \param input The Python object that triggered the need for a replacement
     * \return The C number after replacement
     * \throw exception_is_set If a Python exception is set and needs to be raised
     */
    T replace_value(ReplaceType key, PyObject* input)
    {
        // Locate the value in the mapping for this key.
        const auto key_value_pair = m_replacements.find(key);

        // If the key/value pair was not found in the replacement mapping,
        // that means an error must be raised for this input.
        if (key_value_pair == m_replacements.end()) {
            if (key == ReplaceType::FAIL) {
                PyErr_Format(
                    PyExc_ValueError,
                    "Cannot convert %.200R to C type '%s'",
                    input,
                    type_name<T>()
                );
            } else if (key == ReplaceType::OVERFLOW) {
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
        }

        // Extract the values for use below
        const T number_value = key_value_pair->second.first;
        PyObject* callable_value = key_value_pair->second.second;

        // If the replacement value has no callable attached
        // then return a default value directly.
        if (callable_value == nullptr) {
            return number_value;
        }

        // If here, we have to call a Python function then
        // extract a number from the result.
        PyObject* retval = PyObject_CallFunctionObjArgs(callable_value, input, nullptr);

        // On exception, no need to define our our own message,
        // I'm sure Python's is just fine.
        if (retval == nullptr) {
            throw exception_is_set();
        }

        // If there was no error calling the function, attempt to extract the C type.
        NumericParser parser(retval, m_options);
        const T call_value = parser.as_number<T>();

        // Check for more errors, and if we pass them then return the value.
        if (parser.errored()) {
            if (parser.type_error()) {
                PyObject* type_name = PyType_GetName(Py_TYPE(input));
                PyErr_Format(
                    PyExc_TypeError,
                    "Callable passed to '%s' with input %.200R returned the value "
                    "%.200R that has type %.200R which cannot be converted to a "
                    "numeric value",
                    m_replace_repr.at(key),
                    input,
                    retval,
                    type_name
                );
                Py_DECREF(type_name);
            } else if (parser.overflow()) {
                PyErr_Format(
                    PyExc_OverflowError,
                    "Callable passed to '%s' with input %.200R returned the value "
                    "%.200R that cannot be converted to C type '%s' without "
                    "overflowing",
                    m_replace_repr.at(key),
                    input,
                    retval,
                    type_name<T>()
                );
            } else {
                PyErr_Format(
                    PyExc_ValueError,
                    "Callable passed to '%s' with input %.200R returned the value "
                    "%.200R that cannot be converted to C type '%s'",
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
            m_replacements[key] = std::make_pair(T(), replacement);
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
        m_replacements[key] = std::make_pair(value, nullptr);
    }
};
