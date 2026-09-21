// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.pmull;
constexpr auto arch = native::neon;
__attribute__((target("aes"))) auto invalid(std::uint64_t a, std::uint64_t b) { return native::pmull<arch>(a, b); }
