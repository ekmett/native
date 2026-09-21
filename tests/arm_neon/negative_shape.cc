// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.neon;
constexpr auto architecture = native::feature_closure(native::arm_feature::neon);
using vector = native::simd<std::int64_t, 2, architecture>;

vector rejected(vector a, vector b) {
  return native::sqdmulh(a, b);
}
