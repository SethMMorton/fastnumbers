#pragma once

/// Always evaluates to false - helps with static_assert messages
template <class>
inline constexpr bool always_false_v = false;
