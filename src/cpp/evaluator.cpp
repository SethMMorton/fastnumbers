#include <cstring>
#include <stdexcept>

#include <Python.h>

#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/parser.hpp"

/* TYPE CHECKING */

bool Evaluator::is_type(const UserType ntype) const
{
    // Dispatch to the appropriate parser function based on requested type
    switch (ntype) {
    case UserType::REAL:
        return (options().allow_nan() && m_parser->is_nan())
            || (options().allow_inf() && m_parser->is_infinity()) || m_parser->is_real();

    case UserType::FLOAT:
        return (options().allow_nan() && m_parser->is_nan())
            || (options().allow_inf() && m_parser->is_infinity())
            || m_parser->is_float();

    case UserType::INT:
        return m_parser->is_int();

    case UserType::INTLIKE:
    case UserType::FORCEINT:
        return m_parser->is_intlike();

    default:
        Py_UNREACHABLE();
    }
}

/* TYPE CONVERSION */

static inline Payload typed_error(const UserType ntype, const bool type = true)
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

Payload Evaluator::as_type(const UserType ntype)
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

static inline ActionType inf_action(const bool is_negative)
{
    return is_negative ? ActionType::NEG_INF_ACTION : ActionType::INF_ACTION;
}

static inline ActionType nan_action(const bool is_negative)
{
    return is_negative ? ActionType::NEG_NAN_ACTION : ActionType::NAN_ACTION;
}

static inline ActionType handle_nan_and_inf(Parser* parser)
{
    // Exctract the double from the parser, accounting for errors
    const double value = parser->as_float();
    if (parser->errored()) {
        return ActionType::ERROR_BAD_TYPE_FLOAT;
    }

    // Assume infinity or NaN.
    return parser->is_nan() ? nan_action(value < 0.0) : inf_action(value < 0.0);
}

Payload Evaluator::from_numeric_as_type(const UserType ntype)
{
    // If not a numeric type it is a type error
    if (m_parser->not_float_or_int() && !m_parser->is_user_numeric()) {
        return typed_error(ntype);
    }

    // Otherwise, tell the downstream parser what action to take based
    // on the user requested type
    switch (ntype) {
    case UserType::REAL:
        if (options().allow_coerce() && m_parser->is_intlike()) {
            return Payload(m_parser->as_pyint());
        } else if (m_parser->is_nan() || m_parser->is_infinity()) {
            return Payload(handle_nan_and_inf(m_parser));
        } else if (m_parser->is_user_numeric()) {
            if (m_parser->is_user_numeric_float()) {
                return Payload(m_parser->as_pyfloat());
            } else {
                return Payload(m_parser->as_pyint());
            }
        } else {
            Py_IncRef(m_obj);
            return Payload(m_obj);
        }

    case UserType::FLOAT:
        if (m_parser->is_nan() || m_parser->is_infinity()) {
            return Payload(handle_nan_and_inf(m_parser));
        } else {
            return Payload(m_parser->as_pyfloat());
        }

    case UserType::INT:
    case UserType::INTLIKE:
    case UserType::FORCEINT:
        if (!options().is_default_base()) {
            return Payload(ActionType::ERROR_INVALID_BASE);
        }
        return Payload(m_parser->as_pyint());

    default:
        Py_UNREACHABLE();
    }
}

Payload Evaluator::from_text_as_type(const UserType ntype)
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

/// Helper function to convert to Python float objects
static inline Payload to_pyfloat(Parser* parser)
{
    PyObject* result = parser->as_pyfloat();
    if (parser->errored()) {
        return Payload(ActionType::ERROR_INVALID_FLOAT);
    }
    return Payload(result);
}

Payload Evaluator::from_text_as_int_or_float(const bool force_int)
{
    // Integers are returned as-is
    // NaN and infinity are illegal with force_int
    // Otherwise, grab as a float and convert to int if required
    if (m_parser->is_int()) {
        return from_text_as_int();

    } else if (force_int && (m_parser->is_infinity() || m_parser->is_nan())) {
        return Payload(ActionType::ERROR_INVALID_INT);

    } else {
        Payload payload = from_text_as_float();

        // If the returned value is a double or a python float, annotate
        // whether it should be an integer and then return.
        const bool is_double = payload.payload_type() == PayloadType::DOUBLE;
        const bool is_python = payload.payload_type() == PayloadType::PYOBJECT
            && payload.to_pyobject() != nullptr;
        if (is_double || is_python) {
            const double value = is_double ? payload.to_double()
                                           : PyFloat_AS_DOUBLE(payload.to_pyobject());
            if (is_python) {
                Py_DECREF(payload.to_pyobject());
            }
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

Payload Evaluator::from_text_as_float()
{
    // Special-case handling of infinity and NaN
    if (m_parser->is_infinity()) {
        return Payload(inf_action(m_parser->is_negative()));
    } else if (m_parser->is_nan()) {
        return Payload(nan_action(m_parser->is_negative()));
    }

    // Otherwise, attempt to naively parse if possible, otherwise
    // use the python conversion function
    const double result = m_parser->as_float();
    if (m_parser->errored()) {
        return m_parser->potential_overflow() ? to_pyfloat(m_parser)
                                              : Payload(ActionType::ERROR_INVALID_FLOAT);
    }

    // Successful naive conversion
    return Payload(result);
}

/// Helper function to convert to Python int objects
static inline Payload to_pyint(Parser* parser)
{
    PyObject* result = parser->as_pyint();
    if (parser->errored()) {
        return Payload(ActionType::ERROR_INVALID_INT);
    }
    return Payload(result);
}

Payload Evaluator::from_text_as_int()
{
    // We use python to convert non-base-10 integer strings.
    // Some strings are not allowed to use an explict base,
    // so check that first.
    if (m_parser->options().get_base() != 10) {
        if (m_parser->illegal_explicit_base()) {
            return Payload(ActionType::ERROR_ILLEGAL_EXPLICIT_BASE);
        }
        return to_pyint(m_parser);
    }

    // Otherwise, attempt to naively parse if possible, otherwise
    // use the python conversion function
    const long result = m_parser->as_int();
    if (m_parser->errored()) {
        return m_parser->potential_overflow() ? to_pyint(m_parser)
                                              : Payload(ActionType::ERROR_INVALID_INT);
    }

    // Successful naive conversion
    return Payload(result);
}
