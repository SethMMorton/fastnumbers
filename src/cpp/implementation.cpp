/*
 * This file contains the high-level implementations for the Python-exposed functions
 */
#include <limits>
#include <string_view>
#include <variant>

#include <Python.h>

#include "fastnumbers/ctype_extractor.hpp"
#include "fastnumbers/evaluator.hpp"
#include "fastnumbers/exception.hpp"
#include "fastnumbers/extractor.hpp"
#include "fastnumbers/implementation.hpp"
#include "fastnumbers/iteration.hpp"
#include "fastnumbers/parser.hpp"
#include "fastnumbers/payload.hpp"
#include "fastnumbers/resolver.hpp"
#include "fastnumbers/selectors.hpp"
#include "fastnumbers/user_options.hpp"

PyObject* Implementation::convert(PyObject* input) const noexcept(false)
{
    return m_resolver.resolve(input, collect_payload(input));
}

PyObject* Implementation::check(PyObject* input) const noexcept(false)
{
    // Assess what types we can call this input
    auto [from_str, ok_float, ok_int, ok_intlike] = resolve_types(collect_type(input));

    // For FLOAT, we are OK with integers only if not in strict mode.
    ok_int = m_ntype == UserType::FLOAT ? (from_str && !m_strict && ok_int) : ok_int;

    // The logic of what is True depends on the type we are trying to check
    switch (m_ntype) {
    case UserType::REAL:
    case UserType::FLOAT:
        if (ok_float || ok_int) {
            Py_RETURN_TRUE;
        }
        break;
    default:
        // ok_intline is never true unless set_coerce was given as true
        if (ok_int || ok_intlike) {
            Py_RETURN_TRUE;
        }
    }
    Py_RETURN_FALSE;
}

PyObject* Implementation::query_type(PyObject* input) const noexcept(false)
{
    // Assess what types we can call this input
    auto [from_str, ok_float, ok_int, ok_intlike] = resolve_types(collect_type(input));

    // If the input can be interpreted as a number, return that number
    // type. Otherwise, return the type of the input.
    PyObject* found_type = (ok_int || ok_intlike)
        ? (PyObject*)&PyLong_Type
        : (ok_float ? (PyObject*)&PyFloat_Type : (PyObject*)Py_TYPE(input));

    // If allowed types were given and the found type is not there, return None
    if (m_allowed_types != nullptr
        && !PySequence_Contains(m_allowed_types, found_type)) {
        Py_RETURN_NONE;
    }

    // Return the type of the input
    Py_IncRef(found_type);
    return found_type;
}

void Implementation::set_consider(const PyObject* val) noexcept(false)
{
    const bool ok = val == Py_None || val == Selectors::NUMBER_ONLY
        || val == Selectors::STRING_ONLY;
    if (!ok) {
        throw fastnumbers_exception(
            "allowed values for 'consider' are None, fastnumbers.NUMBER_ONLY, or "
            "fastnumbers.STRING_ONLY"
        );
    }
    m_num_only = val == Selectors::NUMBER_ONLY;
    m_str_only = val == Selectors::STRING_ONLY;
}

void Implementation::set_allowed_types(PyObject* val) noexcept(false)
{
    if (val != nullptr) {
        if (!PySequence_Check(val)) {
            PyErr_Format(
                PyExc_TypeError, "allowed_type is not a sequence type: %R", val
            );
            throw exception_is_set();
        }
        if (PySequence_Length(val) < 1) {
            throw fastnumbers_exception("allowed_type must not be an empty sequence");
        }
    }
    m_allowed_types = Selectors::incref(val);
}

Payload Implementation::collect_payload(PyObject* obj) const noexcept(false)
{
    Buffer buffer;

    // extract_parser() is responsible for taking a python object
    // and returning the Parser object best suited to parser the object's data.
    // std:visit is used to obtain the payload data no matter which parser was
    // returned.
    return std::visit(
        [this, obj](const auto& parser) -> Payload {
            return Evaluator<decltype(parser)>(obj, m_options, parser).as_type(m_ntype);
        },
        extract_parser(obj, buffer, m_options)
    );
}

NumberFlags Implementation::collect_type(PyObject* obj) const noexcept(false)
{
    Buffer buffer;

    // extract_parser() is responsible for taking a python object
    // and returning the Parser object best suited to parser the object's data.
    // std:visit is used to obtain the number flag no matter which parser was
    // returned.
    return std::visit(
        [this, obj](const auto& parser) -> NumberFlags {
            if (m_str_only && parser.parser_type() == ParserType::NUMERIC) {
                return NumberType::INVALID;
            } else if (m_num_only && parser.parser_type() != ParserType::NUMERIC) {
                return NumberType::INVALID;
            }
            return Evaluator<decltype(parser)>(obj, m_options, parser).number_type();
        },
        extract_parser(obj, buffer, m_options)
    );
}

