// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/bf16.h>
constexpr native::isa arch{native::arm_feature::neon_bf16};
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_0_0(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) {
  return native::bfdot<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_4_0(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) {
  return native::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_4_1(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) {
  return native::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_0(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_1(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_2(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_3(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_0_0(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfdot<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_4_0(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_4_1(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_0(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_1(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_2(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_3(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfdot_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmmla_4_0_0(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmmla<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_0_0(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_0(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalb_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_1(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalb_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_2(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalb_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_3(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalb_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_0(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_1(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_2(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_3(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_4(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,4>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_5(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,5>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_6(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,6>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_7(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalb_lane<arch,7>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_0_0(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_0(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalt_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_1(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalt_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_2(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalt_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_3(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::bfmlalt_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_0(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_1(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_2(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_3(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_4(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,4>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_5(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,5>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_6(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,6>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_7(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::bfmlalt_lane<arch,7>(acc,a,b);
}

extern "C" unsigned native_baseline(unsigned a,unsigned b) {return a+b;}
#pragma STDC FENV_ACCESS ON
extern "C" __attribute__((target("bf16"),noinline))
void native_discard(float32x4_t c,bfloat16x8_t a,bfloat16x8_t b) {
  (void)native::bfdot<arch>(c,a,b); (void)native::bfmmla<arch>(c,a,b);
  (void)native::bfmlalb<arch>(c,a,b); (void)native::bfmlalt<arch>(c,a,b);
}
extern "C" __attribute__((target("bf16"),noinline))
float32x4_t native_environment(float32x4_t c,bfloat16x8_t a,bfloat16x8_t b,unsigned long control) {
  auto x=native::bfdot<arch>(c,a,b);
  asm volatile("msr fpcr, %0" :: "r"(control) : "memory");
  auto y=native::bfdot<arch>(c,a,b);
  return x-y;
}
