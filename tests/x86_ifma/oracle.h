// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include <array>
#include <cstdint>

namespace ifma_fixture {
  // Shift/add a 104-bit product in two 64-bit words, independently of the
  // production 26-bit limb multiplication. Input high 12 bits are ignored.
  constexpr std::uint64_t product(std::uint64_t a, std::uint64_t b, bool high) {
    constexpr auto mask = (std::uint64_t{1} << 52) - 1;
    a &= mask;
    b &= mask;
    std::uint64_t low = 0;
    std::uint64_t upper = 0;
    for (unsigned bit = 0; bit < 52; ++bit) {
      if ((b >> bit) & 1) {
        auto previous = low;
        low += a << bit;
        upper += (bit ? a >> (64 - bit) : 0) + (low < previous);
      }
    }
    return high ? (upper << 12) | (low >> 52) : low & mask;
  }

  template<unsigned N>
  using words = std::array<std::uint64_t, N>;
  template<unsigned N>
  using results = std::array<words<N>, 6>;

  template<unsigned N>
  constexpr results<N> oracle(words<N> const & accumulator, words<N> const & a,
    words<N> const & b, std::uint64_t mask) {
    results<N> result{};
    for (unsigned half = 0; half < 2; ++half) {
      for (unsigned lane = 0; lane < N; ++lane) {
        auto value = accumulator[lane] + product(a[lane], b[lane], half != 0);
        result[half * 3][lane] = value;
        result[half * 3 + 1][lane] = (mask >> lane) & 1 ? value : accumulator[lane];
        result[half * 3 + 2][lane] = (mask >> lane) & 1 ? value : 0;
      }
    }
    return result;
  }
}
