// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#include <initializer_list>
#include <native/isa.h>
#include <native/targets.h>

namespace {
  using native::isa, native::x86_feature;
  constexpr auto instruction = isa(x86_feature::avx512vpopcntdq);
  constexpr auto full_width = native::target_features("avx512vpopcntdq");
  constexpr auto short_width = native::target_features("avx512vpopcntdq,avx512vl");

  // Existing public enum values stay fixed when an independent feature is added.
  static_assert(static_cast<unsigned>(x86_feature::popcnt) == 7);
  static_assert(static_cast<unsigned>(x86_feature::avx512f) == 14);
  static_assert(static_cast<unsigned>(x86_feature::waitpkg) == 29);
  static_assert(native::x86_feature_count == static_cast<unsigned>(x86_feature::avx512vpopcntdq) + 1);
  static_assert(instruction != isa(x86_feature::popcnt));
  static_assert(!instruction.has(x86_feature::avx512f));
  static_assert(full_width == native::feature_closure(instruction));
  static_assert(full_width.has(x86_feature::avx512f));
  static_assert(!full_width.has(x86_feature::avx512vl));
  static_assert(!full_width.has(x86_feature::avx512bw));
  static_assert(!full_width.has(x86_feature::avx512dq));
  static_assert(!full_width.has(x86_feature::bmi1));
  static_assert(!full_width.has(x86_feature::bmi2));
  static_assert(short_width == (full_width & x86_feature::avx512vl));
  static_assert(!native::avx512.has(x86_feature::avx512vpopcntdq));
  static_assert(native::target_features("no-avx512vpopcntdq") == native::detail::invalid_features);
  static_assert(native::target_features("avx512vpopcntdq,") == native::detail::invalid_features);

#define NATIVE_TARGET_metadata_vpopcntdq "avx512vpopcntdq"
  static_assert(NATIVE_TARGET_ISA(metadata_vpopcntdq) == full_width);
#if defined(NATIVE_TEST_EXPECT_VPOPCNTDQ_MINIMUM)
  static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::avx512vpopcntdq));
  static_assert(full_width <= NATIVE_TARGET_MINIMUM);
  static_assert(NATIVE_TARGET_MINIMUM <= native::detail::known_features);
#else
  static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::avx512vpopcntdq));
