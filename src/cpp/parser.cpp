#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <Python.h>

#include "fastnumbers/buffer.hpp"
#include "fastnumbers/c_str_parsing.hpp"
#include "fastnumbers/helpers.hpp"
#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/parser/character.hpp"
#include "fastnumbers/parser/numeric.hpp"
#include "fastnumbers/parser/unicode.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/third_party/ipow.hpp"
#include "fastnumbers/user_options.hpp"

// C++ version of https://docs.python.org/3/library/math.html#math.ulp
static double ulp(const double x)
{
    // Assumes positive input
    return std::nexttoward(x, std::numeric_limits<double>::infinity()) - x;
}

// Parse a sequence of characters as a Python long
static PyObject* parse_long_helper(
    const char* start,
    const char* end,
    const std::size_t length,
    const std::size_t length_to_end
)
{
    // We know by construction that this correctly stores a number, so we skip
    // the error checking.
    if (length < overflow_cutoff<uint64_t>()) {
        bool error = false;
        bool overflow = false;
        return pyobject_from_int(
            length ? parse_int<uint64_t>(start, end, 10, error, overflow) : 0ULL
        );
    } else {
        // This Python parse won't let us specify the end of the string, and it errors
        // out on non-numeric characters, so we have to copy our data to a buffer
        // and set the exponent character to '\0' so that Python can parse it.
        Buffer buffer(start, length_to_end);
        buffer.mark_integer_end();
        return PyLong_FromString(buffer.start(), nullptr, 10);
    }
}

// Convert a value into a power of ten Python long in the most efficient method
// depending on the value of the exponent.
static PyObject* exponent_creation_helper(const uint32_t exp_val)
{
    if (exp_val < overflow_cutoff<uint64_t>()) {
        return pyobject_from_int(ipow::ipow(10ULL, exp_val));
    } else {
        PyObject* py_expon = pyobject_from_int(10);
        PyObject* py_exp_component = pyobject_from_int(exp_val);
        in_place_pow(py_expon, py_exp_component);
        Py_DECREF(py_exp_component);
        return py_expon;
    }
}

// Convert a PyObject to a negative number if needed.
static PyObject* do_negative(PyObject* obj, const bool negative)
{
    if (negative) {
        PyObject* temp = obj;
        obj = PyNumber_Negative(temp);
        Py_DECREF(temp);
    }
    return obj;
}

PyObject* Parser::float_as_int_without_noise(PyObject* obj) noexcept
{
    const double val = PyFloat_AsDouble(obj);
    if (val == -1.0 && PyErr_Occurred()) {
        return nullptr;
    }

    // To begin, get the absolute value of the input as a C++ double.
    // Also store the input as a Python int to use as the basis for calculation.
    const double abs_val = std::abs(val);
    PyObject* val_int = PyLong_FromDouble(val);
    if (val_int == nullptr) {
        return nullptr;
    }

    // If the given float can fit a C long without loss then no need
    // to go through the below rounding steps.
    const double floor_val = std::floor(val);
    if (floor_val == static_cast<long>(floor_val)) {
        return val_int;
    }

    // Determine the number of digits that are "noise". Do this by using ULP to
    // get the number that closest represents the noise inherint from the
    // floating point representation, then using log10 to find the number of
    // digits this represents.
    // Note that the largest exponent component of a double is around 308, so
    // we can be confident that this value will fit in an int.
    // There is special case hanlding for if the abs_val is the next lowest value
    // from infinity, in which case double_digits would be returned as infinity...
    // in that case, we return 293, which is known to be the correct result for
    // that number.
    const double double_digits = std::ceil(std::log10(ulp(abs_val)));
    constexpr int MAX_DIGITS = 293;
    const int digits
        = std::isfinite(double_digits) ? static_cast<int>(double_digits) : MAX_DIGITS;

    // If the number of digits is less than 1 (e.g. the float is already noiselessly
    // converted to an integer) just return the integer directly.
    // Because of the floor check above, it is unlikely this will ever be true, but
    // include it for completeness' sake.
    if (digits < 1) {
        return val_int;
    }

    // Use Python's built-in round to round the number to the desired number of
    // digits. A negative input rounds to the left of the decimal place.
    PyObject* retval = PyObject_CallMethod(val_int, "__round__", "i", -digits);

    // Free the int value, and then return the rounded number.
    Py_DecRef(val_int);
    return retval;
}

