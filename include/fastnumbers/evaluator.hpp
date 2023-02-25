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
    Evaluator(PyObject* obj, const UserOptions& options, const ParserT& parser) noexcept
        : m_obj(obj)
        , m_parser(parser)
        , m_options(options)
    {
        Py_INCREF(m_obj);
    }

    // Other constructors, destructors, and assignment
    Evaluator(const Evaluator&) = delete;
    Evaluator(Evaluator&&) = delete;
    Evaluator& operator=(const Evaluator&) = delete;
    ~Evaluator() { Py_DECREF(m_obj); }

    /// Assign a new object to analyze
    void set_object(PyObject* obj) noexcept
    {
        Py_DECREF(m_obj);
        m_obj = obj;
        Py_INCREF(m_obj);
    }

    /// Access the user-given options for evaluating
    const UserOptions& options() const noexcept { return m_options; }

    /// Return the parser type currenly associated with the Evaluator
    ParserType parser_type() const noexcept { return m_parser.parser_type(); }

    /// Return the type of number contained in the given object
    NumberFlags number_type() const noexcept { return m_parser.get_number_type(); }

    /**
     * \brief Convert the stored object to the desired number type
     *
     * Use the appropriate error handling on error.
     *
     * \param ntype PyNumberType indicating the desired type to check
     * \return Payload
     */
    Payload as_type(const UserType ntype) noexcept
    {
        // Send to the appropriate convenience function based on the found type
        switch (parser_type()) {
        case ParserType::UNICODE:
            if (!options().allow_unicode()) {
                return typed_error(ntype, false);
            }
            [[fallthrough]];
        case ParserType::CHARACTER:
            return from_text_as_type(ntype);

        default: // NUMERIC
            return from_numeric_as_type(ntype);
        }
    }

private:
    /// The Python object under evaluation
    PyObject* m_obj;

    /// A Parser object used for evaluating the Python object
    const ParserT& m_parser;

    /// Hold the evaluator options
    const UserOptions m_options;

