// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
import simd_target_metadata;
import simd.cpuid;
int main() {
  auto cpu=simd::observe_x86_capabilities();
  bool avx2=simd::classify_isa(cpu,simd::avx2).admitted();
  bool avx512=simd::classify_isa(cpu,simd::avx512).admitted();
  if(avx2!=simd::classify_x86_profile(cpu,simd::x86_profile::avx2).admitted()) return 1;
  if(avx512!=simd::classify_x86_profile(cpu,simd::x86_profile::avx512).admitted()) return 2;
  std::printf("Actual CPU observation: AVX2=%d AVX512=%d extended_max=%x extended1_ecx=%x\n",
    avx2,avx512,cpu.max_extended_leaf,cpu.extended1_ecx);
}
