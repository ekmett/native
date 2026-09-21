// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/x86/integer_constant.h"

namespace native::detail::x86_ifma_constant {
  template<bool High>
  constexpr std::uint64_t product(std::uint64_t a, std::uint64_t b) noexcept {
    constexpr std::uint64_t mask26 = (std::uint64_t{1} << 26) - 1;
    constexpr std::uint64_t mask52 = (std::uint64_t{1} << 52) - 1;
    a &= mask52;
    b &= mask52;
    auto low = (a & mask26) * (b & mask26);
    auto middle = (a >> 26) * (b & mask26) + (a & mask26) * (b >> 26);
    low += (middle & mask26) << 26;
    if constexpr (High) {
      return (a >> 26) * (b >> 26) + (middle >> 26) + (low >> 52);
    } else {
      return low & mask52;
    }
  }

  template<bool High, class V>
  constexpr V accumulate(V accumulator, V a, V b, std::uint64_t mask, bool zero) noexcept {
    auto result = x86_instruction_constant::lanes(accumulator);
    auto left = x86_instruction_constant::lanes(a);
    auto right = x86_instruction_constant::lanes(b);
    for (unsigned lane = 0; lane < V::lanes; ++lane) {
      if ((mask >> lane) & 1) {
        result[lane] += product<High>(left[lane], right[lane]);
      } else if (zero) {
        result[lane] = 0;
      }
    }
    return V::load(result.data());
  }
}
