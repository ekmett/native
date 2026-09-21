// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <native/arm/i8mm.h>
import native.arm.i8mm;
#include "simd_adapter.h"
constexpr auto feature=native::isa<native::arm>(native::arm_feature::i8mm);
extern "C" {
__attribute__((target("i8mm"),noinline)) int32x4_t native_smmla(int32x4_t c,int8x16_t a,int8x16_t b) { return i8mm_api::smmla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) uint32x4_t native_ummla(uint32x4_t c,uint8x16_t a,uint8x16_t b) { return i8mm_api::ummla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_usmmla(int32x4_t c,uint8x16_t a,int8x16_t b) { return i8mm_api::usmmla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_usdot(int32x4_t c,uint8x16_t a,int8x16_t b) { return i8mm_api::usdot<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_usdot_lane(int32x4_t c,uint8x16_t a,int8x16_t b) { return i8mm_api::usdot_lane<feature,3>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_sudot_lane(int32x4_t c,int8x16_t a,uint8x16_t b) { return i8mm_api::sudot_lane<feature,3>(c,a,b); }
int native_baseline(int a,int b) { return a+b; }
__attribute__((target("i8mm"),noinline)) int32x4_t native_shape_0(int32x4_t acc, int8x16_t a, int8x16_t b) { return i8mm_api::smmla<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) uint32x4_t native_shape_1(uint32x4_t acc, uint8x16_t a, uint8x16_t b) { return i8mm_api::ummla<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_shape_2(int32x4_t acc, uint8x16_t a, int8x16_t b) { return i8mm_api::usmmla<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t native_shape_3(int32x2_t acc, uint8x8_t a, int8x8_t b) { return i8mm_api::usdot<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t native_shape_4(int32x2_t acc, uint8x8_t a, int8x8_t b) { return i8mm_api::usdot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t native_shape_5(int32x2_t acc, int8x8_t a, uint8x8_t b) { return i8mm_api::sudot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t native_shape_6(int32x2_t acc, uint8x8_t a, int8x16_t b) { return i8mm_api::usdot_lane<feature, 3>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t native_shape_7(int32x2_t acc, int8x8_t a, uint8x16_t b) { return i8mm_api::sudot_lane<feature, 3>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_shape_8(int32x4_t acc, uint8x16_t a, int8x16_t b) { return i8mm_api::usdot<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_shape_9(int32x4_t acc, uint8x16_t a, int8x8_t b) { return i8mm_api::usdot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_shape_10(int32x4_t acc, int8x16_t a, uint8x8_t b) { return i8mm_api::sudot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_shape_11(int32x4_t acc, uint8x16_t a, int8x16_t b) { return i8mm_api::usdot_lane<feature, 3>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_shape_12(int32x4_t acc, int8x16_t a, uint8x16_t b) { return i8mm_api::sudot_lane<feature, 3>(acc, a, b); }
}

// Same signatures and targets as the public leaves above.
extern "C" {

__attribute__((target("i8mm"),noinline)) int32x4_t raw_smmla(int32x4_t c,int8x16_t a,int8x16_t b) { return native::detail::smmla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) uint32x4_t raw_ummla(uint32x4_t c,uint8x16_t a,uint8x16_t b) { return native::detail::ummla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_usmmla(int32x4_t c,uint8x16_t a,int8x16_t b) { return native::detail::usmmla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_usdot(int32x4_t c,uint8x16_t a,int8x16_t b) { return native::detail::usdot<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_usdot_lane(int32x4_t c,uint8x16_t a,int8x16_t b) { return native::detail::usdot_lane<feature,3>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_sudot_lane(int32x4_t c,int8x16_t a,uint8x16_t b) { return native::detail::sudot_lane<feature,3>(c,a,b); }
int raw_baseline(int a,int b) { return a+b; }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_shape_0(int32x4_t acc, int8x16_t a, int8x16_t b) { return native::detail::smmla<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) uint32x4_t raw_shape_1(uint32x4_t acc, uint8x16_t a, uint8x16_t b) { return native::detail::ummla<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_shape_2(int32x4_t acc, uint8x16_t a, int8x16_t b) { return native::detail::usmmla<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t raw_shape_3(int32x2_t acc, uint8x8_t a, int8x8_t b) { return native::detail::usdot<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t raw_shape_4(int32x2_t acc, uint8x8_t a, int8x8_t b) { return native::detail::usdot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t raw_shape_5(int32x2_t acc, int8x8_t a, uint8x8_t b) { return native::detail::sudot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t raw_shape_6(int32x2_t acc, uint8x8_t a, int8x16_t b) { return native::detail::usdot_lane<feature, 3>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x2_t raw_shape_7(int32x2_t acc, int8x8_t a, uint8x16_t b) { return native::detail::sudot_lane<feature, 3>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_shape_8(int32x4_t acc, uint8x16_t a, int8x16_t b) { return native::detail::usdot<feature>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_shape_9(int32x4_t acc, uint8x16_t a, int8x8_t b) { return native::detail::usdot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_shape_10(int32x4_t acc, int8x16_t a, uint8x8_t b) { return native::detail::sudot_lane<feature, 1>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_shape_11(int32x4_t acc, uint8x16_t a, int8x16_t b) { return native::detail::usdot_lane<feature, 3>(acc, a, b); }
__attribute__((target("i8mm"),noinline)) int32x4_t raw_shape_12(int32x4_t acc, int8x16_t a, uint8x16_t b) { return native::detail::sudot_lane<feature, 3>(acc, a, b); }
}
