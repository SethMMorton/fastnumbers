#pragma once

#include <functional>
#include <limits>

#include <Python.h>

#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/resolver.hpp"
#include "fastnumbers/selectors.hpp"
#include "fastnumbers/user_options.hpp"

/**
 * \class Implementation
 * \brief Encapsulates the logic of driving conversion/checking
 */
class Implementation {
public:
    /**
     * \brief Construct an Implementation object
     * \param ntype The user type that will dominate the Implementation actions
     * \param base The integer base used in parsing
     */
    Implementation(const UserType ntype, const int base)
        : m_options(create_options_with_base(base))
        , m_resolver(m_options.get_base())
        , m_ntype(ntype)
        , m_allowed_types(nullptr)
        , m_num_only(false)
        , m_str_only(false)
        , m_strict(false)
    { }

    /**
     * \brief Construct an Implementation object
     * \param ntype The user type that will dominate the Implementation actions
     */
    explicit Implementation(const UserType ntype)
        : Implementation(ntype, 10)
    { }

    // Copy, assignment, and destruct are defaults
    Implementation(const Implementation&) = default;
    Implementation(Implementation&&) = default;
    Implementation& operator=(const Implementation&) = default;
    ~Implementation() = default;

    /// Convert the object to the desired user type
    PyObject* convert(PyObject* input) const;

    /// Check if the object is the desired user type
    PyObject* check(PyObject* input) const;

    /// Query the type of the object
    PyObject* query_type(PyObject* input) const;

    /// Set the action to perform when INF is found
    void set_inf_action(PyObject* val)
    {
        validate_not_disallow(val);
        m_resolver.set_inf_action(val);
    }

    /// Set the action to perform when NaN is found
    void set_nan_action(PyObject* val)
    {
        validate_not_disallow(val);
        m_resolver.set_nan_action(val);
    }

    /// Set the action to perform on conversion failure
    void set_fail_action(PyObject* val)
    {
        validate_not_allow_disallow_str_only_num_only(val);
        m_resolver.set_fail_action(val);
    }

    /// Set the action to perform on type error
    void set_type_error_action(PyObject* val)
    {
        validate_not_allow_disallow_str_only_num_only(val);
        m_resolver.set_type_error_action(val);
    }

    /// Set whether or not underscores are allowed in strings
    void set_underscores_allowed(const bool val)
    {
        m_options.set_underscores_allowed(val);
    }

    /// Set whether intlike floats should be returned as ints
    void set_coerce(const bool val) { m_options.set_coerce(val); }

    /// Set whether we accept NaN
    void set_nan_allowed(const PyObject* val)
    {
        validate_allow_disallow_str_only_num_only(val);
        m_options.set_nan_allowed(val);
    }

    /// Set whether we accept INF
    void set_inf_allowed(const PyObject* val)
    {
        validate_allow_disallow_str_only_num_only(val);
        m_options.set_inf_allowed(val);
    }

    /// Set whether we accept unicode characters
    void set_unicode_allowed(const bool val) { m_options.set_unicode_allowed(val); }

    /// Set whether we accept unicode characters based on if base is default or not
    void set_unicode_allowed() { set_unicode_allowed(m_options.is_default_base()); }

    /// Set the types of input we consider for checking
    void set_consider(const PyObject* val);

    /// Set the types of strings allowed for float checking
    void set_strict(const bool val) { m_strict = val; }

    /// Set the types we allow when querying for type
    void set_allowed_types(PyObject* val) { m_allowed_types = val; }

private:
    /// Store the user-specified options
    UserOptions m_options;

    /// The object that resovles a payload into the appropriate return value
    Resolver m_resolver;

    /// The type the user wants to conver/check
    const UserType m_ntype;

    /// For type querying, a sequence of allowed types
    PyObject* m_allowed_types;

    /// For checking, indicate if only numbers are allowed as input
    bool m_num_only;

    /// For checking, indicate if only strings are allowed as input
    bool m_str_only;

    /// For checking floats, indicate input must be strictly floating-point
    bool m_strict;

    /// Return value for resolve_types() function
    struct Types {
        bool from_str;
        bool ok_float;
        bool ok_int;
        bool ok_intlike;
    };

private:
    /// Retrieve the type from the input object
    NumberFlags collect_type(PyObject* obj) const;

    /// Convert the object to the desired user type
    Payload collect_payload(PyObject* obj) const;

    /// Figure out as what types we can label the input
    Types resolve_types(const NumberFlags& flags) const;

    /// Create an options object with base - used in initialization list
    UserOptions create_options_with_base(const int base) const
    {
        UserOptions options;
        options.set_base(base);
        return options;
    }

    /**
     * \brief Validate the selector has only a "yes, no, num, str" value
     * \param selector The python object to validate
     * \throws fastnumbers_exception if not one of the four valid values
     */
    void validate_allow_disallow_str_only_num_only(const PyObject* selector) const;

    /**
     * \brief Validate the selector is not a "yes, no, num, str" value
     * \param selector The python object to validate
     * \throws fastnumbers_exception if one of the four valid values
     */
    void validate_not_allow_disallow_str_only_num_only(const PyObject* selector) const;

    /**
     * \brief Validate the selector is not "DISALLOWED"
     * \param selector The python object to validate
     * \throws fastnumbers_exception if the value is "DISALLOWED"
     */
    void validate_not_disallow(const PyObject* selector) const;
};

/**
 * \brief Iterate over the elements of a collection and convert each one into a list
 *
 * \param input The given input object that should be iterable
 * \param convert A function accepting a single argument that performs the conversion
 * \return A new python list containing the converted results, or nullptr on error
 */
PyObject*
list_iteration_impl(PyObject* input, std::function<PyObject*(PyObject*)> convert);

/**
 * \brief Iterate over the elements of a collection and convert each one into an iterator
 *
 * \param input The given input object that should be iterable
 * \param convert A function accepting a single argument that performs the conversion
 * \return A new python iterator producing the converted results, or nullptr on error
 */
PyObject*
iter_iteration_impl(PyObject* input, std::function<PyObject*(PyObject*)> convert);

/**
 * \brief Iterate over the elements of a collection and convert each one
 *
 * \param input The given input object that should be iterable
 * \param output The object containing the array to populate
 * \param inf The object specifying what action to take if INF is found
 * \param nan The object specifying what action to take if NaN is found
 * \param on_fail The object specifying what action to take on conversion failure
 * \param on_overflow The object specifying what action to take on overflow
 * \param on_type_error The object specifying what action to take on type error
 * \param allow_underscores Whether or not it is OK for numbers to contain underscores
 * \param base The integer base use when parsing ints, use INT_MIN for default
 */
void array_impl(
    PyObject* input,
    PyObject* output,
    PyObject* inf,
    PyObject* nan,
    PyObject* on_fail,
    PyObject* on_overflow,
    PyObject* on_type_error,
    bool allow_underscores,
    const int base = std::numeric_limits<int>::min()
);