#include <cstddef>
#include <variant>

#include <Python.h>

#include "fastnumbers/buffer.hpp"
#include "fastnumbers/extractor.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/user_options.hpp"

// Forward declarations
AnyParser parse_unicode_to_char(
    PyObject* obj, Buffer& char_buffer, const UserOptions& options
) noexcept(false);

AnyParser
extract_parser(PyObject* obj, Buffer& buffer, const UserOptions& options) noexcept(false)
{
    buffer.reset();

    // Checking if the object is numeric is very fast and easy. Do it
    // first. It also handles the case where a string subclass implements
    // a numeric method (in which case it is supposed to be considered numeric).
    PyNumberMethods* nmeth = Py_TYPE(obj)->tp_as_number;
    const bool is_numeric
        = nmeth && (nmeth->nb_index || nmeth->nb_int || nmeth->nb_float);
    if (is_numeric) {
        return NumericParser(obj, options);
    }

    // str, bytes, and bytearray objects can just have the string extracted
    // directly. The only exception is if the str is not stored as ASCII
    // in which case we have to do some special transformations.
    if (PyUnicode_Check(obj)) {
        // Unicode in ASCII form is stored like bytes!
        if (PyUnicode_IS_READY(obj) && PyUnicode_IS_COMPACT_ASCII(obj)) {
            return CharacterParser(
                (const char*)PyUnicode_1BYTE_DATA(obj),
                static_cast<const std::size_t>(PyUnicode_GET_LENGTH(obj)),
                options
            );
        }

        // Here is the special-case handling for non-ASCII unicode.
        return parse_unicode_to_char(obj, buffer, options);
    } else if (PyBytes_Check(obj)) {
        return CharacterParser(
            PyBytes_AS_STRING(obj),
            static_cast<const std::size_t>(PyBytes_GET_SIZE(obj)),
            options
        );
    } else if (PyByteArray_Check(obj)) {
        return CharacterParser(
            PyByteArray_AS_STRING(obj),
            static_cast<const std::size_t>(PyByteArray_GET_SIZE(obj)),
            options
        );
    }

    // A less-common case is a memory buffer, which requires a bit extra
    // handling compared to the above.
    Py_buffer view = { nullptr, nullptr };
    if (PyObject_CheckBuffer(obj) && PyObject_GetBuffer(obj, &view, PyBUF_SIMPLE) == 0) {
        // NOTE: PyBUF_SIMPLE implies zero-dimensional byte data.
        // This buffer could be a memoryview slice. If this is the case, the
        // nul termination of the string will be past the given length, creating
        // unexpected parsing results. Rather than complicate the parsing and
        // adding more operations for a low probability event, a copy of the
        // slice will be made here and null termination will be added.
        const std::size_t len = static_cast<const std::size_t>(view.len);
        buffer.reserve(len + 1);
        PyBuffer_ToContiguous(buffer.start(), &view, len, 'A');
        buffer.start()[len] = '\0';

        // All we care about is the underlying buffer data, not the obj
        // which was allocated when we created the buffer. For this reason
        // it is safe to release the buffer here.
        PyBuffer_Release(&view);
        return CharacterParser(buffer.start(), len, options, false);
    }

    // If here, we have no idea what the type is. The NumericParser is
    // responsible for handling type errors.
    return NumericParser(obj, options);
}

/// Obtain either a CharacterParser or UnicodeParser from unicode data
AnyParser parse_unicode_to_char(
    PyObject* obj, Buffer& char_buffer, const UserOptions& options
) noexcept(false)
{
    const unsigned kind = PyUnicode_KIND(obj); // Unicode storage format.
    const void* data = PyUnicode_DATA(obj); // Raw data
    Py_ssize_t len = PyUnicode_GET_LENGTH(obj);
    Py_ssize_t index = 0;

    // Ensure input is a valid unicode object.
    // If true, then not OK for conversion - unclear how this can happen...
    if (PyUnicode_READY(obj)) {
        return CharacterParser("", 0, options);
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
    const bool negative = PyUnicode_READ(kind, data, index) == '-';

    // Protect against attempting to allocate too much memory
    if (static_cast<std::size_t>(len) + 1 > char_buffer.max_size()) {
        return CharacterParser("", 0, options);
    }

    // Allocate space for the character data, but use a small fixed size
    // buffer if the data is small enough. Ensure a trailing null character.
    char_buffer.reserve(static_cast<std::size_t>(len) + 1);
    char* buffer = char_buffer.start();
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
                return UnicodeParser(u, negative, options);
            }
            return CharacterParser("", 0, options);
        }
        buffer_index += 1;
    }
    buffer[buffer_index] = '\0';

    return CharacterParser(buffer, buffer_index, options);
}