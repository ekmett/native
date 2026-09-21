// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/fcma.h>
constexpr native::isa arch{native::arm_feature::complxnum};
__attribute__((target("complxnum,fullfp16")))
auto invalid(float16x8_t c, float16x8_t a, float16x8_t b) {
  return native::fcmla_lane<arch, 0, 0>(c, a, b);
}
