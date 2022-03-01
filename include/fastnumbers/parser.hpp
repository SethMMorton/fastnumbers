#pragma once

#include <cmath>

#include <Python.h>

#include "fastnumbers/parsing.h"


/// Possible types of Parser objects
enum class ParserType {  // TODO: annotate values
    NUMERIC,
    UNICODE,
    CHARACTER,
    UNKNOWN,
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
        , number_type(NumberType::NOT_NUMERIC)
        , obj(nullptr)
        , negative(false)
        , uchar('\0')
        , numeric_uchar(-1.0)
        , digit_uchar(-1L)
        , start(nullptr)
        , end(nullptr)
        , base(10)
    {}
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    ~Parser() = default;

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
    void set_input(const char* str, const size_t len);

    ///  Tell the analyzer the base to use when parsing ints
    void set_base(const int base) { this->base = base == INT_MIN ? 10 : base; }

    /// Was the passed Python object not numeric?
    bool not_numeric() const;

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
        NOT_NUMERIC,
        FLOAT,
        INT,
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

    /// The potential end of the character array
    const char* end;

    /// The desired base of integers when parsing
    int base;

private:
    /// Reset the Parser object to the uninitialized state
    void reset() {
        ptype = ParserType::UNKNOWN;
        number_type = NumberType::NOT_NUMERIC;
        obj = nullptr;
        negative = false;
        uchar = '\0';
        numeric_uchar = -1.0;
        digit_uchar = -1L;
        start = nullptr;
        end = nullptr;
    }

    /// Integer that can be used to apply the sign of the number in the text
    int sign() const { return negative ? -1 : 1; }
};