PyObject* Parser::float_as_int_without_noise(
    const StringChecker& checker, const bool is_negative
) noexcept
{
    PyObject* py_integer = nullptr;

    // As a special case, if the number of digits in the integer and decimal
    // parts of the float are small enough that we know it could fit into a unsigned
    // 64-bit integer, parse using C++ methods in the name of efficiency.
    if (checker.digit_length() < overflow_cutoff<uint64_t>()) {
        bool error = false;
        bool overflow = false;
        uint64_t integer = 0;

        // First parse the integer component (if there is anything to parse).
        if (checker.integer_length()) {
            integer = parse_int<uint64_t>(
                checker.integer_start(), checker.integer_end(), 10, error, overflow
            );
        }

        // Next, parse the decimal component (if there is anything to parse) and
        // then combine with the existing integer component to get a single integer.
        if (checker.truncated_decimal_length()) {
            uint64_t decimal = parse_int<uint64_t>(
                checker.decimal_start(), checker.decimal_end(), 10, error, overflow
            );
            // We "remove" trailing zeros from the decimal component by dividing by
            // the power of ten that corresponds to the number of trailing zeros.
            if (checker.decimal_trailing_zeros()) {
                decimal /= ipow::ipow(10ULL, checker.decimal_trailing_zeros());
            }
            // Add powers of 10 to the integer that correspond to the number of
            // decimal digits parser (minus trailing zeros) so that when the decimal
            // comppnent is added it results in a integer with all digits represented.
            integer *= ipow::ipow(10ULL, checker.truncated_decimal_length());
            integer += decimal;
        }

        // Convert this integer into a Python long object.
        py_integer = pyobject_from_int(integer);
    }

    // Otherwise, if there are too manu digits to store in a C++ integer type, we
    // need to use Python's arithmatic so we can take advantage of the arbitrarily
    // long integer type.
    else {
        // First parse the integer components of the floating point number.
        py_integer = parse_long_helper(
            checker.integer_start(),
            checker.integer_end(),
            checker.integer_length(),
            checker.total_length()
        );
        if (py_integer == nullptr) {
            return py_integer;
        }

        // We then parse the decimal components if they exist, adding the values to
        // the integer using the exact same algorithm as the C++ method above... it's
        // just that this has to use a bunch more error checking and reference counting.
        if (checker.truncated_decimal_length()) {
            // Parse the decimal component...
            PyObject* py_decimal = parse_long_helper(
                checker.decimal_start(),
                checker.decimal_end(),
                checker.decimal_length(),
                checker.decimal_and_exponent_length()
            );
            if (py_decimal == nullptr) {
                return py_decimal;
            }

            // ... and then "remove" trailing zeros if they exist...
            if (checker.decimal_trailing_zeros()) {
                PyObject* divisor
                    = exponent_creation_helper(checker.decimal_trailing_zeros());
                if (divisor == nullptr) {
                    return divisor;
                }

                in_place_divide(py_decimal, divisor);
                Py_DECREF(divisor);
                if (py_decimal == nullptr) {
                    return py_decimal;
                }
            }

            // ... and then "shift" the integer py powers of ten so we can
            //     add in the decimal component...
            {
                PyObject* offset
                    = exponent_creation_helper(checker.truncated_decimal_length());
                if (offset == nullptr) {
                    Py_DECREF(py_integer);
                    return offset;
                }

                in_place_multiply(py_integer, offset);
                Py_DECREF(offset);
                if (py_integer == nullptr) {
                    return py_integer;
                }
            }

            // ... and finally add in the decimal component.
            in_place_add(py_integer, py_decimal);
            Py_DECREF(py_decimal);
        }
    }

    // Error check this integer result.
    if (py_integer == nullptr) {
        return py_integer;
    }

    // If there is an exponent value (ajusted by the number of decimal digits that were
    // present) then we have to multiply our integer by this value raised to the power
    // of ten. Because this value could be *huge*, we cannot do power nor the
    // mutliplication in the C++ space - it must be all done with Python's types.
    if (checker.adjusted_exponent_value() > 0) {
        PyObject* py_expon = exponent_creation_helper(checker.adjusted_exponent_value());
        if (py_expon == nullptr) {
            Py_DECREF(py_integer);
            return py_expon;
        }

        // We are carrying around the exponent as an unsigned integer,
        // so we handle negatives by dividing, and positives by multiplying.
        if (checker.is_exponent_negative()) {
            in_place_divide(py_integer, py_expon);
        } else {
            in_place_multiply(py_integer, py_expon);
        }
        Py_DECREF(py_expon);
    }

    // Finally, we negate the result if it is supposed to be negative.
    return do_negative(py_integer, is_negative);
}

/**
 * \brief Remove whitespace at the end of a string
 *
 * To do this successfully, move the end pointer
 * back one to not be on the '\0' character, move
 * backwards, then push the end pointer up one
 * as if there were still a '\0' character.
 *
 * \param start The beginning of the string to strip
 * \param end The end of the string to strip - updated in-place
 */
inline void strip_trailing_whitespace(const char* start, const char*& end) noexcept
{
    while (start < end && is_whitespace(*(end - 1))) {
        end -= 1;
    }
}

