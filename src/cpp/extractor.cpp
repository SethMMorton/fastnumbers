#include <cstring>
#include <stdexcept>

#include <Python.h>

#include "fastnumbers/extractor.hpp"
#include "fastnumbers/parser.hpp"

void TextExtractor::extract_string_data()
{
    // Use short-circuit logic to extract string data from the python object
    // from most likely to least likely.
    extract_from_unicode() || extract_from_bytes() || extract_from_bytearray()
        || extract_from_buffer();
}

bool TextExtractor::extract_from_unicode()
{
    if (PyUnicode_Check(m_obj)) {
        // Unicode in ASCII form is stored like bytes!
        if (PyUnicode_IS_READY(m_obj) && PyUnicode_IS_COMPACT_ASCII(m_obj)) {
            m_str = (const char*)PyUnicode_1BYTE_DATA(m_obj);
            m_str_len = static_cast<const std::size_t>(PyUnicode_GET_LENGTH(m_obj));
            return true;
        }
        return parse_unicode_to_char();
    }
    return false;
}

bool TextExtractor::extract_from_bytes()
{
    if (PyBytes_Check(m_obj)) {
        m_str = PyBytes_AS_STRING(m_obj);
        m_str_len = static_cast<const std::size_t>(PyBytes_GET_SIZE(m_obj));
        return true;
    }
    return false;
}

bool TextExtractor::extract_from_bytearray()
{
    if (PyByteArray_Check(m_obj)) {
        m_str = PyByteArray_AS_STRING(m_obj);
        m_str_len = static_cast<const std::size_t>(PyByteArray_GET_SIZE(m_obj));
        return true;
    }
    return false;
}

bool TextExtractor::extract_from_buffer()
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
        m_str_len = static_cast<const std::size_t>(view.len);
        m_char_buffer.copy(static_cast<char*>(view.buf), m_str_len);
        m_char_buffer.start()[m_str_len] = '\0';

        // All we care about is the underlying buffer data, not the obj
        // which was allocated when we created the buffer. For this reason
        // it is safe to release the buffer here.
        PyBuffer_Release(&view);
        m_str = m_char_buffer.start();
        m_explicit_base_allowed = false;
        return true;
    }
    return false;
}

bool TextExtractor::parse_unicode_to_char()
{
    const unsigned kind = PyUnicode_KIND(m_obj); // Unicode storage format.
    const void* data = PyUnicode_DATA(m_obj); // Raw data
    Py_ssize_t len = PyUnicode_GET_LENGTH(m_obj);
    Py_ssize_t index = 0;

    // Ensure input is a valid unicode object.
    // If true, then not OK for conversion.
    if (PyUnicode_READY(m_obj)) {
        m_char_buffer.start()[0] = '\0';
        m_str = m_char_buffer.start();
        m_str_len = 0;
        return true;
    }

    // Strip whitespace from both ends of the data.
    while (Py_UNICODE_ISSPACE(PyUnicode_READ(kind, data, index)) && len >= 0) {
        index += 1;
        len -= 1;
    }
    if (len > 0) {
        while (Py_UNICODE_ISSPACE(PyUnicode_READ(kind, data, index + len - 1))) {
            len -= 1;
        }
    }

    // Remember if it was negative
    m_negative = PyUnicode_READ(kind, data, index) == '-';

    // Protect against attempting to allocate too much memory
    if (static_cast<std::size_t>(len) + 1 > m_char_buffer.max_size()) {
        m_char_buffer.start()[0] = '\0';
        m_str = m_char_buffer.start();
        m_str_len = 0;
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
    long u_as_decimal = 0;
    const Py_ssize_t data_len = len + index;
    static constexpr uint8_t ASCII_MAX = 127;
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
                m_uchar = u;
                return true;
            }
            buffer[0] = '\0';
            m_str = buffer;
            m_str_len = 0;
            return true;
        }
        buffer_index += 1;
    }
    buffer[buffer_index] = '\0';

    m_str = buffer;
    m_str_len = buffer_index;
    return true;
}
