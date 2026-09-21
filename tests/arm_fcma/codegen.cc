// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <arm_neon.h>
#include <native/arm/fcma.h>
import native.arm.fcma;
#include "simd_adapter.h"
constexpr native::isa<native::arm> arch{native::arm_feature::complxnum};
constexpr native::isa<native::arm> half_arch = [] { auto a = arch; a.set(native::arm_feature::neon_fp16, true); return a; }();
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t native_fcadd_32_2_90(float32x2_t a, float32x2_t b) { return fcma_api::fcadd<arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t native_fcadd_32_2_270(float32x2_t a, float32x2_t b) { return fcma_api::fcadd<arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t native_fcmla_32_2_0(float32x2_t acc, float32x2_t a, float32x2_t b) { return fcma_api::fcmla<arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t native_fcmla_32_2_90(float32x2_t acc, float32x2_t a, float32x2_t b) { return fcma_api::fcmla<arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t native_fcmla_32_2_180(float32x2_t acc, float32x2_t a, float32x2_t b) { return fcma_api::fcmla<arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t native_fcmla_32_2_270(float32x2_t acc, float32x2_t a, float32x2_t b) { return fcma_api::fcmla<arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t native_fcadd_32_4_90(float32x4_t a, float32x4_t b) { return fcma_api::fcadd<arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t native_fcadd_32_4_270(float32x4_t a, float32x4_t b) { return fcma_api::fcadd<arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t native_fcmla_32_4_0(float32x4_t acc, float32x4_t a, float32x4_t b) { return fcma_api::fcmla<arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t native_fcmla_32_4_90(float32x4_t acc, float32x4_t a, float32x4_t b) { return fcma_api::fcmla<arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t native_fcmla_32_4_180(float32x4_t acc, float32x4_t a, float32x4_t b) { return fcma_api::fcmla<arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t native_fcmla_32_4_270(float32x4_t acc, float32x4_t a, float32x4_t b) { return fcma_api::fcmla<arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t native_fcadd_64_2_90(float64x2_t a, float64x2_t b) { return fcma_api::fcadd<arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t native_fcadd_64_2_270(float64x2_t a, float64x2_t b) { return fcma_api::fcadd<arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t native_fcmla_64_2_0(float64x2_t acc, float64x2_t a, float64x2_t b) { return fcma_api::fcmla<arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t native_fcmla_64_2_90(float64x2_t acc, float64x2_t a, float64x2_t b) { return fcma_api::fcmla<arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t native_fcmla_64_2_180(float64x2_t acc, float64x2_t a, float64x2_t b) { return fcma_api::fcmla<arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t native_fcmla_64_2_270(float64x2_t acc, float64x2_t a, float64x2_t b) { return fcma_api::fcmla<arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t native_fcadd_16_4_90(float16x4_t a, float16x4_t b) { return fcma_api::fcadd<half_arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t native_fcadd_16_4_270(float16x4_t a, float16x4_t b) { return fcma_api::fcadd<half_arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t native_fcmla_16_4_0(float16x4_t acc, float16x4_t a, float16x4_t b) { return fcma_api::fcmla<half_arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t native_fcmla_16_4_90(float16x4_t acc, float16x4_t a, float16x4_t b) { return fcma_api::fcmla<half_arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t native_fcmla_16_4_180(float16x4_t acc, float16x4_t a, float16x4_t b) { return fcma_api::fcmla<half_arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t native_fcmla_16_4_270(float16x4_t acc, float16x4_t a, float16x4_t b) { return fcma_api::fcmla<half_arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t native_fcadd_16_8_90(float16x8_t a, float16x8_t b) { return fcma_api::fcadd<half_arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t native_fcadd_16_8_270(float16x8_t a, float16x8_t b) { return fcma_api::fcadd<half_arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t native_fcmla_16_8_0(float16x8_t acc, float16x8_t a, float16x8_t b) { return fcma_api::fcmla<half_arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t native_fcmla_16_8_90(float16x8_t acc, float16x8_t a, float16x8_t b) { return fcma_api::fcmla<half_arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t native_fcmla_16_8_180(float16x8_t acc, float16x8_t a, float16x8_t b) { return fcma_api::fcmla<half_arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t native_fcmla_16_8_270(float16x8_t acc, float16x8_t a, float16x8_t b) { return fcma_api::fcmla<half_arch, 270>(acc, a, b); }
extern "C" __attribute__((noinline)) unsigned native_baseline(unsigned x) { return x + 1; }

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from2_lane0_0(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from2_lane0_90(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from2_lane0_180(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from2_lane0_270(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane0_0(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane0_90(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane0_180(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane0_270(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane1_0(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 0, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane1_90(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 90, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane1_180(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 180, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t native_fcmla_32_2_from4_lane1_270(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 270, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from2_lane0_0(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from2_lane0_90(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from2_lane0_180(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from2_lane0_270(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return fcma_api::fcmla_lane<arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane0_0(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane0_90(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane0_180(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane0_270(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane1_0(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 0, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane1_90(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 90, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane1_180(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 180, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t native_fcmla_32_4_from4_lane1_270(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return fcma_api::fcmla_lane<arch, 270, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane0_0(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane0_90(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane0_180(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane0_270(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane1_0(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane1_90(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane1_180(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from4_lane1_270(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane0_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane0_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane0_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane0_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane1_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane1_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane1_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane1_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane2_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane2_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane2_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane2_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane3_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 3>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane3_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 3>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane3_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 3>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t native_fcmla_16_4_from8_lane3_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 3>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane0_0(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane0_90(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane0_180(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane0_270(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane1_0(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane1_90(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane1_180(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from4_lane1_270(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane0_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane0_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane0_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane0_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane1_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane1_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane1_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane1_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane2_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane2_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane2_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane2_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 2>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane3_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 0, 3>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane3_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 90, 3>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane3_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 180, 3>(acc, a, b);
}

extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t native_fcmla_16_8_from8_lane3_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return fcma_api::fcmla_lane<half_arch, 270, 3>(acc, a, b);
}

// Same signatures and target attributes, with only the public simd layer removed.
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t raw_fcadd_32_2_90(float32x2_t a, float32x2_t b) { return native::detail::fcadd<arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t raw_fcadd_32_2_270(float32x2_t a, float32x2_t b) { return native::detail::fcadd<arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t raw_fcmla_32_2_0(float32x2_t acc, float32x2_t a, float32x2_t b) { return native::detail::fcmla<arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t raw_fcmla_32_2_90(float32x2_t acc, float32x2_t a, float32x2_t b) { return native::detail::fcmla<arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t raw_fcmla_32_2_180(float32x2_t acc, float32x2_t a, float32x2_t b) { return native::detail::fcmla<arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x2_t raw_fcmla_32_2_270(float32x2_t acc, float32x2_t a, float32x2_t b) { return native::detail::fcmla<arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t raw_fcadd_32_4_90(float32x4_t a, float32x4_t b) { return native::detail::fcadd<arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t raw_fcadd_32_4_270(float32x4_t a, float32x4_t b) { return native::detail::fcadd<arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t raw_fcmla_32_4_0(float32x4_t acc, float32x4_t a, float32x4_t b) { return native::detail::fcmla<arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t raw_fcmla_32_4_90(float32x4_t acc, float32x4_t a, float32x4_t b) { return native::detail::fcmla<arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t raw_fcmla_32_4_180(float32x4_t acc, float32x4_t a, float32x4_t b) { return native::detail::fcmla<arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float32x4_t raw_fcmla_32_4_270(float32x4_t acc, float32x4_t a, float32x4_t b) { return native::detail::fcmla<arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t raw_fcadd_64_2_90(float64x2_t a, float64x2_t b) { return native::detail::fcadd<arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t raw_fcadd_64_2_270(float64x2_t a, float64x2_t b) { return native::detail::fcadd<arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t raw_fcmla_64_2_0(float64x2_t acc, float64x2_t a, float64x2_t b) { return native::detail::fcmla<arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t raw_fcmla_64_2_90(float64x2_t acc, float64x2_t a, float64x2_t b) { return native::detail::fcmla<arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t raw_fcmla_64_2_180(float64x2_t acc, float64x2_t a, float64x2_t b) { return native::detail::fcmla<arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum"), noinline)) float64x2_t raw_fcmla_64_2_270(float64x2_t acc, float64x2_t a, float64x2_t b) { return native::detail::fcmla<arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t raw_fcadd_16_4_90(float16x4_t a, float16x4_t b) { return native::detail::fcadd<half_arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t raw_fcadd_16_4_270(float16x4_t a, float16x4_t b) { return native::detail::fcadd<half_arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t raw_fcmla_16_4_0(float16x4_t acc, float16x4_t a, float16x4_t b) { return native::detail::fcmla<half_arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t raw_fcmla_16_4_90(float16x4_t acc, float16x4_t a, float16x4_t b) { return native::detail::fcmla<half_arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t raw_fcmla_16_4_180(float16x4_t acc, float16x4_t a, float16x4_t b) { return native::detail::fcmla<half_arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x4_t raw_fcmla_16_4_270(float16x4_t acc, float16x4_t a, float16x4_t b) { return native::detail::fcmla<half_arch, 270>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t raw_fcadd_16_8_90(float16x8_t a, float16x8_t b) { return native::detail::fcadd<half_arch, 90>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t raw_fcadd_16_8_270(float16x8_t a, float16x8_t b) { return native::detail::fcadd<half_arch, 270>(a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t raw_fcmla_16_8_0(float16x8_t acc, float16x8_t a, float16x8_t b) { return native::detail::fcmla<half_arch, 0>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t raw_fcmla_16_8_90(float16x8_t acc, float16x8_t a, float16x8_t b) { return native::detail::fcmla<half_arch, 90>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t raw_fcmla_16_8_180(float16x8_t acc, float16x8_t a, float16x8_t b) { return native::detail::fcmla<half_arch, 180>(acc, a, b); }
extern "C" __attribute__((target("complxnum,fullfp16"), noinline)) float16x8_t raw_fcmla_16_8_270(float16x8_t acc, float16x8_t a, float16x8_t b) { return native::detail::fcmla<half_arch, 270>(acc, a, b); }
extern "C" __attribute__((noinline)) unsigned raw_baseline(unsigned x) { return x + 1; }
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from2_lane0_0(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from2_lane0_90(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from2_lane0_180(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from2_lane0_270(float32x2_t acc, float32x2_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane0_0(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane0_90(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane0_180(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane0_270(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane1_0(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 0, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane1_90(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 90, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane1_180(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 180, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x2_t raw_fcmla_32_2_from4_lane1_270(float32x2_t acc, float32x2_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 270, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from2_lane0_0(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from2_lane0_90(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from2_lane0_180(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from2_lane0_270(float32x4_t acc, float32x4_t a, float32x2_t b) {
  return native::detail::fcmla_lane<arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane0_0(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane0_90(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane0_180(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane0_270(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane1_0(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 0, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane1_90(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 90, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane1_180(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 180, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum"), noinline))
float32x4_t raw_fcmla_32_4_from4_lane1_270(float32x4_t acc, float32x4_t a, float32x4_t b) {
  return native::detail::fcmla_lane<arch, 270, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane0_0(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane0_90(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane0_180(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane0_270(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane1_0(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane1_90(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane1_180(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from4_lane1_270(float16x4_t acc, float16x4_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane0_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane0_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane0_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane0_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane1_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane1_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane1_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane1_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane2_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane2_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane2_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane2_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane3_0(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 3>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane3_90(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 3>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane3_180(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 3>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x4_t raw_fcmla_16_4_from8_lane3_270(float16x4_t acc, float16x4_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 3>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane0_0(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane0_90(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane0_180(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane0_270(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane1_0(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane1_90(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane1_180(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from4_lane1_270(float16x8_t acc, float16x8_t a, float16x4_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane0_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane0_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane0_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane0_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 0>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane1_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane1_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane1_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane1_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 1>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane2_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane2_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane2_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane2_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 2>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane3_0(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 0, 3>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane3_90(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 90, 3>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane3_180(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 180, 3>(acc, a, b);
}
extern "C" __attribute__((target("complxnum,fullfp16"), noinline))
float16x8_t raw_fcmla_16_8_from8_lane3_270(float16x8_t acc, float16x8_t a, float16x8_t b) {
  return native::detail::fcmla_lane<half_arch, 270, 3>(acc, a, b);
}
