// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <initializer_list>
#include <cstdio>
#include <cstring>
#include <native/targets.h>
import native.x86.features;
#if (!NATIVE_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)))
#error Common consumer must not inherit AVX-512 ISA flags
#endif
namespace {
  // Independent raw fixture: removing a register bit exercises the decoder.
  struct raw_snapshot {
    std::uint32_t max_basic_leaf=0,leaf1_ecx=0,leaf1_edx=0,leaf7_ebx=0;
    std::uint64_t xcr0=0;
    bool xcr0_observed=false;
    std::uint32_t max_leaf7_subleaf=0,leaf7_1_eax=0,leaf7_edx=0;
  };
  constexpr raw_snapshot full{
    7, (1u<<0)|(1u<<9)|(1u<<12)|(1u<<19)|(1u<<20)|(1u<<23)|
      (1u<<26)|(1u<<27)|(1u<<28)|(1u<<29),
    (1u<<23)|(1u<<25)|(1u<<26),
    (1u<<5)|(1u<<8)|(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31), 0xe6, true, 1, 1u<<5, 1u<<23};
  constexpr bool synthetic(native::isa profile) {
    using native::avx2, native::avx512, native::avx512_bf16, native::avx512_fp16;
    if (!native::classify_isa(full, profile).admitted()) return false;
    // Independent contract oracle: CPUID.1 ECX SSE3, SSSE3, FMA, SSE4.1,
    // SSE4.2, POPCNT, XSAVE, AVX (+F16C for AVX512); EDX MMX/SSE/SSE2.
    // CPUID.7 EBX AVX2 (+AVX512 F/DQ/BW/VL). OSXSAVE is separate.
    constexpr std::uint32_t expected_edx = 0x06800000;
    auto expected_ecx = profile != avx2 ? 0x34981201u : 0x14981201u;
    auto expected_ebx = profile != avx2 ? 0xc0030020u : 0x00000020u;
    auto expected_xcr0 = profile != avx2 ? 0xe6ull : 0x6ull;
    auto empty = native::classify_isa(native::x86_capabilities{}, profile);
    if (empty.admitted() || empty.missing_features != profile ||
        empty.missing_xcr0 != expected_xcr0 || !empty.missing_xcr0_observation) return false;
    for (unsigned bit = 0; bit != 32; ++bit) {
      auto mask = std::uint32_t(1) << bit;
      auto cpu = full;
      cpu.leaf1_ecx &= ~mask;
      auto result = native::classify_isa(cpu, profile);
      if (result.admitted() == bool((expected_ecx | (1u<<27)) & mask)) return false;
      cpu = full; cpu.leaf1_edx &= ~mask;
      result = native::classify_isa(cpu, profile);
      if (result.admitted() == bool(expected_edx & mask)) return false;
      cpu = full; cpu.leaf7_ebx &= ~mask;
      result = native::classify_isa(cpu, profile);
      if (result.admitted() == bool(expected_ebx & mask)) return false;
    }
    for (unsigned bit = 0; bit != 32; ++bit) {
      auto cpu = full; auto mask = std::uint32_t(1) << bit;
      cpu.leaf7_1_eax &= ~mask;
      auto result = native::classify_isa(cpu, profile);
      auto expected = profile == avx512_bf16 ? (1u << 5) : 0u;
      if (result.admitted() == bool(expected & mask)) return false;
    }
    {
      // Stale subleaf bits must never admit a missing subleaf or leaf.
      auto cpu = full; cpu.max_leaf7_subleaf = 0;
      auto result = native::classify_isa(cpu, profile);
      if (result.admitted() != (profile != avx512_bf16)) return false;
    }
    for (unsigned bit = 0; bit != 32; ++bit) {
      auto cpu = full; auto mask = std::uint32_t(1) << bit;
      cpu.leaf7_edx &= ~mask;
      auto result = native::classify_isa(cpu, profile);
      auto expected = profile == avx512_fp16 ? (1u << 23) : 0u;
      if (result.admitted() == bool(expected & mask)) return false;
    }
    for (unsigned bit = 0; bit != 64; ++bit) {
      auto cpu = full; auto mask = std::uint64_t(1) << bit;
      cpu.xcr0 &= ~mask;
      auto result = native::classify_isa(cpu, profile);
      if (result.missing_xcr0 != (expected_xcr0 & mask)) return false;
      if (result.admitted() == bool(expected_xcr0 & mask)) return false;
    }
    for (unsigned maximum : {0u, 1u, 6u}) {
      auto cpu = full; cpu.max_basic_leaf = maximum;
      auto result = native::classify_isa(cpu, profile);
      if (result.admitted() || !result.missing_features.avx2) return false;
      if (profile == avx512_bf16 && !result.missing_features.avx512bf16) return false;
      if (profile == avx512_fp16 && !result.missing_features.avx512fp16) return false;
      if (maximum == 0 && (!result.missing_features.sse || !result.missing_features.avx ||
          !result.missing_xcr0_observation)) return false;
    }
    auto cpu = full; cpu.xcr0_observed = false;
    auto unread = native::classify_isa(cpu, profile);
    if (unread.admitted() || !unread.missing_xcr0_observation || unread.missing_xcr0 != expected_xcr0) return false;
    auto result = native::classify_isa(full, native::isa(static_cast<native::x86_feature>(-1)));
    return result.invalid_features && !result.admitted();
  }
  static_assert(synthetic(native::avx2));
  static_assert(synthetic(native::avx512));
  static_assert(synthetic(native::avx512_bf16));
  static_assert(synthetic(native::avx512_fp16));

