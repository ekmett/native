// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/fcma.h>
constexpr native::isa arch{native::arm_feature::complxnum};
constexpr native::isa half_arch = [] { auto a = arch; a.set(native::arm_feature::neon_fp16, true); return a; }();

extern "C" __attribute__((target("complxnum")))
void native_add_32_2(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1_f32(a), vb = vld1_f32(b), vc = vld1_f32(acc);
  vst1_f32(output, (native::detail::fcadd<arch, 90>(va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_add_32_2(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1_f32(a), vb = vld1_f32(b), vc = vld1_f32(acc);
  vst1_f32(output, vcadd_rot90_f32(va, vb));
}

extern "C" __attribute__((target("complxnum")))
void native_mla_32_2(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1_f32(a), vb = vld1_f32(b), vc = vld1_f32(acc);
  vst1_f32(output, (native::detail::fcmla<arch, 90>(vc, va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_mla_32_2(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1_f32(a), vb = vld1_f32(b), vc = vld1_f32(acc);
  vst1_f32(output, vcmla_rot90_f32(vc, va, vb));
}

extern "C" __attribute__((target("complxnum")))
void native_lane_32_2_from2(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1_f32(a), vc = vld1_f32(acc);
  auto vb = vld1_f32(b);
  vst1_f32(output, (native::detail::fcmla_lane<arch, 270, 0>(vc, va, vb)));
}

extern "C" __attribute__((target("complxnum")))
void native_lane_32_2_from4(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1_f32(a), vc = vld1_f32(acc);
  auto vb = vld1q_f32(b);
  vst1_f32(output, (native::detail::fcmla_lane<arch, 270, 1>(vc, va, vb)));
}

extern "C" __attribute__((target("complxnum")))
void native_add_32_4(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1q_f32(a), vb = vld1q_f32(b), vc = vld1q_f32(acc);
  vst1q_f32(output, (native::detail::fcadd<arch, 90>(va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_add_32_4(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1q_f32(a), vb = vld1q_f32(b), vc = vld1q_f32(acc);
  vst1q_f32(output, vcaddq_rot90_f32(va, vb));
}

extern "C" __attribute__((target("complxnum")))
void native_mla_32_4(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1q_f32(a), vb = vld1q_f32(b), vc = vld1q_f32(acc);
  vst1q_f32(output, (native::detail::fcmla<arch, 90>(vc, va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_mla_32_4(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1q_f32(a), vb = vld1q_f32(b), vc = vld1q_f32(acc);
  vst1q_f32(output, vcmlaq_rot90_f32(vc, va, vb));
}

extern "C" __attribute__((target("complxnum")))
void native_lane_32_4_from2(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1q_f32(a), vc = vld1q_f32(acc);
  auto vb = vld1_f32(b);
  vst1q_f32(output, (native::detail::fcmla_lane<arch, 270, 0>(vc, va, vb)));
}

extern "C" __attribute__((target("complxnum")))
void native_lane_32_4_from4(float* output, float const* acc, float const* a, float const* b) {
  auto va = vld1q_f32(a), vc = vld1q_f32(acc);
  auto vb = vld1q_f32(b);
  vst1q_f32(output, (native::detail::fcmla_lane<arch, 270, 1>(vc, va, vb)));
}

extern "C" __attribute__((target("complxnum")))
void native_add_64_2(double* output, double const* acc, double const* a, double const* b) {
  auto va = vld1q_f64(a), vb = vld1q_f64(b), vc = vld1q_f64(acc);
  vst1q_f64(output, (native::detail::fcadd<arch, 90>(va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_add_64_2(double* output, double const* acc, double const* a, double const* b) {
  auto va = vld1q_f64(a), vb = vld1q_f64(b), vc = vld1q_f64(acc);
  vst1q_f64(output, vcaddq_rot90_f64(va, vb));
}

extern "C" __attribute__((target("complxnum")))
void native_mla_64_2(double* output, double const* acc, double const* a, double const* b) {
  auto va = vld1q_f64(a), vb = vld1q_f64(b), vc = vld1q_f64(acc);
  vst1q_f64(output, (native::detail::fcmla<arch, 90>(vc, va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_mla_64_2(double* output, double const* acc, double const* a, double const* b) {
  auto va = vld1q_f64(a), vb = vld1q_f64(b), vc = vld1q_f64(acc);
  vst1q_f64(output, vcmlaq_rot90_f64(vc, va, vb));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_add_16_4(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1_f16(a), vb = vld1_f16(b), vc = vld1_f16(acc);
  vst1_f16(output, (native::detail::fcadd<half_arch, 90>(va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_add_16_4(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1_f16(a), vb = vld1_f16(b), vc = vld1_f16(acc);
  vst1_f16(output, vcadd_rot90_f16(va, vb));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_mla_16_4(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1_f16(a), vb = vld1_f16(b), vc = vld1_f16(acc);
  vst1_f16(output, (native::detail::fcmla<half_arch, 90>(vc, va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_mla_16_4(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1_f16(a), vb = vld1_f16(b), vc = vld1_f16(acc);
  vst1_f16(output, vcmla_rot90_f16(vc, va, vb));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_lane_16_4_from4(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1_f16(a), vc = vld1_f16(acc);
  auto vb = vld1_f16(b);
  vst1_f16(output, (native::detail::fcmla_lane<half_arch, 270, 1>(vc, va, vb)));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_lane_16_4_from8(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1_f16(a), vc = vld1_f16(acc);
  auto vb = vld1q_f16(b);
  vst1_f16(output, (native::detail::fcmla_lane<half_arch, 270, 3>(vc, va, vb)));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_add_16_8(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1q_f16(a), vb = vld1q_f16(b), vc = vld1q_f16(acc);
  vst1q_f16(output, (native::detail::fcadd<half_arch, 90>(va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_add_16_8(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1q_f16(a), vb = vld1q_f16(b), vc = vld1q_f16(acc);
  vst1q_f16(output, vcaddq_rot90_f16(va, vb));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_mla_16_8(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1q_f16(a), vb = vld1q_f16(b), vc = vld1q_f16(acc);
  vst1q_f16(output, (native::detail::fcmla<half_arch, 90>(vc, va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_mla_16_8(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1q_f16(a), vb = vld1q_f16(b), vc = vld1q_f16(acc);
  vst1q_f16(output, vcmlaq_rot90_f16(vc, va, vb));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_lane_16_8_from4(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1q_f16(a), vc = vld1q_f16(acc);
  auto vb = vld1_f16(b);
  vst1q_f16(output, (native::detail::fcmla_lane<half_arch, 270, 1>(vc, va, vb)));
}

extern "C" __attribute__((target("complxnum,fullfp16")))
void native_lane_16_8_from8(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1q_f16(a), vc = vld1q_f16(acc);
  auto vb = vld1q_f16(b);
  vst1q_f16(output, (native::detail::fcmla_lane<half_arch, 270, 3>(vc, va, vb)));
}

extern "C" __attribute__((target("v8.3a,fullfp16")))
void acle_lane_16_8_from8(__fp16* output, __fp16 const* acc, __fp16 const* a, __fp16 const* b) {
  auto va = vld1q_f16(a), vc = vld1q_f16(acc);
  auto vb = vld1q_f16(b);
  vst1q_f16(output, vcmlaq_rot270_laneq_f16(vc, va, vb, 3));
}
