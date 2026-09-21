// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.rdm;

constexpr auto requirements = native::feature_closure(native::arm_feature::rdm);
__attribute__((target("rdm")))
auto rejected(native::simd<std::int32_t, 2, requirements> value) {
  return native::sqrdmlah_lane<requirements, 2>(value, value, value);
}
__attribute__((target("rdm")))
auto rejected(native::simd<std::int32_t, 4, requirements> value) {
  return native::sqrdmlsh_lane<requirements, 4>(value, value, value);
}
__attribute__((target("rdm")))
auto rejected(int32_t value, native::simd<std::int32_t, 2, requirements> rhs) {
  return native::sqrdmlah_lane<requirements, 2>(value, value, rhs);
}
