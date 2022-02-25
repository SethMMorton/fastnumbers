#pragma once

#include <cmath>

#include <Python.h>

#include "fastnumbers/payload.hpp"
#include "fastnumbers/options.h"

/**
 * \brief Determine input and output types of Python numbers
 *
 * Internally stores the type of a Python number, and then also
 * can tell you what to do with the number given a desired end-state.
 */
class NumericAnalyzer
{
public:

    /**
     * \brief Default construct the NumericAnalyzer.
     *
     * All values are null-like.
     */
    NumericAnalyzer() : NumericAnalyzer(nullptr) {}

    /**
     * \brief Construct with an object.
     * \param obj A PyObject pointer to analyze
     */
    NumericAnalyzer(PyObject* obj)
        : obj(obj)
        , type(analyze_input(obj))
        , coerce(false)
        , nan_action(false)
        , inf_action(false)
    {}

    /// Default copy constructor
    NumericAnalyzer(const NumericAnalyzer&) = default;

    /// Default assignment operator
    NumericAnalyzer& operator=(const NumericAnalyzer&) = default;

    /// Default destructor
    ~NumericAnalyzer() = default;

    /**
     * \brief Assign a new object to analyze
     * \param obj A PyObject pointer to analyze
     */
    void set_object(PyObject* obj) {
        this->obj = obj;
        type = analyze_input(obj);
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

    /// Was the passed Python object not numeric?
    bool not_numeric() const { return type == NumberType::NOT_A_NUMBER; }

    /// Was the passed Python object real (e.g. float or int)?
    bool is_real() const { return type != NumberType::NOT_A_NUMBER; }

    /// Was the passed Python object a float?
    bool is_float() const { return type == NumberType::FLOAT; }

    /// Was the passed Python object an int?
    bool is_int() const { return type == NumberType::INT; }

    /**
     * \brief Was the passed Python objecte intlike?
     *
     * "intlike" is defined as either an int, or a float that can be
     * converted to an int with no loss of information.
     */
    bool is_intlike() const {
        switch (type) {
        case NumberType::NOT_A_NUMBER:
            return false;
        case NumberType::INT:
            return true;
        case NumberType::FLOAT:
            return float_is_intlike(PyFloat_AS_DOUBLE(obj));
        default:
            return false;  // cannot reach, but this silences compiler warnings
        }
    }

    /**
     * \brief Was the passed Python objecte intlike?
     *
     * \param ntype PyNumberType indicating the desired type to check
     * \return bool
     */
    bool is_type(const PyNumberType ntype) const {
        switch (ntype) {
        case PyNumberType::REAL:
            return is_real();

        case PyNumberType::FLOAT:
            return is_float();

        case PyNumberType::INT:
            return is_int();

        case PyNumberType::INTLIKE:
        case PyNumberType::FORCEINT:
            return is_intlike();

        default:
            return false;  // cannot reach, but this silences compiler warnings
        }
    }

    /**
     * \brief Assess what action should happen to the Python object
     *
     * One of the valid ActionType objects will be returned based on
     * the user requested type given by ntype.
     *
     * \param ntype PyNumberType indicating the desired conversion to be taken
     * \return ActionType
     */
    ActionType required_action(const PyNumberType ntype) const {
        switch (ntype) {
        case PyNumberType::REAL:
            if (coerce and is_float() and float_is_intlike(PyFloat_AS_DOUBLE(obj))) {
                return ActionType::INT;
            }
            return possible_special_case(ActionType::AS_IS);

        case PyNumberType::FLOAT:
            return possible_special_case(ActionType::FLOAT);

        case PyNumberType::INT:
        case PyNumberType::INTLIKE:
        case PyNumberType::FORCEINT:
            return possible_special_case(ActionType::INT);
        }
    }

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
    /// The Python object under analysis
    PyObject* obj;

    /// The types of data this class can store
    enum NumberType {
        NOT_A_NUMBER,
        FLOAT,
        INT,
    };

    /// Tracker of what type is being stored
    NumberType type;

    /// Whether or not floats should be coerced to integers if user wants REAL
    bool coerce;

    /// Whether or not an NaN requires a special handling
    bool nan_action;

    /// Whether or not an infinity requires a special handling
    bool inf_action;

private:
    /// Assess what type to store as given an the PyObject input
    static NumberType analyze_input(const PyObject* obj) {
        if (obj == nullptr) {
            return NumberType::NOT_A_NUMBER;
        } else if (PyFloat_Check(obj)) {
            return NumberType::FLOAT;
        } else if (PyLong_Check(obj)) {
            return NumberType::INT;
        } else {
            return NumberType::NOT_A_NUMBER;
        }
    }

    /**
     * \brief If the number is a special case, modify the return ActionType
     *
     * It is checked if the object is not numeric, NaN, or infinity, and
     * if so possible alternate ActionTypes are returned. Otherwise,
     * the given ActionType is returned.
     *
     * \param default_action The ActionType to return if not a special case
     * \return ActionType
     */
    ActionType possible_special_case(const ActionType default_action) const {
        if (not_numeric()) {
            return ActionType::ERROR;
        } else if ((nan_action or inf_action) and is_float()) {
            if (nan_action and std::isnan(PyFloat_AS_DOUBLE(obj))) {
                return ActionType::NAN_ACTION;
            } else if (inf_action and std::isinf(PyFloat_AS_DOUBLE(obj))) {
                return ActionType::INF_ACTION;
            }
        }
        return default_action;
    }
};
