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
        , number_type(NumberType::NOT_FLOAT_OR_INT)
        , obj(nullptr)
        , negative(false)
        , uchar('\0')
        , numeric_uchar(-1.0)
        , digit_uchar(-1L)
        , start(nullptr)
        , end(nullptr)
        , base(10)
        , default_base(true)
        , errcode(0)
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

    /// Tell the analyzer the base to use when parsing ints
    void set_base(const int base) {
        default_base = base == INT_MIN;
        this->base = default_base ? 10 : base;
    }

    /// Get the stored base
    int get_base() const { return base; }

    /// Was the default base given?
    bool is_default_base() const { return default_base; }

    /// Convert the stored object to a long (-1 if not possible, check error state)
    long as_int();

    /// Convert the stored object to a double (-1.0 if not possible, check error state)
    double as_float();

    /// Whether the last conversion encountered an error
    bool errored() const { return errcode != 0; }

    /// Whether the last conversion potentially had an overflow
    bool potential_overflow() const { return errcode == 2; }

    /// Whether the last conversion encountered an underscore
    bool underscore_error() const { return errcode < 0; }

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

    /// The potential end of the character array
    const char* end;

    /// The desired base of integers when parsing
    int base;

    /// If the user-given base is the default base
    bool default_base;

    /// Track if a number conversion failed.
    int errcode;

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
        end = nullptr;
        unset_error_code();
    }

    /// Integer that can be used to apply the sign of the number in the text
    int sign() const { return negative ? -1 : 1; }

    /// Record that the conversion encountered an error
    void encountered_conversion_error() { errcode = 1; }

    /// Record that the conversion encountered a potential overflow
    void encountered_potential_overflow_error() { errcode = 2; }

    /// Record that the conversion encountered an underscore
    void encountered_underscore_in_conversion() { errcode = -1; }

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