Implementation::Types Implementation::resolve_types(const NumberFlags& flags
) const noexcept
{
    // Build up the logic with individual "Boolean chunks"
    const bool from_str = bool(flags & (NumberType::FromStr | NumberType::FromUni));
    const bool from_num = bool(flags & NumberType::FromNum);
    const bool no_inf_str = from_str && !m_options.allow_inf_str();
    const bool no_nan_str = from_str && !m_options.allow_nan_str();
    const bool no_inf_num = from_num && !m_options.allow_inf_num();
    const bool no_nan_num = from_num && !m_options.allow_nan_num();
    const bool no_inf = no_inf_str || no_inf_num;
    const bool no_nan = no_nan_str || no_nan_num;
    const bool bad_inf = no_inf && flags & NumberType::Infinity;
    const bool bad_nan = no_nan && flags & NumberType::NaN;

    // Set the final results as results of the booleans
    return Types { from_str,
                   /* ok_float */ flags & NumberType::Float && !(bad_inf || bad_nan),
                   /* ok_int */ bool(flags & NumberType::Integer),
                   /* ok_intlike */ m_options.allow_coerce()
                       && flags & NumberType::IntLike };
}

void Implementation::validate_allow_disallow_str_only_num_only(const PyObject* selector
) const noexcept(false)
{
    const bool ok = selector == Selectors::ALLOWED || selector == Selectors::DISALLOWED
        || selector == Selectors::NUMBER_ONLY || selector == Selectors::STRING_ONLY;
    if (!ok) {
        throw fastnumbers_exception(
            "allowed values for 'inf' and 'nan' are fastnumbers.ALLOWED, "
            "fastnumbers.DISALLOWED, fastnumbers.NUMBER_ONLY, or "
            "fastnumbers.STRING_ONLY"
        );
    }
}

void Implementation::validate_not_allow_disallow_str_only_num_only(
    const PyObject* selector
) const noexcept(false)
{
    const bool bad = selector == Selectors::ALLOWED || selector == Selectors::DISALLOWED
        || selector == Selectors::NUMBER_ONLY || selector == Selectors::STRING_ONLY;
    if (bad) {
        throw fastnumbers_exception(
            "values for 'on_fail' and 'on_type_error' cannot be fastnumbers.ALLOWED, "
            "fastnumbers.DISALLOWED, fastnumbers.NUMBER_ONLY, or "
            "fastnumbers.STRING_ONLY"
        );
    }
}

void Implementation::validate_not_disallow(const PyObject* selector) const
    noexcept(false)
{
    const bool bad = selector == Selectors::DISALLOWED
        || selector == Selectors::STRING_ONLY || selector == Selectors::NUMBER_ONLY;
    if (bad) {
        throw fastnumbers_exception("'inf' and 'nan' cannot be fastnumbers.DISALLOWED, "
                                    "fastnumbers.STRING_ONLY, or fastnumbers.NUMBER_ONLY"
        );
    }
}

// Implementation for iterating over a collection to populate a list
PyObject* list_iteration_impl(
    PyObject* input, std::function<PyObject*(PyObject*)> convert
) noexcept(false)
{
    // Create a python list into which to store the return values
    ListBuilder list_builder(input);

    // The helper for iterating over the Python iterable
    IterableManager<PyObject*> iter_manager(input, convert);

    // For each element in the Python iterable, convert it and append to the list
    for (auto& value : iter_manager) {
        list_builder.append(value);
    }

    // Return the list to the user
    return list_builder.get();
}

/**
 * \struct FastnumbersIterator
 * \brief Object containing the state of the fastnumbers iterator
 *
 * This is a PyObject "subclass" that enables iterating over fastnumbers
 * results.
 *
 * It is written in a very C-like way because it has to interface with C-code.
 */
struct FastnumbersIterator {
    // clang-format off
    PyObject_HEAD

    /// The Python object over which to iterated
    PyObject* it_input;
    // clang-format on

    /// Pointer to the active IterableManager
    IterableManager<PyObject*>* it_man;

    /// The C++ iterator
    IterableManager<PyObject*>::ItemIterator it_iter;

    // Track if this is the first iteration or not
    bool it_first;

    /// Deallocate the itertor object
    static void dealloc(FastnumbersIterator* it) noexcept
    {
        Py_DECREF(it->it_input);
        delete it->it_man;
    }

    /// Get a guess of the length of the iterator
    static PyObject*
    len_guess(FastnumbersIterator* it, PyObject* Py_UNUSED(ignored)) noexcept(false)
    {
        return PyLong_FromSsize_t(get_length_hint(it->it_input));
    }

