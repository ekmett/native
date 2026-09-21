// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/fcma.h>
constexpr native::isa arch = native::arm_feature::complxnum & native::arm_feature::neon_fp16;
__attribute__((target("complxnum,fullfp16")))
auto invalid_short(float32x2_t c, float32x2_t a, float32x2_t b) {
  return native::fcmla_lane<arch, 0, 1>(c, a, b);
}
__attribute__((target("complxnum,fullfp16")))
auto invalid_full(float32x4_t c, float32x4_t a, float32x4_t b) {
  return native::fcmla_lane<arch, 0, 2>(c, a, b);
}
