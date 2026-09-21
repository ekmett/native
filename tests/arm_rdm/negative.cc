// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.rdm;
constexpr auto required = native::feature_closure(native::arm_feature::rdm);
// Deliberately lacks the function target even though each vector has its ISA bit.
auto rejected(native::simd<std::int16_t, 8, required> a) {
  return native::sqrdmlah<required>(a, a, a);
}
