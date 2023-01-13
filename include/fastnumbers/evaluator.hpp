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

    /// Was the passed Python object of the correct type?
    bool is_type(const UserType ntype) const
    {
        const NumberFlags typeflags = m_parser.get_number_type();

        // Dispatch to the appropriate parser function based on requested type
        switch (ntype) {
        case UserType::REAL:
            return typeflags & NumberType::NaN
                ? allow_nan()
                : (typeflags & NumberType::Infinity
                       ? allow_inf()
                       : bool(typeflags & (NumberType::Integer | NumberType::Float)));

        case UserType::FLOAT:
            return typeflags & NumberType::NaN
                ? allow_nan()
                : (typeflags & NumberType::Infinity
                       ? allow_inf()
                       : bool(typeflags & NumberType::Float));

        case UserType::INT:
            return bool(typeflags & NumberType::Integer);

        case UserType::INTLIKE:
        case UserType::FORCEINT:
            return bool(typeflags & (NumberType::Integer | NumberType::IntLike));

        default:
            Py_UNREACHABLE();
        }
    }

    /// Is the stored type a float? Account for nan_action and inf_action.
    bool type_is_float() const
    {
        const NumberFlags typeflags = m_parser.get_number_type();
        return typeflags & NumberType::NaN
            ? allow_nan()
            : (typeflags & NumberType::Infinity ? allow_inf()
                                                : bool(typeflags & NumberType::Float));
    }

    /// Is the stored type an integer? If coerce is true, is the type intlike?
    bool type_is_int() const
    {
        NumberFlags to_check = NumberType::Integer;
        if (options().allow_coerce()) {
            to_check |= NumberType::IntLike;
        }
        return bool(m_parser.get_number_type() & to_check);
    }

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
    /// Assess if inf is allowed
    // Be sure to check out the template specialization in the .cpp file
    bool allow_inf() const { return options().allow_inf_str(); }

    /// Assess if nan is allowed
    // Be sure to check out the template specialization in the .cpp file
    bool allow_nan() const { return options().allow_nan_str(); }

    /// Logic for evaluating a numeric python object
    Payload from_numeric_as_type(const UserType ntype)
    {
        const NumberFlags typeflags = m_parser.get_number_type();
        const NumberFlags nan_or_inf = NumberType::Infinity | NumberType::NaN;
        const NumberFlags is_intlike = NumberType::Integer | NumberType::IntLike;

        // If not a numeric type it is a type error
        if (typeflags & NumberType::INVALID) {
            return typed_error(ntype);
        }

        // Otherwise, tell the downstream parser what action to take based
        // on the user requested type
        switch (ntype) {
        case UserType::REAL:
            if (options().allow_coerce() && (typeflags & is_intlike)) {
                return Payload(m_parser.as_pyint());
            } else if (typeflags & nan_or_inf) {
                return Payload(handle_nan_and_inf());
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
        const NumberFlags typeflags = m_parser.get_number_type();
        const NumberFlags nan_or_inf = NumberType::Infinity | NumberType::NaN;

        // Integers are returned as-is
        // NaN and infinity are illegal with force_int
        // Otherwise, grab as a float and convert to int if required
        if (typeflags & NumberType::Integer) {
            return from_text_as_int();

        } else if (force_int && (typeflags & nan_or_inf)) {
            return Payload(ActionType::ERROR_INVALID_INT);

        } else {
            Payload payload = from_text_as_float();

            // If the returned value is a double or a python float, annotate
            // whether it should be an integer and then return.
            const bool is_python = payload.get_action() == ActionType::PY_OBJECT
                && payload.to_pyobject() != nullptr;
            if (is_python) {
                const double value = PyFloat_AS_DOUBLE(payload.to_pyobject());
                Py_DECREF(payload.to_pyobject());
                return Payload(
                    value,
                    force_int
                        || (options().allow_coerce() && Parser::float_is_intlike(value))
                );
            }

            // Otherwise return the payload as-is.
            return payload;
        }
    }

    /// Logic for evaluating a text python object as a float
    Payload from_text_as_float()
    {
        const NumberFlags typeflags = m_parser.get_number_type();

        // Special-case handling of infinity and NaN
        if (typeflags & NumberType::Infinity) {
            return Payload(inf_action(m_parser.is_negative()));
        } else if (typeflags & NumberType::NaN) {
            return Payload(nan_action(m_parser.is_negative()));
        }

        // Otherwise, attempt to naively parse if possible, otherwise
        // use the python conversion function
        const double result = m_parser.as_float();
        if (m_parser.errored()) {
            return m_parser.potential_overflow()
                ? to_pyfloat()
                : Payload(ActionType::ERROR_INVALID_FLOAT);
        }

        // Successful naive conversion
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
            return to_pyint();
        }

        // Otherwise, attempt to naively parse if possible, otherwise
        // use the python conversion function
        const long result = m_parser.as_int();
        if (m_parser.errored()) {
            return m_parser.potential_overflow()
                ? to_pyint()
                : Payload(ActionType::ERROR_INVALID_INT);
        }

        // Successful naive conversion
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
        // Exctract the double from the parser, accounting for errors
        const double value = m_parser.as_float();
        if (m_parser.errored()) {
            return ActionType::ERROR_BAD_TYPE_FLOAT;
        }

        // Assume infinity or NaN.
        const NumberFlags typeflags = m_parser.get_number_type();
        return typeflags & NumberType::NaN ? nan_action(value < 0.0)
                                           : inf_action(value < 0.0);
    }

    /// Helper function to convert to Python float objects
    Payload to_pyfloat()
    {
        PyObject* result = m_parser.as_pyfloat();
        if (m_parser.errored()) {
            return Payload(ActionType::ERROR_INVALID_FLOAT);
        }
        return Payload(result);
    }

    /// Helper function to convert to Python int objects
    Payload to_pyint()
    {
        PyObject* result = m_parser.as_pyint();
        if (m_parser.errored()) {
            return Payload(ActionType::ERROR_INVALID_INT);
        }
        return Payload(result);
    }
};
