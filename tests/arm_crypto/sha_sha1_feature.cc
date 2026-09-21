// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.sha;
constexpr auto arch = native::neon;
__attribute__((target("sha2"))) auto invalid(native::simd<std::uint32_t, 4, arch> abcd, std::uint32_t e, native::simd<std::uint32_t, 4, arch> wk) { return native::sha1c<arch>(abcd, e, wk); }
