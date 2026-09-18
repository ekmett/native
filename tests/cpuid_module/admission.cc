// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <initializer_list>
#include <cstdio>
#include <cstring>
import simd.cpuid;
#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__FMA__) || defined(__BMI2__) || defined(__POPCNT__)
#error Admission consumer inherited profile flags
#endif
namespace {
  constexpr simd::x86_capabilities full{
    7, (1u<<0)|(1u<<9)|(1u<<12)|(1u<<19)|(1u<<20)|(1u<<23)|
      (1u<<26)|(1u<<27)|(1u<<28)|(1u<<29),
    (1u<<23)|(1u<<25)|(1u<<26),
    (1u<<5)|(1u<<8)|(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31), 0xe6, true};
  constexpr bool synthetic() {
    using enum simd::x86_profile;
    for (auto profile : {avx2, avx512}) {
      if (!simd::classify_x86_profile(full, profile).admitted()) return false;
      // Independent contract oracle: CPUID.1 ECX SSE3, SSSE3, FMA, SSE4.1,
      // SSE4.2, POPCNT, XSAVE, AVX (+F16C for AVX512); EDX MMX/SSE/SSE2.
      // CPUID.7 EBX AVX2/BMI2 (+AVX512 F/DQ/BW/VL). OSXSAVE is separate.
      constexpr std::uint32_t expected_edx = 0x06800000;
      auto expected_ecx = profile == avx512 ? 0x34981201u : 0x14981201u;
      auto expected_ebx = profile == avx512 ? 0xc0030120u : 0x00000120u;
      auto expected_xcr0 = profile == avx512 ? 0xe6ull : 0x6ull;
      auto empty = simd::classify_x86_profile({}, profile);
      if (empty.missing_leaf1_ecx != expected_ecx || empty.missing_leaf1_edx != expected_edx ||
          empty.missing_leaf7_ebx != expected_ebx || empty.missing_xcr0 != expected_xcr0) return false;
      for (unsigned bit = 0; bit != 32; ++bit) {
        auto mask = std::uint32_t(1) << bit;
        auto cpu = full;
        cpu.leaf1_ecx &= ~mask;
        auto result = simd::classify_x86_profile(cpu, profile);
        if (result.missing_leaf1_ecx != (expected_ecx & mask)) return false;
        if (result.admitted() == bool((expected_ecx | (1u<<27)) & mask)) return false;
        cpu = full; cpu.leaf1_edx &= ~mask;
        result = simd::classify_x86_profile(cpu, profile);
        if (result.missing_leaf1_edx != (expected_edx & mask)) return false;
        if (result.admitted() == bool(expected_edx & mask)) return false;
        cpu = full; cpu.leaf7_ebx &= ~mask;
        result = simd::classify_x86_profile(cpu, profile);
        if (result.missing_leaf7_ebx != (expected_ebx & mask)) return false;
        if (result.admitted() == bool(expected_ebx & mask)) return false;
      }
      for (unsigned bit = 0; bit != 64; ++bit) {
        auto cpu = full; auto mask = std::uint64_t(1) << bit;
        cpu.xcr0 &= ~mask;
        auto result = simd::classify_x86_profile(cpu, profile);
        if (result.missing_xcr0 != (expected_xcr0 & mask)) return false;
        if (result.admitted() == bool(expected_xcr0 & mask)) return false;
      }
      for (unsigned maximum : {0u, 1u, 6u}) {
        auto cpu = full; cpu.max_basic_leaf = maximum;
        auto result = simd::classify_x86_profile(cpu, profile);
        if (result.admitted() || !result.missing_leaf7) return false;
        if (result.missing_leaf1 != (maximum == 0)) return false;
        if (result.missing_leaf7_ebx != expected_ebx) return false;
        if (maximum == 0 && (result.missing_leaf1_ecx != expected_ecx ||
            result.missing_leaf1_edx != expected_edx || !result.missing_osxsave)) return false;
      }
      auto cpu = full; cpu.xcr0_observed = false;
      if (simd::classify_x86_profile(cpu, profile).admitted()) return false;
    }
    auto result = simd::classify_x86_profile(full, static_cast<simd::x86_profile>(255));
    return result.invalid_profile && !result.admitted();
  }
  static_assert(synthetic());
}
int main() {
  if (!synthetic()) return 1;
  auto cpu = full; cpu.leaf7_ebx &= ~(1u << 8);
  if (std::strcmp(simd::classify_x86_profile(cpu, simd::x86_profile::avx2).reason(), "CPU lacks BMI2")) return 2;
  cpu = full; cpu.xcr0 &= ~(1u << 2);
  if (std::strcmp(simd::classify_x86_profile(cpu, simd::x86_profile::avx2).reason(), "OS has not enabled YMM state")) return 3;
  auto native = simd::observe_x86_capabilities();
  if (native.max_basic_leaf < 1 && (native.leaf1_ecx || native.leaf1_edx || native.xcr0_observed)) return 4;
  if (native.max_basic_leaf < 7 && native.leaf7_ebx) return 5;
  constexpr auto xsave = (1u<<26)|(1u<<27);
  if (native.xcr0_observed != (native.max_basic_leaf >= 1 && (native.leaf1_ecx & xsave) == xsave)) return 6;
  for (auto profile : {simd::x86_profile::avx2, simd::x86_profile::avx512})
    std::puts(simd::classify_x86_profile(native, profile).reason());
}
