// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/bf16.h>
constexpr native::isa arch{native::arm_feature::neon_bf16};
extern "C" __attribute__((target("bf16")))
void native_bfdot_2_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1_bf16(b);
  vst1_f32(out,(native::bfdot<arch>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void acle_bfdot_2_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1_bf16(b);
  vst1_f32(out,(vbfdot_f32(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_2_4_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1_bf16(b);
  vst1_f32(out,(native::bfdot_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_2_4_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1_bf16(b);
  vst1_f32(out,(native::bfdot_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_2_8_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1_f32(out,(native::bfdot_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_2_8_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1_f32(out,(native::bfdot_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_2_8_2(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1_f32(out,(native::bfdot_lane<arch,2>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_2_8_3(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1_f32(acc);
  auto va=vld1_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1_f32(out,(native::bfdot_lane<arch,3>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfdot<arch>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void acle_bfdot_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(vbfdotq_f32(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_4_4_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfdot_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_4_4_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfdot_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_4_8_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfdot_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_4_8_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfdot_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_4_8_2(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfdot_lane<arch,2>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfdot_4_8_3(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfdot_lane<arch,3>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmmla_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmmla<arch>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void acle_bfmmla_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(vbfmmlaq_f32(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb<arch>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void acle_bfmlalb_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(vbfmlalbq_f32(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_4_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_4_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_4_2(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,2>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_4_3(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,3>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_2(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,2>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_3(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,3>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_4(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,4>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_5(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,5>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_6(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,6>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalb_4_8_7(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalb_lane<arch,7>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt<arch>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void acle_bfmlalt_4_0_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(vbfmlaltq_f32(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_4_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_4_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_4_2(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,2>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_4_3(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,3>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_0(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,0>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_1(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,1>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_2(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,2>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_3(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,3>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_4(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,4>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_5(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,5>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_6(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,6>(vc,va,vb)));
}
extern "C" __attribute__((target("bf16")))
void native_bfmlalt_4_8_7(float* out,float const* acc,bfloat16_t const* a,bfloat16_t const* b) {
  auto vc=vld1q_f32(acc);
  auto va=vld1q_bf16(a);
  auto vb=vld1q_bf16(b);
  vst1q_f32(out,(native::bfmlalt_lane<arch,7>(vc,va,vb)));
}
