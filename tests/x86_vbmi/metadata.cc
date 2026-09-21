// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <native/isa.h>
#include <native/targets.h>

namespace {
  using native::x86_feature;
  constexpr auto full_width = native::target_features<native::x86>("avx512vbmi");
  constexpr auto short_width = native::target_features<native::x86>("avx512vbmi,avx512vl");

  static_assert(full_width.has(x86_feature::avx512f));
  static_assert(full_width.has(x86_feature::avx512vbmi));
  static_assert(!full_width.has(x86_feature::avx512vl));
  static_assert(full_width.has(x86_feature::avx512bw));
  static_assert(!full_width.has(x86_feature::avx512vpopcntdq));
  static_assert(!native::avx512.has(x86_feature::avx512vbmi));
  static_assert(!full_width.has(x86_feature::avx512dq));
  static_assert(!full_width.has(x86_feature::avx512bitalg));
  static_assert(!full_width.has(x86_feature::avx512vbmi2));
  static_assert(short_width == (full_width & x86_feature::avx512vl));

  static_assert(static_cast<unsigned>(x86_feature::avx512bitalg) == 38);
  static_assert(static_cast<unsigned>(x86_feature::avx512vbmi) == 39);
  static_assert(native::x86_feature_count > 39);
  static_assert(full_width == native::feature_closure(x86_feature::avx512vbmi));
  static_assert(native::target_features<native::x86>("no-avx512vbmi") ==
    native::detail::invalid_features<native::x86>);
  static_assert(native::target_features<native::x86>("avx512vbmi,") ==
    native::detail::invalid_features<native::x86>);

#define NATIVE_TARGET_metadata_vbmi "avx512vbmi"
  static_assert(NATIVE_TARGET_ISA(metadata_vbmi) == full_width);
#if NATIVE_TEST_VBMI_MINIMUM
  static_assert(NATIVE_BASELINE.has(x86_feature::avx512vbmi));
  static_assert(full_width <= NATIVE_TARGET_MINIMUM);
  static_assert(NATIVE_TARGET_MINIMUM <= native::detail::known_features<native::x86>);
#else
  static_assert(!NATIVE_BASELINE.has(x86_feature::avx512vbmi));
  static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::avx512vbmi));
#endif

  constexpr bool properties() {
    native::isa<native::x86> features = x86_feature::popcnt;
    features.avx512vbmi = true;
    if (!features.avx512vbmi || features != (x86_feature::popcnt & x86_feature::avx512vbmi)) {
      return false;
    }
    features.avx512vbmi = false;
    return features == native::isa<native::x86>(x86_feature::popcnt);
  }
  static_assert(properties());

  struct normalized_snapshot {
    native::isa<native::x86> present = short_width;
    native::isa<native::x86> observed = short_width;
    std::uint64_t xcr0 = 0xe6;
    bool xcr0_observed = true;
  };

  constexpr bool normalized_admission() {
    for (auto requirements : {full_width, short_width}) {
      if (!native::classify_isa(normalized_snapshot{}, requirements).admitted()) {
        return false;
      }
      for (auto feature : {x86_feature::avx512f, x86_feature::avx512bw,
                           x86_feature::avx512vbmi, x86_feature::avx512vl}) {
        normalized_snapshot cpu;
        cpu.present.set(feature, false);
        if (native::classify_isa(cpu, requirements).admitted() == requirements.has(feature)) {
          return false;
        }
        cpu = {};
        cpu.observed.set(feature, false);
        if (native::classify_isa(cpu, requirements).admitted() == requirements.has(feature)) {
          return false;
        }
      }
      for (unsigned bit = 0; bit < 64; ++bit) {
        normalized_snapshot cpu;
        auto mask = std::uint64_t{1} << bit;
        cpu.xcr0 &= ~mask;
        auto result = native::classify_isa(cpu, requirements);
        if (result.missing_xcr0 != (0xe6 & mask) || result.admitted() != ((0xe6 & mask) == 0)) {
          return false;
        }
      }
      normalized_snapshot cpu;
      cpu.xcr0_observed = false;
      auto result = native::classify_isa(cpu, requirements);
      if (result.admitted() || !result.missing_xcr0_observation || result.missing_xcr0 != 0xe6) {
        return false;
      }
    }
    return true;
  }

  // Independent raw CPUID words exercise AVX512VBMI at leaf 7, subleaf 0, ECX1.
  struct raw_snapshot {
    std::uint32_t max_basic_leaf = 7;
    std::uint32_t leaf1_ecx = (1u << 0) | (1u << 9) | (1u << 12) | (1u << 19) |
      (1u << 20) | (1u << 23) | (1u << 26) | (1u << 27) | (1u << 28) | (1u << 29);
    std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
    std::uint32_t leaf7_ebx = (1u << 5) | (1u << 16) | (1u << 30);
    std::uint32_t leaf7_ecx = 1u << 1;
    std::uint32_t max_leaf7_subleaf = 0;
    std::uint32_t leaf7_1_eax = 0;
    std::uint32_t leaf7_edx = 0;
    std::uint64_t xcr0 = 0xe6;
    bool xcr0_observed = true;
  };

  constexpr bool raw_admission() {
    if (!native::classify_isa(raw_snapshot{}, full_width).admitted()) {
      return false;
    }
    for (unsigned bit = 0; bit < 32; ++bit) {
      raw_snapshot cpu;
      cpu.leaf7_ecx = 1u << bit;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (!decoded.observed.has(x86_feature::avx512vbmi) ||
          decoded.present.has(x86_feature::avx512vbmi) != (bit == 1) ||
          native::classify_isa(cpu, full_width).admitted() != (bit == 1)) {
        return false;
      }
    }
    raw_snapshot cpu;
    if (native::classify_isa(cpu, short_width).admitted()) {
      return false;
    }
    cpu.leaf7_ebx |= 1u << 31;
    if (!native::classify_isa(cpu, short_width).admitted()) {
      return false;
    }
    cpu.leaf7_ebx &= ~(1u << 30);
    if (native::classify_isa(cpu, full_width).admitted()) {
      return false;
    }
    cpu = {};
    cpu.leaf7_ebx &= ~(1u << 16);
    if (native::classify_isa(cpu, full_width).admitted()) {
      return false;
    }
    for (auto maximum : {0u, 1u, 6u}) {
      cpu = {};
      cpu.max_basic_leaf = maximum;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (decoded.observed.has(x86_feature::avx512vbmi) ||
          decoded.present.has(x86_feature::avx512vbmi) ||
          native::classify_isa(cpu, full_width).admitted()) {
        return false;
      }
    }
    for (unsigned bit : {26u, 27u}) {
      cpu = {};
      cpu.leaf1_ecx &= ~(1u << bit);
      auto result = native::classify_isa(cpu, full_width);
      if (result.admitted() || !result.missing_xcr0_observation || result.missing_xcr0 != 0xe6) {
        return false;
      }
    }
    cpu = {};
    cpu.leaf7_ebx |= 1u << 31;
    cpu.xcr0 = 0x6;
    auto result = native::classify_isa(cpu, short_width);
    return !result.admitted() && result.missing_xcr0 == 0xe0;
  }

  static_assert(normalized_admission());
  static_assert(raw_admission());
}

int main() {
  if (!properties() || !normalized_admission() || !raw_admission()) {
    return 1;
  }
  std::puts("AVX512VBMI feature and OS-state admission checks passed.");
  return 0;
}
