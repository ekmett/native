// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import native;
using H=native::simd<native::fp16,8,native::neon_fp16>;
static_assert(sizeof(H)==16);
static_assert(std::same_as<decltype(native::fma(H{},H{},H{})),H>);
static_assert(std::same_as<decltype(H{}<H{}),H::mask>);
static_assert(std::same_as<decltype(H{}/H{}),H> && noexcept(H{}/H{}));
static_assert(std::same_as<decltype(sqrt(H{})),H> && noexcept(sqrt(H{})));
