// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
import simd.avx2;
import simd.avx512;
import simd.avx512_bf16;
using old2 = simd::vec<float,8,simd::avx2>;
using old5 = simd::vec<float,8,simd::avx512>;
using next = simd::vec<float,8,simd::avx512_bf16>;
static_assert(!std::same_as<old2,next> && !std::same_as<old5,next>);
static_assert(!old2::mask::compact && old5::mask::compact && next::mask::compact);
static_assert(sizeof(next) == 32 && sizeof(simd::vec<float,3,simd::avx512_bf16>) == 16);
template<class A,class B> concept addable = requires(A a,B b) { a+b; };
static_assert(!addable<old2,next> && !addable<old5,next>);
template<class T> concept complete = requires { sizeof(T); };
static_assert(!complete<simd::vec<simd::bf16,32,simd::avx512>>);
static_assert(!complete<simd::vec<simd::bf16,16,simd::avx512_bf16>>);
static_assert(!complete<simd::vec<simd::fp16,32,simd::avx512_bf16>>);
template<class A,class B,class F> concept has_dot2 = requires(A a,B b,F c) { simd::dot2(a,b,c); };
using bf16x32=simd::vec<simd::bf16,32,simd::avx512_bf16>;
static_assert(!has_dot2<bf16x32,bf16x32,simd::vec<float,16,simd::avx512>>);
// Old profiles retain their predicates even in a stronger importing TU.
extern "C" bool bf16_profile_identity() { return true; }
