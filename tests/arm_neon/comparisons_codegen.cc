// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <cstddef>
#if NATIVE_COMPARISON_HEADER
#include <native/simd.h>
#else
import native.arm.neon;
#endif

template<std::size_t N> using floats = native::simd<float, N, native::neon>;

// Native-register results isolate comparison lowering from memory transfer and
// mask-to-bitset work. Typed short inputs already satisfy padding invariants.
extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_eq_2(floats<2> x, floats<2> y) {
  return __builtin_bit_cast(uint32x4_t, (x == y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_ne_2(floats<2> x, floats<2> y) {
  return __builtin_bit_cast(uint32x4_t, (x != y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_lt_2(floats<2> x, floats<2> y) {
  return __builtin_bit_cast(uint32x4_t, (x < y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_gt_2(floats<2> x, floats<2> y) {
  return __builtin_bit_cast(uint32x4_t, (x > y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_le_2(floats<2> x, floats<2> y) {
  return __builtin_bit_cast(uint32x4_t, (x <= y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_ge_2(floats<2> x, floats<2> y) {
  return __builtin_bit_cast(uint32x4_t, (x >= y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_eq_3(floats<3> x, floats<3> y) {
  return __builtin_bit_cast(uint32x4_t, (x == y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_ne_3(floats<3> x, floats<3> y) {
  return __builtin_bit_cast(uint32x4_t, (x != y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_lt_3(floats<3> x, floats<3> y) {
  return __builtin_bit_cast(uint32x4_t, (x < y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_gt_3(floats<3> x, floats<3> y) {
  return __builtin_bit_cast(uint32x4_t, (x > y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_le_3(floats<3> x, floats<3> y) {
  return __builtin_bit_cast(uint32x4_t, (x <= y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_ge_3(floats<3> x, floats<3> y) {
  return __builtin_bit_cast(uint32x4_t, (x >= y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_eq_4(float32x4_t a, float32x4_t b) {
  auto const x = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, a));
  auto const y = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, b));
  return __builtin_bit_cast(uint32x4_t, (x == y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_ne_4(float32x4_t a, float32x4_t b) {
  auto const x = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, a));
  auto const y = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, b));
  return __builtin_bit_cast(uint32x4_t, (x != y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_lt_4(float32x4_t a, float32x4_t b) {
  auto const x = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, a));
  auto const y = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, b));
  return __builtin_bit_cast(uint32x4_t, (x < y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_gt_4(float32x4_t a, float32x4_t b) {
  auto const x = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, a));
  auto const y = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, b));
  return __builtin_bit_cast(uint32x4_t, (x > y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_le_4(float32x4_t a, float32x4_t b) {
  auto const x = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, a));
  auto const y = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, b));
  return __builtin_bit_cast(uint32x4_t, (x <= y).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t comparison_ge_4(float32x4_t a, float32x4_t b) {
  auto const x = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, a));
  auto const y = floats<4>::from_native(__builtin_bit_cast(floats<4>::native_type, b));
  return __builtin_bit_cast(uint32x4_t, (x >= y).to_native());
}

#if !NATIVE_COMPARISON_HEADER
// The public half vector is module-owned; no scalar conversion participates.
using halves = native::simd<native::fp16, 8, native::neon_fp16>;

extern "C" __attribute__((noinline, used, target("neon,fullfp16")))
uint16x8_t comparison_eq_half8(float16x8_t a, float16x8_t b) {
  return __builtin_bit_cast(uint16x8_t,
    (halves::from_native(a) == halves::from_native(b)).to_native());
}

extern "C" __attribute__((noinline, used, target("neon,fullfp16")))
uint16x8_t comparison_ne_half8(float16x8_t a, float16x8_t b) {
  return __builtin_bit_cast(uint16x8_t,
    (halves::from_native(a) != halves::from_native(b)).to_native());
}

extern "C" __attribute__((noinline, used, target("neon,fullfp16")))
uint16x8_t comparison_lt_half8(float16x8_t a, float16x8_t b) {
  return __builtin_bit_cast(uint16x8_t,
    (halves::from_native(a) < halves::from_native(b)).to_native());
}

extern "C" __attribute__((noinline, used, target("neon,fullfp16")))
uint16x8_t comparison_gt_half8(float16x8_t a, float16x8_t b) {
  return __builtin_bit_cast(uint16x8_t,
    (halves::from_native(a) > halves::from_native(b)).to_native());
}

extern "C" __attribute__((noinline, used, target("neon,fullfp16")))
uint16x8_t comparison_le_half8(float16x8_t a, float16x8_t b) {
  return __builtin_bit_cast(uint16x8_t,
    (halves::from_native(a) <= halves::from_native(b)).to_native());
}

extern "C" __attribute__((noinline, used, target("neon,fullfp16")))
uint16x8_t comparison_ge_half8(float16x8_t a, float16x8_t b) {
  return __builtin_bit_cast(uint16x8_t,
    (halves::from_native(a) >= halves::from_native(b)).to_native());
}

#endif
