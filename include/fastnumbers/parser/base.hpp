#pragma once

#include <cmath>
#include <cstring>

#include <Python.h>

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
    Parser()
        : Parser(ParserType::UNKNOWN)
    { }
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    virtual ~Parser() = default;

    /// What type of parser is this?
    ParserType parser_type() const { return m_ptype; };

    /// Whether the last conversion encountered an error
    bool errored() const { return m_errcode != 0; }

    /// Whether the last conversion potentially had an overflow
    bool potential_overflow() const { return m_errcode == 2; }

    /// Tell the analyzer the base to use when parsing ints
    void set_base(const int base)
    {
        m_default_base = base == INT_MIN;
        m_base = m_default_base ? 10 : base;
    }

    /// Get the stored base
    int get_base() const { return m_base; }

    /// Was the default base given?
    bool is_default_base() const { return m_default_base; }

    /// Convert the stored object to a long (-1 if not possible, check error state)
    virtual long as_int()
    {
        encountered_conversion_error();
        return -1L;
    }

    /// Convert the stored object to a double (-1.0 if not possible, check error state)
    virtual double as_float()
    {
        encountered_conversion_error();
        return -1.0;
    }

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
    Parser(const ParserType ptype)
        : m_ptype(ptype)
        , m_number_type(NumberType::NOT_FLOAT_OR_INT)
        , m_errcode(0)
        , m_base(10)
        , m_default_base(true)
    { }

    /// Define this parser as "unknown"
    void set_as_unknown_parser() { m_ptype = ParserType::UNKNOWN; }

    /// The stored number type is "float"
    void set_as_float_type() { m_number_type = NumberType::FLOAT; }

    /// The stored number type is "int"
    void set_as_int_type() { m_number_type = NumberType::INT; }

    /// Record that the conversion encountered an error
    void encountered_conversion_error() { m_errcode = 1; }

    /// Record that the conversion encountered a potential overflow
    void encountered_potential_overflow_error() { m_errcode = 2; }

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

    /// Track if a number conversion failed.
    int m_errcode;

    /// The desired base of integers when parsing
    int m_base;

    /// If the user-given base is the default base
    bool m_default_base;
};

/**
 * \class SignedParser
 * \brief Flexible parser of various possible Python input where
 *        storing the sign as an independent value makes sense
 *
 * Intended to be used as a base class for more specialized parsers.
 */
class SignedParser : public Parser {
public:
    // Default copy/assignment/destructors
    SignedParser(const SignedParser&) = default;
    SignedParser(SignedParser&&) = default;
    SignedParser& operator=(const SignedParser&) = default;
    virtual ~SignedParser() = default;

    /// Toggle whether the store value is negative or not
    void set_negative(const bool negative = true) { m_negative = negative; }

    /// Is the stored number negative?
    bool is_negative() const { return m_negative; }

protected:
    /// Dispatch construction to the base class - only avalable to sub-classes
    SignedParser(const ParserType ptype)
        : Parser(ptype)
        , m_negative(false)
    { }

    // No default constructor
    SignedParser() = delete;

    /// Integer that can be used to apply the sign of the number in the text
    int sign() const { return m_negative ? -1 : 1; }

private:
    /// Whether or not the text is a negative number
    bool m_negative;
};
