// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.sha;
constexpr auto arch = native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3});
auto invalid(native::simd<std::uint8_t, 16, arch> a, native::simd<std::uint8_t, 16, arch> b, native::simd<std::uint8_t, 16, arch> c) { return native::eor3<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha3})>(a, b, c); }
