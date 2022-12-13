#pragma once

#include <cmath>
#include <cstring>
#include <stdexcept>

#include <Python.h>

#include "fastnumbers/user_options.hpp"

/// Possible types of Parser objects
enum class ParserType {
    NUMERIC, ///< The parser is handling numeric Python objects
    UNICODE, ///< The parser is handling single unicode characters
    CHARACTER, ///< The parser is handling C-style character arrays
    UNKNOWN, ///< The incoming object unknown to the parser
};

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
    bool is_negative() const { return m_negative; }

    /// Access the user-given options for parsing
    const UserOptions& options() const { return m_options; }

    /// Convert the stored object to a long (check error state)
    /// Base implementation does nothing but throw a runtime error
    virtual long as_int() = 0;

    /// Convert the stored object to a double (check error state)
    /// Base implementation does nothing but throw a runtime error
    virtual double as_float() = 0;

    /// Convert the stored object to a python int (check error state)
    /// Base implementation does nothing but throw a runtime error
    virtual PyObject* as_pyint() = 0;

    /// Convert the stored object to a python float (check error state)
    /// Base implementation does nothing but throw a runtime error
    virtual PyObject* as_pyfloat() = 0;

    /// Was the passed Python object not float or int?
    virtual bool not_float_or_int() const
    {
        return m_number_type == NumberType::NOT_FLOAT_OR_INT;
    }

    /// Was the passed Python object finite?
    virtual bool is_finite() const { return is_real() && !(is_infinity() || is_nan()); }

    /// Was the passed Python object infinity?
    virtual bool is_infinity() const { return false; }

    /// Was the passed Python object NaN?
    virtual bool is_nan() const { return false; }

    /// Was the passed Python object real (e.g. float or int)?
    virtual bool is_real() const { return is_float() || is_int(); }

    /// Was the passed Python object a float?
    virtual bool is_float() const { return m_number_type == NumberType::FLOAT; }

    /// Was the passed Python object an int?
    virtual bool is_int() const { return m_number_type == NumberType::INT; }

    /**
     * \brief Was the passed Python object intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    virtual bool is_intlike() const { return is_int(); }

    /// Is the object is a a user-defined numeric class?
    virtual bool is_user_numeric() const { return false; }

    /// Is the object is a a user-defined numeric float?
    virtual bool is_user_numeric_float() const { return false; }

    /// Is the object is a a user-defined numeric int?
    virtual bool is_user_numeric_int() const { return false; }

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
        , m_number_type(NumberType::NOT_FLOAT_OR_INT)
        , m_error_type(ErrorType::NONE)
        , m_negative(false)
        , m_explicit_base_allowed(explicit_base_allowed)
        , m_options(options)
    { }

    /// Define this parser as "unknown"
    void set_as_unknown_parser() { m_ptype = ParserType::UNKNOWN; }

    /// The stored number type is "float"
    void set_as_float_type() { m_number_type = NumberType::FLOAT; }

    /// The stored number type is "int"
    void set_as_int_type() { m_number_type = NumberType::INT; }

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

    /// The types of data this class can store
    enum NumberType {
        NOT_FLOAT_OR_INT,
        FLOAT,
        INT,
    };

    /// Tracker of what type is being stored
    NumberType m_number_type;

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
