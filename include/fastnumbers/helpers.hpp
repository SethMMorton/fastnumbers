#pragma once

/// Always evaluates to false - helps with static_assert messages
template <class>
inline constexpr bool always_false_v = false;

/// Aid in constructing multiple overloads from lambdas for std::visit
/// See https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
