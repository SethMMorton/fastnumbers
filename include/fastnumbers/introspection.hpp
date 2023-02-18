#pragma once

/// Return a string form of the given type
template <typename T>
constexpr const char* type_name();

template <>
constexpr const char* type_name<double>()
{
    return "double";
}

template <>
constexpr const char* type_name<float>()
{
    return "float";
}

template <>
constexpr const char* type_name<signed long long>()
{
    return "signed long long";
}

template <>
constexpr const char* type_name<signed long>()
{
    return "signed long";
}

template <>
constexpr const char* type_name<signed int>()
{
    return "signed int";
}

template <>
constexpr const char* type_name<signed short>()
{
    return "signed short";
}

template <>
constexpr const char* type_name<signed char>()
{
    return "signed char";
}

template <>
constexpr const char* type_name<unsigned long long>()
{
    return "unsigned long long";
}

template <>
constexpr const char* type_name<unsigned long>()
{
    return "unsigned long";
}

template <>
constexpr const char* type_name<unsigned int>()
{
    return "unsigned int";
}

template <>
constexpr const char* type_name<unsigned short>()
{
    return "unsigned short";
}

template <>
constexpr const char* type_name<unsigned char>()
{
    return "unsigned char";
}
