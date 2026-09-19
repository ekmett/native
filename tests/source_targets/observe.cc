// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
import simd_target_metadata;
import simd.x86;
int main() {
  auto cpu=simd::observe_x86_capabilities();
  bool avx2=simd::classify_isa(cpu,simd::avx2).admitted();
  bool avx512=simd::classify_isa(cpu,simd::avx512).admitted();
  // Independent native check of the published presets, including readable OS state.
  bool state=cpu.max_basic_leaf>=1 && (cpu.leaf1_ecx&0x0c000000u)==0x0c000000u && cpu.xcr0_observed;
  bool expected_avx2=cpu.max_basic_leaf>=7 &&
    (cpu.leaf1_ecx&0x14981201u)==0x14981201u &&
    (cpu.leaf1_edx&0x06800000u)==0x06800000u &&
    (cpu.leaf7_ebx&0x120u)==0x120u && state && (cpu.xcr0&6)==6;
  bool expected_avx512=expected_avx2 && (cpu.leaf1_ecx&(1u<<29)) &&
    (cpu.leaf7_ebx&0xc0030000u)==0xc0030000u && (cpu.xcr0&0xe6)==0xe6;
  if(avx2!=expected_avx2 || avx512!=expected_avx512) return 1;
  std::printf("Actual CPU observation: AVX2=%d AVX512=%d extended_max=%x extended1_ecx=%x\n",
    avx2,avx512,cpu.max_extended_leaf,cpu.extended1_ecx);
}
