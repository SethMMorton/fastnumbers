#pragma once

#include <cmath>
#include <type_traits>

#include <Python.h>

#include "fastnumbers/parser/base.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class NumericParser
 * \brief Parses numeric python objects
 */
class NumericParser final : public Parser {
public:
    /// Construct with a Python object
    explicit NumericParser(PyObject* obj, const UserOptions& options)
        : Parser(ParserType::NUMERIC, options)
        , m_obj(obj)
    {
        // Store the type of number that was found
        const NumberFlags flags = get_number_type();
        set_number_type(flags);

        // Increment the reference count for this object
        Py_INCREF(m_obj);

        // Record the sign
        // XXX: We are cheating a bit here - we are only getting the sign
        // for simple floats because we *know* that that is the only time
        // the sign is used for numbers. Otherwise, we would have to spend
        // more effort getting the sign for all cases.
        if (flags & NumberType::Float && !(flags & NumberType::User)) {
            set_negative(get_double() < 0);
        }
    }

    // No default constructor
    NumericParser() = delete;

    // Default copy/assignment
    NumericParser(const NumericParser&) = default;
    NumericParser(NumericParser&&) = default;
    NumericParser& operator=(const NumericParser&) = default;

    /// Descructor decreases reference count of the stored object
    ~NumericParser() { Py_DECREF(m_obj); };

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() override
    {
        reset_error();
        return PyNumber_Long(m_obj);
    }

    /**
     * \brief Convert the stored object to a python float but possibly
     *        coerce to an integer (check error state)
     * \param force_int Force the output to integer (takes precidence)
     * \param coerce Return as integer if the float is int-like
     */
    PyObject*
    as_pyfloat(const bool force_int = false, const bool coerce = false) override
    {
        reset_error();
        if (force_int) {
            return PyNumber_Long(m_obj);
        } else if (coerce) {
            if (get_number_type() & (NumberType::IntLike | NumberType::Integer)) {
                return PyNumber_Long(m_obj);
            } else {
                return PyNumber_Float(m_obj);
            }
        } else {
            return PyNumber_Float(m_obj);
        }
    }

    /// Check the type of the number.
    NumberFlags get_number_type() const override
    {
        // If this value is cached, use that instead of re-calculating
        static constexpr NumberFlags unset = NumberType::UNSET;
        if (Parser::get_number_type() != unset) {
            return Parser::get_number_type();
        }

        // Return quickly if the object is strictly of float or long (int) type.
        if (PyFloat_Check(m_obj)) {
            return float_properties(get_double(), NumberType::Float);
        } else if (PyLong_Check(m_obj)) {
            return flag_wrap(NumberType::Integer);
        }

        // In addition to being strictly a float or long, an object can
        // behave like one of those types without being one by defining
        // the __float__, __int__, or __index__ dunder methods.
        // Check for these, and if so return the same as above but with
        // the additional "User" property.
        PyNumberMethods* nmeth = Py_TYPE(m_obj)->tp_as_number;
        if (nmeth != nullptr) {
            if (nmeth->nb_float != nullptr) {
                const double val = PyFloat_AsDouble(m_obj);
                if (val == -1.0 && PyErr_Occurred()) {
                    PyErr_Clear();
                    // Yes, an error occurred, but it still *looks* like a float.
                    // Attempting to retrieve the value will expose the error.
                    return flag_wrap(NumberType::Float | NumberType::User);
                }
                return float_properties(val, NumberType::Float | NumberType::User);

            } else if (nmeth->nb_index != nullptr || nmeth->nb_int != nullptr) {
                return flag_wrap(NumberType::Integer | NumberType::User);
            }
        }

        // If here, the object is not numeric.
        return NumberType::INVALID;
    }

    /**
     * \brief Convert the contained value into a number C++
     *
     * You will need to check for conversion errors and overflows.
     */
    template <typename T>
    T as_number()
    {
        // Special handling for floating point numbers
        if constexpr (std::is_floating_point_v<T>) {
            // Fast path if we know it is not numeric
            if (!(get_number_type() & (NumberType::Float | NumberType::Integer))) {
                encountered_conversion_error();
                return 0.0;
            }

            // Otherwise use the Python conversion function - this should handle
            // converting integers to double as well. Watch out for errors here too.
            const double value = PyFloat_AsDouble(m_obj);
            if (value == -1.0 && PyErr_Occurred()) {
                encountered_conversion_error();
                PyErr_Clear();
                return 0.0;
            }

            // Don't worry about overflow on casting to a smaller type because
            // too big will just become infinity, too small becomes zero.
            return static_cast<T>(value);
        } else {
            // Fast path if we know it is not numeric
            if (!(get_number_type() & NumberType::Integer)) {
                encountered_conversion_error();
                return 0;
            }

            // Use special logic for the largest types, otherwise use a generic logic.
            if constexpr (std::is_same_v<T, long long>) {
                return check_for_error_py<long long>(
                    m_obj, PyLong_AsLongLongAndOverflow
                );
            } else if constexpr (std::is_same_v<T, unsigned long long>) {
                return check_for_error_py(PyLong_AsUnsignedLongLong(m_obj));
            } else if constexpr (std::is_signed_v<T>) {
                return cast_num_check_overflow<T>(
                    check_for_error_py<long>(m_obj, PyLong_AsLongAndOverflow)
                );
            } else if constexpr (std::is_unsigned_v<T>) {
                return cast_num_check_overflow<T>(
                    check_for_error_py<unsigned long>(PyLong_AsUnsignedLong(m_obj))
                );
            } else {
                static_assert(
                    !std::is_integral_v<T>,
                    "invalid type given to NumericParser::as_number()"
                );
            }
        }
    }

private:
    /// The Python object potentially under analysis
    PyObject* m_obj;

private:
    /// Return the object as a double. No error checking is performed.
    double get_double() const { return PyFloat_AS_DOUBLE(m_obj); }

    /// Use the value of the float to add qualifiers
    /// if the float is infinite, NaN, or intlike.
    NumberFlags float_properties(const double val, NumberFlags props) const
    {
        if (std::isinf(val)) {
            props |= NumberType::Infinity;
        } else if (std::isnan(val)) {
            props |= NumberType::NaN;
        } else if (Parser::float_is_intlike(val)) {
            props |= NumberType::IntLike;
        }
        return flag_wrap(props);
    }

    /// Add FromNum to the return NumberFlags
    static constexpr NumberFlags flag_wrap(const NumberFlags val)
    {
        return NumberType::FromNum | val;
    }

    /// Helper for performing error checking
    template <typename T>
    T check_for_error_py(T value)
    {
        if (value == static_cast<T>(-1) && PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_OverflowError)) {
                encountered_overflow();
            } else {
                encountered_conversion_error();
            }
            PyErr_Clear();
            return static_cast<T>(0);
        }
        return value;
    }

    /// Helper for performing error checking
    template <typename T, typename Function>
    T check_for_error_py(PyObject* obj, Function func)
    {
        int overflow = false;
        const T value = func(m_obj, &overflow);
        if (overflow) {
            encountered_overflow();
            return 0;
        }
        if (value == static_cast<T>(-1) && PyErr_Occurred()) {
            PyErr_Clear();
            encountered_conversion_error();
            return static_cast<T>(0);
        }
        return value;
    }
};
