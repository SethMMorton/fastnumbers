#pragma once

#include <cmath>
#include <cstring>
#include <vector>

#include <Python.h>


// Size of the fix-with buffer
constexpr Py_ssize_t FIXED_BUFFER_SIZE = 32;


/// Possible types of Parser objects
enum class ParserType {
    NUMERIC,    ///< The parser is handling numeric Python objects
    UNICODE,    ///< The parser is handling single unicode characters
    CHARACTER,  ///< The parser is handling C-style character arrays
    UNKNOWN,    ///< The incoming object unknown to the parser
};


/**
 * \class Parser
 * \brief Flexible parser of various possible Python input types
 * 
 * Parses the input as a number.
 */
class Parser
{
public:
    // Constructors, destructors, and assignment
    // There is no constructor with arguments
    Parser()
        : ptype(ParserType::UNKNOWN)
        , number_type(NumberType::NOT_FLOAT_OR_INT)
        , obj(nullptr)
        , negative(false)
        , uchar('\0')
        , numeric_uchar(-1.0)
        , digit_uchar(-1L)
        , start(nullptr)
        , str_len(0)
        , base(10)
        , default_base(true)
        , errcode(0)
        , underscore_allowed(true)
    {}
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    ~Parser()  { Py_XDECREF(obj); };

    /// What type of parser is this?
    ParserType parser_type() const { return ptype; };

    /**
     * \brief Assign a Python object to be parsed as a number
     * \param obj Pointer to the Python object to be parsed
     */
    void set_input(PyObject* obj);

    /**
     * \brief Assign a unicode character to be parsed as a number
     * \param uchar The unicode character to be parsed
     * \param negative Whether or not the character was negative
     */
    void set_input(const Py_UCS4 uchar, const bool negative=false);

    /**
     * \brief Assign a character array to be parsed as a number
     * \param str The character array to be parsed
     * \param len The length of the character array
     */
    void set_input(const char* str, const std::size_t len);

    /// Tell the analyzer the base to use when parsing ints
    void set_base(const int base) {
        default_base = base == INT_MIN;
        this->base = default_base ? 10 : base;
    }

    /// Get the stored base
    int get_base() const { return base; }

    /// Was the default base given?
    bool is_default_base() const { return default_base; }

    /// Define whether or not underscores are allowed
    void set_allow_underscores(const bool val) { underscore_allowed = val; }

    /// Are underscores allowed?
    bool are_underscores_allowed() const { return underscore_allowed; }

    /// Convert the stored object to a long (-1 if not possible, check error state)
    long as_int();

    /// Convert the stored object to a double (-1.0 if not possible, check error state)
    double as_float();

    /// Whether the last conversion encountered an error
    bool errored() const { return errcode != 0; }

    /// Whether the last conversion potentially had an overflow
    bool potential_overflow() const { return errcode == 2; }

    /// Was the passed Python object a user class with __float__ or __int__?
    bool is_special_numeric() const {
        return number_type == NumberType::SPECIAL_NUMERIC;
    }

    /// Is the stored number negative?
    bool is_negative() const { return negative; }

    /// Was the passed Python object not float or int?
    bool not_float_or_int() const;

    /// Was the passed Python object finite?
    bool is_finite() const;

    /// Was the passed Python object infinity?
    bool is_infinity() const;

    /// Was the passed Python object NaN?
    bool is_nan() const;

    /// Was the passed Python object real (e.g. float or int)?
    bool is_real() const;

    /// Was the passed Python object a float?
    bool is_float() const;

    /// Was the passed Python object an int?
    bool is_int() const;

    /**
     * \brief Was the passed Python object intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    bool is_intlike() const;

    /**
     * \brief Determine if a float is "intlike"
     *
     * This is defined as a float that can be converted to an int with
     * no information loss.
     */
    static bool float_is_intlike(const double x) {
        errno = 0;
        return std::isfinite(x) and std::floor(x) == x and errno == 0;
    }

private:
    ParserType ptype;

    /// The types of data this class can store
    enum NumberType {
        NOT_FLOAT_OR_INT,
        FLOAT,
        INT,
        SPECIAL_NUMERIC,
    };

    /// Tracker of what type is being stored
    NumberType number_type;

    /// The Python object potentially under analysis
    PyObject* obj;

    /// Whether or not the text is a negative number
    bool negative;

    /// The single unicode character to potentially parse
    Py_UCS4 uchar;

    /// The potential numeric value of a unicode character
    double numeric_uchar;

    /// The potential digit value of a unicode character
    long digit_uchar;

    /// The potential start of the character array
    const char* start;

