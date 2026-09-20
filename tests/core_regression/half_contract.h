#pragma once
#include "support/fp_environment.h"
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>

namespace half_conversion_test {
  struct sample { std::uint32_t input; std::uint16_t expected; };
  constexpr bool nan32(std::uint32_t word) { return (word & 0x7fffffffu) > 0x7f800000u; }
  template<unsigned Fraction, int Bias> std::uint32_t reference_decode(unsigned word) {
    auto sign = (word & 0x8000u) << 16;
    auto exponent = (word & 0x7fffu) >> Fraction;
    auto fraction = word & ((1u << Fraction) - 1u);
    auto top = 0x7fffu >> Fraction;
    if (exponent == top) return sign | (fraction ? 0x7fc00000u : 0x7f800000u);
    // Every source value is exactly representable in binary32. ldexp evaluates
    // the mathematical dyadic value, independently of the production bit graph.
    double significand = exponent ? (1u << Fraction) + fraction : fraction;
    int power = (exponent ? int(exponent) - Bias : 1 - Bias) - int(Fraction);
    auto value = float(std::ldexp(significand, power));
    return sign | std::bit_cast<std::uint32_t>(value);
  }
  template<class T, unsigned Fraction, int Bias> bool check() {
    std::array<std::uint32_t, 65536> decoded;
    std::vector<sample> bank;
    {
      native::test::fp_scope scope(native::test::fp_mode::gradual);
      for (unsigned word = 0; word < decoded.size(); ++word)
        decoded[word] = reference_decode<Fraction, Bias>(word);
      unsigned infinity = (0x7fffu >> Fraction) << Fraction;
      for (unsigned word = 0; word + 1 < infinity; ++word) {
        double a = std::bit_cast<float>(decoded[word]);
        double b = std::bit_cast<float>(decoded[word+1]);
        auto midpoint = std::bit_cast<std::uint32_t>(float((a+b)*0.5));
        // The half-way points are all exactly representable binary32 numbers.
        for (unsigned sign : {0u, 0x8000u}) {
          auto input_sign = sign << 16;
          bank.push_back({input_sign | (midpoint-1), std::uint16_t(sign | word)});
          bank.push_back({input_sign | midpoint, std::uint16_t(sign | (word+(word&1u)))});
          bank.push_back({input_sign | (midpoint+1), std::uint16_t(sign | (word+1))});
        }
      }
      auto overflow = Fraction == 10 ? 0x477ff000u : 0x7f7f8000u;
      for (unsigned sign : {0u, 0x8000u}) {
        bank.push_back({(sign<<16)|(overflow-1), std::uint16_t(sign|(infinity-1))});
        bank.push_back({(sign<<16)|overflow, std::uint16_t(sign|infinity)});
        bank.push_back({(sign<<16)|(overflow+1), std::uint16_t(sign|infinity)});
      }
    }
    auto before = native::test::read_fp_state();
    for (auto mode : {native::test::fp_mode::gradual, native::test::fp_mode::flush}) {
      native::test::fp_scope scope(mode);
      for (int rounding : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
        if (std::fesetround(rounding)) return false;
        for (unsigned word = 0; word < decoded.size(); ++word) {
          volatile std::uint16_t supplied = std::uint16_t(word);
          auto value = T::from_bits(supplied);
          auto actual = std::bit_cast<std::uint32_t>(static_cast<float>(value));
          if (actual != decoded[word] && !(nan32(actual) && nan32(decoded[word]))) {
            std::fprintf(stderr,"half decode frac=%u word=%04x actual=%08x expected=%08x\n",Fraction,word,actual,decoded[word]);
            return false;
          }
          if (!nan32(actual) && T(std::bit_cast<float>(actual)).to_bits() != word) return false;
          // These are runtime wrapper comparisons, not implicit native-half
          // conversions supplied by a compiler runtime.
          bool unordered = nan32(actual);
          if ((value == value) == unordered || (value != value) != unordered ||
              (value <= value) == unordered || (value >= value) == unordered ||
              (value < value) || (value > value)) return false;
          auto order = value <=> value;
          if (unordered ? order != std::partial_ordering::unordered : order != 0) return false;
        }
        for (auto item : bank) {
          volatile std::uint32_t supplied = item.input;
          auto actual = T(std::bit_cast<float>(std::uint32_t(supplied))).to_bits();
          if (actual != item.expected) {
            std::fprintf(stderr,"half encode frac=%u word=%08x actual=%04x expected=%04x\n",Fraction,item.input,unsigned(actual),unsigned(item.expected));
            return false;
          }
        }
        for (auto word : {0x7f800001u,0x7fc00000u,0xffa12345u}) {
          volatile std::uint32_t supplied = word;
          auto value = T(std::bit_cast<float>(std::uint32_t(supplied)));
          if (!nan32(std::bit_cast<std::uint32_t>(static_cast<float>(value)))) return false;
        }
      }
    }
    if (before != native::test::read_fp_state()) return false;
    std::printf("half conversion fraction=%u decode=65536 encode_boundaries=%zu modes=8 exact\n",Fraction,bank.size());
    return true;
  }
  static_assert(native::fp16(0x1.002p0f).to_bits() == 0x3c00);
  static_assert(native::fp16(0x1.006p0f).to_bits() == 0x3c02);
  static_assert(native::bf16(0x1.01p0f).to_bits() == 0x3f80);
  static_assert(native::bf16(0x1.03p0f).to_bits() == 0x3f82);
}
