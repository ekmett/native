// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.sha;
constexpr auto arch = native::feature_closure(native::isa{native::arm_feature::sha1});
__attribute__((target("sha2"))) auto invalid(native::simd<std::uint8_t, 16, arch> abcd, std::uint32_t e, native::simd<std::uint32_t, 4, arch> wk) { return native::sha1c<native::feature_closure(native::isa{native::arm_feature::sha1})>(abcd, e, wk); }
