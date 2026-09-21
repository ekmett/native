// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.fcma;

constexpr auto arch = native::feature_closure(native::arm_feature::complxnum);
native::simd<float, 4, arch> invalid(native::simd<float, 4, arch> c, native::simd<float, 4, arch> a, native::simd<float, 4, arch> b) {
  return native::fcmla<arch, 0>(c, a, b);
}
