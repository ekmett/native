// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/i8mm.h>
constexpr auto feature=native::isa(native::arm_feature::i8mm);
extern "C" {
__attribute__((target("i8mm"),noinline)) int32x4_t native_smmla(int32x4_t c,int8x16_t a,int8x16_t b) { return native::smmla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) uint32x4_t native_ummla(uint32x4_t c,uint8x16_t a,uint8x16_t b) { return native::ummla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_usmmla(int32x4_t c,uint8x16_t a,int8x16_t b) { return native::usmmla<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_usdot(int32x4_t c,uint8x16_t a,int8x16_t b) { return native::usdot<feature>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_usdot_lane(int32x4_t c,uint8x16_t a,int8x16_t b) { return native::usdot_lane<feature,3>(c,a,b); }
__attribute__((target("i8mm"),noinline)) int32x4_t native_sudot_lane(int32x4_t c,int8x16_t a,uint8x16_t b) { return native::sudot_lane<feature,3>(c,a,b); }
int native_baseline(int a,int b) { return a+b; }
}
