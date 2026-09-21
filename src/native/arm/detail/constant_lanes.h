// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <type_traits>

namespace native::detail::arm_constant {
  // Work with logical lanes, including the padded two-word integer shape.
  template<class V> constexpr auto lanes(V value) noexcept {
    using T = typename V::value_type;
    auto physical = __builtin_bit_cast(std::array<T, sizeof(typename V::native_type) / sizeof(T)>, value.to_native());
    std::array<T, V::lanes> result{};
    for (unsigned i = 0; i < V::lanes; ++i) result[i] = physical[i];
    return result;
  }

  template<class V, class T, std::size_t N>
  constexpr V pack(std::array<T, N> const &values) noexcept {
    static_assert(N == V::lanes);
    std::array<T, sizeof(typename V::native_type) / sizeof(T)> physical{};
    for (unsigned i = 0; i < N; ++i) physical[i] = values[i];
    return V::from_native(__builtin_bit_cast(typename V::native_type, physical));
  }
}
