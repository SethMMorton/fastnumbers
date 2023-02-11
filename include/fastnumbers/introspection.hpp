#pragma once

/// Return a string form of the given type
template <typename T>
constexpr const char* type_name();

template <>
constexpr const char* type_name<long double>()
{
    return "long double";
}

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
constexpr const char* type_name<long long>()
{
    return "long long";
}

template <>
constexpr const char* type_name<long>()
{
    return "long";
}

template <>
constexpr const char* type_name<int>()
{
    return "int";
}

template <>
constexpr const char* type_name<short>()
{
    return "short";
}

template <>
constexpr const char* type_name<char>()
{
    return "char";
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
