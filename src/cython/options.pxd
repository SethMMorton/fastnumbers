from cpython.ref cimport PyObject

cdef extern from "fastnumbers/options.h":

    # Selector for the type of number to check/convert.
    enum PyNumberType:
        REAL, FLOAT, INT, INTLIKE, FORCEINT

    # This struct holds all the user options.
    # Makes adding new future options easier to manage.
    struct Options:
        PyObject *retval
        PyObject *input
        PyObject *on_fail
        PyObject *handle_inf
        PyObject *handle_nan
        int coerce
        int num_only
        int str_only
        int allow_underscores
        bint allow_uni
        int base

    # Some query shortcuts.
    bint Options_Coerce_True(const Options *o)
    bint Options_Has_NaN_Sub(const Options *o)
    bint Options_Has_INF_Sub(const Options *o)
    bint Options_Return_NaN_Sub(const Options *o)
    bint Options_Return_INF_Sub(const Options *o)
    bint Options_Should_Raise(const Options *o)
    bint Options_Default_Base(const Options *o)
    bint Options_Allow_UnicodeCharacter(const Options *o)
    bint Options_Allow_Infinity(const Options *o)
    bint Options_Allow_NAN(const Options *o)
    bint Options_Allow_Underscores(const Options *o)
    bint Options_String_Only(const Options *o)
    bint Options_Number_Only(const Options *o)

    # Set allow unicode.
    void Options_Set_Disallow_UnicodeCharacter(Options *o)

    # Return the correct result based on user-input.
    # Expects the Options struct as a pointer.
    PyObject* Options_Return_Correct_Result_On_Error(Options *o)

    # Set the correct return value based on given input.
    # Expects the Options struct NOT as a pointer.
    void Options_Set_Return_Value(
        Options o, PyObject *input, PyObject *default_value, bint raise_
    )
