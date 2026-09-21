// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/fcma.h>
constexpr native::isa arch{native::arm_feature::complxnum};
float32x4_t invalid(float32x4_t c, float32x4_t a, float32x4_t b) {
  return native::fcmla<arch, 0>(c, a, b);
}
