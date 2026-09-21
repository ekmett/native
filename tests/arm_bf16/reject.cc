// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/bf16.h>
constexpr native::isa arch{native::arm_feature::neon_bf16};
#if NATIVE_BF16_REJECT_CASE == 0
float32x4_t bad(float32x4_t c,bfloat16x8_t a) {return native::bfdot<arch>(c,a,a);}
#elif NATIVE_BF16_REJECT_CASE == 1
float32x4_t bad(float32x4_t c,bfloat16x8_t a) {return native::bfmmla<arch>(c,a,a);}
#elif NATIVE_BF16_REJECT_CASE == 2
float32x4_t bad(float32x4_t c,bfloat16x8_t a) {return native::bfmlalb<arch>(c,a,a);}
#else
__attribute__((target("bf16")))
void bad(float32x2_t c2,float32x4_t c4,bfloat16x4_t a4,bfloat16x8_t a8,float16x8_t wrong) {
#if NATIVE_BF16_REJECT_CASE == 3
  (void)native::bfdot<native::neon>(c4,a8,a8);
#elif NATIVE_BF16_REJECT_CASE == 4
  (void)native::bfmmla<native::neon>(c4,a8,a8);
#elif NATIVE_BF16_REJECT_CASE == 5
  (void)native::bfmlalt<native::neon>(c4,a8,a8);
#elif NATIVE_BF16_REJECT_CASE == 6
  (void)native::bfdot_lane<arch,2>(c2,a4,a4);
#elif NATIVE_BF16_REJECT_CASE == 7
  (void)native::bfdot_lane<arch,4>(c4,a8,a8);
#elif NATIVE_BF16_REJECT_CASE == 8
  (void)native::bfmlalb_lane<arch,4>(c4,a8,a4);
#elif NATIVE_BF16_REJECT_CASE == 9
  (void)native::bfmlalt_lane<arch,8>(c4,a8,a8);
#elif NATIVE_BF16_REJECT_CASE == 10
  (void)native::bfdot<arch>(c4,wrong,wrong);
#elif NATIVE_BF16_REJECT_CASE == 11
  (void)native::bfmmla<arch>(c4,wrong,wrong);
#elif NATIVE_BF16_REJECT_CASE == 12
  (void)native::bfmlalt<arch>(c4,wrong,wrong);
#endif
}
#endif
