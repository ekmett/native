// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.bf16;
constexpr auto arch = native::feature_closure(native::isa<native::arm>{native::arm_feature::neon_bf16});
#if NATIVE_BF16_REJECT_CASE == 0
native::simd<float, 4, arch> bad(native::simd<float, 4, arch> c,native::simd<native::bf16, 8, arch> a) {return native::bfdot<arch>(c,a,a);}
#elif NATIVE_BF16_REJECT_CASE == 1
native::simd<float, 4, arch> bad(native::simd<float, 4, arch> c,native::simd<native::bf16, 8, arch> a) {return native::bfmmla<arch>(c,a,a);}
#elif NATIVE_BF16_REJECT_CASE == 2
native::simd<float, 4, arch> bad(native::simd<float, 4, arch> c,native::simd<native::bf16, 8, arch> a) {return native::bfmlalb<arch>(c,a,a);}
#elif NATIVE_BF16_REJECT_CASE >= 3 && NATIVE_BF16_REJECT_CASE <= 5
__attribute__((target("bf16"))) void bad(native::simd<float,4,native::neon> c,native::simd<native::bf16,8,native::neon> a) {
#if NATIVE_BF16_REJECT_CASE == 3
  (void)native::bfdot<native::neon>(c,a,a);
#elif NATIVE_BF16_REJECT_CASE == 4
  (void)native::bfmmla<native::neon>(c,a,a);
#else
  (void)native::bfmlalt<native::neon>(c,a,a);
#endif
}
#elif NATIVE_BF16_REJECT_CASE == 13
__attribute__((target("bf16"))) auto bad(float32x4_t c,bfloat16x8_t a) {return native::bfdot<arch>(c,a,a);}
#elif NATIVE_BF16_REJECT_CASE == 14
__attribute__((target("bf16"))) auto bad(native::simd<float,4,native::neon> c,native::simd<native::bf16,8,arch> a) {return native::bfdot<arch>(c,a,a);}
#else
__attribute__((target("bf16")))
void bad(native::simd<float, 2, arch> c2,native::simd<float, 4, arch> c4,native::simd<native::bf16, 4, arch> a4,native::simd<native::bf16, 8, arch> a8,native::simd<std::uint16_t, 8, arch> wrong) {
#if NATIVE_BF16_REJECT_CASE == 6
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
