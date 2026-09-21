// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/pmull.h>
auto invalid(uint32x4_t a, poly8x8_t b) { return native::pmull<native::isa{native::arm_feature::neon}>(a, b); }
