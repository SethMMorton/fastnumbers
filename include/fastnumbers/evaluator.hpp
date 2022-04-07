#pragma once

#include <climits>
#include <vector>

#include <Python.h>

#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"


// Size of the fix-with buffer
constexpr Py_ssize_t FIXED_BUFFER_SIZE = 32;


enum class UserType {
    REAL,
    FLOAT,
    INT,
    INTLIKE,
    FORCEINT,
};


/**
 * \class Evaluator
 * \brief Evaluate the contents of a Python object
 */
class Evaluator {
public:

    /// Constructor from a Python object
    explicit Evaluator(PyObject* obj)
        : obj(obj)
        , fixed_buffer()
        , variable_buffer()
        , coerce(false)
        , nan_allowed(false)
        , inf_allowed(false)
        , unicode_allowed(true)
        , parser()
    {
        evaluate_stored_object();
    }

    // Other constructors, destructors, and assignment
    Evaluator() : Evaluator(nullptr) {}
    Evaluator(const Evaluator&) = delete;
    Evaluator(Evaluator&&) = delete;
    Evaluator& operator=(const Evaluator&) = delete;
    ~Evaluator() { Py_XDECREF(obj); }

    /// Assign a new object to analyze
    void set_object(PyObject* obj) {
        this->obj = obj;
        evaluate_stored_object();
    }

    /// Tell the analyzer the base to use when parsing ints
    void set_base(const int base) {
        parser.set_base(base);
    }

    /// Tell the analyzer whether or not to coerce to int for REAL
    void set_coerce(const bool coerce) {
        this->coerce = coerce;
    }

    /// Tell the analyzer if NaN is allowed when type checking
    void set_nan_allowed(const bool nan_allowed) {
        this->nan_allowed = nan_allowed;
    }

    /// Tell the analyzer if infinity is allowed when type checking
    void set_inf_allowed(const bool inf_allowed) {
        this->inf_allowed = inf_allowed;
    }

    /// Tell the analyzer if unicode characters are allowed as input
    void set_unicode_allowed(const bool unicode_allowed) {
        this->unicode_allowed = unicode_allowed;
    }

    /// Return the base used for integer conversion
    int get_base() const { return parser.get_base(); }

    /// Was the default base given?
    bool is_default_base() const { return parser.is_default_base(); }

    /// Return the parser type currenly associated with the Evaluator
    ParserType parser_type() const { return parser.parser_type(); }

    /// Was the passed Python object of the correct type?
    bool is_type(const UserType ntype) const;

    /// Is the stored type a float? Account for nan_action and inf_action.
    bool type_is_float() const {
        return (nan_allowed and parser.is_nan())
            or (inf_allowed and parser.is_infinity())
            or parser.is_float();
    }

    /// Is the stored type an integer? If coerce is true, is the type intlike?
    bool type_is_int() const {
        return coerce ? parser.is_intlike() : parser.is_int();
    }

    /**
     * \brief Convert the stored object to the desired number type
     *
     * Use the appropriate error handling on error.
     *
     * \param ntype PyNumberType indicating the desired type to check
     * \return Payload
     */
    Payload as_type(const UserType ntype);

private:
    /// The Python object under evaluation
    PyObject* obj;

    /// A string buffer of fixed size, in case data must be copied
    char fixed_buffer[FIXED_BUFFER_SIZE];

    /// A string buffer of variable size, in case large data must be copied
    std::vector<char> variable_buffer;

    /// Whether or not floats should be coerced to integers if user wants REAL
    bool coerce;

    /// Whether or not an NaN is allowed
    bool nan_allowed;

    /// Whether or not an infinity is allowed
    bool inf_allowed;

    /// Whether or not a unicode character is allowed
    bool unicode_allowed;

    /// A Parser object used for evaluating the Python object
    Parser parser;

private:
    /// Generate a Parser object from Python object data
    void evaluate_stored_object() {
        if (obj != nullptr) {
            Py_IncRef(obj);
            parser.set_input(obj);
            if (parser.not_float_or_int() and not parser.is_special_numeric()) {
                extract_string_data();
            }
        }
    }

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

    /// Logic for evaluating a numeric python object
    Payload from_numeric_as_type(const UserType ntype);

    /// Logic for evaluating a text python object
    Payload from_text_as_type(const UserType ntype);

    /// Logic for evaluating a text python object as a float or integer
    Payload from_text_as_int_or_float(const bool force_int);

    /// Logic for evaluating a text python object as a float
    Payload from_text_as_float();

    /// Logic for evaluating a text python object as an int
    Payload from_text_as_int();

};
