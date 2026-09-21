// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/dotprod.h>
// Deliberately lacks the function target even though the template has its ISA bit.
int32x4_t rejected(int32x4_t a, int8x16_t b, int8x16_t c) {
  return native::sdot<native::isa(native::arm_feature::dotprod)>(a, b, c);
}
