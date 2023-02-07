#pragma once

#include <stdexcept>
#include <utility>

#include <Python.h>

#include "fastnumbers/selectors.hpp"

/// Custom exception class to tell the handler to just return NULL
class just_return_null_exception : public std::runtime_error {
public:
    just_return_null_exception()
        : std::runtime_error("")
    { }
};

/**
 * \class ListBuilder
 * \brief Handles the details of creating and managing a Python list
 */
class ListBuilder {
public:
    /**
     * \brief Construct the manager with a list of a fixed size
     * \param length The initial length of the list to construct
     */
    explicit ListBuilder(const Py_ssize_t length)
        : m_list(PyList_New(length))
        , m_index(0)
    {
        if (m_list == nullptr) {
            throw just_return_null_exception();
        }
    }

    /**
     * \brief Construct the manager with a list of a size based
     *         on a hint from another object
     * \param length The object with the length hint
     */
    explicit ListBuilder(PyObject* length_hint_base)
        : ListBuilder(get_length_hint(length_hint_base))
    { }

    // Deleted
    ListBuilder(const ListBuilder&) = delete;
    ListBuilder(ListBuilder&&) = delete;
    ListBuilder& operator=(const ListBuilder&) = delete;

    // Default
    ~ListBuilder() = default;

    /**
     * \brief Add an item to the end of the list
     * \param item The item to add to the list
     */
    void append(PyObject* item)
    {
        // Protect against incoming NULLs.
        if (item == nullptr) {
            throw just_return_null_exception();
        }

        // The list may have been pre-allocated using the length hint.
        // If so, the elements will be just populated with NULL.
        // If the current index equals the list size, that means we are
        // at the end of the list and we can just use append. Otherwise,
        // we insert the object directly into the list, replacing NULL.
        if (PyList_GET_SIZE(m_list) == m_index) {
            if (PyList_Append(m_list, item)) {
                Py_DECREF(m_list);
                throw just_return_null_exception();
            }
        } else {
            PyList_SET_ITEM(m_list, m_index, item);
        }

        // Increment the index of where we are currently at in the list.
        m_index += 1;
    }

    /// Return the stored list to the user
    PyObject* get() { return m_list; }

private:
    /// The list itself
    PyObject* m_list;

    /// The current location where we should add to the list
    Py_ssize_t m_index;

private:
    /// Obtain the length hint from a Python object
    static Py_ssize_t get_length_hint(PyObject* length_hint_base)
    {
        Py_ssize_t length_hint = PyObject_LengthHint(length_hint_base, 0);
        if (length_hint < 0) {
            throw just_return_null_exception();
        }
        return length_hint;
    }
};

/// Track the state of the iteration
enum class IterState {
    CONTINUE, ///< Keep the iteration going
    STOP, ///< Stop the iteration
};

/**
 * \class IterableManager
 * \brief Makes iteration over a Python iterable with a ranged for loop possible
 */
template <typename Function>
class IterableManager {
public:
    /// Constructor
    explicit IterableManager(PyObject* potential_iterable, Function convert)
        : m_object(potential_iterable)
        , m_iterator(nullptr)
        , m_index(0)
        , m_seq_size(0)
        , m_convert(convert)
    {
        if (PyList_Check(m_object) || PyTuple_Check(m_object)) {
            m_seq_size = PySequence_Fast_GET_SIZE(m_object);
        } else {
            if ((m_iterator = PyObject_GetIter(m_object)) == nullptr) {
                throw just_return_null_exception();
            }
        }
    }

    /// Destructor
    ~IterableManager() { Py_XDECREF(m_iterator); }

    // Deleted
    IterableManager(const IterableManager&) = delete;
    IterableManager(IterableManager&&) = delete;
    IterableManager& operator=(const IterableManager&) = delete;

    /**
     * \class ItemIterator
     * \brief An iterator over the IterableManager
     */
    class ItemIterator {
    public:
        // Define the iterator type
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = PyObject*;
        using pointer = value_type*;
        using reference = value_type&;

        /**
         * \brief Construct the ItemIterator and obtain the first
         *        ViolationData if available.
         */
        explicit ItemIterator(IterableManager* parent)
            : m_parent(parent)
            , m_payload(nullptr)
            , m_state(IterState::STOP)
        {
            // Prime the iterator
            if (m_parent != nullptr) {
                this->operator++();
            }
        }

        /// Default constructor
        ItemIterator()
            : ItemIterator(nullptr)
        { }

        // Default other constructors/destructors
        ItemIterator(const ItemIterator& other) = default;
        ItemIterator(ItemIterator&&) = default;
        ItemIterator& operator=(const ItemIterator& other) = default;
        ~ItemIterator() = default;

        /// Access the IterableManager data
        reference operator*() { return m_payload; }

        /// Access the IterableManager data as a pointer
        pointer operator->() { return &m_payload; }

        /// Increment the ItemIterator
        ItemIterator& operator++()
        {
            if (m_parent != nullptr) {
                std::tie(m_payload, m_state) = m_parent->next();
            }
            return *this;
        }

        /// Compare two ItemIterator objects for equality.
        bool operator==(const ItemIterator& rhs) const
        {
            return m_payload == rhs.m_payload && m_state == rhs.m_state;
        }

        /// Compare two ItemIterator objects for inequality.
        bool operator!=(const ItemIterator& rhs) const { return !operator==(rhs); }

    private:
        /// A pointer to the object that instantiated the iterator
        IterableManager* m_parent;

        /// The data returned by the parent for the current iteration
        PyObject* m_payload;

        /// State indicating if iteration should stop or not
        IterState m_state;
    };

    /// Convenient name for the ItemIterator
    typedef ItemIterator iterator;

    /// Return an iterator over the IterableManager
    iterator begin() { return iterator(this); };

    /// The end of the iterator over the IterableManager
    iterator end() { return iterator(); };

private:
    /// The object that is currently being iterated over
    PyObject* m_object;

    /// NULL if a fast sequence (e.g. list/tuple), the iterator object otherwise
    PyObject* m_iterator;

    /// The location we are in the sequence, if the input is a sequence
    Py_ssize_t m_index;

    /// The size of the sequence, if the input is a sequence
    Py_ssize_t m_seq_size;

    /// The function used to convert data
    Function m_convert;

private:
    std::pair<PyObject*, IterState> next()
    {
        PyObject* item = nullptr;

        // If no iterator is stored, then the object was a fast sequence and
        // we can access the data directly.
        if (m_iterator == nullptr) {
            // When at the end of the sequence, return the sigil
            if (m_index == m_seq_size) {
                return std::make_pair(nullptr, IterState::STOP);
            }

            // Access the data in the input sequence directly.
            // The returned object is a borrowed reference, so we do not
            // need to manage the reference counts.
            item = PySequence_Fast_GET_ITEM(m_object, m_index);

            // Before moving on, increment our internal counter.
            m_index += 1;

            // Conver the item to the correct value and return.
            return std::make_pair(m_convert(item), IterState::CONTINUE);
        }

        // Otherwise, the object was an iterator and we use the iteration
        // protocol to get each next item.
        // When a nullptr is returned then it is the end of the iteration
        // and we return return the sigil.
        if ((item = PyIter_Next(m_iterator)) == nullptr) {
            return std::make_pair(nullptr, IterState::STOP);
        }

        // Convert the item and return the value. When complete we must decrease
        // the item reference count (because it was not returned as a borrowed
        // reference), so some error handling must be done to ensure it happens.
        try {
            return std::make_pair(m_convert(item), IterState::CONTINUE);
        } catch (...) {
            Py_DECREF(item);
            throw;
        }
        Py_DECREF(item);
    }
};