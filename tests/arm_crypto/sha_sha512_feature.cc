// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/sha.h>
__attribute__((target("sha3"))) auto invalid(uint64x2_t ed, uint64x2_t gf, uint64x2_t kwh_kwh2) { return native::sha512h<native::scalar>(ed, gf, kwh_kwh2); }
