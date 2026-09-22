// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
namespace native::detail::x86_constant {
  template<class V> constexpr auto lanes(V value) noexcept {
    std::array<typename V::value_type, V::lanes> result{};
    value.store(result.data());
    return result;
  }
  template<class V, class T, std::size_t N>
  constexpr V pack(std::array<T, N> const & values) noexcept {
    static_assert(V::lanes == N);
    return V::load(values.data());
  }
}
