#pragma once

#include <cmath>

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
        if (m_obj != nullptr) {
            // Store the type of number that was found
            set_number_type(get_number_type());

            // Increment the reference count for this object
            Py_IncRef(m_obj);
        } else {
            // If given a null object, make the parser unknown
            set_as_unknown_parser();
        }
    }

    // No default constructor
    NumericParser() = delete;

    // Default copy/assignment
    NumericParser(const NumericParser&) = default;
    NumericParser(NumericParser&&) = default;
    NumericParser& operator=(const NumericParser&) = default;

    /// Descructor decreases reference count of the stored object
    ~NumericParser() { Py_XDECREF(m_obj); };

    /// Convert the stored object to a long (check error state)
    long as_int() override
    {
        reset_error();

        if (get_number_type() & NumberType::Integer) {
            int overflow = 0;
            const long value = PyLong_AsLongAndOverflow(m_obj, &overflow);
            if (overflow) {
                encountered_potential_overflow_error();
                return -1L;
            } else if (value == -1 && PyErr_Occurred()) {
                PyErr_Clear();
            }
            return value;
        }
        encountered_conversion_error();
        return -1L;
    }

    /// Convert the stored object to a double (check error state)
    double as_float() override
    {
        reset_error();

        if (get_number_type() & (NumberType::Float | NumberType::User)) {
            const double value = PyFloat_AsDouble(m_obj);
            if (value == -1.0 && PyErr_Occurred()) {
                PyErr_Clear();
            } else {
                return value;
            }
        } else if (get_number_type() & NumberType::Float) {
            return get_double();
        }
        encountered_conversion_error();
        return -1.0;
    }

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() override
    {
        reset_error();
        return PyNumber_Long(m_obj);
    }

    /// Convert the stored object to a python float (check error state)
    PyObject* as_pyfloat() override
    {
        reset_error();
        return PyNumber_Float(m_obj);
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
};
