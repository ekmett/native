// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.fp16fml;

constexpr auto arch = native::feature_closure(native::arm_feature::fp16fml);
native::simd<float, 4, arch> invalid(native::simd<float, 4, arch> c, native::simd<native::fp16, 8, arch> a, native::simd<native::fp16, 8, arch> b) {
  return native::fmlal<arch>(c, a, b);
}
