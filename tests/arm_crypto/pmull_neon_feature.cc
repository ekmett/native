// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.pmull;
constexpr auto arch = native::scalar;
auto invalid(native::simd<std::uint8_t, 8, arch> const & a, native::simd<std::uint8_t, 8, arch> const & b) { return native::pmull<arch>(a, b); }
