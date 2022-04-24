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
        return (m_nan_allowed && m_parser->is_nan())
            || (m_inf_allowed && m_parser->is_infinity()) || m_parser->is_real();

    case UserType::FLOAT:
        return (m_nan_allowed && m_parser->is_nan())
            || (m_inf_allowed && m_parser->is_infinity()) || m_parser->is_float();

    case UserType::INT:
        return m_parser->is_int();

    case UserType::INTLIKE:
    case UserType::FORCEINT:
        return m_parser->is_intlike();

    default:
        throw std::runtime_error(
            "An impossible code path was encountered in Evaluator::is_type"
        );
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

static inline ActionType inf_action(const bool is_negative)
{
    return is_negative ? ActionType::NEG_INF_ACTION : ActionType::INF_ACTION;
}

static inline ActionType nan_action(const bool is_negative)
{
    return is_negative ? ActionType::NEG_NAN_ACTION : ActionType::NAN_ACTION;
}

static inline ActionType handle_nan_and_inf(NumericParser* nparser)
{
    // Exctract the double from the parser, accounting for errors
    const double value = nparser->as_float();
    if (nparser->errored()) {
        return ActionType::ERROR_BAD_TYPE_FLOAT;
    }

    // Assume infinity or NaN.
    return nparser->is_nan() ? nan_action(value < 0.0) : inf_action(value < 0.0);
}

Payload Evaluator::from_numeric_as_type(const UserType ntype)
{
    NumericParser* nparser = static_cast<NumericParser*>(m_parser);

    // If not a numeric type it is a type error
    if (nparser->not_float_or_int() && !nparser->is_user_numeric()) {
        return typed_error(ntype);
    }

    // Otherwise, tell the downstream parser what action to take based
    // on the user requested type
    switch (ntype) {
    case UserType::REAL:
        if (m_coerce && nparser->is_intlike()) {
            return Payload(nparser->as_pyint());
        } else if (nparser->is_nan() || nparser->is_infinity()) {
            return Payload(handle_nan_and_inf(nparser));
        } else if (nparser->is_user_numeric()) {
            if (nparser->is_user_numeric_float()) {
                return Payload(nparser->as_pyfloat());
            } else {
                return Payload(nparser->as_pyint());
            }
        } else {
            Py_IncRef(m_obj);
            return Payload(m_obj);
        }

    case UserType::FLOAT:
        if (nparser->is_nan() || nparser->is_infinity()) {
            return Payload(handle_nan_and_inf(nparser));
        } else {
            return Payload(nparser->as_pyfloat());
        }

    case UserType::INT:
    case UserType::INTLIKE:
    case UserType::FORCEINT:
        if (!nparser->is_default_base()) {
            return Payload(ActionType::ERROR_INVALID_BASE);
        }
        return Payload(nparser->as_pyint());

    default:
        throw std::runtime_error(
            "An impossible code path was encountered in Evaluator::from_numeric_as_type"
        );
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
        throw std::runtime_error(
            "An impossible code path was encountered in Evaluator::from_text_as_type"
        );
    }
}

/// Helper function to convert to Python float objects
static inline Payload to_pyfloat(SignedParser* parser)
{
    PyObject* result = parser->as_pyfloat();
    if (parser->errored()) {
        return Payload(ActionType::ERROR_INVALID_FLOAT);
    }
    return Payload(result);
}

Payload Evaluator::from_text_as_int_or_float(const bool force_int)
{
    SignedParser* sparser = static_cast<SignedParser*>(m_parser);

    // Integers are returned as-is
    // NaN and infinity are illegal with force_int
    // Otherwise, grab as a float and convert to int if required
    if (sparser->is_int()) {
        return from_text_as_int();

    } else if (force_int && (sparser->is_infinity() || sparser->is_nan())) {
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
                value, force_int || (m_coerce && Parser::float_is_intlike(value))
            );
        }

        // Otherwise return the payload as-is.
        return payload;
    }
}

