// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <bit>
#include <cstdint>
namespace reference {
  constexpr std::uint32_t half(std::uint16_t x) {
    auto e = (x >> 10) & 31u, f = x & 1023u;
    auto sign = std::uint32_t(x & 0x8000u) << 16;
    if (e == 31)
      return sign | 0x7f800000u | (f << 13) | (f ? 0x00400000u : 0u);
    // Exact arithmetic oracle: integer significand times a binary power.
    float v = float(e ? 1024 + f : f);
    int shift = e ? int(e) - 25 : -24;
    while (shift < 0) {
      v *= 0.5f;
      ++shift;
    }
    while (shift > 0) {
      v *= 2.f;
      --shift;
    }
    return std::bit_cast<std::uint32_t>(v) | sign;
  }
  constexpr std::uint16_t narrow(std::uint32_t x) {
    auto sign = std::uint16_t((x >> 16) & 0x8000u);
    auto magnitude = x & 0x7fffffffu;
    if (magnitude < 0x00800000u)
      return sign;
    if (magnitude > 0x7f800000u)
      return std::uint16_t((x >> 16) | 0x40u);
    auto high = x >> 16, low = x & 65535u;
    return std::uint16_t(high + (low > 32768u || (low == 32768u && (high & 1u))));
  }
  constexpr std::uint32_t random(std::uint32_t & state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
  }
} // namespace reference
