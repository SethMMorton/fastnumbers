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

    /**
     * \brief Convert the stored object to a python float but possibly
     *        coerce to an integer (check error state)
     * \param force_int Force the output to integer (takes precidence)
     * \param coerce Return as integer if the float is int-like
     */
    PyObject*
    as_pyfloat(const bool force_int = false, const bool coerce = false) override;

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

    /**
     * \brief Convert the contained value into a number C++
     *
     * This template specialization is for integral types.
     *
     * You will need to check for conversion errors and overflows.
     */
    template <typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
    T as_number()
    {
        reset_error();

        bool error;
        bool overflow;
        constexpr bool always_convert = true;
        T result = parse_int<T>(
            signed_start(), end(), options().get_base(), error, overflow, always_convert
        );

        // If an error occured because of underscores or a pesky sign and base prefix
        // combo, remove them and re-parse
        const bool underscore_error = error && has_valid_underscores();
        const bool prefix_overflow = overflow && has_base_prefix(m_start, m_str_len);
        if (underscore_error || prefix_overflow) {
            Buffer buffer(signed_start(), signed_len());
            buffer.remove_valid_underscores(options().get_base() != 10);
            int base = options().get_base();
            if (base == 0) {
                base = detect_base(buffer.start(), buffer.end());
            }
            buffer.remove_base_prefix();
            result = parse_int<T>(
                buffer.start(), buffer.end(), base, error, overflow, always_convert
            );
        }

        // If there is still an error then it is real
        // We also will short-circuit overflows here
        if (error || overflow) {
            if (error) {
                encountered_conversion_error();
            } else {
                encountered_overflow();
            }
            return static_cast<T>(0);
        }

        // If here, we can just return the result
        return static_cast<T>(result);
    }

    /**
     * \brief Convert the contained value into a number C++
     *
     * This template specialization is for floating point types.
     *
     * You will need to check for conversion errors and overflows.
     */
    template <
        typename T,
        typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    T as_number()
    {
        reset_error();

        bool error;
        T result = parse_float<T>(signed_start(), end(), error);

        // If an error occured because of underscores, remove them and re-parse
        if (error && has_valid_underscores()) {
            Buffer buffer(signed_start(), signed_len());
            buffer.remove_valid_underscores();
            result = parse_float<T>(buffer.start(), buffer.end(), error);
        }

        // If there is still an error then it is real
        if (error) {
            encountered_conversion_error();
            return static_cast<T>(0.0);
        }

        // Return with the sign
        return static_cast<T>(result);
    }

    /**
     * \brief Convert the contained value into a number C++
     *
     * You will need to check for conversion errors and overflows.
     */
    template <typename T>
    void as_number(T& value)
    {
        value = as_number<T>();
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
        return options().allow_underscores() && m_str_len > 0
            && std::memchr(m_start, '_', m_str_len);
    }

    /// Check if the character array contains invalid underscores
    bool has_invalid_underscores() const
    {
        return !options().allow_underscores() && m_str_len > 0
            && std::memchr(m_start, '_', m_str_len);
    }

    /// The end of the stored character array
    const char* end() const { return m_start + m_str_len; }

    /// Return the start of the character array when accounting for '-'
    const char* signed_start() const
    {
        return m_start - static_cast<int>(is_negative());
    }

    /// Return the length of the character array when accounting for '-'
    const std::size_t signed_len() const
    {
        return m_str_len + static_cast<int>(is_negative());
    }

    /// Add FromStr to the return NumberFlags
    static constexpr NumberFlags flag_wrap(const NumberFlags val)
    {
        return NumberType::FromStr | val;
    }
};