Payload Evaluator::from_text_as_float()
{
    SignedParser* sparser = static_cast<SignedParser*>(m_parser);

    // Special-case handling of infinity and NaN
    if (sparser->is_infinity()) {
        return Payload(inf_action(sparser->is_negative()));
    } else if (sparser->is_nan()) {
        return Payload(nan_action(sparser->is_negative()));
    }

    // Otherwise, attempt to naively parse if possible, otherwise
    // use the python conversion function
    const double result = sparser->as_float();
    if (sparser->errored()) {
        return sparser->potential_overflow() ? to_pyfloat(sparser)
                                             : Payload(ActionType::ERROR_INVALID_FLOAT);
    }

    // Successful naive conversion
    return Payload(result);
}

/// Helper function to convert to Python int objects
static inline Payload to_pyint(SignedParser* parser)
{
    PyObject* result = parser->as_pyint();
    if (parser->errored()) {
        return Payload(ActionType::ERROR_INVALID_INT);
    }
    return Payload(result);
}

Payload Evaluator::from_text_as_int()
{
    SignedParser* sparser = static_cast<SignedParser*>(m_parser);

    // We use python to convert non-base-10 integer strings.
    // Some strings are not allowed to use an explict base,
    // so check that first.
    if (sparser->get_base() != 10) {
        if (sparser->illegal_explicit_base()) {
            return Payload(ActionType::ERROR_ILLEGAL_EXPLICIT_BASE);
        }
        return to_pyint(sparser);
    }

    // Otherwise, attempt to naively parse if possible, otherwise
    // use the python conversion function
    const long result = sparser->as_int();
    if (sparser->errored()) {
        return sparser->potential_overflow() ? to_pyint(sparser)
                                             : Payload(ActionType::ERROR_INVALID_INT);
    }

    // Successful naive conversion
    return Payload(result);
}

/* TEXT EXTRACTION */

void Evaluator::extract_string_data()
{
    // Use short-circuit logic to extract string data from the python object
    // from most likely to least likely.
    const bool success = extract_from_unicode() || extract_from_bytes()
        || extract_from_bytearray() || extract_from_buffer();
    if (!success) {
        m_parser = new Parser;
    }
}

bool Evaluator::extract_from_unicode()
{
    if (PyUnicode_Check(m_obj)) {
        // Unicode in ASCII form is stored like bytes!
        if (PyUnicode_IS_READY(m_obj) && PyUnicode_IS_COMPACT_ASCII(m_obj)) {
            m_parser = new CharacterParser(
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
        m_parser = new CharacterParser(
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
        m_parser = new CharacterParser(
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
        m_char_buffer.copy(static_cast<char*>(view.buf), str_len);
        m_char_buffer.start()[str_len] = '\0';

        // All we care about is the underlying buffer data, not the obj
        // which was allocated when we created the buffer. For this reason
        // it is safe to release the buffer here.
        PyBuffer_Release(&view);
        constexpr bool explict_base_allowed = true;
        m_parser
            = new CharacterParser(m_char_buffer.start(), str_len, !explict_base_allowed);
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

    // Ensure input is a valid unicode object.
    // If true, then not OK for conversion.
    if (PyUnicode_READY(m_obj)) {
        m_char_buffer.start()[0] = '\0';
        m_parser = new CharacterParser(m_char_buffer.start(), 0);
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

    // Remember if it was negative
    const bool negative = PyUnicode_READ(kind, data, index) == '-';

    // Protect against attempting to allocate too much memory
    if (static_cast<std::size_t>(len) + 1 > m_char_buffer.max_size()) {
        m_char_buffer.start()[0] = '\0';
        m_parser = new CharacterParser(m_char_buffer.start(), 0);
        return true;
    }

    // Allocate space for the character data, but use a small fixed size
    // buffer if the data is small enough. Ensure a trailing null character.
    m_char_buffer.reserve(static_cast<std::size_t>(len));
    char* buffer = m_char_buffer.start();
    std::size_t buffer_index = 0;

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
                m_parser = new UnicodeParser(u, negative);
                return true;
            }
            buffer[0] = '\0';
            m_parser = new CharacterParser(buffer, 0);
            return true;
        }
        buffer_index += 1;
    }
    buffer[buffer_index] = '\0';

    m_parser = new CharacterParser(buffer, buffer_index);
    return true;
}
