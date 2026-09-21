// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.dotprod;
constexpr auto required = native::feature_closure(native::arm_feature::dotprod);
// Deliberately lacks the function target even though each vector has its ISA bit.
auto rejected(native::simd<std::int32_t, 4, required> a,
    native::simd<std::int8_t, 16, required> b) {
  return native::sdot<required>(a, b, b);
}
