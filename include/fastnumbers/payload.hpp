#pragma once


/// Possible actions that can be performed on input objects
enum class ActionType {  // TODO: annotate values
    AS_IS,
    AS_FLOAT,
    AS_INT,
    TRY_INT_IN_PYTHON,
    TRY_FLOAT_IN_PYTHON,
    TRY_FLOAT_THEN_FORCE_INT_IN_PYTHON,
    TRY_FLOAT_THEN_COERCE_INT_IN_PYTHON,
    NAN_ACTION,
    INF_ACTION,
    NEG_NAN_ACTION,
    NEG_INF_ACTION,
    ERROR_INVALID_INT,
    ERROR_INVALID_FLOAT,
    ERROR_INVALID_BASE,
    ERROR_INFINITY_TO_INT,
    ERROR_NAN_TO_INT,
    ERROR_BAD_TYPE_INT,
    ERROR_BAD_TYPE_FLOAT,
};


/// The types of data this class can store
enum class PayloadType {
    ACTION,
    LONG,
    DOUBLE,
    DOUBLE_TO_LONG,
};


/**
 * \brief Transfer data intended to be converted to Python objects
 * 
 * Various functions can return different values (or errors) depending
 * on user parameters. This class uniformly stores all types for smoothest
 * transfer into "Python-land".
 */
class Payload
{
public:
    /// Default construct - needed for use with Cython.
    Payload() : type(PayloadType::ACTION), actval(ActionType::AS_IS) {}

    /// Construct the payload with an action.
    explicit Payload(const ActionType atype) : type(PayloadType::ACTION), actval(atype) {}

    /// Construct the payload with a double.
    explicit Payload(const double val) : type(PayloadType::DOUBLE), dval(val) {}

    /// Construct the payload with a double that needs to be be converted to an int.
    explicit Payload(const double val, const bool needs_int_conversion)
        : type(needs_int_conversion ? PayloadType::DOUBLE_TO_LONG : PayloadType::DOUBLE)
        , dval(val)
    {}

    /// Construct the payload with a long.
    explicit Payload(const long val) : type(PayloadType::LONG), ival(val) {}

    // Copy, assignment, and destruct are defaults
    Payload(const Payload&) = default;
    Payload(Payload&&) = default;
    Payload& operator=(const Payload&) = default;
    ~Payload() = default;

    /// What type of payload is being carried?
    PayloadType payload_type() const { return type; }

    /// Return the Payload as an ActionType.
    ActionType get_action() const { return actval; }

    /// Return the Payload as a double.
    double to_double() const { return dval; }

    /// Return the Payload as a long.
    long to_long() const { return ival; }

private:

    /// Tracker of what type is being stored
    PayloadType type;

    /**
     * \brief All possible Payload types, occupying the same memory
     *
     * To minimize copy overhead, all possible Payload types are
     * placed in a union so that there is no wasted space. Only one
     * of these can be valid at a time.
     */
    union {
        /// The Payload as a long
        long ival;
        /// The Payload as a double
        double dval;
        /// The Payload as an ActionType
        ActionType actval;
    };

};
