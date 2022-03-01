#pragma once

#include <climits>
#include <vector>

#include <Python.h>

#include "fastnumbers/options.h"
#include "fastnumbers/parser.hpp"

// Size of the fix-with buffer
constexpr Py_ssize_t FIXED_BUFFER_SIZE = 32;

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
        , nan_action(false)
        , inf_action(false)
        , parser()
    {
        evaluate_stored_object();
    }

    // Other constructors, destructors, and assignment
    Evaluator() : Evaluator(nullptr) {}
    Evaluator(const Evaluator&) = delete;
    Evaluator(Evaluator&&) = delete;
    Evaluator& operator=(const Evaluator&) = delete;
    ~Evaluator() = default;

    /**
     * \brief Assign a new object to analyze
     * \param obj A PyObject pointer to analyze
     */
    void set_object(PyObject* obj) {
        this->obj = obj;
        evaluate_stored_object();
    }

    /**
     * \brief Tell the analyzer the base to use when parsing ints
     * \param base The integer base
     */
    void set_base(const int base) {
        parser.set_base(base);
    }

    /**
     * \brief Tell the analyzer whether or not to coerce to int for REAL
     * \param coerce Whether or not to coerce
     */
    void set_coerce(const bool coerce) {
        this->coerce = coerce;
    }

    /**
     * \brief Tell the analyzer how to deal with NaN
     * \param nan_action Whether or not NaN will need special handling
     */
    void set_nan_action(const bool nan_action) {
        this->nan_action = nan_action;
    }

    /**
     * \brief Tell the analyzer how to deal with infinity
     * \param inf_action Whether or not infinity will need special handling
     */
    void set_inf_action(const bool inf_action) {
        this->inf_action = inf_action;
    }

    /// Return the parser type currenly associated with the Evaluator
    ParserType parser_type() {
        return parser.parser_type();
    }

    /**
     * \brief Was the passed Python object of the correct type?
     *
     * \param ntype PyNumberType indicating the desired type to check
     * \return bool
     */
    bool is_type(const PyNumberType ntype) const;

    /// Is the stored type a float? Account for nan_action and inf_action.
    bool type_is_float() const {
        return (nan_action and parser.is_nan())
            or (inf_action and parser.is_infinity())
            or parser.is_float();
    }

    /// Is the stored type an integer? If coerce is true, is the type intlike?
    bool type_is_int() const {
        return coerce ? parser.is_intlike() : parser.is_int();
    }

private:
    /// The Python object under evaluation
    PyObject* obj;

    /// A string buffer of fixed size, in case data must be copied
    char fixed_buffer[FIXED_BUFFER_SIZE];

    /// A string buffer of variable size, in case large data must be copied
    std::vector<char> variable_buffer;

    /// Whether or not floats should be coerced to integers if user wants REAL
    bool coerce;

    /// Whether or not an NaN requires a special handling
    bool nan_action;

    /// Whether or not an infinity requires a special handling
    bool inf_action;

    /// A Parser object used for evaluating the Python object
    Parser parser;

private:
    /// Generate a Parser object from Python object data
    void evaluate_stored_object() {
        if (obj != nullptr) {
            parser.set_input(obj);
            if (parser.not_numeric()) {
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

};
