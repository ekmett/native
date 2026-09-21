// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <cstddef>
#include <cstdint>
import native;
#if defined(__x86_64__) || defined(_M_X64)
#define RAW_TARGET avx512
#define FP_TARGET avx512_fp16
#define BF_TARGET avx512_bf16
constexpr std::size_t lanes=16;
#else
#define RAW_TARGET neon
#define FP_TARGET neon_fp16
#define BF_TARGET neon_bf16
constexpr std::size_t lanes=4;
#endif
#if TEST_FP16 && (NATIVE_MINIMAL_HAS_AVX512_FP16 || NATIVE_MINIMAL_HAS_NEON_FP16)
#error NATIVE_TARGET_NEGATIVE_UNAVAILABLE
#elif !TEST_FP16 && (NATIVE_MINIMAL_HAS_AVX512_BF16 || NATIVE_MINIMAL_HAS_NEON_BF16)
#error NATIVE_TARGET_NEGATIVE_UNAVAILABLE
#endif
constexpr auto advertised=NATIVE_TARGET_ISA(FP_TARGET)&NATIVE_TARGET_ISA(BF_TARGET);
NATIVE_TARGET_PUSH(RAW_TARGET)
#if TEST_FP16
void missing_half_target(std::uint16_t const * input,std::uint16_t * output) {
  using H=native::simd<native::fp16,lanes*2,advertised>;
  sqrt(H::load_bits(input)).store_bits(output);
}
#else
void missing_half_target(std::uint16_t const * input,float * output) {
  using B=native::simd<native::bf16,lanes*2,advertised>;
  using V=native::simd<float,lanes,advertised>;
  auto value=B::load_bits(input);
  dot2(value,value,V(0.f)).store(output);
}
#endif
NATIVE_TARGET_POP()
