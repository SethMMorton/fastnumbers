#pragma once


/// Possible actions that can be performed on input objects
enum class ActionType {  // TODO: annotate values
    AS_IS,
    FLOAT,
    INT,
    NAN_ACTION,
    INF_ACTION,
    ERROR,
};


/// Known classes of errors that can occur
enum class ErrorType {  // TODO: annotate values
    INF_AS_INT,
    NAN_AS_INT,
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
    /// Construct the payload with an error.
    explicit Payload(const ErrorType etype): type(PayloadType::ERROR), errval(etype) {}

    /// Construct the payload with a double.
    explicit Payload(const double val): type(PayloadType::FLOAT), dval(val) {}

    /// Construct the payload with a long.
    explicit Payload(const long val): type(PayloadType::INT), ival(val) {}

    /// Default copy constructor
    Payload(const Payload&) = default;

    /// Default assignment operator
    Payload& operator=(const Payload&) = default;

    /// Default destructor
    ~Payload() = default;

    /// Is the Payload storing an error?
    bool errored() const { return type == PayloadType::ERROR; }

    /// Is the Payload storing a double?
    bool has_double() const { return type == PayloadType::FLOAT; }

    /// Is the Payload storing a long?
    bool has_long() const { return type == PayloadType::INT; }

    /// Return the Payload as an ErrorType.
    ErrorType get_error() const { return errval; }

    /// Return the Payload as a double.
    double to_double() const { return dval; }

    /// Return the Payload as a long.
    long to_long() const { return ival; }

private:
    /// The types of data this class can store
    enum PayloadType {
        ERROR,
        INT,
        FLOAT,
    };

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
        /// The Payload as an ErrorType
        ErrorType errval;
    };

};
