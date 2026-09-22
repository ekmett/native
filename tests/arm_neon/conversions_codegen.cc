// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <cstdint>
#if NATIVE_CONVERSION_HEADER
#include <native/simd.h>
#else
import native.arm.neon;
#endif

constexpr auto conversion_neon = native::neon;
using scalar_float = native::simd<float, 1, native::scalar>;
using neon_float1 = native::simd<float, 1, conversion_neon>;
using neon_float4 = native::simd<float, 4, conversion_neon>;

extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t raw_fcvtzs_scalar(float value) { return vcvts_s32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t native_fcvtzs_scalar(float value) { return native::fcvtzs(value); }

extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t raw_fcvtzs_explicit(float value) { return vcvts_s32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t native_fcvtzs_explicit(float value) { return native::fcvtzs<conversion_neon>(value); }

extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t raw_fcvtzs_scalar_simd1(float value) { return vcvts_s32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t native_fcvtzs_scalar_simd1(float value) {
  return native::fcvtzs(scalar_float(value)).to_native();
}

extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t raw_fcvtzs_neon_simd1(float value) { return vcvts_s32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::int32_t native_fcvtzs_neon_simd1(float value) {
  return native::fcvtzs(neon_float1(value)).to_native();
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t raw_fcvtzs_neon_simd4(float32x4_t value) { return vcvtq_s32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t native_fcvtzs_neon_simd4(float32x4_t value) {
  return __builtin_bit_cast(int32x4_t, native::fcvtzs(neon_float4::from_native(value)).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t raw_fcvtzu_scalar(float value) { return vcvts_u32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t native_fcvtzu_scalar(float value) { return native::fcvtzu(value); }

extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t raw_fcvtzu_explicit(float value) { return vcvts_u32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t native_fcvtzu_explicit(float value) { return native::fcvtzu<conversion_neon>(value); }

extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t raw_fcvtzu_scalar_simd1(float value) { return vcvts_u32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t native_fcvtzu_scalar_simd1(float value) {
  return native::fcvtzu(scalar_float(value)).to_native();
}

extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t raw_fcvtzu_neon_simd1(float value) { return vcvts_u32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
std::uint32_t native_fcvtzu_neon_simd1(float value) {
  return native::fcvtzu(neon_float1(value)).to_native();
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t raw_fcvtzu_neon_simd4(float32x4_t value) { return vcvtq_u32_f32(value); }
extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t native_fcvtzu_neon_simd4(float32x4_t value) {
  return __builtin_bit_cast(uint32x4_t, native::fcvtzu(neon_float4::from_native(value)).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t raw_fcvtzs_neon_simd2(native::simd<float, 2, conversion_neon> value) {
  return vcvtq_s32_f32(__builtin_bit_cast(float32x4_t, value.to_native()));
}
extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t native_fcvtzs_neon_simd2(native::simd<float, 2, conversion_neon> value) {
  return __builtin_bit_cast(int32x4_t, native::fcvtzs(value).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t raw_fcvtzs_neon_simd3(native::simd<float, 3, conversion_neon> value) {
  return vcvtq_s32_f32(__builtin_bit_cast(float32x4_t, value.to_native()));
}
extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t native_fcvtzs_neon_simd3(native::simd<float, 3, conversion_neon> value) {
  return __builtin_bit_cast(int32x4_t, native::fcvtzs(value).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t raw_fcvtzu_neon_simd2(native::simd<float, 2, conversion_neon> value) {
  return vcvtq_u32_f32(__builtin_bit_cast(float32x4_t, value.to_native()));
}
extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t native_fcvtzu_neon_simd2(native::simd<float, 2, conversion_neon> value) {
  return __builtin_bit_cast(uint32x4_t, native::fcvtzu(value).to_native());
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t raw_fcvtzu_neon_simd3(native::simd<float, 3, conversion_neon> value) {
  return vcvtq_u32_f32(__builtin_bit_cast(float32x4_t, value.to_native()));
}
extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t native_fcvtzu_neon_simd3(native::simd<float, 3, conversion_neon> value) {
  return __builtin_bit_cast(uint32x4_t, native::fcvtzu(value).to_native());
}
