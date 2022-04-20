#pragma once

#include <cmath>
#include <cstring>
#include <vector>

#include <Python.h>

#include "fastnumbers/parser/base.hpp"

/**
 * \class CharacterParser
 * \brief Parses an ASCII character array for numeric properties
 */
class CharacterParser : public SignedParser {

public:
    /**
     * \brief Assign a character array to be parsed as a number
     * \param str The character array to be parsed
     * \param len The length of the character array
     */
    void set_input(const char* str, const std::size_t len);

    /// Construct with a single unicode character and sign
    CharacterParser(const char* str, const std::size_t len);

    // No default constructor
    CharacterParser() = delete;

    // Default copy/assignment and desctructor
    CharacterParser(const CharacterParser&) = default;
    CharacterParser(CharacterParser&&) = default;
    CharacterParser& operator=(const CharacterParser&) = default;
    ~CharacterParser() final = default;

    /// Convert the stored object to a long (-1 if not possible, check error state)
    long as_int() final;

    /// Convert the stored object to a double (-1.0 if not possible, check error state)
    double as_float() final;

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
};
