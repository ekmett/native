// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/sha.h>
__attribute__((target("sha2"))) auto invalid(uint32x4_t abcd, uint32x4_t efgh, uint32x4_t wk) { return native::sha256h<native::scalar>(abcd, efgh, wk); }