    /// Return the next value of the iterator
    static PyObject* next(FastnumbersIterator* it) noexcept(false)
    {
        assert(it != nullptr);
        assert(it->it_man != nullptr);

        // Run inside an exception handler to ensure anything we throw gets converted
        // into a Python exception.
        return ExceptionHandler(it->it_input).run([&it]() -> PyObject* {
            // On the first iteration, prime the iterator.
            // On subsequent iterations, just increment it.
            if (it->it_first) {
                it->it_first = false;
                it->it_iter = it->it_man->begin();
            } else {
                ++it->it_iter;
            }

            // If the C++ iterator is exhausted, return NULL to
            // tell Python the iterator is exhausted
            if (it->it_iter == it->it_man->end()) {
                return nullptr;
            }

            // Return the current value held in the itertor
            return *it->it_iter;
        });
    }
};

/// Extra methods of the fastnumbers iterable object no standard in a type object
static PyMethodDef fastnumbers_iterator_methods[] = {
    { "__length_hint__",
      (PyCFunction)FastnumbersIterator::len_guess,
      METH_NOARGS,
      "Get a length guess for the fastnumbers iterator" },
    { NULL, NULL } /* sentinel */
};

/// The fastnumbers iterator type object definition
PyTypeObject FastnumbersIteratorType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0) "fastnumbers_iterator", /* tp_name */
    sizeof(FastnumbersIterator), /* tp_basicsize */
    0, /* tp_itemsize */
    /* methods */
    (destructor)FastnumbersIterator::dealloc, /* tp_dealloc */
    0, /* tp_vectorcall_offset */
    0, /* tp_getattr */
    0, /* tp_setattr */
    0, /* tp_as_async */
    0, /* tp_repr */
    0, /* tp_as_number */
    0, /* tp_as_sequence */
    0, /* tp_as_mapping */
    0, /* tp_hash */
    0, /* tp_call */
    0, /* tp_str */
    PyObject_GenericGetAttr, /* tp_getattro */
    0, /* tp_setattro */
    0, /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT, /* tp_flags */
    0, /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    PyObject_SelfIter, /* tp_iter */
    (iternextfunc)FastnumbersIterator::next, /* tp_iternext */
    fastnumbers_iterator_methods, /* tp_methods */
    0,
};

// Implementation for iterating over a collection to populate an iterator
PyObject* iter_iteration_impl(
    PyObject* input, std::function<PyObject*(PyObject*)> convert
) noexcept(false)
{
    // Create an instance of our iterator object as our iterator type
    FastnumbersIterator* it
        = PyObject_New(FastnumbersIterator, &FastnumbersIteratorType);
    if (it == nullptr) {
        return nullptr;
    }

    // Add to it the helper for iterating over the Python iterable
    it->it_man = new IterableManager<PyObject*>(input, convert);

    // Initialize with the end
    it->it_iter = it->it_man->end();

    // Store the input object over which we are iterating
    it->it_input = input;
    Py_INCREF(it->it_input);

    // Indicate this is the first iteration
    it->it_first = true;

    // Return our iterator instance to Python-land
    return (PyObject*)it;
}

/**
 * \struct ArrayImpl
 * \brief Executor of array population, manages Python memory buffer
 */
struct ArrayImpl {
    /// The input object as given by Python
    PyObject* m_input;

    /// The output object, represented as a memory view buffer
    Py_buffer& m_output;

    /// The action to take if INF is found
    PyObject* m_inf;

    /// The action to take if NaN is found
    PyObject* m_nan;

    /// The action to take if input is invalid
    PyObject* m_on_fail;

    /// The action to take if input overflows
    PyObject* m_on_overflow;

    /// The action to take if input is of incorrect type
    PyObject* m_on_type_error;

    /// Whether or not to allow underscores in strings
    bool m_allow_underscores;

    /// The base to use when parsing integers
    int m_base;

    /// Release the Python memoryview buffer
    ~ArrayImpl() noexcept { PyBuffer_Release(&m_output); }

    /// Perform the actual array population logic
    template <typename T>
    void execute() noexcept(false)
    {
        UserOptions options;
        options.set_base(m_base);
        options.set_underscores_allowed(m_allow_underscores);

        // Define how a Python object can be converted into a C number type
        CTypeExtractor<T> extractor(options);
        extractor.set_inf_replacement(m_inf);
        extractor.set_nan_replacement(m_nan);
        extractor.set_fail_replacement(m_on_fail);
        extractor.set_overflow_replacement(m_on_overflow);
        extractor.set_type_error_replacement(m_on_type_error);

        // Define how we convert each element of the iterable
        IterableManager<T> iter_man(m_input, [&extractor](PyObject* x) -> T {
            return extractor.extract_c_number(x);
        });

        // Create a handler for inserting data into the output memory buffer
        ArrayPopulator pop(m_output, iter_man.get_size());

        // Iterate over the input data, convert it, and place it in the output
        for (const auto& value : iter_man) {
            pop.place_next(value);
        }
    }
};

