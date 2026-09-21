// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.neon;
constexpr native::isa<native::arm> architecture{};
using vector = native::simd<std::int64_t, 1, architecture>;

vector rejected(vector a, vector b) {
  return native::sqadd(a, b);
}
