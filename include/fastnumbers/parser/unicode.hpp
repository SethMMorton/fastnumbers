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
        // Store the type of number that was found
        set_number_type(get_number_type());

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

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() override
    {
        reset_error();
        if (get_number_type() & NumberType::Integer) {
            return PyLong_FromLong(sign() * m_digit);
        }
        encountered_conversion_error();
        return nullptr;
    }

    /// Convert the stored object to a python float (check error state)
    PyObject* as_pyfloat() override
    {
        reset_error();

        const NumberFlags ntype = get_number_type();

        if (ntype & NumberType::Integer) {
            return PyFloat_FromDouble(static_cast<double>(sign() * m_digit));
        } else if (ntype & NumberType::Float) {
            return PyFloat_FromDouble(sign() * m_numeric);
        }

        encountered_conversion_error();
        return nullptr;
    }

    /// Check the type of the number.
    NumberFlags get_number_type() const override
    {
        // If this value is cached, use that instead of re-calculating
        static constexpr NumberFlags unset = NumberType::UNSET;
        if (Parser::get_number_type() != unset) {
            return Parser::get_number_type();
        }

        // Evaluate if the stored number is a float or int.
        // There are no single unicode representations for infinity
        // nor NaN so we do not have to check for that.
        if (m_digit > -1) {
            return flag_wrap(NumberType::Integer);
        } else if (m_numeric > -1.0) {
            if (Parser::float_is_intlike(m_numeric)) {
                return flag_wrap(NumberType::Float | NumberType::IntLike);
            } else {
                return flag_wrap(NumberType::Float);
            }
        }

        // If here, the object is not numeric.
        return NumberType::INVALID;
    }

    /// Check if the should be parsed as an integer
    bool peek_try_as_int() const override
    {
        return bool(get_number_type() & NumberType::Integer);
    }

private:
    /// The potential numeric value of a unicode character
    double m_numeric;

    /// The potential digit value of a unicode character
    long m_digit;

    /// Add FromUni to the return NumberFlags
    static constexpr NumberFlags flag_wrap(const NumberFlags val)
    {
        return NumberType::FromUni | val;
    }
};
