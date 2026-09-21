// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/fp16fml.h>
constexpr native::isa arch{native::arm_feature::fp16fml};

extern "C" __attribute__((target("fp16fml")))
void native_fmlal_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlal<arch>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlal_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = vfmlal_low_f16(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal_2_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlal_lane<arch, 3>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal_2_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlal_lane<arch, 7>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlal<arch>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlal_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = vfmlalq_low_f16(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal_4_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlal_lane<arch, 3>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal_4_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlal_lane<arch, 7>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal2_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlal2<arch>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlal2_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = vfmlal_high_f16(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal2_2_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlal2_lane<arch, 3>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal2_2_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlal2_lane<arch, 7>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal2_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlal2<arch>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlal2_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = vfmlalq_high_f16(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal2_4_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlal2_lane<arch, 3>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlal2_4_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlal2_lane<arch, 7>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlsl<arch>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlsl_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = vfmlsl_low_f16(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl_2_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlsl_lane<arch, 3>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl_2_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlsl_lane<arch, 7>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlsl<arch>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlsl_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = vfmlslq_low_f16(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl_4_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlsl_lane<arch, 3>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl_4_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlsl_lane<arch, 7>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl2_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlsl2<arch>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlsl2_2(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = vfmlsl_high_f16(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl2_2_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlsl2_lane<arch, 3>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl2_2_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1_f32(acc);
  auto x = vld1_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlsl2_lane<arch, 7>(c, x, y);
  vst1_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl2_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlsl2<arch>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void acle_fmlsl2_4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = vfmlslq_high_f16(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl2_4_from4(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1_f16(b);
  auto result = native::fmlsl2_lane<arch, 3>(c, x, y);
  vst1q_f32(output, result);
}

extern "C" __attribute__((target("fp16fml")))
void native_fmlsl2_4_from8(float* output, float const* acc, __fp16 const* a, __fp16 const* b) {
  auto c = vld1q_f32(acc);
  auto x = vld1q_f16(a); auto y = vld1q_f16(b);
  auto result = native::fmlsl2_lane<arch, 7>(c, x, y);
  vst1q_f32(output, result);
}
