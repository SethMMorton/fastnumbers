#pragma once

#include <cstring>

#include <Python.h>

#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 11
// This function was introduced in Python 3.11, but is super-useful
// in obtaining the name of a type.
// The implementation was basically copied from the 3.11 source code,
// but adjustments were made to make it C++.
inline PyObject* PyType_GetName(PyTypeObject* type)
{
    auto _PyType_Name = [](PyTypeObject* type) -> const char* {
        assert(type->tp_name != nullptr);
        const char* s = std::strrchr(type->tp_name, '.');
        if (s == nullptr) {
            s = type->tp_name;
        } else {
            s++;
        }
        return s;
    };

    auto type_name = [_PyType_Name](PyTypeObject* type, void* context) -> PyObject* {
        if (type->tp_flags & Py_TPFLAGS_HEAPTYPE) {
            PyHeapTypeObject* et = (PyHeapTypeObject*)type;

            Py_INCREF(et->ht_name);
            return et->ht_name;
        } else {
            return PyUnicode_FromString(_PyType_Name(type));
        }
    };

    return type_name(type, nullptr);
}
#endif
