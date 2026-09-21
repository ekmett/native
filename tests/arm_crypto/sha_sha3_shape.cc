// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/sha.h>
__attribute__((target("sha3"))) auto invalid(uint32x4_t a, uint8x16_t b, uint8x16_t c) { return native::eor3<native::isa{native::arm_feature::sha3}>(a, b, c); }
