// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <native/attributes.h>
#if NATIVE_HOST_NEON
#include <arm_neon.h>
#elif NATIVE_HOST_X86
#include <immintrin.h>
#endif
import native.simd;

// Register bit bridges must compile exactly like identity under the same ABI.
#if NATIVE_HOST_NEON
constexpr auto storage_arch = native::neon;
using fp16_4 = native::simd<native::fp16,4,storage_arch>;
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_fp16_4_bits(uint8x8_t value) {
  return fp16_4::from_native(value).bits().to_native();
}
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_fp16_4_bits_raw(uint8x8_t value) {
  return value;
}
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_fp16_4_from_bits(uint8x8_t value) {
  return fp16_4::from_bits(fp16_4::bits_type::from_native(value)).to_native();
}
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_fp16_4_from_bits_raw(uint8x8_t value) {
  return value;
}
using fp16_8 = native::simd<native::fp16,8,storage_arch>;
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_fp16_8_bits(uint8x16_t value) {
  return fp16_8::from_native(value).bits().to_native();
}
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_fp16_8_bits_raw(uint8x16_t value) {
  return value;
}
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_fp16_8_from_bits(uint8x16_t value) {
  return fp16_8::from_bits(fp16_8::bits_type::from_native(value)).to_native();
}
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_fp16_8_from_bits_raw(uint8x16_t value) {
  return value;
}
using bf16_4 = native::simd<native::bf16,4,storage_arch>;
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_bf16_4_bits(uint8x8_t value) {
  return bf16_4::from_native(value).bits().to_native();
}
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_bf16_4_bits_raw(uint8x8_t value) {
  return value;
}
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_bf16_4_from_bits(uint8x8_t value) {
  return bf16_4::from_bits(bf16_4::bits_type::from_native(value)).to_native();
}
extern "C" native_noinline native_target("neon") uint8x8_t native_storage_arm_bf16_4_from_bits_raw(uint8x8_t value) {
  return value;
}
using bf16_8 = native::simd<native::bf16,8,storage_arch>;
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_bf16_8_bits(uint8x16_t value) {
  return bf16_8::from_native(value).bits().to_native();
}
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_bf16_8_bits_raw(uint8x16_t value) {
  return value;
}
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_bf16_8_from_bits(uint8x16_t value) {
  return bf16_8::from_bits(bf16_8::bits_type::from_native(value)).to_native();
}
extern "C" native_noinline native_target("neon") uint8x16_t native_storage_arm_bf16_8_from_bits_raw(uint8x16_t value) {
  return value;
}
#endif
#if NATIVE_HOST_X86
constexpr auto storage_arch = native::feature_closure(native::x86_feature::sse2);
using fp16_4 = native::simd<native::fp16,4,storage_arch>;
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_4_bits(__m128i value) {
  return fp16_4::from_native(value).bits().to_native();
}
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_4_bits_raw(__m128i value) {
  return value;
}
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_4_from_bits(__m128i value) {
  return fp16_4::from_bits(fp16_4::bits_type::from_native(value)).to_native();
}
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_4_from_bits_raw(__m128i value) {
  return value;
}
using fp16_8 = native::simd<native::fp16,8,storage_arch>;
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_8_bits(__m128i value) {
  return fp16_8::from_native(value).bits().to_native();
}
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_8_bits_raw(__m128i value) {
  return value;
}
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_8_from_bits(__m128i value) {
  return fp16_8::from_bits(fp16_8::bits_type::from_native(value)).to_native();
}
extern "C" native_noinline native_target("sse2") __m128i native_storage_x86_fp16_8_from_bits_raw(__m128i value) {
  return value;
}
#endif