#endif

  constexpr bool properties() {
    isa features = x86_feature::popcnt;
    features.avx512vpopcntdq = true;
    if (!features.avx512vpopcntdq || features != (x86_feature::popcnt & instruction)) return false;
    features.avx512vpopcntdq = false;
    if (features != isa(x86_feature::popcnt)) return false;
    native::feature_set<x86_feature> bits(x86_feature::avx512vpopcntdq);
    return bits.valid() && bits.has(x86_feature::avx512vpopcntdq) &&
      !bits.has(x86_feature::popcnt) && isa(bits) == instruction;
  }
  static_assert(properties());

  struct normalized_snapshot {
    native::feature_set<x86_feature> present{}, observed{};
    std::uint64_t xcr0 = 0xe6;
    bool xcr0_observed = true;
  };
  constexpr normalized_snapshot normalized_full() {
    normalized_snapshot cpu;
    // These are compiler prerequisites, not inferred hardware implications.
    for (auto feature : {x86_feature::mmx, x86_feature::sse, x86_feature::sse2,
        x86_feature::sse3, x86_feature::ssse3, x86_feature::sse41,
        x86_feature::sse42, x86_feature::crc32, x86_feature::popcnt, x86_feature::avx,
        x86_feature::avx2, x86_feature::fma, x86_feature::f16c,
        x86_feature::avx512f, x86_feature::avx512vpopcntdq})
      cpu.present.set(feature, true);
    cpu.observed = cpu.present;
    return cpu;
  }
  constexpr bool normalized_admission() {
    auto cpu = normalized_full();
    if (!native::classify_isa(cpu, instruction).admitted()) return false;
    if (native::classify_isa(cpu, short_width).admitted()) return false;
    cpu.present.set(x86_feature::avx512vl, true);
    cpu.observed.set(x86_feature::avx512vl, true);
    if (!native::classify_isa(cpu, short_width).admitted()) return false;
    cpu.observed.set(x86_feature::avx512vl, false);
    if (native::classify_isa(cpu, short_width).admitted()) return false;
    for (auto feature : {x86_feature::avx512f, x86_feature::avx512vpopcntdq}) {
      cpu = normalized_full();
      cpu.present.set(feature, false);
      if (native::classify_isa(cpu, instruction).admitted()) return false;
      cpu = normalized_full();
      cpu.observed.set(feature, false);
      if (native::classify_isa(cpu, instruction).admitted()) return false;
    }
    for (auto requirements : {full_width, short_width}) {
      for (unsigned bit = 0; bit != 64; ++bit) {
        cpu = normalized_full();
        cpu.present.set(x86_feature::avx512vl, true);
        cpu.observed.set(x86_feature::avx512vl, true);
        auto mask = std::uint64_t{1} << bit;
        cpu.xcr0 &= ~mask;
        auto result = native::classify_isa(cpu, requirements);
        if (result.missing_xcr0 != (0xe6 & mask)) return false;
        if (result.admitted() != ((0xe6 & mask) == 0)) return false;
      }
      cpu = normalized_full();
      cpu.present.set(x86_feature::avx512vl, true);
      cpu.observed.set(x86_feature::avx512vl, true);
      cpu.xcr0_observed = false;
      auto unread = native::classify_isa(cpu, requirements);
      if (unread.admitted() || !unread.missing_xcr0_observation || unread.missing_xcr0 != 0xe6)
        return false;
    }
    return true;
  }
  static_assert(normalized_admission());

  // Explicit raw CPUID words independently exercise register and bit decoding.
  struct legacy_snapshot {
    std::uint32_t max_basic_leaf = 7;
    std::uint32_t leaf1_ecx = (1u << 0) | (1u << 9) | (1u << 12) | (1u << 19) |
      (1u << 20) | (1u << 23) | (1u << 26) | (1u << 27) | (1u << 28) | (1u << 29);
    std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
    std::uint32_t leaf7_ebx = (1u << 5) | (1u << 16);
    std::uint32_t max_leaf7_subleaf = 0, leaf7_1_eax = 0, leaf7_edx = 0;
    std::uint64_t xcr0 = 0xe6;
    bool xcr0_observed = true;
  };
  struct raw_snapshot : legacy_snapshot {
    std::uint32_t leaf7_ecx = 1u << 14;
  };

  constexpr bool raw_admission() {
    if (!native::classify_isa(raw_snapshot{}, instruction).admitted()) return false;
    if (native::classify_isa(legacy_snapshot{}, instruction).admitted()) return false;
    for (unsigned bit = 0; bit != 32; ++bit) {
      raw_snapshot cpu;
      cpu.leaf7_ecx = 1u << bit;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (!decoded.observed.has(x86_feature::avx512vpopcntdq)) return false;
      if (decoded.present.has(x86_feature::avx512vpopcntdq) != (bit == 14)) return false;
      if (native::classify_isa(cpu, instruction).admitted() != (bit == 14)) return false;
    }
    // AVX512_BITALG (ECX12) and scalar POPCNT (leaf 1 ECX23) do not imply ECX14.
    raw_snapshot cpu;
    cpu.leaf7_ecx = 1u << 12;
    if (!native::classify_isa(cpu, x86_feature::popcnt).admitted()) return false;
    if (native::classify_isa(cpu, instruction).admitted()) return false;
    cpu = {};
    cpu.leaf1_ecx &= ~(1u << 23);
    auto decoded = native::detail::decode_x86_features(cpu);
    if (!decoded.present.has(x86_feature::avx512vpopcntdq) ||
        decoded.present.has(x86_feature::popcnt)) return false;
    // 512-bit forms need F but not VL, DQ or BW. Short forms additionally need VL.
    cpu = {};
    if (native::classify_isa(cpu, short_width).admitted()) return false;
    cpu.leaf7_ebx |= 1u << 31;
    if (!native::classify_isa(cpu, short_width).admitted()) return false;
    cpu.leaf7_ebx &= ~(1u << 16);
    if (native::classify_isa(cpu, instruction).admitted()) return false;
    for (auto maximum : {0u, 1u, 6u}) {
      cpu = {};
      cpu.max_basic_leaf = maximum;
      decoded = native::detail::decode_x86_features(cpu);
      if (decoded.observed.has(x86_feature::avx512vpopcntdq) ||
          decoded.present.has(x86_feature::avx512vpopcntdq)) return false;
      if (native::classify_isa(cpu, instruction).admitted()) return false;
    }
    for (unsigned bit : {26u, 27u}) {
      cpu = {};
      cpu.leaf1_ecx &= ~(1u << bit);
      auto result = native::classify_isa(cpu, instruction);
      if (result.admitted() || !result.missing_xcr0_observation || result.missing_xcr0 != 0xe6)
        return false;
    }
    cpu = {};
    cpu.xcr0_observed = false;
    if (native::classify_isa(cpu, instruction).admitted()) return false;
    cpu = {};
    cpu.leaf7_ebx |= 1u << 31;
    cpu.xcr0 = 0x6; // AVX state alone cannot execute even the 128-bit EVEX forms.
    auto result = native::classify_isa(cpu, short_width);
    return !result.admitted() && result.missing_features == native::scalar && result.missing_xcr0 == 0xe0;
  }
  static_assert(raw_admission());
}

int main() {
  if (!properties() || !normalized_admission() || !raw_admission()) return 1;
  std::puts("PASS: VPOPCNTDQ feature metadata, CPUID and OS-state admission");
}