    /// The potential length of the character array
    std::size_t str_len;

    /// The desired base of integers when parsing
    int base;

    /// If the user-given base is the default base
    bool default_base;

    /// Track if a number conversion failed.
    int errcode;

    /// Whether or not underscores are allowed when parsing
    bool underscore_allowed;

private:
    /// Reset the Parser object to the uninitialized state
    void reset() {
        Py_XDECREF(obj);
        ptype = ParserType::UNKNOWN;
        number_type = NumberType::NOT_FLOAT_OR_INT;
        obj = nullptr;
        negative = false;
        uchar = '\0';
        numeric_uchar = -1.0;
        digit_uchar = -1L;
        start = nullptr;
        str_len = 0;
        unset_error_code();
    }

    /// Integer that can be used to apply the sign of the number in the text
    int sign() const { return negative ? -1 : 1; }

    /// Check if the character array contains valid underscores
    bool has_valid_underscores() const {
        return start != nullptr
            and are_underscores_allowed()
            and str_len > 0
            and std::memchr(start, '_', str_len);
    }

    /// Check if the character array contains invalid underscores
    bool has_invalid_underscores() const {
        return start != nullptr
            and not are_underscores_allowed()
            and str_len > 0
            and std::memchr(start, '_', str_len);
    }

    /// The end of the stored character array
    const char* end() const { return start == nullptr ? nullptr : (start + str_len); }

    /// Record that the conversion encountered an error
    void encountered_conversion_error() { errcode = 1; }

    /// Record that the conversion encountered a potential overflow
    void encountered_potential_overflow_error() { errcode = 2; }

    /// Forget any tracked error code
    void unset_error_code() { errcode = 0; }
};


/**
 * \class NumericMethodsParser
 * \brief Determine if an object defines numeric dunder methods
 */
class NumericMethodsAnalyzer {
public:
    NumericMethodsAnalyzer(PyObject *obj)
        : nmeth(obj ? Py_TYPE(obj)->tp_as_number : nullptr)
    {}
    NumericMethodsAnalyzer(const NumericMethodsAnalyzer&) = default;
    NumericMethodsAnalyzer(NumericMethodsAnalyzer&&) = default;
    NumericMethodsAnalyzer& operator=(const NumericMethodsAnalyzer&) = default;
    ~NumericMethodsAnalyzer() = default;

    /// Does this object define __index__, __int__, or __float?
    bool is_numeric() const { return is_int() or is_float(); };

    /// Does this object define __float?
    bool is_float() const { return nmeth and nmeth->nb_float; }

    /// Does this object define __index__ or __int__?
    bool is_int() const { return nmeth and (nmeth->nb_index or nmeth->nb_int); }

private:
    /// The struct providing access to numeric dunder methods
    PyNumberMethods *nmeth;

};


/**
 * \class Buffer
 * \brief A buffer of character data
 */
class Buffer {
public:
    /// Allocate buffer space
    Buffer(const std::size_t needed_length)
        : fixed_buffer()
        , variable_buffer()
        , buffer(nullptr)
        , len(needed_length)
    {
        if (len + 1 < FIXED_BUFFER_SIZE) {
            buffer = fixed_buffer;
        } else {
            variable_buffer.reserve(len + 1);
            buffer = variable_buffer.data();
        }
    }

    /// Allocate buffer space and copy data into it
    Buffer(const char* data, const std::size_t length)
        : Buffer(length)
    {
        std::memcpy(buffer, data, len);
    }

    Buffer(const Buffer&) = default;
    Buffer(Buffer&&) = default;
    Buffer& operator=(const Buffer&) = default;
    ~Buffer() = default;

    /// Return the start of the buffer
    char* start() { return buffer; }

    /// Return the end of the buffer
    char* end() { return buffer + len; }

    /// Return the length of the buffer
    std::size_t length() { return len; }

    /// Remove underscores that are syntactically valid in a number
    void remove_valid_underscores() { remove_valid_underscores(false); }

    /// Remove underscores that are syntactically valid in a number,
    /// possibly accounting for non-base-10 integers
    void remove_valid_underscores(const bool based);

private:
    /// Size of the fix-with buffer
    static const std::size_t FIXED_BUFFER_SIZE = 32;

    /// A string buffer of fixed size, in case data must be copied
    char fixed_buffer[FIXED_BUFFER_SIZE];

    /// A string buffer of variable size, in case large data must be copied
    std::vector<char> variable_buffer;

    /// Pointer to the character buffer being used
    char* buffer;

    /// The length of the character array
    std::size_t len;
};
