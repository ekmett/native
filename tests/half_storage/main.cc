// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#if HALF_REQUIRED_NEON_BF16 || HALF_REQUIRED_NEON_FP16 || HALF_REQUIRED_NEON
import simd.arm;
#elif HALF_REQUIRED_AVX2 || HALF_REQUIRED_AVX512 || HALF_REQUIRED_AVX512_BF16
import simd.cpuid;
#endif
int half_storage_entry();
int main() {
#if HALF_REQUIRED_NEON_BF16 || HALF_REQUIRED_NEON_FP16 || HALF_REQUIRED_NEON
#if HALF_REQUIRED_NEON_FP16
  auto profile=simd::arm_profile::neon_fp16;
#else
  auto profile=simd::arm_profile::neon;
#endif
  auto admission=simd::classify_arm_profile(simd::observe_arm_capabilities(),profile);
#elif HALF_REQUIRED_AVX2 || HALF_REQUIRED_AVX512 || HALF_REQUIRED_AVX512_BF16
#if HALF_REQUIRED_AVX512_BF16
  auto profile=simd::x86_profile::avx512_bf16;
#elif HALF_REQUIRED_AVX512
  auto profile=simd::x86_profile::avx512;
#else
  auto profile=simd::x86_profile::avx2;
#endif
  auto admission=simd::classify_x86_profile(simd::observe_x86_capabilities(),profile);
#endif
#if HALF_REQUIRED_NEON_BF16 || HALF_REQUIRED_NEON_FP16 || HALF_REQUIRED_NEON || HALF_REQUIRED_AVX2 || HALF_REQUIRED_AVX512 || HALF_REQUIRED_AVX512_BF16
  if(!admission.admitted()) {std::puts(admission.reason());return 77;}
#endif
#if HALF_REQUIRED_NEON_BF16
  {
    auto bf16_admission=simd::classify_arm_profile(simd::observe_arm_capabilities(),simd::arm_profile::neon_bf16);
    if(!bf16_admission.admitted()) {std::puts(bf16_admission.reason());return 77;}
  }
#endif
  return half_storage_entry();
}
