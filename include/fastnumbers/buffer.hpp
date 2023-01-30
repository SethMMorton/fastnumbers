#pragma once

#include "fastnumbers/c_str_parsing.hpp"
#include <cstring>
#include <limits>

/**
 * \class Buffer
 * \brief A buffer of character data
 */
class Buffer {
public:
    /// Create a zero-sized buffer
    Buffer()
        : Buffer(0)
    { }

    /// Allocate buffer space
    explicit Buffer(const std::size_t needed_length)
        : m_fixed_buffer()
        , m_variable_buffer(nullptr)
        , m_buffer(nullptr)
        , m_len(needed_length)
    {
        reserve();
    }

    /// Allocate buffer space and copy data into it
    Buffer(const char* data, const std::size_t needed_length)
        : Buffer(needed_length)
    {
        copy(data);
    }

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    ~Buffer() { delete[] m_variable_buffer; };

    /// Return the start of the buffer
    char* start() { return m_buffer; }

    /// Return the end of the buffer
    char* end() { return m_buffer + m_len; }

    /// Return the length of the buffer
    std::size_t length() { return m_len; }

    /// Set aside a fixed lenth of data
    void reserve(const std::size_t needed_length)
    {
        m_len = needed_length;
        reserve();
    }

    /// Copy a fixed length of data into the buffer
    void copy(const char* data, const std::size_t needed_length)
    {
        reserve(needed_length);
        copy(data);
    }

    /// Remove underscores that are syntactically valid in a number
    void remove_valid_underscores() { remove_valid_underscores(false); }

    /// Remove underscores that are syntactically valid in a number,
    /// possibly accounting for non-base-10 integers
    void remove_valid_underscores(const bool based)
    {
        const char* new_end = end();
        ::remove_valid_underscores(start(), new_end, based);
        m_len = static_cast<std::size_t>(new_end - start());
    }

    /// The largest amount of data the buffer can contain
    std::size_t max_size() const { return std::numeric_limits<std::size_t>::max(); }

private:
    /// Size of the fix-with buffer
    static const std::size_t FIXED_BUFFER_SIZE = 32;

    /// A string buffer of fixed size, in case data must be copied
    char m_fixed_buffer[FIXED_BUFFER_SIZE];

    /// A string buffer of variable size, in case large data must be copied
    char* m_variable_buffer;

    /// Pointer to the character buffer being used
    char* m_buffer;

    /// The length of the character array
    std::size_t m_len;

private:
    /// Set aside the amount of data stored in m_len
    void reserve()
    {
        if (m_len + 1 < FIXED_BUFFER_SIZE) {
            m_buffer = m_fixed_buffer;
        } else {
            delete[] m_variable_buffer;
            m_variable_buffer = new char[m_len + 1];
            m_buffer = m_variable_buffer;
        }
    }

    /// Copy data into the buffer of the currently stored length
    void copy(const char* data) { std::memcpy(m_buffer, data, m_len); }
};
