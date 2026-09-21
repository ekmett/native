// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <native/isa.h>
#include <native/targets.h>

namespace {
  using native::isa;
  using native::x86_feature;
  constexpr std::array features{x86_feature::avxvnni, x86_feature::avx512vnni,
    x86_feature::avxvnniint8, x86_feature::avxvnniint16};
  constexpr std::array spellings{"avxvnni", "avx512vnni", "avxvnniint8", "avxvnniint16"};
  constexpr auto vex_base = native::feature_closure(x86_feature::avx2);
  constexpr auto evex_base = native::feature_closure(x86_feature::avx512f);

  static_assert(unsigned(x86_feature::crc32) == 30);
  static_assert(unsigned(x86_feature::gfni) == 31);
  static_assert(unsigned(x86_feature::avx512vpopcntdq) == 32);
  static_assert(unsigned(x86_feature::avxvnni) == 34);
  static_assert(unsigned(x86_feature::avx512vnni) == 35);
  static_assert(unsigned(x86_feature::avxvnniint8) == 36);
  static_assert(unsigned(x86_feature::avxvnniint16) == 37);
  static_assert(unsigned(native::arm_feature::i8mm) == 15);

  constexpr bool properties() {
    isa value;
    value.avxvnni = true;
    value.avx512vnni = true;
    value.avxvnniint8 = true;
    value.avxvnniint16 = true;
    if (!value.avxvnni || !value.avx512vnni || !value.avxvnniint8 || !value.avxvnniint16)
      return false;
    if (value != (features[0] & features[1] & features[2] & features[3])) return false;
    value.avxvnni = false;
    value.avx512vnni = false;
    value.avxvnniint8 = false;
    value.avxvnniint16 = false;
    if (value != native::scalar) return false;
    for (unsigned i = 0; i != features.size(); ++i) {
      isa exact{features[i]};
      if (exact != isa(native::feature_set{features[i]})) return false;
      if (exact.has(x86_feature::avx2) || exact.has(x86_feature::avx512f)) return false;
      auto closure = native::feature_closure(exact);
      if (closure != (exact & (i == 1 ? evex_base : vex_base))) return false;
      if (native::target_features(spellings[i]) != closure) return false;
      for (auto other : features)
        if (closure.has(other) != (other == features[i])) return false;
      if (native::avx2.has(features[i]) || native::avx512.has(features[i])) return false;
    }
    return true;
  }
  static_assert(properties());

#if defined(NATIVE_VNNI_EXPECT_UNKNOWN)
  static_assert(!(NATIVE_TARGET_MINIMUM <= native::detail::known_features));
#else
  static_assert(NATIVE_TARGET_MINIMUM <= native::detail::known_features);
#endif
#ifdef __AVXVNNI__
  static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::avxvnni));
#else
  static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::avxvnni));
#endif
#ifdef __AVX512VNNI__
  static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::avx512vnni));
#else
  static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::avx512vnni));
#endif
#ifdef __AVXVNNIINT8__
  static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::avxvnniint8));
#else
  static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::avxvnniint8));
#endif
#ifdef __AVXVNNIINT16__
  static_assert(NATIVE_TARGET_MINIMUM.has(x86_feature::avxvnniint16));
#else
  static_assert(!NATIVE_TARGET_MINIMUM.has(x86_feature::avxvnniint16));
