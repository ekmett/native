#include <bit>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <string_view>
#include <type_traits>
#include "support/fp_environment.h"
import native.numerics;
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

static_assert(sizeof(native::fp16) == 2 && sizeof(native::bf16) == 2);
static_assert(std::is_trivially_copyable_v<native::fp16>);
static_assert(std::is_trivially_copyable_v<native::bf16>);
static_assert(native::fp16::from_bits(0x3c00).to_bits() == 0x3c00);
static_assert(native::bf16::from_bits(0x3f80).to_bits() == 0x3f80);
static_assert(std::same_as<native::uint_t<float>, std::uint32_t>);
static_assert(std::same_as<native::int_t<double>, std::int64_t>);
static_assert(native::one_of<4, 2, 4, 8> && native::not_one_of<3, 2, 4, 8>);
static_assert(native::cmpint<native::CMPINT::LT>(std::int16_t(-1), std::int16_t(0)));
static_assert(native::cmpint<native::CMPINT::NLT>(std::uint64_t(0), std::uint64_t(0)));

namespace half_limits {
  template<class T, unsigned Min, unsigned Max, unsigned Epsilon,
      unsigned Half, int Digits, int MinPower, int EpsilonPower, int DenormPower>
  constexpr bool exact() {
    using L = std::numeric_limits<T>;
    return L::min().to_bits() == Min && L::max().to_bits() == Max &&
      L::lowest().to_bits() == (Max | 0x8000u) && L::epsilon().to_bits() == Epsilon &&
      L::round_error().to_bits() == Half && L::denorm_min().to_bits() == 1 &&
      L::digits == Digits && L::is_bounded &&
      static_cast<float>(L::min()) == (MinPower == -14 ? 0x1p-14f : 0x1p-126f) &&
      static_cast<float>(L::epsilon()) == (EpsilonPower == -10 ? 0x1p-10f : 0x1p-7f) &&
      static_cast<float>(L::denorm_min()) == (DenormPower == -24 ? 0x1p-24f : 0x1p-133f);
  }
  static_assert(exact<native::fp16, 0x0400, 0x7bff, 0x1400, 0x3800, 11, -14, -10, -24>());
  static_assert(exact<native::bf16, 0x0080, 0x7f7f, 0x3c00, 0x3f00, 8, -126, -7, -133>());
  static_assert(std::numeric_limits<native::fp16>::max_digits10 == 5);
  static_assert(std::numeric_limits<native::bf16>::max_digits10 == 4);
  static_assert((std::numeric_limits<native::bf16>::signaling_NaN().to_bits() & 0x0040u) == 0);
  // Exercise the deprecated compatibility members deliberately, without weakening
  // diagnostics for the implementation or other tests.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
  static_assert(std::numeric_limits<native::fp16>::has_denorm == std::denorm_present);
  static_assert(std::numeric_limits<native::bf16>::has_denorm == std::denorm_present);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
  template<class T> bool runtime(float min, float max, float epsilon, float tiny) {
    using L = std::numeric_limits<T>;
    volatile float input[] = {min, max, -max, epsilon, tiny};
    unsigned expected[] = {L::min().to_bits(), L::max().to_bits(), L::lowest().to_bits(),
      L::epsilon().to_bits(), L::denorm_min().to_bits()};
    for (unsigned i = 0; i < 5; ++i) {
      T value(input[i]);
      if (value.to_bits() != expected[i] || static_cast<float>(value) != input[i]) return false;
    }
    return true;
  }
  bool check() {
    auto before = native::test::read_fp_state();
    bool result;
    {
      // These storage types expose subnormal encodings. Ambient FP controls may
      // affect conversion instructions; the limits describe representation.
      native::test::fp_scope scope(native::test::fp_mode::gradual);
      result = runtime<native::fp16>(0x1p-14f, 65504.0f, 0x1p-10f, 0x1p-24f) &&
        runtime<native::bf16>(0x1p-126f, 0x1.fep127f, 0x1p-7f, 0x1p-133f) &&
        scope.controls_match();
    }
    return result && before == native::test::read_fp_state();
  }
}

namespace user_numeric {
  struct number {};
  inline int calls = 0;
  bool isnan(number) noexcept(false) {
    ++calls;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    throw 42;
#else
    return false;
#endif
  }
  static_assert(!noexcept(native::cmp_ord(number{}, number{})));
  static_assert(!noexcept(native::cmp_unord(number{}, number{})));
  struct nothrow_number {};
  bool isnan(nothrow_number) noexcept { return false; }
  static_assert(noexcept(native::cmp_ord(nothrow_number{}, nothrow_number{})));
  static_assert(noexcept(native::cmp_unord(nothrow_number{}, nothrow_number{})));
  bool check() {
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    try { (void)native::cmp_ord(number{}, number{}); }
    catch (int x) { if (x != 42 || calls != 1) return false; }
    if (calls != 1) return false;
    try { (void)native::cmp_unord(number{}, number{}); }
    catch (int x) { return x == 42 && calls == 2; }
    return false;
#else
    bool ordered = native::cmp_ord(number{}, number{});
    bool unordered = native::cmp_unord(number{}, number{});
    return ordered && !unordered && calls == 4;
#endif
  }
}

int main() {
  if (!user_numeric::check()) return 10;
  if (!half_limits::check()) return 11;
  if (!half_conversion_test::check<native::fp16,10,15>()) return 12;
  if (!half_conversion_test::check<native::bf16,7,127>()) return 13;
  for (std::uint32_t i = 0; i < 65536; ++i) {
    auto bits = static_cast<std::uint16_t>(i);
    if (native::fp16::from_bits(bits).to_bits() != bits) return 1;
    if (native::bf16::from_bits(bits).to_bits() != bits) return 2;
  }
  if (static_cast<float>(native::fp16(1.5f)) != 1.5f) return 3;
  if (static_cast<float>(native::bf16(1.5f)) != 1.5f) return 4;
  if (!(native::fp16(-1.0f) < native::fp16(1.0f))) return 5;
  if (!(native::bf16(-1.0f) < native::bf16(1.0f))) return 6;
  if (native::type<int>.empty() || native::type_of(1).empty()) return 7;
  if (native::type_of(1) != native::type<int>) return 8;
  if (native::scalef(2.0f, 3.0f) != 16.0f) return 9;
  std::puts("fp16/bf16: all 65536 storage encodings preserved; basic conversions, comparisons, traits and RTTI work");
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
