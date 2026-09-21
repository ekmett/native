// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/pmull.h>
__attribute__((target("aes"))) auto invalid(double a, poly64_t b) { return native::pmull<native::isa{native::arm_feature::pmull}>(a, b); }
