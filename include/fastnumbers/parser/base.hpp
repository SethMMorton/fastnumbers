#pragma once

#include <cmath>
#include <cstring>
#include <stdexcept>

#include <Python.h>

#include "fastnumbers/third_party/EnumClass.h"
#include "fastnumbers/user_options.hpp"

/// Possible types of Parser objects
enum class ParserType {
    NUMERIC, ///< The parser is handling numeric Python objects
    UNICODE, ///< The parser is handling single unicode characters
    CHARACTER, ///< The parser is handling C-style character arrays
    UNKNOWN, ///< The incoming object unknown to the parser
};

enum class NumberType : unsigned {
    UNSET = 0U << 0,
    INVALID = 1U << 0,
    Integer = 1U << 1,
    Float = 1U << 2,
    NaN = 1U << 3,
    Infinity = 1U << 4,
    IntLike = 1U << 5,
    User = 1U << 6,
    FromStr = 1U << 7,
    FromUni = 1U << 8,
    FromNum = 1U << 9,
};
enableEnumClassBitmask(NumberType);
using NumberFlags = bitmask<NumberType>;

/**
 * \class Parser
 * \brief Flexible parser of various possible Python input types
 *
 * Parses the input as a number. Intended to be used as a base class
 * for more specialized parsers.
 */
class Parser {
public:
    // Constructors, destructors, and assignment
    // There is no constructor with arguments
    Parser(const UserOptions& options)
        : Parser(ParserType::UNKNOWN, options)
    { }
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    virtual ~Parser() = default;

    /// What type of parser is this?
    ParserType parser_type() const { return m_ptype; };

    /// Whether the last conversion encountered an error
    bool errored() const { return m_error_type != ErrorType::NONE; }

    /// Whether the last conversion potentially had an overflow
    bool potential_overflow() const
    {
        return m_error_type == ErrorType::POTENTIAL_OVERFLOW;
    }

    /// Was an explict base given illegally?
    bool illegal_explicit_base() const
    {
        return !is_explict_base_allowed() && !options().is_default_base();
    }

    /// Is the stored number negative?
    virtual bool is_negative() const { return m_negative; }

    /// Access the user-given options for parsing
    const UserOptions& options() const { return m_options; }

    /// Convert the stored object to a python int (check error state)
    virtual PyObject* as_pyint() = 0;

    /// Convert the stored object to a python float (check error state)
    virtual PyObject* as_pyfloat() = 0;

    /**
     * \brief Convert the stored object to a python float but possible
     *        coerce to an integer (check error state)
     * \param force_int Force the output to integer (takes precidence)
     * \param coerce Return as integer if the float is int-like
     */
    virtual PyObject* as_pyfloat(const bool force_int, const bool coerce) = 0;

    /// Check the type of the number.
    virtual NumberFlags get_number_type() const { return m_number_type; }

    /// Check if the number is INF
    virtual bool peek_inf() const { return false; }

    /// Check if the number is NaN
    virtual bool peek_nan() const { return false; }

    /// Check if the should be parsed as an integer
    virtual bool peek_try_as_int() const
    {
        return bool(get_number_type() & NumberType::Integer);
    }

    /**
     * \brief Determine if a float is "intlike"
     *
     * This is defined as a float that can be converted to an int with
     * no information loss.
     */
    static bool float_is_intlike(const double x)
    {
        errno = 0;
        return std::isfinite(x) && std::floor(x) == x && errno == 0;
    }

protected:
    /// Constructor for use only by base-classes to define the parser type
    /// and base requirements
    Parser(
        const ParserType ptype,
        const UserOptions& options,
        const bool explicit_base_allowed = false
    )
        : m_ptype(ptype)
        , m_number_type(NumberType::UNSET)
        , m_error_type(ErrorType::NONE)
        , m_negative(false)
        , m_explicit_base_allowed(explicit_base_allowed)
        , m_options(options)
    { }

    /// Define this parser as "unknown"
    void set_as_unknown_parser() { m_ptype = ParserType::UNKNOWN; }

    /// Cache the number type
    void set_number_type(const NumberFlags ntype) { m_number_type = ntype; }

    /// Record that the conversion encountered an error
    void encountered_conversion_error() { m_error_type = ErrorType::CANNOT_PARSE; }

    /// Record that the conversion encountered a potential overflow
    void encountered_potential_overflow_error()
    {
        m_error_type = ErrorType::POTENTIAL_OVERFLOW;
    }

    /// Reset the error state to "no error"
    void reset_error() { m_error_type = ErrorType::NONE; }

    /// Define whether an explicitly give base is allowed for integers
    void set_explict_base_allowed(bool explicit_base_allowed)
    {
        m_explicit_base_allowed = explicit_base_allowed;
    }

    /// Is an explicitly give base allowed for integers?
    bool is_explict_base_allowed() const { return m_explicit_base_allowed; }

    /// Toggle whether the store value is negative or not
    void set_negative(const bool negative = true) { m_negative = negative; }

    /// Integer that can be used to apply the sign of the number in the text
    int sign() const { return is_negative() ? -1 : 1; }

private:
    /// The type of the parser
    ParserType m_ptype;

    /// Tracker of what type is being stored
    NumberFlags m_number_type;

    /// The types of errors this class can encounter
    enum ErrorType {
        NONE,
        CANNOT_PARSE,
        POTENTIAL_OVERFLOW,
    };

    /// Tracker of what error is being stored
    ErrorType m_error_type;

    /// Whether or not text is a negative number
    bool m_negative;

    /// Whether or not explicit base are allowed when parsing
    bool m_explicit_base_allowed;

    /// Hold the parser options
    UserOptions m_options;
};
