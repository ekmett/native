// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.sha;
constexpr auto arch = native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2});
auto invalid(native::simd<std::uint32_t, 4, arch> abcd, native::simd<std::uint32_t, 4, arch> efgh, native::simd<std::uint32_t, 4, arch> wk) { return native::sha256h<native::feature_closure(native::isa<native::arm>{native::arm_feature::sha2})>(abcd, efgh, wk); }
