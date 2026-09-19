// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <initializer_list>
#include <cstdio>
#include <cstring>
import simd.cpu.x86;
#if (!SIMD_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)))
#error Common consumer must not inherit AVX-512 ISA flags
#endif
namespace {
  constexpr simd::x86_capabilities full{
    7, (1u<<0)|(1u<<9)|(1u<<12)|(1u<<19)|(1u<<20)|(1u<<23)|
      (1u<<26)|(1u<<27)|(1u<<28)|(1u<<29),
    (1u<<23)|(1u<<25)|(1u<<26),
    (1u<<5)|(1u<<8)|(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31), 0xe6, true, 1, 1u<<5, 1u<<23};
  constexpr bool synthetic(simd::isa profile) {
    using simd::avx2, simd::avx512, simd::avx512_bf16, simd::avx512_fp16;
    if (!simd::classify_isa(full, profile).admitted()) return false;
    // Independent contract oracle: CPUID.1 ECX SSE3, SSSE3, FMA, SSE4.1,
    // SSE4.2, POPCNT, XSAVE, AVX (+F16C for AVX512); EDX MMX/SSE/SSE2.
    // CPUID.7 EBX AVX2/BMI2 (+AVX512 F/DQ/BW/VL). OSXSAVE is separate.
    constexpr std::uint32_t expected_edx = 0x06800000;
    auto expected_ecx = profile != avx2 ? 0x34981201u : 0x14981201u;
    auto expected_ebx = profile != avx2 ? 0xc0030120u : 0x00000120u;
    auto expected_xcr0 = profile != avx2 ? 0xe6ull : 0x6ull;
    auto empty = simd::classify_isa(simd::x86_capabilities{}, profile);
    if (empty.admitted() || empty.missing_features != profile ||
        empty.missing_xcr0 != expected_xcr0 || !empty.missing_xcr0_observation) return false;
    for (unsigned bit = 0; bit != 32; ++bit) {
      auto mask = std::uint32_t(1) << bit;
      auto cpu = full;
      cpu.leaf1_ecx &= ~mask;
      auto result = simd::classify_isa(cpu, profile);
      if (result.admitted() == bool((expected_ecx | (1u<<27)) & mask)) return false;
      cpu = full; cpu.leaf1_edx &= ~mask;
      result = simd::classify_isa(cpu, profile);
      if (result.admitted() == bool(expected_edx & mask)) return false;
      cpu = full; cpu.leaf7_ebx &= ~mask;
      result = simd::classify_isa(cpu, profile);
      if (result.admitted() == bool(expected_ebx & mask)) return false;
    }
    for (unsigned bit = 0; bit != 32; ++bit) {
      auto cpu = full; auto mask = std::uint32_t(1) << bit;
      cpu.leaf7_1_eax &= ~mask;
      auto result = simd::classify_isa(cpu, profile);
      auto expected = profile == avx512_bf16 ? (1u << 5) : 0u;
      if (result.admitted() == bool(expected & mask)) return false;
    }
    {
      // Stale subleaf bits must never admit a missing subleaf or leaf.
      auto cpu = full; cpu.max_leaf7_subleaf = 0;
      auto result = simd::classify_isa(cpu, profile);
      if (result.admitted() != (profile != avx512_bf16)) return false;
    }
    for (unsigned bit = 0; bit != 32; ++bit) {
      auto cpu = full; auto mask = std::uint32_t(1) << bit;
      cpu.leaf7_edx &= ~mask;
      auto result = simd::classify_isa(cpu, profile);
      auto expected = profile == avx512_fp16 ? (1u << 23) : 0u;
      if (result.admitted() == bool(expected & mask)) return false;
    }
    for (unsigned bit = 0; bit != 64; ++bit) {
      auto cpu = full; auto mask = std::uint64_t(1) << bit;
      cpu.xcr0 &= ~mask;
      auto result = simd::classify_isa(cpu, profile);
      if (result.missing_xcr0 != (expected_xcr0 & mask)) return false;
      if (result.admitted() == bool(expected_xcr0 & mask)) return false;
    }
    for (unsigned maximum : {0u, 1u, 6u}) {
      auto cpu = full; cpu.max_basic_leaf = maximum;
      auto result = simd::classify_isa(cpu, profile);
      if (result.admitted() || !result.missing_features.avx2 || !result.missing_features.bmi2) return false;
      if (profile == avx512_bf16 && !result.missing_features.avx512bf16) return false;
      if (profile == avx512_fp16 && !result.missing_features.avx512fp16) return false;
      if (maximum == 0 && (!result.missing_features.sse || !result.missing_features.avx ||
          !result.missing_xcr0_observation)) return false;
    }
    auto cpu = full; cpu.xcr0_observed = false;
    auto unread = simd::classify_isa(cpu, profile);
    if (unread.admitted() || !unread.missing_xcr0_observation || unread.missing_xcr0 != expected_xcr0) return false;
    auto result = simd::classify_isa(full, simd::isa(static_cast<simd::x86_feature>(-1)));
    return result.invalid_features && !result.admitted();
  }
  static_assert(synthetic(simd::avx2));
  static_assert(synthetic(simd::avx512));
  static_assert(synthetic(simd::avx512_bf16));
  static_assert(synthetic(simd::avx512_fp16));
}
int main() {
  for(auto profile:{simd::avx2,simd::avx512,simd::avx512_bf16,simd::avx512_fp16})
    if (!synthetic(profile)) return 1;
  auto cpu = full; cpu.leaf7_ebx &= ~(1u << 8);
  if (std::strcmp(simd::classify_isa(cpu, simd::avx2).reason(), "bmi2")) return 2;
  cpu = full; cpu.xcr0 &= ~(1u << 2);
  if (std::strcmp(simd::classify_isa(cpu, simd::avx2).reason(), "YMM state unavailable")) return 3;
  auto native = simd::observe_x86_capabilities();
  auto identity=simd::cpuid(0,0);
  std::int32_t vendor_words[]{identity.ebx,identity.edx,identity.ecx};
  if(native.vendor_id[12]!=0 || std::memcmp(native.vendor_id.data(),vendor_words,12)) return 10;
  auto expected_vendor=std::strcmp(native.vendor_id.data(),"GenuineIntel")==0 ? simd::cpu_vendor::intel :
    std::strcmp(native.vendor_id.data(),"AuthenticAMD")==0 ? simd::cpu_vendor::amd : simd::cpu_vendor::unknown;
  if(native.vendor!=expected_vendor) return 11;
  if (native.max_basic_leaf < 1 && (native.leaf1_ecx || native.leaf1_edx || native.xcr0_observed)) return 4;
  if (native.max_basic_leaf < 7 && (native.leaf7_ebx || native.leaf7_edx)) return 5;
  constexpr auto xsave = (1u<<26)|(1u<<27);
  if (native.xcr0_observed != (native.max_basic_leaf >= 1 && (native.leaf1_ecx & xsave) == xsave)) return 6;
  if ((native.max_basic_leaf < 7 || native.max_leaf7_subleaf < 1) && native.leaf7_1_eax) return 7;
  cpu = full; cpu.leaf7_1_eax = 0;
  if (std::strcmp(simd::classify_isa(cpu, simd::avx512_bf16).reason(), "avx512bf16")) return 8;
  cpu = full; cpu.leaf7_edx = 0;
  if (std::strcmp(simd::classify_isa(cpu, simd::avx512_fp16).reason(), "avx512fp16")) return 9;
  for (auto profile : {simd::avx2, simd::avx512, simd::avx512_bf16, simd::avx512_fp16})
    std::puts(simd::classify_isa(native, profile).reason());
}
