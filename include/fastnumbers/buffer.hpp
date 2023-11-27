#pragma once

#include <cstring>
#include <limits>

#include "fastnumbers/c_str_parsing.hpp"

/**
 * \class Buffer
 * \brief A buffer of character data
 */
class Buffer {
public:
    /// Create a zero-sized buffer
    Buffer() noexcept
        : Buffer(0)
    { }

    /// Allocate buffer space
    explicit Buffer(const std::size_t needed_length)
        : m_fixed_buffer()
        , m_variable_buffer(nullptr)
        , m_buffer(nullptr)
        , m_len(needed_length)
        , m_size(0)
    {
        reserve(true);
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
    ~Buffer() noexcept { delete[] m_variable_buffer; };

    /// Restore the Buffer to an empty-like state
    void reset() noexcept
    {
        if (m_variable_buffer == nullptr) {
            m_buffer = m_fixed_buffer;
        } else {
            m_buffer = m_variable_buffer;
        }
        m_len = 0;
    }

    /// Return the start of the buffer
    char* start() noexcept { return m_buffer; }

    /// Return the end of the buffer
    char* end() noexcept { return m_buffer + m_len; }

    /// Return the length of the buffer
    std::size_t length() noexcept { return m_len; }

    /// Set aside a fixed lenth of data
    void reserve(const std::size_t needed_length) noexcept(false)
    {
        m_len = needed_length;
        reserve();
    }

    /// Copy a fixed length of data into the buffer
    void copy(const char* data, const std::size_t needed_length) noexcept(false)
    {
        reserve(needed_length);
        copy(data);
    }

    /// Remove underscores that are syntactically valid in a number
    void remove_valid_underscores() noexcept { remove_valid_underscores(false); }

    /// Remove underscores that are syntactically valid in a number,
    /// possibly accounting for non-base-10 integers
    void remove_valid_underscores(const bool based) noexcept
    {
        const char* new_end = end();
        ::remove_valid_underscores(m_buffer, new_end, based);
        m_len = static_cast<std::size_t>(new_end - m_buffer);
    }

    /// Remove a base prefix
    void remove_base_prefix() noexcept
    {
        // To remove the base prefix, first move past a possible negative sign,
        // then skip the base prefix. If the there was a negative sign, we
        // place it in the position before the first digit after the prefix.
        // Then, the internal buffer pointer is set to point to that new position.
        const bool is_signed = *m_buffer == '-';
        char* tracker = m_buffer;
        std::size_t len = m_len;
        if (is_signed) {
            tracker += 1;
            len -= 1;
        }
        if (has_base_prefix(tracker, len)) {
            tracker += 2;
            len -= 2;
            if (is_signed) {
                tracker -= 1;
                *tracker = '-';
                len += 1;
            }
            m_buffer = tracker;
            m_len = len;
        }
    }

    /// Mark the first '.', 'e', or 'E' as '\0'.
    void mark_integer_end() noexcept
    {
        // Technically, we set the exponent character to '\0'
        // and then adjust the length. We are not actually removing data.
        // Look for '.' or 'e'/'E'. Instead of searching simulataneously
        // use memchr because it is so darn fast.
        char* exp_loc = nullptr;
        for (const char c : { '.', 'e', 'E' }) {
            exp_loc = static_cast<char*>(std::memchr(m_buffer, c, m_len));
            if (exp_loc != nullptr) {
                *exp_loc = '\0';
                m_len = m_size = exp_loc - m_buffer;
                return;
            }
        }
    }

    /// The largest amount of data the buffer can contain
    std::size_t max_size() const noexcept
    {
        return std::numeric_limits<std::size_t>::max();
    }

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

    /// The current size of the buffer - should
    /// equal m_len except in intermediate states
    std::size_t m_size;

private:
    /// Set aside the amount of data stored in m_len
    void reserve(const bool force = false) noexcept(false)
    {
        // Only increase the size if needed
        if (m_len > m_size || force) {
            m_size = m_len;
            if (m_size < FIXED_BUFFER_SIZE) {
                m_buffer = m_fixed_buffer;
            } else {
                delete[] m_variable_buffer;
                m_variable_buffer = new char[m_size];
                m_buffer = m_variable_buffer;
            }
        }
    }

    /// Copy data into the buffer of the currently stored length
    void copy(const char* data) noexcept { std::memcpy(m_buffer, data, m_len); }
};
