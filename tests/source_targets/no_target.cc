// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd;
#if defined(__aarch64__) || defined(_M_ARM64)
#ifdef __ARM_FEATURE_BF16_VECTOR_ARITHMETIC
#error SIMD_TARGET_NEGATIVE_UNAVAILABLE
#endif
using strong=simd::vec<simd::bf16,8,simd::neon_bf16>;
void no_target(strong a,strong b,float * output) {
  dot2(a,b,simd::vec<float,4,simd::neon_bf16>{0.f}).store(output);
}
#else
#ifdef __AVX512F__
#error SIMD_TARGET_NEGATIVE_UNAVAILABLE
#endif
void no_target(float const * input,float * output) {
  simd::vec<float,16,simd::avx512>::load(input).store(output);
}
#endif