/**
 * \brief Validate the selector is not a "yes, no, num, str, input" value
 * \param selector The python object to validate
 * \throws fastnumbers_exception if one of the four valid values
 */
static inline void
validate_not_allow_disallow_str_only_num_only_input(const PyObject* selector
) noexcept(false)
{
    const bool bad = selector == Selectors::ALLOWED || selector == Selectors::DISALLOWED
        || selector == Selectors::NUMBER_ONLY || selector == Selectors::STRING_ONLY
        || selector == Selectors::INPUT;
    if (bad) {
        throw fastnumbers_exception(
            "values for 'on_fail', 'on_overflow', and 'on_type_error' cannot be "
            "fastnumbers.ALLOWED, fastnumbers.DISALLOWED, fastnumbers.NUMBER_ONLY, "
            "fastnumbers.STRING_ONLY, or fastnumbers.INPUT"
        );
    }
}

/**
 * \brief Validate the selector is not a "no, num, str, input" value
 * \param selector The python object to validate
 * \throws fastnumbers_exception if one of the four valid values
 */
static inline void validate_not_disallow_str_only_num_only_input(const PyObject* selector
) noexcept(false)
{
    const bool bad = selector == Selectors::DISALLOWED
        || selector == Selectors::NUMBER_ONLY || selector == Selectors::STRING_ONLY
        || selector == Selectors::INPUT || selector == Selectors::RAISE;
    if (bad) {
        throw fastnumbers_exception(
            "values for 'inf' and 'nan' cannot be fastnumbers.DISALLOWED, "
            "fastnumbers.NUMBER_ONLY, fastnumbers.STRING_ONLY, fastnumbers.INPUT "
            "or fastnumbers.RAISE"
        );
    }
}

// Implementation for iterating over a collection to populate an array
void array_impl(
    PyObject* input,
    PyObject* output,
    PyObject* inf,
    PyObject* nan,
    PyObject* on_fail,
    PyObject* on_overflow,
    PyObject* on_type_error,
    bool allow_underscores,
    int base
) noexcept(false)
{
    // Ensure the given parameters are valid.
    validate_not_disallow_str_only_num_only_input(inf);
    validate_not_disallow_str_only_num_only_input(nan);
    validate_not_allow_disallow_str_only_num_only_input(on_fail);
    validate_not_allow_disallow_str_only_num_only_input(on_overflow);
    validate_not_allow_disallow_str_only_num_only_input(on_type_error);

    // Extract the underlying buffer data from the output object
    Py_buffer buf { nullptr, nullptr };
    constexpr auto flags = PyBUF_WRITABLE | PyBUF_STRIDES | PyBUF_FORMAT;
    if (PyObject_GetBuffer(output, &buf, flags) != 0) {
        // This should be impossible to encounter because of guards in the python code
        throw exception_is_set();
    }

    // Pass on all arguments to the actual implementation
    // NOTE: This will manage the buffer object for us
    ArrayImpl impl {
        input, buf, inf, nan, on_fail, on_overflow, on_type_error, allow_underscores,
        base,
    };

    // Use the format to determine the code path to execute
    // Attempt to order this if-branch by anticipated frequency of use
    const std::string_view format(buf.format == nullptr ? "<NULL>" : buf.format);
    if (format == "d") {
        return impl.execute<double>();
    } else if (format == "l") {
        return impl.execute<signed long>();
    } else if (format == "q") {
        return impl.execute<signed long long>();
    } else if (format == "i") {
        return impl.execute<signed int>();
    } else if (format == "f") {
        return impl.execute<float>();
    } else if (format == "L") {
        return impl.execute<unsigned long>();
    } else if (format == "Q") {
        return impl.execute<unsigned long long>();
    } else if (format == "I") {
        return impl.execute<unsigned int>();
    } else if (format == "h") {
        return impl.execute<signed short>();
    } else if (format == "b") {
        return impl.execute<signed char>();
    } else if (format == "H") {
        return impl.execute<unsigned short>();
    } else if (format == "B") {
        return impl.execute<unsigned char>();
    }

    // This should be impossible to encounter because of guards in the python code
    PyErr_Format(
        PyExc_TypeError,
        "Unknown buffer format '%s' for object '%.200R'",
        buf.format,
        output
    );
    throw exception_is_set();
}