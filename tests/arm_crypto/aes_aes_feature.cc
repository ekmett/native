// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/aes.h>
__attribute__((target("aes"))) auto invalid(uint8x16_t state, uint8x16_t round_key) { return native::aese<native::scalar>(state, round_key); }
