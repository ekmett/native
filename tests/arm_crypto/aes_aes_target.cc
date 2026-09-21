// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/aes.h>
auto invalid(uint8x16_t state, uint8x16_t round_key) { return native::aese<native::isa{native::arm_feature::aes}>(state, round_key); }
