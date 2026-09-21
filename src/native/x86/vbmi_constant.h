// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace native::detail::x86_vbmi_constant {
  template<bool Two, class V>
  constexpr V permute(V indices, V a, V b, V source, std::uint64_t mask) noexcept {
    std::array<std::uint8_t, V::lanes> selectors{}, first{}, second{}, result{};
    indices.store(selectors.data());
    a.store(first.data());
    b.store(second.data());
    source.store(result.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        auto index = selectors[lane] & (V::lanes * (Two ? 2 : 1) - 1);
        result[lane] = index < V::lanes ? first[index] : second[index - V::lanes];
      }
    }
    return V::load(result.data());
  }

  template<class C, class V>
  constexpr C multishift(C control, V value, C source, std::uint64_t mask) noexcept {
    std::array<std::uint8_t, C::lanes> selectors{}, result{};
    std::array<std::uint64_t, V::lanes> words{};
    control.store(selectors.data());
    value.store(words.data());
    source.store(result.data());
    for (std::size_t lane = 0; lane < C::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        // Eight selected bits wrap within the corresponding qword.
        result[lane] = static_cast<std::uint8_t>(std::rotr(words[lane / 8], selectors[lane] & 63));
      }
    }
    return C::load(result.data());
  }
}
