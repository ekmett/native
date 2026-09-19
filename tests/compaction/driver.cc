// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#if SIMD_TEST_PROFILE == 256 || SIMD_TEST_PROFILE == 512
import simd.x86;
#elif SIMD_TEST_PROFILE == 128 && (SIMD_TEST_FP16 || SIMD_TEST_BF16)
import simd.arm;
#endif
#if (!SIMD_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)))
#error Common consumer must not inherit AVX-512 ISA flags
#endif
int compaction_entry(int,char **);
int main(int argc,char ** argv) {
#if SIMD_TEST_PROFILE == 256 || SIMD_TEST_PROFILE == 512
#if SIMD_TEST_AVX512_FP16
  constexpr auto profile = simd::x86_profile::avx512_fp16;
#elif SIMD_TEST_BF16
  constexpr auto profile = simd::x86_profile::avx512_bf16;
#else
  constexpr auto profile = SIMD_TEST_PROFILE == 512 ? simd::x86_profile::avx512 : simd::x86_profile::avx2;
#endif
  auto admission=simd::classify_x86_profile(simd::observe_x86_capabilities(),profile);
  if(!admission.admitted()) { std::puts(admission.reason());return 77; }
#endif
#if SIMD_TEST_PROFILE == 128 && SIMD_TEST_BF16
  auto admission=simd::classify_arm_profile(simd::observe_arm_capabilities(),simd::arm_profile::neon_bf16);
  if(!admission.admitted()) {std::puts(admission.reason());return 77;}
#elif SIMD_TEST_PROFILE == 128 && SIMD_TEST_FP16
  auto admission=simd::classify_arm_profile(simd::observe_arm_capabilities(),simd::arm_profile::neon_fp16);
  if(!admission.admitted()) { std::puts(admission.reason());return 77; }
#endif
  return compaction_entry(argc,argv);
}
