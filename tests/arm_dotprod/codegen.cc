// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <native/arm/dotprod.h>
import native.arm.dotprod;
#include "simd_adapter.h"
#if defined(__ARM_FEATURE_DOTPROD) || defined(__ARM_FEATURE_QRDMX) || \
    defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) || defined(__ARM_FEATURE_FP16_FML) || \
    defined(__ARM_FEATURE_COMPLEX) || defined(__ARM_FEATURE_MATMUL_INT8) || \
    defined(__ARM_FEATURE_SVE) || defined(__ARM_FEATURE_ATOMICS)
#error The code-generation fixture must start from baseline AArch64.
#endif
constexpr auto requirements = native::isa(native::arm_feature::dotprod);
extern "C" __attribute__((target("dotprod"), noinline))
int32x2_t native_sdot_int32x2(int32x2_t a, int8x8_t b, int8x8_t c) noexcept {
  return dotprod_api::sdot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x2_t native_sdot_int32x2_lane8(int32x2_t a, int8x8_t b, int8x8_t c) noexcept {
  return dotprod_api::sdot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x2_t native_sdot_int32x2_lane16(int32x2_t a, int8x8_t b, int8x16_t c) noexcept {
  return dotprod_api::sdot_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x4_t native_sdot_int32x4(int32x4_t a, int8x16_t b, int8x16_t c) noexcept {
  return dotprod_api::sdot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x4_t native_sdot_int32x4_lane8(int32x4_t a, int8x16_t b, int8x8_t c) noexcept {
  return dotprod_api::sdot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x4_t native_sdot_int32x4_lane16(int32x4_t a, int8x16_t b, int8x16_t c) noexcept {
  return dotprod_api::sdot_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x2_t native_udot_uint32x2(uint32x2_t a, uint8x8_t b, uint8x8_t c) noexcept {
  return dotprod_api::udot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x2_t native_udot_uint32x2_lane8(uint32x2_t a, uint8x8_t b, uint8x8_t c) noexcept {
  return dotprod_api::udot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x2_t native_udot_uint32x2_lane16(uint32x2_t a, uint8x8_t b, uint8x16_t c) noexcept {
  return dotprod_api::udot_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x4_t native_udot_uint32x4(uint32x4_t a, uint8x16_t b, uint8x16_t c) noexcept {
  return dotprod_api::udot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x4_t native_udot_uint32x4_lane8(uint32x4_t a, uint8x16_t b, uint8x8_t c) noexcept {
  return dotprod_api::udot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x4_t native_udot_uint32x4_lane16(uint32x4_t a, uint8x16_t b, uint8x16_t c) noexcept {
  return dotprod_api::udot_lane<requirements, 3>(a, b, c);
}

// Same signatures and target attributes, with only the public simd layer removed.
extern "C" __attribute__((target("dotprod"), noinline))
int32x2_t raw_sdot_int32x2(int32x2_t a, int8x8_t b, int8x8_t c) noexcept {
  return native::detail::sdot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x2_t raw_sdot_int32x2_lane8(int32x2_t a, int8x8_t b, int8x8_t c) noexcept {
  return native::detail::sdot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x2_t raw_sdot_int32x2_lane16(int32x2_t a, int8x8_t b, int8x16_t c) noexcept {
  return native::detail::sdot_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x4_t raw_sdot_int32x4(int32x4_t a, int8x16_t b, int8x16_t c) noexcept {
  return native::detail::sdot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x4_t raw_sdot_int32x4_lane8(int32x4_t a, int8x16_t b, int8x8_t c) noexcept {
  return native::detail::sdot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
int32x4_t raw_sdot_int32x4_lane16(int32x4_t a, int8x16_t b, int8x16_t c) noexcept {
  return native::detail::sdot_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x2_t raw_udot_uint32x2(uint32x2_t a, uint8x8_t b, uint8x8_t c) noexcept {
  return native::detail::udot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x2_t raw_udot_uint32x2_lane8(uint32x2_t a, uint8x8_t b, uint8x8_t c) noexcept {
  return native::detail::udot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x2_t raw_udot_uint32x2_lane16(uint32x2_t a, uint8x8_t b, uint8x16_t c) noexcept {
  return native::detail::udot_lane<requirements, 3>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x4_t raw_udot_uint32x4(uint32x4_t a, uint8x16_t b, uint8x16_t c) noexcept {
  return native::detail::udot<requirements>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x4_t raw_udot_uint32x4_lane8(uint32x4_t a, uint8x16_t b, uint8x8_t c) noexcept {
  return native::detail::udot_lane<requirements, 1>(a, b, c);
}
extern "C" __attribute__((target("dotprod"), noinline))
uint32x4_t raw_udot_uint32x4_lane16(uint32x4_t a, uint8x16_t b, uint8x16_t c) noexcept {
  return native::detail::udot_lane<requirements, 3>(a, b, c);
}
