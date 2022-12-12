#pragma once

#include <climits>
#include <cstddef>

#include <Python.h>

#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"

/**
 * \class TextExtractor
 * \brief Extract text data from an object
 */
class TextExtractor {
public:
    /// Constructor from a Python object
    TextExtractor(PyObject* obj, Buffer& buffer)
        : m_obj(obj)
        , m_char_buffer(buffer)
        , m_str(nullptr)
        , m_str_len(0)
        , m_uchar(0)
        , m_negative(false)
        , m_explicit_base_allowed(true)
    {
        Py_XINCREF(m_obj);
        PyNumberMethods* nmeth = Py_TYPE(m_obj)->tp_as_number;
        const bool user_numeric
            = nmeth && (nmeth->nb_index || nmeth->nb_int || nmeth->nb_float);
        if (!user_numeric) {
            extract_string_data();
        }
    }

    // Other constructors, destructors, and assignment
    TextExtractor() = delete;
    TextExtractor(const TextExtractor&) = delete;
    TextExtractor(TextExtractor&&) = delete;
    TextExtractor& operator=(const TextExtractor&) = delete;
    ~TextExtractor() { Py_XDECREF(m_obj); }

    /// Is text stored in this extractor?
    bool is_text() const { return m_str != nullptr; }

    /// Is a unicode character in this extractor?
    bool is_unicode_character() const { return m_uchar != 0; }

    /// Is there no text?
    bool is_non_text() const { return !(is_text() || is_unicode_character()); }

    /**
     * \brief Return a CharacterParser for the conained data
     *
     * Call is_text first to ensue it is valid
     * to create this parser.
     *
     * \param options A UserOptions instance containing the options
     *                specified by the user.
     *
     * \return CharacterParser
     */
    CharacterParser text_parser(const UserOptions& options) const
    {
        return CharacterParser(m_str, m_str_len, options, m_explicit_base_allowed);
    }

    /**
     * \brief Return a UnicodeParser for the conained data
     *
     * Call is_unicode_character first to ensue it is valid
     * to create this parser.
     *
     * \param options A UserOptions instance containing the options
     *                specified by the user.
     *
     * \return UnicodeParser
     */
    UnicodeParser unicode_char_parser(const UserOptions& options) const
    {
        return UnicodeParser(m_uchar, m_negative, options);
    }

private:
    /// The Python object under evaluation
    PyObject* m_obj;

    /// Buffer object into which to store character data
    Buffer& m_char_buffer;

    /// Pointer to string data
    const char* m_str;

    /// The length of the string
    std::size_t m_str_len;

    /// A single unicode characgter that is read
    Py_UCS4 m_uchar;

    /// Whether or not the unicode character was negative
    bool m_negative;

    /// Whether or not an explict base is allowed for text processing
    bool m_explicit_base_allowed;

private:
    /// Generate a text parsing Parser object
    void extract_string_data();

    /// Obtain character data from a Python unicode object
    bool extract_from_unicode();

    /// Obtain character data from a Python bytes object
    bool extract_from_bytes();

    /// Obtain character data from a Python bytearray object
    bool extract_from_bytearray();

    /// Obtain character data from a Python buffer object
    bool extract_from_buffer();

    /// Parse unicode data and convert to character data
    bool parse_unicode_to_char();
};
