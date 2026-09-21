// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_0_0_raw(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfdot<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_4_0_raw(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_4_1_raw(float32x2_t acc, bfloat16x4_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_0_raw(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_1_raw(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_2_raw(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x2_t native_bfdot_2_8_3_raw(float32x2_t acc, bfloat16x4_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_0_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_4_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_4_1_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_1_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_2_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfdot_4_8_3_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfdot_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmmla_4_0_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmmla<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_0_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_1_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_2_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_4_3_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_1_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_2_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_3_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_4_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,4>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_5_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,5>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_6_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,6>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalb_4_8_7_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalb_lane<arch,7>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_0_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt<arch>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_1_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_2_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_4_3_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x4_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_0_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,0>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_1_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,1>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_2_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,2>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_3_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,3>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_4_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,4>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_5_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,5>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_6_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,6>(acc,a,b);
}
extern "C" __attribute__((target("bf16"),noinline)) float32x4_t native_bfmlalt_4_8_7_raw(float32x4_t acc, bfloat16x8_t a, bfloat16x8_t b) {
  return native::detail::arm_bf16::bfmlalt_lane<arch,7>(acc,a,b);
}

extern "C" unsigned native_baseline_raw(unsigned a,unsigned b) {return a+b;}
#pragma STDC FENV_ACCESS ON
extern "C" __attribute__((target("bf16"),noinline))
void native_discard_raw(float32x4_t c,bfloat16x8_t a,bfloat16x8_t b) {
  (void)native::detail::arm_bf16::bfdot<arch>(c,a,b); (void)native::detail::arm_bf16::bfmmla<arch>(c,a,b);
  (void)native::detail::arm_bf16::bfmlalb<arch>(c,a,b); (void)native::detail::arm_bf16::bfmlalt<arch>(c,a,b);
}
extern "C" __attribute__((target("bf16"),noinline))
float32x4_t native_environment_raw(float32x4_t c,bfloat16x8_t a,bfloat16x8_t b,unsigned long control) {
  auto x=native::detail::arm_bf16::bfdot<arch>(c,a,b);
  asm volatile("msr fpcr, %0" :: "r"(control) : "memory");
  auto y=native::detail::arm_bf16::bfdot<arch>(c,a,b);
  return x-y;
}
