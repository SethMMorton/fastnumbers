#pragma once

#include <cmath>

#include <Python.h>

#include "fastnumbers/parser/base.hpp"

/**
 * \class NumericParser
 * \brief Parses numeric python objects
 */
class NumericParser : public Parser {
public:
    /// Construct with a Python object
    explicit NumericParser(PyObject* obj)
        : Parser(ParserType::NUMERIC)
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
    ~NumericParser() final { Py_XDECREF(m_obj); };

    /// Was the passed Python object finite?
    bool is_finite() const final
    {
        return Parser::is_int() || (Parser::is_float() && std::isfinite(get_double()));
    }

    /// Was the passed Python object infinity?
    bool is_infinity() const final
    {
        return Parser::is_float() && std::isinf(get_double());
    }

    /// Was the passed Python object NaN?
    bool is_nan() const final { return Parser::is_float() && std::isnan(get_double()); }

    /**
     * \brief Was the passed Python object intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    bool is_intlike() const final
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
