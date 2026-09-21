// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.neon;
constexpr auto architecture = native::feature_closure(native::arm_feature::neon);
using vector = native::simd<std::int64_t, 1, architecture>;

__attribute__((target("cpu=generic,arch=armv8-a+nosimd"))) vector rejected(vector a, vector b) {
  return native::sqadd(a, b);
}
