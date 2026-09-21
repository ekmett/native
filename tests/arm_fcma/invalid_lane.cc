// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.fcma;

constexpr auto arch = native::feature_closure(native::arm_feature::complxnum & native::arm_feature::neon_fp16);
__attribute__((target("complxnum,fullfp16")))
auto invalid_short(native::simd<float, 2, arch> c, native::simd<float, 2, arch> a, native::simd<float, 2, arch> b) {
  return native::fcmla_lane<arch, 0, 1>(c, a, b);
}
__attribute__((target("complxnum,fullfp16")))
auto invalid_full(native::simd<float, 4, arch> c, native::simd<float, 4, arch> a, native::simd<float, 4, arch> b) {
  return native::fcmla_lane<arch, 0, 2>(c, a, b);
}
