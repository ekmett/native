// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.pmull;
constexpr auto arch = native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull});
auto invalid(std::uint64_t a, std::uint64_t b) { return native::pmull<native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull})>(a, b); }
