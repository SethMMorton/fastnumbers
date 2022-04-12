#pragma once

#include <climits>

#include <Python.h>

#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"

/// The conversion the user has requested
enum class UserType {
    REAL, ///< Convert to/check a real
    FLOAT, ///< Convert to/check a float
    INT, ///< Convert to/check an int
    INTLIKE, ///< Check int-like
    FORCEINT, ///< Force conversion to int
};

/**
 * \class Evaluator
 * \brief Evaluate the contents of a Python object
 */
class Evaluator {
public:
    /// Constructor from a Python object
    explicit Evaluator(PyObject* obj)
        : m_obj(obj)
        , m_coerce(false)
        , m_nan_allowed(false)
        , m_inf_allowed(false)
        , m_unicode_allowed(true)
        , m_parser()
        , m_char_buffer(nullptr)
    {
        evaluate_stored_object();
    }

    // Other constructors, destructors, and assignment
    Evaluator()
        : Evaluator(nullptr)
    { }
    Evaluator(const Evaluator&) = delete;
    Evaluator(Evaluator&&) = delete;
    Evaluator& operator=(const Evaluator&) = delete;
    ~Evaluator()
    {
        Py_XDECREF(m_obj);
        delete m_char_buffer;
    }

    /// Assign a new object to analyze
    void set_object(PyObject* obj)
    {
        m_obj = obj;
        evaluate_stored_object();
    }

    /// Tell the analyzer the base to use when parsing ints
    void set_base(const int base) { m_parser.set_base(base); }

    /// Tell the analyzer whether or not to coerce to int for REAL
    void set_coerce(const bool coerce) { m_coerce = coerce; }

    /// Tell the analyzer if NaN is allowed when type checking
    void set_nan_allowed(const bool nan_allowed) { m_nan_allowed = nan_allowed; }

    /// Tell the analyzer if infinity is allowed when type checking
    void set_inf_allowed(const bool inf_allowed) { m_inf_allowed = inf_allowed; }

    /// Tell the analyzer if unicode characters are allowed as input
    void set_unicode_allowed(const bool unicode_allowed)
    {
        m_unicode_allowed = unicode_allowed;
    }

    /// Return the base used for integer conversion
    int get_base() const { return m_parser.get_base(); }

    /// Was the default base given?
    bool is_default_base() const { return m_parser.is_default_base(); }

    /// Tell the analyzer whether or not underscores are allowed when parsing
    void set_underscores_allowed(const bool underscores_allowed)
    {
        m_parser.set_allow_underscores(underscores_allowed);
    }

    /// Return the parser type currenly associated with the Evaluator
    ParserType parser_type() const { return m_parser.parser_type(); }

    /// Was the passed Python object of the correct type?
    bool is_type(const UserType ntype) const;

    /// Is the stored type a float? Account for nan_action and inf_action.
    bool type_is_float() const
    {
        return (m_nan_allowed && m_parser.is_nan())
            || (m_inf_allowed && m_parser.is_infinity()) || m_parser.is_float();
    }

    /// Is the stored type an integer? If coerce is true, is the type intlike?
    bool type_is_int() const
    {
        return m_coerce ? m_parser.is_intlike() : m_parser.is_int();
    }

    /**
     * \brief Convert the stored object to the desired number type
     *
     * Use the appropriate error handling on error.
     *
     * \param ntype PyNumberType indicating the desired type to check
     * \return Payload
     */
    Payload as_type(const UserType ntype);

private:
    /// The Python object under evaluation
    PyObject* m_obj;

    /// Whether or not floats should be coerced to integers if user wants REAL
    bool m_coerce;

    /// Whether or not an NaN is allowed
    bool m_nan_allowed;

    /// Whether or not an infinity is allowed
    bool m_inf_allowed;

    /// Whether or not a unicode character is allowed
    bool m_unicode_allowed;

    /// A Parser object used for evaluating the Python object
    Parser m_parser;

    /// Buffer object into which to store character data
    Buffer* m_char_buffer;

private:
    /// Generate a Parser object from Python object data
    void evaluate_stored_object()
    {
        if (m_obj != nullptr) {
            Py_IncRef(m_obj);
            m_parser.set_input(m_obj);
            if (m_parser.not_float_or_int() && !m_parser.is_special_numeric()) {
                extract_string_data();
            }
        }
    }

    /// Generate a text parsing Parser object
    void extract_string_data();

    /// Obtain character data from a Python unicode object
    bool extract_from_unicode();

    /// Obtain character data from a Python bytes object
    bool extract_from_bytes();

    /// Obtain character data from a Python bytearray object
    bool extract_from_bytearray();

    /// Obtain character data from a Python buffer object
    bool extract_from_buffer();

    /// Parse unicode data and convert to character data
    bool parse_unicode_to_char();

    /// Logic for evaluating a numeric python object
    Payload from_numeric_as_type(const UserType ntype);

    /// Logic for evaluating a text python object
    Payload from_text_as_type(const UserType ntype);

    /// Logic for evaluating a text python object as a float or integer
    Payload from_text_as_int_or_float(const bool force_int);

    /// Logic for evaluating a text python object as a float
    Payload from_text_as_float();

    /// Logic for evaluating a text python object as an int
    Payload from_text_as_int();
};
