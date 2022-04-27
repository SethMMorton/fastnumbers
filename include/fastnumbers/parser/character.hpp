#pragma once

#include <cmath>
#include <cstring>
#include <vector>

#include <Python.h>

#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/buffer.hpp"

/**
 * \class CharacterParser
 * \brief Parses an ASCII character array for numeric properties
 */
class CharacterParser : public SignedParser {

public:
    /// Construct with a single unicode character and sign
    CharacterParser(
        const char* str, const std::size_t len, const bool explict_base_allowed = true
    );

    // No default constructor
    CharacterParser() = delete;

    // Default copy/assignment and desctructor
    CharacterParser(const CharacterParser&) = default;
    CharacterParser(CharacterParser&&) = default;
    CharacterParser& operator=(const CharacterParser&) = default;
    ~CharacterParser() final = default;

    /// Convert the stored object to a long (check error state)
    long as_int() final;

    /// Convert the stored object to a double (check error state)
    double as_float() final;

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() final;

    /// Convert the stored object to a python float (check error state)
    PyObject* as_pyfloat() final;

    /// Was the passed Python object infinity?
    bool is_infinity() const final;

    /// Was the passed Python object NaN?
    bool is_nan() const final;

    /// Was the passed Python object real (e.g. float or int)?
    bool is_real() const final { return is_float(); }

    /// Was the passed Python object a float?
    bool is_float() const final;

    /// Was the passed Python object an int?
    bool is_int() const final;

    /**
     * \brief Was the passed Python object intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    bool is_intlike() const final;

    /// Define whether or not underscores are allowed
    void set_allow_underscores(const bool val) { m_underscore_allowed = val; }

    /// Are underscores allowed?
    bool are_underscores_allowed() const { return m_underscore_allowed; }

private:
    /// The potential start of the character array
    const char* m_start;

    /// The original end of the character array
    const char* m_end_orig;

    /// The potential length of the character array
    std::size_t m_str_len;

    /// Whether or not underscores are allowed when parsing
    bool m_underscore_allowed;

private:
    /// Check if the character array contains valid underscores
    bool has_valid_underscores() const
    {
        return m_start != nullptr && are_underscores_allowed() && m_str_len > 0
            && std::memchr(m_start, '_', m_str_len);
    }

    /// Check if the character array contains invalid underscores
    bool has_invalid_underscores() const
    {
        return m_start != nullptr && !are_underscores_allowed() && m_str_len > 0
            && std::memchr(m_start, '_', m_str_len);
    }

    /// The end of the stored character array
    const char* end() const
    {
        return m_start == nullptr ? nullptr : (m_start + m_str_len);
    }

    /// Template for conversion to base types
    template <
        typename T,
        typename CheckFunction,
        typename OverflowCheckFunction,
        typename ConvertFunction>
    T as_type(
        CheckFunction check_function,
        OverflowCheckFunction overflow_check_function,
        ConvertFunction convert_function
    );

    /// Template for checking that in input contains a type, accounting for underscores
    template <typename Function>
    bool check_string_for_number(
        Buffer& buffer,
        const char*& start,
        const char*& new_end,
        const std::size_t offset,
        Function string_contains_number
    );
};
