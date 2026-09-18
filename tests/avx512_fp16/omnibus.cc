// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import simd;
using H=simd::vec<simd::fp16,32,simd::avx512_fp16>;
static_assert(sizeof(H)==64);
#if TEST_OMNIBUS_BF16
using B=simd::vec<simd::bf16,32,simd::avx512_bf16>;
static_assert(sizeof(B)==64);
static_assert(!std::same_as<H,B>);
static_assert(std::same_as<decltype(simd::dot2(B{},B{},simd::vec<float,16,simd::avx512_bf16>{})),simd::vec<float,16,simd::avx512_bf16>>);
#endif
static_assert(std::same_as<decltype(simd::fma(H{},H{},H{})),H>);
static_assert(std::same_as<decltype(H{}<H{}),H::mask>);
static_assert(std::same_as<decltype(H{}/H{}),H> && noexcept(H{}/H{}));
static_assert(std::same_as<decltype(sqrt(H{})),H> && noexcept(sqrt(H{})));
