#pragma once

#include <climits>

#include <Python.h>

#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/user_options.hpp"

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
    Evaluator(PyObject* obj, const UserOptions& options, Parser* parser)
        : m_obj(obj)
        , m_parser(parser)
        , m_options(options)
    {
        Py_XINCREF(m_obj);
        if (m_parser->parser_type() == ParserType::NUMERIC) {
            m_options.set_nan_allowed(true);
            m_options.set_inf_allowed(true);
        }
    }

    // Other constructors, destructors, and assignment
    Evaluator(const Evaluator&) = delete;
    Evaluator(Evaluator&&) = delete;
    Evaluator& operator=(const Evaluator&) = delete;
    ~Evaluator() { Py_XDECREF(m_obj); }

    /// Assign a new object to analyze
    void set_object(PyObject* obj)
    {
        Py_XDECREF(m_obj);
        m_obj = obj;
        Py_XINCREF(m_obj);
    }

    /// Access the user-given options for evaluating
    const UserOptions& options() const { return m_options; }

    /// Return the parser type currenly associated with the Evaluator
    ParserType parser_type() const { return m_parser->parser_type(); }

    /// Was the passed Python object of the correct type?
    bool is_type(const UserType ntype) const;

    /// Is the stored type a float? Account for nan_action and inf_action.
    bool type_is_float() const
    {
        return (options().allow_nan() && m_parser->is_nan())
            || (options().allow_inf() && m_parser->is_infinity())
            || m_parser->is_float();
    }

    /// Is the stored type an integer? If coerce is true, is the type intlike?
    bool type_is_int() const
    {
        return options().allow_coerce() ? m_parser->is_intlike() : m_parser->is_int();
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

    /// A Parser object used for evaluating the Python object
    Parser* m_parser;

    /// Hold the evaluator options
    UserOptions m_options;

private:
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
