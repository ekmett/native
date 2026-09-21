// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/fcma.h>
constexpr native::isa arch = native::arm_feature::complxnum & native::arm_feature::neon_fp16;
__attribute__((target("complxnum")))
float16x8_t invalid(float16x8_t c, float16x8_t a, float16x8_t b) {
  return native::fcmla<arch, 0>(c, a, b);
}
