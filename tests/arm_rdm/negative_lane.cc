// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/rdm.h>
constexpr auto requirements = native::isa(native::arm_feature::rdm);
__attribute__((target("rdm")))
auto rejected(int32x2_t value) {
  return native::sqrdmlah_lane<requirements, 2>(value, value, value);
}
__attribute__((target("rdm")))
auto rejected(int32x4_t value) {
  return native::sqrdmlsh_lane<requirements, 4>(value, value, value);
}
__attribute__((target("rdm")))
auto rejected(int32_t value, int32x2_t rhs) {
  return native::sqrdmlah_lane<requirements, 2>(value, value, rhs);
}