#endif

  struct normalized_snapshot {
    native::feature_set<x86_feature> present{}, observed{};
    std::uint64_t xcr0 = 0;
    bool xcr0_observed = false;
  };
  constexpr normalized_snapshot available(isa requested) {
    normalized_snapshot cpu;
    auto closure = native::feature_closure(requested);
    for (unsigned i = 0; i != native::x86_feature_count; ++i) {
      auto feature = static_cast<x86_feature>(i);
      cpu.present.set(feature, closure.has(feature));
      cpu.observed.set(feature, closure.has(feature));
    }
    cpu.xcr0 = requested.has(x86_feature::avx512vnni) ? 0xe6 : 0x6;
    cpu.xcr0_observed = true;
    return cpu;
  }
  constexpr bool normalized_features(x86_feature feature, bool clear_observation) {
    auto full = available(feature);
    if (!native::classify_isa(full, feature).admitted()) return false;
    auto closure = native::feature_closure(feature);
    for (unsigned i = 0; i != native::x86_feature_count; ++i) {
      auto removed = static_cast<x86_feature>(i);
      auto cpu = full;
      if (clear_observation) cpu.observed.set(removed, false);
      else cpu.present.set(removed, false);
      auto status = native::classify_isa(cpu, feature);
      if (status.admitted() == closure.has(removed)) return false;
    }
    return true;
  }
  constexpr bool normalized_state(x86_feature feature) {
    auto full = available(feature);
    for (unsigned bit = 0; bit != 8; ++bit) {
      auto cpu = full;
      cpu.xcr0 &= ~(std::uint64_t{1} << bit);
      auto status = native::classify_isa(cpu, feature);
      auto missing = full.xcr0 & (std::uint64_t{1} << bit);
      if (status.missing_xcr0 != missing || status.admitted() == bool(missing)) return false;
      if (!cpu.present.has(feature)) return false;
    }
    auto cpu = full;
    cpu.xcr0_observed = false;
    auto unread = native::classify_isa(cpu, feature);
    if (unread.admitted() || !unread.missing_xcr0_observation || unread.missing_xcr0 != full.xcr0)
      return false;
    for (auto other : features)
      if (native::classify_isa(full, other).admitted() != (other == feature)) return false;
    auto invalid = static_cast<x86_feature>(native::x86_feature_count);
    if (!native::classify_isa(full, invalid).invalid_features) return false;
    cpu = full;
    cpu.present.set(invalid, true);
    if (!native::classify_isa(cpu, feature).invalid_features) return false;
    return !native::classify_isa(full, native::target_features("avxvnni,unregistered")).admitted();
  }
  // Separate evaluations keep each complete feature sweep within Clang's
  // default constexpr step budget as the feature registry grows.
  template<x86_feature Feature> struct normalized_checks {
    static_assert(normalized_features(Feature, false));
    static_assert(normalized_features(Feature, true));
    static_assert(normalized_state(Feature));
    static constexpr bool checked = true;
  };
  static_assert(normalized_checks<features[0]>::checked);
  static_assert(normalized_checks<features[1]>::checked);
  static_assert(normalized_checks<features[2]>::checked);
  static_assert(normalized_checks<features[3]>::checked);

  // The raw fixture supplies independent CPUID words, not registry-derived masks.
  // Older snapshots do not contain leaf 7 subleaf 1 EDX and must still compile.
  struct older_snapshot {
    std::uint32_t max_basic_leaf = 7;
    std::uint32_t leaf1_ecx = (1u << 0) | (1u << 9) | (1u << 12) | (1u << 19) |
      (1u << 20) | (1u << 23) | (1u << 26) | (1u << 27) | (1u << 28) | (1u << 29);
    std::uint32_t leaf1_edx = (1u << 23) | (1u << 25) | (1u << 26);
    std::uint32_t leaf7_ebx = (1u << 5) | (1u << 16);
    std::uint32_t leaf7_ecx = 1u << 11, leaf7_edx = 0;
    std::uint32_t max_leaf7_subleaf = 1, leaf7_1_eax = 1u << 4;
    std::uint64_t xcr0 = 0xe6;
    bool xcr0_observed = true;
  };
  struct raw_snapshot : older_snapshot {
    std::uint32_t leaf7_1_edx = (1u << 4) | (1u << 10);
  };
  constexpr bool raw_mapping(x86_feature feature, unsigned expected_bit,
      std::uint32_t raw_snapshot::* register_word) {
    for (unsigned bit = 0; bit != 32; ++bit) {
      raw_snapshot cpu;
      cpu.*register_word = 1u << bit;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (!decoded.observed.has(feature)) return false;
      if (decoded.present.has(feature) != (bit == expected_bit)) return false;
      if (native::classify_isa(cpu, feature).admitted() != (bit == expected_bit)) return false;
    }
    return true;
  }
  static_assert(raw_mapping(features[0], 4, &raw_snapshot::leaf7_1_eax));
  static_assert(raw_mapping(features[1], 11, &raw_snapshot::leaf7_ecx));
  static_assert(raw_mapping(features[2], 4, &raw_snapshot::leaf7_1_edx));
  static_assert(raw_mapping(features[3], 10, &raw_snapshot::leaf7_1_edx));

  constexpr bool raw_admission(x86_feature feature) {
    raw_snapshot cpu;
    if (!native::classify_isa(cpu, feature).admitted()) return false;
    for (auto maximum : {0u, 1u, 6u}) {
      cpu = {};
      cpu.max_basic_leaf = maximum;
      auto decoded = native::detail::decode_x86_features(cpu);
      if (decoded.present.has(feature) || decoded.observed.has(feature)) return false;
      if (native::classify_isa(cpu, feature).admitted()) return false;
    }
    cpu = {};
    cpu.max_leaf7_subleaf = 0;
    auto decoded = native::detail::decode_x86_features(cpu);
    bool independent_subleaf_zero = feature == x86_feature::avx512vnni;
    if (decoded.present.has(feature) != independent_subleaf_zero ||
        decoded.observed.has(feature) != independent_subleaf_zero) return false;
    if (native::classify_isa(cpu, feature).admitted() != independent_subleaf_zero) return false;
    for (unsigned bit : {26u, 27u}) {
      cpu = {};
      cpu.leaf1_ecx &= ~(1u << bit);
      decoded = native::detail::decode_x86_features(cpu);
      if (!decoded.present.has(feature) || !decoded.observed.has(feature)) return false;
      auto result = native::classify_isa(cpu, feature);
      if (result.admitted() || !result.missing_xcr0_observation) return false;
    }
    cpu = {};
    cpu.xcr0 = 0x6;
    if (native::classify_isa(cpu, feature).admitted() != !independent_subleaf_zero) return false;
    auto older = native::classify_isa(older_snapshot{}, feature);
    return older.admitted() == (feature == x86_feature::avxvnni || independent_subleaf_zero);
  }
  static_assert(raw_admission(features[0]));
  static_assert(raw_admission(features[1]));
  static_assert(raw_admission(features[2]));
  static_assert(raw_admission(features[3]));
}

int main() {
  if (!properties()) return 1;
  for (auto feature : features)
    if (!normalized_features(feature, false) || !normalized_features(feature, true) ||
        !normalized_state(feature) || !raw_admission(feature)) return 2;
  std::puts("PASS: VNNI feature metadata, CPUID and OS-state admission");
}
