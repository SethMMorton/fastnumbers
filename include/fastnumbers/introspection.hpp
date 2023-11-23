#pragma once

/// Return a string form of the given type
template <typename T>
constexpr inline const char* type_name() noexcept;

template <>
constexpr inline const char* type_name<double>() noexcept
{
    return "double";
}

template <>
constexpr inline const char* type_name<float>() noexcept
{
    return "float";
}

template <>
constexpr inline const char* type_name<signed long long>() noexcept
{
    return "signed long long";
}

template <>
constexpr inline const char* type_name<signed long>() noexcept
{
    return "signed long";
}

template <>
constexpr inline const char* type_name<signed int>() noexcept
{
    return "signed int";
}

template <>
constexpr inline const char* type_name<signed short>() noexcept
{
    return "signed short";
}

template <>
constexpr inline const char* type_name<signed char>() noexcept
{
    return "signed char";
}

template <>
constexpr inline const char* type_name<unsigned long long>() noexcept
{
    return "unsigned long long";
}

template <>
constexpr inline const char* type_name<unsigned long>() noexcept
{
    return "unsigned long";
}

template <>
constexpr inline const char* type_name<unsigned int>() noexcept
{
    return "unsigned int";
}

template <>
constexpr inline const char* type_name<unsigned short>() noexcept
{
    return "unsigned short";
}

template <>
constexpr inline const char* type_name<unsigned char>() noexcept
{
    return "unsigned char";
}
