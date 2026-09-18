// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#if SIMD_TEST_PROFILE == 256 || SIMD_TEST_PROFILE == 512
import simd.cpuid;
#endif
#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__FMA__) || defined(__BMI2__)
#error The compaction admission driver must remain baseline
#endif
int compaction_entry(int,char **);
int main(int argc,char ** argv) {
#if SIMD_TEST_PROFILE == 256 || SIMD_TEST_PROFILE == 512
  constexpr auto profile = SIMD_TEST_PROFILE == 512 ? simd::x86_profile::avx512 : simd::x86_profile::avx2;
  auto admission=simd::classify_x86_profile(simd::observe_x86_capabilities(),profile);
  if(!admission.admitted()) { std::puts(admission.reason());return 77; }
#endif
  return compaction_entry(argc,argv);
}
