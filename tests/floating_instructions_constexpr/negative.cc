// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.fp16fml;
import native.arm.fcma;
import native.arm.bf16;
constexpr auto weak=native::neon;
using f=native::simd<float,4,weak>;
using h=native::simd<native::fp16,8,weak>;
using b=native::simd<native::bf16,8,weak>;
using d=native::simd<double,2,weak>;
// Enabling all instructions in the compiler does not admit runtime calls whose
// architecture tag lacks the operation's required feature.
__attribute__((target("fp16fml,complxnum,fullfp16,bf16")))
void rejected(f c,h x,h y,b p,b q,d u,d v) {
#if NATIVE_FLOAT_REJECT == 0
  (void)native::fmlal<weak>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 1
  (void)native::fmlal2<weak>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 2
  (void)native::fmlsl<weak>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 3
  (void)native::fmlsl2<weak>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 4
  (void)native::fmlal_lane<weak,7>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 5
  (void)native::fmlal2_lane<weak,7>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 6
  (void)native::fmlsl_lane<weak,7>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 7
  (void)native::fmlsl2_lane<weak,7>(c,x,y);
#elif NATIVE_FLOAT_REJECT == 8
  (void)native::fcadd<weak,90>(c,c);
#elif NATIVE_FLOAT_REJECT == 9
  (void)native::fcmla<weak,270>(c,c,c);
#elif NATIVE_FLOAT_REJECT == 10
  (void)native::fcmla_lane<weak,90,1>(c,c,c);
#elif NATIVE_FLOAT_REJECT == 11
  (void)native::fcadd<weak,270>(x,y);
#elif NATIVE_FLOAT_REJECT == 12
  (void)native::fcmla<weak,180>(u,u,v);
#elif NATIVE_FLOAT_REJECT == 13
  (void)native::bfdot<weak>(c,p,q);
#elif NATIVE_FLOAT_REJECT == 14
  (void)native::bfmmla<weak>(c,p,q);
#elif NATIVE_FLOAT_REJECT == 15
  (void)native::bfmlalb<weak>(c,p,q);
#elif NATIVE_FLOAT_REJECT == 16
  (void)native::bfmlalt<weak>(c,p,q);
#elif NATIVE_FLOAT_REJECT == 17
  (void)native::bfdot_lane<weak,3>(c,p,q);
#elif NATIVE_FLOAT_REJECT == 18
  (void)native::bfmlalb_lane<weak,7>(c,p,q);
#elif NATIVE_FLOAT_REJECT == 19
  (void)native::bfmlalt_lane<weak,7>(c,p,q);
#else
#error Select a weak-runtime rejection
#endif
}
