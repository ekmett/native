// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.aes;
constexpr auto arch = native::neon;
__attribute__((target("aes"))) auto invalid(native::simd<std::uint8_t, 16, arch> state, native::simd<std::uint8_t, 16, arch> round_key) { return native::aese<arch>(state, round_key); }
