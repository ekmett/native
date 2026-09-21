// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.pmull;
constexpr auto arch=native::feature_closure(native::isa<native::arm>{native::arm_feature::pmull});
__attribute__((target("aes"))) auto invalid(uint64x2_t a) { return native::pmull2<arch>(a,a); }
