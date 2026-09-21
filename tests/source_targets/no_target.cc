// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native;
#if defined(__aarch64__) || defined(_M_ARM64)
#ifdef __ARM_FEATURE_BF16_VECTOR_ARITHMETIC
#error NATIVE_TARGET_NEGATIVE_UNAVAILABLE
#endif
using strong=native::vec<native::bf16,8,native::neon_bf16>;
void no_target(strong a,strong b,float * output) {
  dot2(a,b,native::vec<float,4,native::neon_bf16>{0.f}).store(output);
}
#else
#ifdef __AVX512F__
#error NATIVE_TARGET_NEGATIVE_UNAVAILABLE
#endif
void no_target(float const * input,float * output) {
  native::vec<float,16,native::avx512>::load(input).store(output);
}
#endif