  // The SIMD preset does not require BMI2; BMI2 instructions remain independent.
  static_assert([] {
    auto cpu = full;
    cpu.leaf7_ebx &= ~(1u << 8);
    return native::classify_isa(cpu, native::avx2).admitted() &&
           !native::classify_isa(cpu, native::x86_feature::bmi2).admitted();
  }());

#define NATIVE_TARGET_wait_mwaitx "mwaitx"
#define NATIVE_TARGET_wait_waitpkg "waitpkg"
  constexpr auto wait_features = native::x86_feature::mwaitx & native::x86_feature::waitpkg;
  static_assert(NATIVE_TARGET_ISA(wait_mwaitx) == native::isa(native::x86_feature::mwaitx));
  static_assert(NATIVE_TARGET_ISA(wait_waitpkg) == native::isa(native::x86_feature::waitpkg));
  static_assert(native::target_features("mwaitx,waitpkg") == wait_features);
  static_assert(native::feature_closure(wait_features) == wait_features);
  static_assert([] {
    native::isa value;
    value.mwaitx = true;
    value.waitpkg = true;
    if (value != wait_features) return false;
    value.mwaitx = false;
    return !value.mwaitx && value.waitpkg && value == native::isa(native::x86_feature::waitpkg);
  }());

  struct wait_snapshot : raw_snapshot {
    std::uint32_t leaf7_ecx = 0;
    std::uint32_t max_extended_leaf = 0, extended1_ecx = 0;
  };
  constexpr bool synthetic_wait_features() {
    wait_snapshot full_wait;
    full_wait.max_basic_leaf = 7;
    full_wait.leaf7_ecx = 1u << 5;
    full_wait.max_extended_leaf = 0x80000001u;
    full_wait.extended1_ecx = 1u << 29;
    // No SIMD, XSAVE or XCR0 bits are required for either wait feature.
    auto result = native::classify_isa(full_wait, wait_features);
    if (!result.admitted() || result.missing_xcr0 || result.missing_xcr0_observation) return false;
    auto cpu = full_wait;
    cpu.leaf7_ecx = 0;
    result = native::classify_isa(cpu, wait_features);
    if (result.admitted() || result.missing_features != native::isa(native::x86_feature::waitpkg)) return false;
    cpu = full_wait;
    cpu.extended1_ecx = 0;
    result = native::classify_isa(cpu, wait_features);
    if (result.admitted() || result.missing_features != native::isa(native::x86_feature::mwaitx)) return false;
    // Stale register bits do not authorize features from unavailable leaves.
    cpu = full_wait;
    cpu.max_basic_leaf = 6;
    if (native::classify_isa(cpu, native::x86_feature::waitpkg).admitted()) return false;
    cpu = full_wait;
    cpu.max_extended_leaf = 0x80000000u;
    if (native::classify_isa(cpu, native::x86_feature::mwaitx).admitted()) return false;
    // Older structural raw snapshots remain valid, but cannot admit unrecorded bits.
    if (native::classify_isa(full, wait_features).admitted()) return false;

    native::x86_capabilities normalized;
    normalized.present.set(native::x86_feature::mwaitx, true);
    normalized.present.set(native::x86_feature::waitpkg, true);
    normalized.observed = normalized.present;
    if (!native::classify_isa(normalized, wait_features).admitted()) return false;
    normalized.observed.set(native::x86_feature::waitpkg, false);
    normalized.raw.max_basic_leaf = 7;
    normalized.raw.leaf7_ecx = 1u << 5;
    return !native::classify_isa(normalized, native::x86_feature::waitpkg).admitted() &&
           native::classify_isa(normalized, native::x86_feature::mwaitx).admitted();
  }
  static_assert(synthetic_wait_features());
}
int main() {
  for(auto profile:{native::avx2,native::avx512,native::avx512_bf16,native::avx512_fp16})
    if (!synthetic(profile)) return 1;
  auto cpu = full; cpu.leaf7_ebx &= ~(1u << 8);
  if (!native::classify_isa(cpu, native::avx2).admitted() ||
      std::strcmp(native::classify_isa(cpu, native::x86_feature::bmi2).reason(), "bmi2")) return 2;
  if (!synthetic_wait_features()) return 12;
  cpu = full; cpu.xcr0 &= ~(1u << 2);
  if (std::strcmp(native::classify_isa(cpu, native::avx2).reason(), "YMM state unavailable")) return 3;
  auto native = native::observe_x86_capabilities();
  auto identity=native::cpuid(0,0);
  std::int32_t vendor_words[]{identity.ebx,identity.edx,identity.ecx};
  if(native.vendor_id[12]!=0 || std::memcmp(native.vendor_id.data(),vendor_words,12)) return 10;
  auto expected_vendor=std::strcmp(native.vendor_id.data(),"GenuineIntel")==0 ? native::cpu_vendor::intel :
    std::strcmp(native.vendor_id.data(),"AuthenticAMD")==0 ? native::cpu_vendor::amd : native::cpu_vendor::unknown;
  if(native.vendor!=expected_vendor) return 11;
  if (native.raw.max_basic_leaf < 1 && (native.raw.leaf1_ecx || native.raw.leaf1_edx || native.xcr0_observed)) return 4;
  if (native.raw.max_basic_leaf < 7 && (native.raw.leaf7_ebx || native.raw.leaf7_ecx || native.raw.leaf7_edx)) return 5;
  constexpr auto xsave = (1u<<26)|(1u<<27);
  if (native.xcr0_observed != (native.raw.max_basic_leaf >= 1 && (native.raw.leaf1_ecx & xsave) == xsave)) return 6;
  if ((native.raw.max_basic_leaf < 7 || native.raw.max_leaf7_subleaf < 1) && native.raw.leaf7_1_eax) return 7;
  cpu = full; cpu.leaf7_1_eax = 0;
  if (std::strcmp(native::classify_isa(cpu, native::avx512_bf16).reason(), "avx512bf16")) return 8;
  cpu = full; cpu.leaf7_edx = 0;
  if (std::strcmp(native::classify_isa(cpu, native::avx512_fp16).reason(), "avx512fp16")) return 9;
  for (auto profile : {native::avx2, native::avx512, native::avx512_bf16, native::avx512_fp16})
    std::puts(native::classify_isa(native, profile).reason());
}
