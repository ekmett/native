// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.fcma;

constexpr auto arch = native::feature_closure(native::arm_feature::complxnum);
__attribute__((target("complxnum,fullfp16")))
auto invalid(native::simd<native::fp16, 8, arch> c, native::simd<native::fp16, 8, arch> a, native::simd<native::fp16, 8, arch> b) {
  return native::fcmla_lane<arch, 0, 0>(c, a, b);
}
