// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/sha.h>
__attribute__((target("sha2"))) auto invalid(uint32x4_t abcd, std::uint32_t e, uint32x4_t wk) { return native::sha1c<native::scalar>(abcd, e, wk); }
