// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>

namespace f16c_fixture {
  // Decode the binary16 sign, exponent and fraction without any FP arithmetic.
  constexpr std::uint32_t widen(std::uint16_t h) {
    std::uint32_t sign = std::uint32_t(h & 0x8000) << 16;
    unsigned exponent = (h >> 10) & 31, fraction = h & 1023;
    if (exponent == 31)
      return sign | 0x7f800000u | (fraction << 13) | (fraction ? 0x400000u : 0);
    if (exponent) return sign | ((exponent + 112) << 23) | (fraction << 13);
    if (!fraction) return sign;
    int e = -14;
    while (!(fraction & 1024)) { fraction <<= 1; --e; }
    return sign | (unsigned(e + 127) << 23) | ((fraction & 1023) << 13);
  }

  // Round an integer significand at the destination quantum. Only integer
  // divisions, remainders and comparisons determine the result, including ties.
  constexpr std::uint16_t narrow(std::uint32_t bits, unsigned mode, bool daz) {
    unsigned sign = bits >> 31, e = (bits >> 23) & 255, m = bits & 0x7fffff;
    auto signed_result = [=](unsigned h) { return std::uint16_t((sign << 15) | h); };
    if (e == 255) return signed_result(m ? (0x7e00 | (m >> 13)) : 0x7c00);
    if (!e && (daz || !m)) return signed_result(0);
    int exponent = e ? int(e) - 127 : -126;
    if (e) m |= 0x800000;
    // For subnormal half results, the quantum is 2^-24 instead of 2^(e-10).
    int shift = exponent < -14 ? -exponent - 1 : 13;
    unsigned q = shift < 32 ? m >> shift : 0;
    unsigned remainder = shift < 32 ? m & ((1u << shift) - 1) : m;
    bool increment = false;
    if (remainder) {
      if (mode == 0 && shift < 32) {
        unsigned half = 1u << (shift - 1);
        increment = remainder > half || (remainder == half && (q & 1));
      } else if (mode == 1) increment = sign;
      else if (mode == 2) increment = !sign;
    }
    q += increment;
    unsigned magnitude = exponent < -14 ? q : (unsigned(exponent + 14) << 10) + q;
    if (magnitude >= 0x7c00) {
      bool infinity = mode == 0 || (mode == 1 && sign) || (mode == 2 && !sign);
      magnitude = infinity ? 0x7c00 : 0x7bff;
    }
    return signed_result(magnitude);
  }
  static_assert(widen(1) == 0x33800000 && widen(0x8000) == 0x80000000);
  static_assert(widen(0x7c01) == 0x7fc02000 && widen(0x7bff) == 0x477fe000);
  static_assert(narrow(0x3f801000, 0, false) == 0x3c00);
  static_assert(narrow(0x3f803000, 0, false) == 0x3c02);
  static_assert(narrow(0x477ff000, 0, false) == 0x7c00);
  static_assert(narrow(1, 2, false) == 1 && narrow(1, 2, true) == 0);
}
