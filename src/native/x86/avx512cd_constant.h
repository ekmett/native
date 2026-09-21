// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cstdint>

namespace native::detail::x86_avx512cd_constant {
  // Used only during constant evaluation, independently of the runtime target.
  template<bool Conflict, class V>
  constexpr V evaluate(V value, V source, std::uint64_t mask) noexcept {
    using T = typename V::value_type;
    std::array<T, V::lanes> input{}, result{};
    value.store(input.data());
    source.store(result.data());
    for (std::size_t lane = 0; lane < V::lanes; ++lane) {
      if (!((mask >> lane) & 1)) continue;
      if constexpr (Conflict) {
        T matches = 0;
        // The writemask controls destination lanes, never comparison inputs.
        for (std::size_t earlier = 0; earlier < lane; ++earlier)
          if (input[earlier] == input[lane]) matches |= T{1} << earlier;
        result[lane] = matches;
      } else {
        result[lane] = static_cast<T>(std::countl_zero(input[lane]));
      }
    }
    return V::load(result.data());
  }
}
