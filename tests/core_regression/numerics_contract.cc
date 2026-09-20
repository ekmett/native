#include <bit>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <string_view>
#include <type_traits>
#include "support/fp_environment.h"
import native.numerics;
#include "half_contract.h"

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
