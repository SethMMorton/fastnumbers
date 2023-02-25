#pragma once

#include <limits>
#include <type_traits>

#include <Python.h>

#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class UnicodeParser
 * \brief Parses single unicode characters for numeric properties
 */
class UnicodeParser final : public Parser {
public:
    /// Construct with a single unicode character and sign
    UnicodeParser(
        const Py_UCS4 uchar, const bool negative, const UserOptions& options
    ) noexcept
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

    /// Convert the stored object to a python int
    RawPayload<PyObject*> as_pyint() const noexcept(false) override
    {
        if (get_number_type() & NumberType::Integer) {
            return PyLong_FromLong(m_digit);
        }
        return ErrorType::BAD_VALUE;
    }

    /**
     * \brief Convert the stored object to a python float but possibly
     *        coerce to an integer
     * \param force_int Force the output to integer (takes precidence)
     * \param coerce Return as integer if the float is int-like
     */
    RawPayload<PyObject*>
    as_pyfloat(const bool force_int = false, const bool coerce = false) const
        noexcept(false) override
    {
        const NumberFlags ntype = get_number_type();

        // Quit here if not a valid number
        if (!(ntype & (NumberType::Integer | NumberType::Float))) {
            return ErrorType::BAD_VALUE;
        }

        if (force_int) {
            return (ntype & NumberType::Integer) ? PyLong_FromLong(m_digit)
                                                 : PyLong_FromDouble(m_numeric);
        } else if (coerce) {
            return (ntype & NumberType::Integer)
                ? PyLong_FromLong(m_digit)
                : ((ntype & NumberType::IntLike) ? PyLong_FromDouble(m_numeric)
                                                 : PyFloat_FromDouble(m_numeric));
        } else {
            return (ntype & NumberType::Integer)
                ? PyFloat_FromDouble(static_cast<double>(m_digit))
                : PyFloat_FromDouble(m_numeric);
        }
    }

    /// Check the type of the number.
    NumberFlags get_number_type() const noexcept override
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

    /**
     * \brief Convert the contained value into a number C++
     *
     * This template specialization is for integral types.
     *
     * You will need to check for conversion errors and overflows.
     */
    template <typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
    RawPayload<T> as_number() const noexcept
    {
        if (get_number_type() & NumberType::Integer) {
            return cast_num_check_overflow<T>(m_digit);
        }
        return ErrorType::BAD_VALUE;
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
    RawPayload<T> as_number() const noexcept
    {
        const NumberFlags ntype = get_number_type();

        // Quit here if not a valid number
        if (!(ntype & (NumberType::Integer | NumberType::Float))) {
            return ErrorType::BAD_VALUE;
        }

        // Cast to the desired output type. No need to worry about overflow
        // when casting to floating point type, because too big will become
        // infinity and too small will become zero.
        return static_cast<T>((ntype & NumberType::Integer) ? m_digit : m_numeric);
    }

    /**
     * \brief Convert the contained value into a number C++
     *
     * You will need to check for conversion errors and overflows.
     */
    template <typename T>
    void as_number(RawPayload<T>& value) const noexcept
    {
        value = as_number<T>();
    }

private:
    /// The potential numeric value of a unicode character
    double m_numeric;

    /// The potential digit value of a unicode character
    long m_digit;

    /// Add FromUni to the return NumberFlags
    static constexpr NumberFlags flag_wrap(const NumberFlags val) noexcept
    {
        return NumberType::FromUni | val;
    }
};
