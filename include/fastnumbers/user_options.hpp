#pragma once

#include <limits>

#include <Python.h>

#include "fastnumbers/selectors.hpp"

/// The conversion the user has requested
enum class UserType {
    REAL, ///< Convert to/check a real
    FLOAT, ///< Convert to/check a float
    INT, ///< Convert to/check an int
    INTLIKE, ///< Check int-like
    FORCEINT, ///< Force conversion to int
};

/**
 * \class UserOptions
 * \brief Container for options the user specifies that affect evaluation
 */
class UserOptions final {
public:
    UserOptions()
        : m_base(10)
        , m_default_base(true)
        , m_underscore_allowed(false)
        , m_coerce(false)
        , m_nan_allowed_str(false)
        , m_nan_allowed_num(false)
        , m_inf_allowed_str(false)
        , m_inf_allowed_num(false)
        , m_unicode_allowed(true)
    { }
    UserOptions(const UserOptions&) = default;
    UserOptions(UserOptions&&) = default;
    UserOptions& operator=(const UserOptions&) = default;
    ~UserOptions() = default;

    /// Tell the analyzer the base to use when parsing ints
    void set_base(const int base)
    {
        m_default_base = base == std::numeric_limits<int>::min();
        m_base = m_default_base ? 10 : base;
    }

    /// Get the stored base
    int get_base() const { return m_base; }

    /// Was the default base given?
    bool is_default_base() const { return m_default_base; }

    /// Define whether or not underscores are allowed
    void set_underscores_allowed(const bool val) { m_underscore_allowed = val; }

    /// Are underscores allowed?
    bool allow_underscores() const { return m_underscore_allowed; }

    /// Tell the analyzer whether or not to coerce to int for REAL
    void set_coerce(const bool coerce) { m_coerce = coerce; }

    /// Indicate if we are allowing coersion of floats to ints
    bool allow_coerce() const { return m_coerce; }

    /// Tell the analyzer if NaN is allowed when type checking
    void set_nan_allowed(const bool nan_allowed)
    {
        m_nan_allowed_num = m_nan_allowed_str = nan_allowed;
    }

    /// Tell the analyzer if NaN is allowed when type checking
    void set_nan_allowed(const PyObject* selector)
    {
        set_nan_allowed_num(
            selector == Selectors::ALLOWED || selector == Selectors::NUMBER_ONLY
        );
        set_nan_allowed_str(
            selector == Selectors::ALLOWED || selector == Selectors::STRING_ONLY
        );
    }

    /// Tell the analyzer if NaN is allowed for strings when type checking
    void set_nan_allowed_str(const bool nan_allowed) { m_nan_allowed_str = nan_allowed; }

    /// Tell the analyzer if NaN is allowed for numbers when type checking
    void set_nan_allowed_num(const bool nan_allowed) { m_nan_allowed_num = nan_allowed; }

    /// Indicate if "NaN" is a valid input string
    bool allow_nan_str() const { return m_nan_allowed_str; }

    /// Indicate if "NaN" is a valid input number
    bool allow_nan_num() const { return m_nan_allowed_num; }

    /// Tell the analyzer if infinity is allowed when type checking
    void set_inf_allowed(const bool inf_allowed)
    {
        m_inf_allowed_num = m_inf_allowed_str = inf_allowed;
    }

    /// Tell the analyzer if infinity is allowed when type checking
    void set_inf_allowed(const PyObject* selector)
    {
        set_inf_allowed_num(
            selector == Selectors::ALLOWED || selector == Selectors::NUMBER_ONLY
        );
        set_inf_allowed_str(
            selector == Selectors::ALLOWED || selector == Selectors::STRING_ONLY
        );
    }

    /// Tell the analyzer if infinity is allowed for strings when type checking
    void set_inf_allowed_str(const bool inf_allowed) { m_inf_allowed_str = inf_allowed; }

    /// Tell the analyzer if infinity is allowed for numbers when type checking
    void set_inf_allowed_num(const bool inf_allowed) { m_inf_allowed_num = inf_allowed; }

    /// Indicate if "inf" is a valid input string
    bool allow_inf_str() const { return m_inf_allowed_str; }

    /// Indicate if "inf" is a valid input number
    bool allow_inf_num() const { return m_inf_allowed_num; }

    /// Tell the analyzer if unicode characters are allowed as input
    void set_unicode_allowed(const bool unicode_allowed)
    {
        m_unicode_allowed = unicode_allowed;
    }

    /// Indicate if we allow non-ASCII unicode characters as input
    bool allow_unicode() const { return m_unicode_allowed; }

private:
    /// The desired base of integers when parsing
    int m_base;

    /// If the user-given base is the default base
    bool m_default_base;

    /// Whether or not underscores are allowed when parsing
    bool m_underscore_allowed;

    /// Whether or not floats should be coerced to integers if user wants REAL
    bool m_coerce;

    /// Whether or not an NaN is allowed for strings
    bool m_nan_allowed_str;

    /// Whether or not an NaN is allowed for numbers
    bool m_nan_allowed_num;

    /// Whether or not an infinity is allowed for strings
    bool m_inf_allowed_str;

    /// Whether or not an infinity is allowed for numbers
    bool m_inf_allowed_num;

    /// Whether or not a unicode character is allowed
    bool m_unicode_allowed;
};