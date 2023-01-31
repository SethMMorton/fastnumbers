#pragma once

#include <climits>

#include <Python.h>

#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class Evaluator
 * \brief Evaluate the contents of a Python object
 */
template <typename ParserT>
class Evaluator {
public:
    /// Constructor from a Python object
    Evaluator(PyObject* obj, const UserOptions& options, ParserT& parser)
        : m_obj(obj)
        , m_parser(parser)
        , m_options(options)
    {
        Py_XINCREF(m_obj);
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
    ParserType parser_type() const { return m_parser.parser_type(); }

    /// Return the type of number contained in the given object
    NumberFlags number_type() const { return m_parser.get_number_type(); }

    /**
     * \brief Convert the stored object to the desired number type
     *
     * Use the appropriate error handling on error.
     *
     * \param ntype PyNumberType indicating the desired type to check
     * \return Payload
     */
    Payload as_type(const UserType ntype)
    {
        // Send to the appropriate convenience function based on the found type
        switch (parser_type()) {
        case ParserType::NUMERIC:
            return from_numeric_as_type(ntype);

        case ParserType::UNICODE:
            if (!options().allow_unicode()) {
                return typed_error(ntype, false);
            }
            /* DELIBERATE FALL-THROUGH */
        case ParserType::CHARACTER:
            return from_text_as_type(ntype);

        case ParserType::UNKNOWN:
        default:
            break;
        }

        // If here, the input type is not valid
        return typed_error(ntype);
    }

private:
    /// The Python object under evaluation
    PyObject* m_obj;

    /// A Parser object used for evaluating the Python object
    ParserT& m_parser;

    /// Hold the evaluator options
    UserOptions m_options;

private:
    /// Logic for evaluating a numeric python object
    Payload from_numeric_as_type(const UserType ntype)
    {
        const NumberFlags typeflags = m_parser.get_number_type();
        constexpr NumberFlags nan_or_inf = NumberType::Infinity | NumberType::NaN;

        // If not a numeric type it is a type error
        if (typeflags & NumberType::INVALID) {
            return typed_error(ntype);
        }

        // Otherwise, tell the downstream parser what action to take based
        // on the user requested type
        switch (ntype) {
        case UserType::REAL:
            if (typeflags & nan_or_inf) {
                return Payload(handle_nan_and_inf());
            } else if (options().allow_coerce()) {
                return Payload(m_parser.as_pyfloat(false, true));
            } else if (typeflags & NumberType::User) {
                if (typeflags & NumberType::Float) {
                    return Payload(m_parser.as_pyfloat());
                } else {
                    return Payload(m_parser.as_pyint());
                }
            } else {
                Py_IncRef(m_obj);
                return Payload(m_obj);
            }

        case UserType::FLOAT:
            if (typeflags & nan_or_inf) {
                return Payload(handle_nan_and_inf());
            } else {
                return Payload(m_parser.as_pyfloat());
            }

        case UserType::INT:
        case UserType::INTLIKE:
        case UserType::FORCEINT:
            if (!options().is_default_base()) {
                return Payload(ActionType::ERROR_INVALID_BASE);
            }
            return Payload(m_parser.as_pyint());

        default:
            Py_UNREACHABLE();
        }
    }

    /// Logic for evaluating a text python object
    Payload from_text_as_type(const UserType ntype)
    {
        switch (ntype) {
        case UserType::REAL:
        case UserType::INTLIKE:
        case UserType::FORCEINT:
            // REAL will only try to coerce to integer... the others will force
            return from_text_as_int_or_float(ntype != UserType::REAL);

        case UserType::FLOAT:
            return from_text_as_float();

        case UserType::INT:
            return from_text_as_int();

        default:
            Py_UNREACHABLE();
        }
    }

    /// Logic for evaluating a text python object as a float or integer
    Payload from_text_as_int_or_float(const bool force_int)
    {
        // Integers are returned as-is
        // NaN and infinity are illegal with force_int
        // Otherwise, grab as a float and convert to int if required
        if (m_parser.peek_try_as_int()) {
            return from_text_as_int();

        } else if (force_int && (m_parser.peek_inf() || m_parser.peek_nan())) {
            return Payload(ActionType::ERROR_INVALID_INT);

        } else {
            // Special-case handling of infinity and NaN
            if (m_parser.peek_inf()) {
                return Payload(inf_action(m_parser.is_negative()));
            } else if (m_parser.peek_nan()) {
                return Payload(nan_action(m_parser.is_negative()));
            }

            // Otherwise, attempt to convert to a python float
            // and optionally make as integer and if not signal an error.
            PyObject* result = m_parser.as_pyfloat(force_int, options().allow_coerce());
            if (m_parser.errored()) {
                return Payload(ActionType::ERROR_INVALID_FLOAT);
            }
            return Payload(result);
        }
    }

    /// Logic for evaluating a text python object as a float
    Payload from_text_as_float()
    {
        // Special-case handling of infinity and NaN
        if (m_parser.peek_inf()) {
            return Payload(inf_action(m_parser.is_negative()));
        } else if (m_parser.peek_nan()) {
            return Payload(nan_action(m_parser.is_negative()));
        }

        // Otherwise, attempt to convert to a python float
        // and if not signal an error.
        PyObject* result = m_parser.as_pyfloat();
        if (m_parser.errored()) {
            return Payload(ActionType::ERROR_INVALID_FLOAT);
        }
        return Payload(result);
    }

    /// Logic for evaluating a text python object as an int
    Payload from_text_as_int()
    {
        // We use python to convert non-base-10 integer strings.
        // Some strings are not allowed to use an explict base,
        // so check that first.
        if (m_parser.options().get_base() != 10) {
            if (m_parser.illegal_explicit_base()) {
                return Payload(ActionType::ERROR_ILLEGAL_EXPLICIT_BASE);
            }
        }

        // Otherwise, attempt to convert to a python int
        // and if not signal an error.
        PyObject* result = m_parser.as_pyint();
        if (m_parser.errored()) {
            return Payload(ActionType::ERROR_INVALID_INT);
        }
        return Payload(result);
    }

    /// Return an error due to a bad type
    static Payload typed_error(const UserType ntype, const bool type = true)
    {
        if (ntype == UserType::REAL || ntype == UserType::FLOAT) {
            if (type) {
                return Payload(ActionType::ERROR_BAD_TYPE_FLOAT);
            } else {
                return Payload(ActionType::ERROR_INVALID_FLOAT);
            }
        } else {
            if (type) {
                return Payload(ActionType::ERROR_BAD_TYPE_INT);
            } else {
                return Payload(ActionType::ERROR_INVALID_INT);
            }
        }
    }

    /// Return the correct action if an INF was found
    static ActionType inf_action(const bool is_negative)
    {
        return is_negative ? ActionType::NEG_INF_ACTION : ActionType::INF_ACTION;
    }

    /// Return the correct action if an NAN was found
    static ActionType nan_action(const bool is_negative)
    {
        return is_negative ? ActionType::NEG_NAN_ACTION : ActionType::NAN_ACTION;
    }

    /// Helper to properly respond to NaN and infinity
    ActionType handle_nan_and_inf()
    {
        // Assume infinity or NaN.
        const NumberFlags typeflags = m_parser.get_number_type();
        return typeflags & NumberType::NaN ? nan_action(m_parser.is_negative())
                                           : inf_action(m_parser.is_negative());
    }
};
