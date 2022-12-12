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
        , m_user_numeric(false)
    {
        if (m_obj != nullptr) {
            // Store whether the number is a float or integer
            if (PyFloat_Check(m_obj)) {
                set_as_float_type();
            } else if (PyLong_Check(m_obj)) {
                set_as_int_type();
            } else if (is_user_numeric_int() || is_user_numeric_float()) {
                m_user_numeric = true;
            }

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
    long as_int()
    {
        reset_error();

        if (Parser::is_int() || is_user_numeric_int()) {
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
    double as_float()
    {
        reset_error();

        if (Parser::is_float()) {
            return get_double();
        } else if (is_user_numeric_float()) {
            const double value = PyFloat_AsDouble(m_obj);
            if (value == -1.0 && PyErr_Occurred()) {
                PyErr_Clear();
            } else {
                return value;
            }
        }
        encountered_conversion_error();
        return -1.0;
    }

    /// Convert the stored object to a python int (check error state)
    PyObject* as_pyint() { return PyNumber_Long(m_obj); }

    /// Convert the stored object to a python float (check error state)
    PyObject* as_pyfloat() { return PyNumber_Float(m_obj); }

    /// Was the passed Python object finite?
    bool is_finite() const
    {
        return Parser::is_int() || (Parser::is_float() && std::isfinite(get_double()));
    }

    /// Was the passed Python object infinity?
    bool is_infinity() const { return Parser::is_float() && std::isinf(get_double()); }

    /// Was the passed Python object NaN?
    bool is_nan() const { return Parser::is_float() && std::isnan(get_double()); }

    /**
     * \brief Was the passed Python object intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    bool is_intlike() const
    {
        return Parser::is_int()
            || (Parser::is_float() && Parser::float_is_intlike(get_double()));
    }

    /// Is the object is a a user-defined numeric class?
    bool is_user_numeric() const { return m_user_numeric; }

    /// Is the object is a a user-defined numeric float?
    bool is_user_numeric_float() const
    {
        return Py_TYPE(m_obj)->tp_as_number && Py_TYPE(m_obj)->tp_as_number->nb_float;
    }

    /// Is the object is a a user-defined numeric int?
    bool is_user_numeric_int() const
    {
        PyNumberMethods* m_nmeth = Py_TYPE(m_obj)->tp_as_number;
        return m_nmeth && (m_nmeth->nb_index || m_nmeth->nb_int);
    }

private:
    /// The Python object potentially under analysis
    PyObject* m_obj;

    /// Track if the object is a a user-defined numeric class
    bool m_user_numeric;

private:
    /// Return the object as a double. No error checking is performed.
    double get_double() const { return PyFloat_AS_DOUBLE(m_obj); }
};
