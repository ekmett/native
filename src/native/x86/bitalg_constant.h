// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace native::detail::x86_bitalg_constant {
  template<class V>
  constexpr V population(V value, V source, std::uint64_t mask) noexcept {
    using value_type = typename V::value_type;
    std::array<value_type, V::lanes> input{};
    std::array<value_type, V::lanes> result{};
    value.store(input.data());
    source.store(result.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        result[lane] = static_cast<value_type>(std::popcount(input[lane]));
      }
    }
    return V::load(result.data());
  }

  template<class P, class V, class C>
  constexpr P bitshuffle(V value, C control, std::uint64_t mask) noexcept {
    std::array<std::uint64_t, V::lanes> words{};
    std::array<std::uint8_t, C::lanes> selectors{};
    value.store(words.data());
    control.store(selectors.data());
    std::uint64_t result = 0;
    for (std::size_t lane = 0; lane < C::lanes; ++lane) {
      // Each control byte selects within its own source qword. Bits 6/7 are ignored.
      auto bit = (words[lane / 8] >> (selectors[lane] & 63)) & 1;
      result |= bit << lane;
    }
    return P::from_bitset(result & mask);
  }
}
