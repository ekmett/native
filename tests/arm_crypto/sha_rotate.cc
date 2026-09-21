// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.sha;
constexpr auto arch = native::feature_closure(native::isa{native::arm_feature::sha3});
__attribute__((target("sha3"))) auto invalid(native::simd<std::uint64_t, 2, arch> a) { return native::xar<native::feature_closure(native::isa{native::arm_feature::sha3}), 64>(a,a); }
