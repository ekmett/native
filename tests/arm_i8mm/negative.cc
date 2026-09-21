// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/i8mm.h>
#if defined(TEST_NO_FEATURE)
__attribute__((target("i8mm")))
int32x4_t rejected(int32x4_t c,int8x16_t a) { return native::smmla<native::neon>(c,a,a); }
#elif defined(TEST_BAD_LANE)
__attribute__((target("i8mm")))
int32x4_t rejected(int32x4_t c,uint8x16_t a,int8x8_t b) { return native::usdot_lane<native::isa(native::arm_feature::i8mm),2>(c,a,b); }
#else
int32x4_t rejected(int32x4_t c,int8x16_t a) { return native::smmla<native::isa(native::arm_feature::i8mm)>(c,a,a); }
#endif
