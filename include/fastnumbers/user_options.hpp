#pragma once

#include <limits>

class UserOptions final {
public:
    UserOptions()
        : m_base(10)
        , m_default_base(true)
        , m_underscore_allowed(false)
        , m_coerce(false)
        , m_nan_allowed(false)
        , m_inf_allowed(false)
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
    void set_nan_allowed(const bool nan_allowed) { m_nan_allowed = nan_allowed; }

    /// Indicate if "NaN" is a valid input string
    bool allow_nan() const { return m_nan_allowed; }

    /// Tell the analyzer if infinity is allowed when type checking
    void set_inf_allowed(const bool inf_allowed) { m_inf_allowed = inf_allowed; }

    /// Indicate if "inf" is a valid input string
    bool allow_inf() const { return m_inf_allowed; }

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

    /// Whether or not an NaN is allowed
    bool m_nan_allowed;

    /// Whether or not an infinity is allowed
    bool m_inf_allowed;

    /// Whether or not a unicode character is allowed
    bool m_unicode_allowed;
};