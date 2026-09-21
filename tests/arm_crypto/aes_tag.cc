// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.aes;
constexpr auto arch=native::feature_closure(native::isa<native::arm>{native::arm_feature::aes});
__attribute__((target("aes"))) auto invalid(native::simd<std::uint8_t,16,arch> a, native::simd<std::uint8_t,16,native::neon> b) { return native::aese<arch>(a,b); }