private:
    /// Logic for evaluating a numeric python object
    Payload from_numeric_as_type(const UserType ntype) noexcept
    {
        const NumberFlags typeflags = m_parser.get_number_type();
        constexpr NumberFlags nan_or_inf = NumberType::Infinity | NumberType::NaN;

        // Otherwise, tell the downstream parser what action to take based
        // on the user requested type
        switch (ntype) {
        case UserType::REAL:
            if (typeflags & nan_or_inf) {
                return handle_nan_and_inf();
            } else if (options().allow_coerce()) {
                return convert(m_parser.as_pyfloat(false, true), ntype);
            } else if (typeflags & NumberType::Float) {
                return convert(m_parser.as_pyfloat(), ntype);
            } else {
                return convert(m_parser.as_pyint(), ntype);
            }

        case UserType::FLOAT:
            if (typeflags & nan_or_inf) {
                return handle_nan_and_inf();
            } else {
                return convert(m_parser.as_pyfloat(), ntype);
            }

        case UserType::INT:
            if (!options().is_default_base()) {
                return ActionType::ERROR_INVALID_BASE;
            }
            [[fallthrough]];
        default: // INTLIKE, FORCEINT
            return convert(
                (typeflags & NumberType::Float) ? m_parser.as_pyfloat(true, false)
                                                : m_parser.as_pyint(),
                ntype
            );
        }
    }

    /// Logic for evaluating a text python object
    Payload from_text_as_type(const UserType ntype) noexcept
    {
        switch (ntype) {
        case UserType::FLOAT:
            return from_text_as_float();

        case UserType::INT:
            return from_text_as_int();

        default: // REAL, FORCEINT, INTLIKE
            // REAL will only try to coerce to integer... the others will force
            return from_text_as_int_or_float(ntype != UserType::REAL);
        }
    }

    /// Logic for evaluating a text python object as a float or integer
    Payload from_text_as_int_or_float(const bool force_int) noexcept
    {
        // Integers are returned as-is
        // NaN and infinity are illegal with force_int
        // Otherwise, grab as a float and convert to int if required
        if (m_parser.peek_try_as_int()) {
            return from_text_as_int();

        } else if (force_int && (m_parser.peek_inf() || m_parser.peek_nan())) {
            return ActionType::ERROR_INVALID_INT;

        } else {
            // Special-case handling of infinity and NaN
            if (m_parser.peek_inf()) {
                return inf_action(m_parser.is_negative());
            } else if (m_parser.peek_nan()) {
                return nan_action(m_parser.is_negative());
            }

            // Otherwise, attempt to convert to a python float
            // and optionally make as integer
            return convert(
                m_parser.as_pyfloat(force_int, options().allow_coerce()), UserType::FLOAT
            );
        }
    }

    /// Logic for evaluating a text python object as a float
    Payload from_text_as_float() noexcept
    {
        // Special-case handling of infinity and NaN
        if (m_parser.peek_inf()) {
            return inf_action(m_parser.is_negative());
        } else if (m_parser.peek_nan()) {
            return nan_action(m_parser.is_negative());
        }

        // Otherwise, attempt to convert to a python float
        return convert(m_parser.as_pyfloat(), UserType::FLOAT);
    }

    /// Logic for evaluating a text python object as an int
    Payload from_text_as_int() noexcept
    {
        // We use python to convert non-base-10 integer strings.
        // Some strings are not allowed to use an explict base,
        // so check that first.
        if (m_parser.options().get_base() != 10) {
            if (m_parser.illegal_explicit_base()) {
                return ActionType::ERROR_ILLEGAL_EXPLICIT_BASE;
            }
        }

        // Otherwise, attempt to convert to a python int
        return convert(m_parser.as_pyint(), UserType::INT);
    }

    /// Return an error due to a bad type
    static Payload typed_error(const UserType ntype, const bool type = true) noexcept
    {
        if (ntype == UserType::REAL || ntype == UserType::FLOAT) {
            if (type) {
                return ActionType::ERROR_BAD_TYPE_FLOAT;
            } else {
                return ActionType::ERROR_INVALID_FLOAT;
            }
        } else {
            if (type) {
                return ActionType::ERROR_BAD_TYPE_INT;
            } else {
                return ActionType::ERROR_INVALID_INT;
            }
        }
    }

    /// Return the correct action if an INF was found
    static ActionType inf_action(const bool is_negative) noexcept
    {
        return is_negative ? ActionType::NEG_INF_ACTION : ActionType::INF_ACTION;
    }

    /// Return the correct action if an NAN was found
    static ActionType nan_action(const bool is_negative) noexcept
    {
        return is_negative ? ActionType::NEG_NAN_ACTION : ActionType::NAN_ACTION;
    }

    /// Helper to properly respond to NaN and infinity
    ActionType handle_nan_and_inf() noexcept
    {
        // Assume infinity or NaN.
        const NumberFlags typeflags = m_parser.get_number_type();
        return typeflags & NumberType::NaN ? nan_action(m_parser.is_negative())
                                           : inf_action(m_parser.is_negative());
    }

    /// Convert the RawPayload data into Paylod data
    Payload
    convert(const RawPayload<PyObject*>& payload, const UserType ntype) const noexcept
    {
        return std::visit(
            overloaded {

                // If the stored object is not an error, pass it along
                [](PyObject* retval) -> Payload {
                    return retval;
                },

                // For errors, convert from "raw" errors to "action" errors
                [ntype](const ErrorType err) -> Payload {
                    // NOTE: We explicitly are not handling ErrorType:OVERFLOW_
                    // because it cannot be returned in the PyObject* code path.
                    if (err == ErrorType::BAD_VALUE) {
                        if (ntype == UserType::FLOAT || ntype == UserType::REAL) {
                            return ActionType::ERROR_INVALID_FLOAT;
                        } else {
                            return ActionType::ERROR_INVALID_INT;
                        }
                    } else {
                        return typed_error(ntype);
                    }
                },
            },
            payload
        );
    }
};
