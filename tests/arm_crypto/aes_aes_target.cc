// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.aes;
constexpr auto arch = native::feature_closure(native::isa{native::arm_feature::aes});
auto invalid(native::simd<std::uint8_t, 16, arch> state, native::simd<std::uint8_t, 16, arch> round_key) { return native::aese<native::feature_closure(native::isa{native::arm_feature::aes})>(state, round_key); }
