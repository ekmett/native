// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.pmull;
constexpr auto arch = native::feature_closure(native::isa{native::arm_feature::neon});
auto invalid(native::simd<std::uint32_t, 4, arch> a, native::simd<std::uint8_t, 8, arch> b) { return native::pmull<native::feature_closure(native::isa{native::arm_feature::neon})>(a, b); }
