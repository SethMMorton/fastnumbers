#pragma once

#include <Python.h>

#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class UnicodeParser
 * \brief Parses single unicode characters for numeric properties
 */
class UnicodeParser final : public Parser {
public:
    /// Construct with a single unicode character and sign
    UnicodeParser(const Py_UCS4 uchar, const bool negative, const UserOptions& options)
        : Parser(ParserType::UNICODE, options)
        , m_numeric(Py_UNICODE_TONUMERIC(uchar))
        , m_digit(Py_UNICODE_TODIGIT(uchar))
    {
        // Evaluate if the stored number is a float or int
        if (m_digit > -1) {
            set_as_int_type();
        } else if (m_numeric > -1.0) {
            set_as_float_type();
        }

        // Store the sign
        set_negative(negative);
    }

    // No default constructor
    UnicodeParser() = delete;

    // Default copy/assignment and desctructor
    UnicodeParser(const UnicodeParser&) = default;
    UnicodeParser(UnicodeParser&&) = default;
    UnicodeParser& operator=(const UnicodeParser&) = default;
    ~UnicodeParser() = default;

    /// Convert the stored object to a long (check error state)
    long as_int() override
    {
        reset_error();

        if (Parser::is_int()) {
            return sign() * m_digit;
        }
        encountered_conversion_error();
        return -1L;
    }

    /// Convert the stored object to a double (check error state)
    double as_float() override
    {
        reset_error();

        if (Parser::is_int()) {
            return static_cast<double>(sign() * m_digit);
        } else if (Parser::is_float()) {
            return sign() * m_numeric;
        }
        encountered_conversion_error();
        return -1.0;
    }

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() override
    {
        reset_error();

        const long value = as_int();
        if (!errored()) {
            return PyLong_FromLong(value);
        }
        encountered_conversion_error();
        return nullptr;
    }

    /// Convert the stored object to a python float (check error state)
    PyObject* as_pyfloat() override
    {
        reset_error();

        const double value = as_float();
        if (!errored()) {
            return PyFloat_FromDouble(value);
        }
        encountered_conversion_error();
        return nullptr;
    }

    /// Was the passed Python object a float?
    bool is_float() const override { return Parser::is_float() || Parser::is_int(); }

    /**
     * \brief Was the passed Python object intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    bool is_intlike() const override
    {
        return Parser::is_int()
            || (Parser::is_float() && Parser::float_is_intlike(m_numeric));
    }

private:
    /// The potential numeric value of a unicode character
    double m_numeric;

    /// The potential digit value of a unicode character
    long m_digit;
};
