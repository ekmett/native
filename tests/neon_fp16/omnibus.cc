// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import simd;
using H=simd::vec<simd::fp16,8,simd::neon_fp16>;
static_assert(sizeof(H)==16);
static_assert(std::same_as<decltype(simd::fma(H{},H{},H{})),H>);
static_assert(std::same_as<decltype(H{}<H{}),H::mask>);
