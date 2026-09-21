// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.aes;
constexpr auto arch=native::feature_closure(native::isa<native::arm>{native::arm_feature::aes});
__attribute__((target("aes"))) auto invalid(uint8x16_t a) { return native::aese<arch>(a,a); }