CharacterParser::CharacterParser(
    const char* str,
    const std::size_t len,
    const UserOptions& options,
    const bool explict_base_allowed
) noexcept
    : Parser(ParserType::CHARACTER, options, explict_base_allowed)
    , m_start(str)
    , m_start_orig(str)
    , m_end_orig(str + len)
    , m_str_len(0)
{
    // Store the end point of the character array
    const char* end = m_end_orig;

    // Strip leading whitespace
    while (is_whitespace(*m_start)) {
        m_start += 1;
    }

    // Strip trailing whitespace.
    strip_trailing_whitespace(m_start, end);

    // Remove the sign if present and remember what it represents
    if (*m_start == '+') {
        m_start += 1;
    } else if (*m_start == '-') {
        m_start += 1;
        set_negative();
    }

    // We can only have a sign here if there are two consecutive signs.
    // Two or more signs is illegal - let's treat it as such.
    // Reset the start to before the first sign.
    // All parsers will treat this as illegal now.
    if (is_sign(*m_start)) {
        m_start -= 1;
        set_negative(false);
    }

    // Calculate the length of the string after accounting for
    // whitespace and signs
    m_str_len = static_cast<std::size_t>(end - m_start);
}

RawPayload<PyObject*> CharacterParser::as_pyint() const noexcept(false)
{
    // We use the fast path method even if the result overflows,
    // so that we can determine if the integer was at least valid.
    // If it was valid but overflowed, we use Python's parser, otherwise
    // return an error.
    // The only thing special handling we need is underscores or base prefixes
    // with negative signs that caused overflow.
    bool error;
    bool overflow;
    int64_t result = parse_int<int64_t>(
        signed_start(), end(), options().get_base(), error, overflow
    );
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
        result = parse_int<int64_t>(buffer.start(), buffer.end(), base, error, overflow);
    }
    if (error) {
        return ErrorType::BAD_VALUE;
    }
    if (!overflow) {
        return pyobject_from_int(result);
    }

    // Parse and record the location where parsing ended (including trailing
    // whitespace) No need to do input validation with the second argument because we
    // already know the input is valid from above. Return the value without checking
    // python's error state.
    return PyLong_FromString(m_start_orig, nullptr, options().get_base());
}

RawPayload<PyObject*>
CharacterParser::as_pyfloat(const bool force_int, const bool coerce) const
    noexcept(false)
{
    // If denoise is requested, use this algorithm *instead* of converting to a double
    if (options().do_denoise() && (force_int || coerce)) {
        StringChecker checker(m_start, end(), options().get_base());
        if (checker.is_intlike_float()) {
            return Parser::float_as_int_without_noise(checker, is_negative());
        } else if (checker.is_invalid() && has_valid_underscores()) {
            Buffer buffer(m_start, m_str_len);
            buffer.remove_valid_underscores(options().get_base() != 10);
            StringChecker checker(buffer.start(), buffer.end(), options().get_base());
            if (checker.is_intlike_float()) {
                return Parser::float_as_int_without_noise(checker, is_negative());
            }
        }
    }

    // Perform the correct action depending on the payload's contents
    return std::visit(
        overloaded {

            // If the payload contained a double, convert it to a PyObject*
            [force_int, coerce](const double result) -> RawPayload<PyObject*> {
                // force_int takes precidence,
                // and coerce conditionally returns as an integer
                if (force_int) {
                    return PyLong_FromDouble(result);
                } else if (coerce) {
                    return Parser::float_is_intlike(result) ? PyLong_FromDouble(result)
                                                            : PyFloat_FromDouble(result);
                } else {
                    return PyFloat_FromDouble(result);
                }
            },

            // If the payload contained an error, pass the error along
            [](const ErrorType err) -> RawPayload<PyObject*> {
                return err;
            },
        },
        as_number<double>()
    );
}

NumberFlags CharacterParser::get_number_type() const noexcept
{
    // If this value is cached, use that instead of re-calculating
    if (Parser::get_number_type() != static_cast<NumberFlags>(NumberType::UNSET)) {
        return Parser::get_number_type();
    }

    // If the string contains an infinity or NaN then we don't need to do any
    // other fancy processing and can return now.
    if (quick_detect_infinity(m_start, m_str_len)) {
        return flag_wrap(NumberType::Float | NumberType::Infinity);

    } else if (quick_detect_nan(m_start, m_str_len)) {
        return flag_wrap(NumberType::Float | NumberType::NaN);
    }

    // If the string contains a numeric representation,
    // report which representation type is contained.
    StringType value = StringChecker(m_start, end(), options().get_base()).get_type();

    // If it still looks like there is no numeric representation in the string,
    // check to see if it it contains underscores, and if so remove them and
    // try a numeric representation again. No need to check for infinity and
    // NaN here because those are not allowed to contain underscores.
    if (value == StringType::INVALID && has_valid_underscores()) {
        Buffer buffer(m_start, m_str_len);
        buffer.remove_valid_underscores(!options().is_default_base());
        value = StringChecker(buffer.start(), buffer.end(), options().get_base())
                    .get_type();
    }

    // Return the found type
    switch (value) {
    case StringType::INVALID:
        return NumberType::INVALID;
    case StringType::INTEGER:
        return flag_wrap(NumberType::Integer);
    case StringType::FLOAT:
        return flag_wrap(NumberType::Float);
    case StringType::INTLIKE_FLOAT:
        return flag_wrap(NumberType::Float | NumberType::IntLike);
    }

    /* Is not reachable, but silences compiler warnings. */
    return NumberType::INVALID;
}
