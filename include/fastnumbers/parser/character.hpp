#pragma once

#include <cmath>
#include <cstring>
#include <vector>

#include <Python.h>

#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/buffer.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class CharacterParser
 * \brief Parses an ASCII character array for numeric properties
 */
class CharacterParser final : public Parser {

public:
    /// Construct with a single unicode character and sign
    CharacterParser(
        const char* str,
        const std::size_t len,
        const UserOptions& options,
        const bool explict_base_allowed = true
    );

    // No default constructor
    CharacterParser() = delete;

    // Default copy/assignment and desctructor
    CharacterParser(const CharacterParser&) = default;
    CharacterParser(CharacterParser&&) = default;
    CharacterParser& operator=(const CharacterParser&) = default;
    ~CharacterParser() = default;

    /// Convert the stored object to a long (check error state)
    long as_int() override;

    /// Convert the stored object to a double (check error state)
    double as_float() override;

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() override;

    /// Convert the stored object to a python float (check error state)
    PyObject* as_pyfloat() override;

    /// Was the passed Python object infinity?
    bool is_infinity() const override;

    /// Was the passed Python object NaN?
    bool is_nan() const override;

    /// Was the passed Python object real (e.g. float or int)?
    bool is_real() const override { return is_float(); }

    /// Was the passed Python object a float?
    bool is_float() const override;

    /// Was the passed Python object an int?
    bool is_int() const override;

    /**
     * \brief Was the passed Python object intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    bool is_intlike() const override;

private:
    /// The potential start of the character array
    const char* m_start;

    /// The original end of the character array
    const char* m_end_orig;

    /// The potential length of the character array
    std::size_t m_str_len;

private:
    /// Check if the character array contains valid underscores
    bool has_valid_underscores() const
    {
        return m_start != nullptr && options().allow_underscores() && m_str_len > 0
            && std::memchr(m_start, '_', m_str_len);
    }

    /// Check if the character array contains invalid underscores
    bool has_invalid_underscores() const
    {
        return m_start != nullptr && !options().allow_underscores() && m_str_len > 0
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
