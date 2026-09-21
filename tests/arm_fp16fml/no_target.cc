// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/fp16fml.h>
constexpr native::isa arch{native::arm_feature::fp16fml};
float32x4_t invalid(float32x4_t c, float16x8_t a, float16x8_t b) {
  return native::fmlal<arch>(c, a, b);
}
