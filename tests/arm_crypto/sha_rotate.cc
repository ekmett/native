// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/sha.h>
__attribute__((target("sha3"))) auto invalid(uint64x2_t a) { return native::xar<native::isa{native::arm_feature::sha3}, 64>(a,a); }
