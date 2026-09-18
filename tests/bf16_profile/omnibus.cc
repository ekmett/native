// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import simd;
using B=simd::vec<simd::bf16,32,simd::avx512_bf16>;
using F=simd::vec<float,16,simd::avx512_bf16>;
static_assert(sizeof(B)==64);
static_assert(std::same_as<decltype(simd::dot2(B{},B{},F{})),F>);
