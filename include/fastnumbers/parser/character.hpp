#pragma once

#include <cmath>
#include <cstring>
#include <vector>

#include <Python.h>

#include "fastnumbers/buffer.hpp"
#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/parser/base.hpp"
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

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() override;

    /// Convert the stored object to a python float (check error state)
    PyObject* as_pyfloat() override;

    /**
     * \brief Convert the stored object to a python float but possible
     *        coerce to an integer (check error state)
     * \param force_int Force the output to integer (takes precidence)
     * \param coerce Return as integer if the float is int-like
     */
    PyObject* as_pyfloat(const bool force_int, const bool coerce) override;

    /// Check the type of the number.
    NumberFlags get_number_type() const override;

    /// Check if the number is INF
    bool peek_inf() const override { return quick_detect_infinity(m_start, m_str_len); }

    /// Check if the number is NaN
    bool peek_nan() const override { return quick_detect_nan(m_start, m_str_len); }

    /// Check if the should be parsed as an integer
    bool peek_try_as_int() const override
    {
        // Attempt to simply see if all the characters are digits.
        // If so it is an integer.
        const char* str = m_start;
        consume_digits(str, m_str_len);
        return str != m_start && str == (m_start + m_str_len);
    }

private:
    /// The potential start of the character array
    const char* m_start;

    /// The original start of the character array
    const char* m_start_orig;

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

    /// The string as a double (check error state)
    double as_double();

    /// Add FromStr to the return NumberFlags
    static constexpr NumberFlags flag_wrap(const NumberFlags val)
    {
        return NumberType::FromStr | val;
    }
};
