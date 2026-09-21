// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.sha;
constexpr auto arch = native::feature_closure(native::isa{native::arm_feature::sha512});
__attribute__((target("sha3"))) auto invalid(native::simd<std::uint32_t, 4, arch> ed, native::simd<std::uint64_t, 2, arch> gf, native::simd<std::uint64_t, 2, arch> kwh_kwh2) { return native::sha512h<native::feature_closure(native::isa{native::arm_feature::sha512})>(ed, gf, kwh_kwh2); }
