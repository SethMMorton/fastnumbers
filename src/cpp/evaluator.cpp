#include <cstring>

#include <Python.h>

#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/parser.hpp"

/* TYPE CHECKING */

bool Evaluator::is_type(const UserType ntype) const
{
    // Dispatch to the appropriate parser function based on requested type
    switch (ntype) {
    case UserType::REAL:
        return (m_nan_allowed && m_parser.is_nan())
            || (m_inf_allowed && m_parser.is_infinity()) || m_parser.is_real();

    case UserType::FLOAT:
        return (m_nan_allowed && m_parser.is_nan())
            || (m_inf_allowed && m_parser.is_infinity()) || m_parser.is_float();

    case UserType::INT:
        return m_parser.is_int();

    case UserType::INTLIKE:
    case UserType::FORCEINT:
        return m_parser.is_intlike();

    default:
        return false;
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
        if (!m_unicode_allowed) {
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

Payload Evaluator::from_numeric_as_type(const UserType ntype)
{
    // If not a direct numeric type, assume it might be a user
    // class with e.g __int__ or __float__ defined. Otherwise it's a
    // type error
    if (m_parser.not_float_or_int()) {
        if (m_parser.is_special_numeric()) {
            switch (ntype) {
            case UserType::REAL:
                if (NumericMethodsAnalyzer(m_obj).is_float()) {
                    return Payload(ActionType::TRY_FLOAT_IN_PYTHON);
                } else {
                    return Payload(ActionType::TRY_INT_IN_PYTHON);
                }
            case UserType::FLOAT:
                return Payload(ActionType::TRY_FLOAT_IN_PYTHON);
            case UserType::INT:
            case UserType::INTLIKE:
            case UserType::FORCEINT:
                if (!m_parser.is_default_base()) {
                    return Payload(ActionType::ERROR_INVALID_BASE);
                }
                return Payload(ActionType::TRY_INT_IN_PYTHON);
            }
        }
        return typed_error(ntype);
    }

    // Otherwise, tell the downstream parser what action to take based
    // on the user requested type.
    switch (ntype) {
    case UserType::REAL:
        if (m_coerce && m_parser.is_intlike()) {
            return Payload(ActionType::AS_INT);
        } else if (m_parser.is_nan()) {
            return Payload(ActionType::NAN_ACTION);
        } else if (m_parser.is_infinity()) {
            if (PyFloat_AS_DOUBLE(m_obj) < 0) {
                return Payload(ActionType::NEG_INF_ACTION);
            } else {
                return Payload(ActionType::INF_ACTION);
            }
        } else {
            return Payload(ActionType::AS_IS);
        }

    case UserType::FLOAT:
        if (m_parser.is_nan()) {
            return Payload(ActionType::NAN_ACTION);
        } else if (m_parser.is_infinity()) {
            if (PyFloat_AS_DOUBLE(m_obj) < 0) {
                return Payload(ActionType::NEG_INF_ACTION);
            } else {
                return Payload(ActionType::INF_ACTION);
            }
        } else {
            return Payload(ActionType::AS_FLOAT);
        }

    case UserType::INT:
    case UserType::INTLIKE:
    case UserType::FORCEINT:
        if (!m_parser.is_default_base()) {
            return Payload(ActionType::ERROR_INVALID_BASE);
        } else if (m_parser.is_finite()) {
            return Payload(ActionType::AS_INT);
        } else if (m_parser.is_infinity()) {
            return Payload(ActionType::ERROR_INFINITY_TO_INT);
        } else {
            return Payload(ActionType::ERROR_NAN_TO_INT);
        }

    default:
        return Payload(ActionType::AS_IS); // should never happen
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
        return Payload(ActionType::AS_IS); // should never happen
    }
}

Payload Evaluator::from_text_as_int_or_float(const bool force_int)
{
    // If already an integer, no special care is needed
    if (m_parser.is_int()) {
        const long result = m_parser.as_int();
        if (m_parser.errored()) {
            return m_parser.potential_overflow()
                ? Payload(ActionType::TRY_INT_IN_PYTHON)
                : Payload(ActionType::ERROR_INVALID_INT);
        }
        return Payload(result);

        // For infinity and NaN, if attemptying to force to integer then
        // this value is not valid, but if just coercing then these values
        // are fine.
    } else if (m_parser.is_infinity()) {
        return Payload(
            force_int ? ActionType::ERROR_INVALID_INT
                      : (m_parser.is_negative() ? ActionType::NEG_INF_ACTION
                                                : ActionType::INF_ACTION)
        );

    } else if (m_parser.is_nan()) {
        return Payload(
            force_int ? ActionType::ERROR_INVALID_INT
                      : (m_parser.is_negative() ? ActionType::NEG_NAN_ACTION
                                                : ActionType::NAN_ACTION)
        );

        // Otherwise, extract as a float and tell the downstream parser if
        // it needs to be converted to an integer or not.
    } else {
        const double result = m_parser.as_float();
        if (m_parser.errored()) {
            if (m_parser.potential_overflow()) {
                if (force_int) {
                    return Payload(ActionType::TRY_FLOAT_THEN_FORCE_INT_IN_PYTHON);
                } else if (m_coerce) {
                    return Payload(ActionType::TRY_FLOAT_THEN_COERCE_INT_IN_PYTHON);
                } else {
                    return Payload(ActionType::TRY_FLOAT_IN_PYTHON);
                }
            } else {
                return Payload(ActionType::ERROR_INVALID_FLOAT);
            }
        }
        return Payload(
            result, force_int || (m_coerce && Parser::float_is_intlike(result))
        );
    }
}

Payload Evaluator::from_text_as_float()
{
    if (m_parser.is_infinity()) {
        return Payload(
            m_parser.is_negative() ? ActionType::NEG_INF_ACTION : ActionType::INF_ACTION
        );

    } else if (m_parser.is_nan()) {
        return Payload(
            m_parser.is_negative() ? ActionType::NEG_NAN_ACTION : ActionType::NAN_ACTION
        );

    } else {
        const double result = m_parser.as_float();
        if (m_parser.errored()) {
            return m_parser.potential_overflow()
                ? Payload(ActionType::TRY_FLOAT_IN_PYTHON)
                : Payload(ActionType::ERROR_INVALID_FLOAT);
        }
        return Payload(result);
    }
}

Payload Evaluator::from_text_as_int()
{
    if (m_parser.get_base() != 10) {
        return Payload(ActionType::TRY_INT_IN_PYTHON);
    }
    const long result = m_parser.as_int();
    if (m_parser.errored()) {
        return m_parser.potential_overflow() ? Payload(ActionType::TRY_INT_IN_PYTHON)
                                             : Payload(ActionType::ERROR_INVALID_INT);
    }
    return Payload(result);
}

/* TEXT EXTRACTION */

void Evaluator::extract_string_data()
{
    // Use short-circuit logic to extract string data from the python object
    // from most likely to least likely.
    extract_from_unicode() || extract_from_bytes() || extract_from_bytearray()
        || extract_from_buffer();
}

bool Evaluator::extract_from_unicode()
{
    if (PyUnicode_Check(m_obj)) {
        // Unicode in ASCII form is stored like bytes!
        if (PyUnicode_IS_READY(m_obj) && PyUnicode_IS_COMPACT_ASCII(m_obj)) {
            m_parser.set_input(
                (const char*)PyUnicode_1BYTE_DATA(m_obj),
                static_cast<const std::size_t>(PyUnicode_GET_LENGTH(m_obj))
            );
            return true;
        }
        return parse_unicode_to_char();
    }
    return false;
}

bool Evaluator::extract_from_bytes()
{
    if (PyBytes_Check(m_obj)) {
        m_parser.set_input(
            PyBytes_AS_STRING(m_obj),
            static_cast<const std::size_t>(PyBytes_GET_SIZE(m_obj))
        );
        return true;
    }
    return false;
}

bool Evaluator::extract_from_bytearray()
{
    if (PyByteArray_Check(m_obj)) {
        m_parser.set_input(
            PyByteArray_AS_STRING(m_obj),
            static_cast<const std::size_t>(PyByteArray_GET_SIZE(m_obj))
        );
        return true;
    }
    return false;
}

bool Evaluator::extract_from_buffer()
{
    Py_buffer view = { NULL, NULL };
    if (PyObject_CheckBuffer(m_obj)
        && PyObject_GetBuffer(m_obj, &view, PyBUF_SIMPLE) == 0) {
        // This buffer could be a memoryview slice. If this is the case, the
        // nul termination of the string will be past the given length, creating
        // unexpected parsing results. Rather than complicate the parsing and
        // adding more operations for a low probability event, a copy of the
        // slice will be made here and null termination will be added.
        // If the data amount is small enough, we use a fixed-sized buffer for speed.
        const std::size_t str_len = static_cast<const std::size_t>(view.len);
        m_char_buffer = new Buffer(static_cast<char*>(view.buf), str_len);
        m_char_buffer->start()[str_len] = '\0';

        // All we care about is the underlying buffer data, not the obj
        // which was allocated when we created the buffer. For this reason
        // it is safe to release the buffer here.
        PyBuffer_Release(&view);
        m_parser.set_input(m_char_buffer->start(), str_len);
        return true;
    }
    return false;
}

bool Evaluator::parse_unicode_to_char()
{
    const unsigned kind = PyUnicode_KIND(m_obj); // Unicode storage format.
    const void* data = PyUnicode_DATA(m_obj); // Raw data
    Py_ssize_t len = PyUnicode_GET_LENGTH(m_obj);
    Py_ssize_t index = 0;
    char sign = '\0';
    bool negative = false;

    // Ensure input is a valid unicode object.
    // If true, then not OK for conversion.
    // In that case just store as a 0-length string.
    if (PyUnicode_READY(m_obj)) {
        m_char_buffer = new Buffer(0);
        m_char_buffer->start()[0] = '\0';
        m_parser.set_input(m_char_buffer->start(), 0);
        return true;
    }

    // Strip whitespace from both ends of the data.
    while (Py_UNICODE_ISSPACE(PyUnicode_READ(kind, data, index))) {
        index += 1;
        len -= 1;
    }
    while (Py_UNICODE_ISSPACE(PyUnicode_READ(kind, data, index + len - 1))) {
        len -= 1;
    }

    // Remove the sign - remember if it is negative.
    if (PyUnicode_READ(kind, data, index) == '-') {
        negative = true;
        sign = '-';
        index += 1;
        len -= 1;
    } else if (PyUnicode_READ(kind, data, index) == '+') {
        sign = '+';
        index += 1;
        len -= 1;
    }

    // Allocate space for the character data, but use a small fixed size
    // buffer if the data is small enough. Ensure a trailing null character.
    m_char_buffer = new Buffer(static_cast<std::size_t>(len + (sign ? 1 : 0) + 1));
    char* buffer = m_char_buffer->start();
    std::size_t buffer_index = 0;

    // If the string had a sign, add it back to the front of the buffer
    if (sign) {
        buffer[buffer_index] = sign;
        buffer_index += 1;
    }

    // Iterate over the unicode data and transform to ASCII-compatible
    // data. If at any point this fails, exit and just save as a 0-length
    // string, unless the length was one, in which case we save the one
    // character.
    constexpr std::size_t ASCII_MAX = 127;
    long u_as_decimal = 0;
    const Py_ssize_t data_len = len + index;
    for (; index < data_len; index++) {
        const Py_UCS4 u = (Py_UCS4)PyUnicode_READ(kind, data, index);
        if (u < ASCII_MAX) {
            buffer[buffer_index] = static_cast<char>(u);
        } else if ((u_as_decimal = Py_UNICODE_TODECIMAL(u)) > -1) {
            buffer[buffer_index] = '0' + static_cast<char>(u_as_decimal);
        } else if (Py_UNICODE_ISSPACE(u)) {
            buffer[buffer_index] = ' ';
        } else {
            if (len == 1) {
                m_parser.set_input(u, negative);
                return true;
            }
            buffer[0] = '\0';
            m_parser.set_input(buffer, 0);
            return true;
        }
        buffer_index += 1;
    }
    buffer[buffer_index] = '\0';

    m_parser.set_input(buffer, buffer_index);
    return true;
}
