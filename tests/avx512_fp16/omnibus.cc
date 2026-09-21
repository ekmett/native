// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import native;
using H=native::vec<native::fp16,32,native::avx512_fp16>;
static_assert(sizeof(H)==64);
#if TEST_OMNIBUS_BF16
using B=native::vec<native::bf16,32,native::avx512_bf16>;
static_assert(sizeof(B)==64);
static_assert(!std::same_as<H,B>);
static_assert(std::same_as<decltype(native::dot2(B{},B{},native::vec<float,16,native::avx512_bf16>{})),native::vec<float,16,native::avx512_bf16>>);
#endif
static_assert(std::same_as<decltype(native::fma(H{},H{},H{})),H>);
static_assert(std::same_as<decltype(H{}<H{}),H::mask>);
static_assert(std::same_as<decltype(H{}/H{}),H> && noexcept(H{}/H{}));
static_assert(std::same_as<decltype(sqrt(H{})),H> && noexcept(sqrt(H{})));
