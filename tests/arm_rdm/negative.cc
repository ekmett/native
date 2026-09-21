// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/rdm.h>
// Deliberately lacks the function target even though the template has its ISA bit.
int16x8_t rejected(int16x8_t a, int16x8_t b, int16x8_t c) {
  return native::sqrdmlah<native::isa(native::arm_feature::rdm)>(a, b, c);
}
