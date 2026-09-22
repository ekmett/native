// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
import native;
using old2 = native::simd<float,8,native::avx2>;
using old5 = native::simd<float,8,native::avx512>;
using next = native::simd<float,8,native::avx512_bf16>;
static_assert(!std::same_as<old2,next> && !std::same_as<old5,next>);
static_assert(!old2::mask::compact && old5::mask::compact && next::mask::compact);
static_assert(sizeof(next) == 32 && sizeof(native::simd<float,3,native::avx512_bf16>) == 16);
template<class A,class B> concept addable = requires(A a,B b) { a+b; };
static_assert(!addable<old2,next> && !addable<old5,next>);
template<class T> concept complete = requires { sizeof(T); };
static_assert(!complete<native::simd<native::bf16,32,native::avx512>>);
static_assert(complete<native::simd<native::bf16,8,native::avx512_bf16>>);
static_assert(complete<native::simd<native::bf16,16,native::avx512_bf16>>);
using bf16x4 = native::simd<native::bf16,4,native::avx512_bf16>;
static_assert(complete<bf16x4> && sizeof(bf16x4) == 16);
static_assert(!addable<bf16x4,bf16x4>);
static_assert(!complete<native::simd<native::bf16,64,native::avx512_bf16>>);
static_assert(!complete<native::simd<native::fp16,32,native::avx512_bf16>>);
template<class A,class B,class F> concept has_dot2 = requires(A a,B b,F c) { native::dot2(a,b,c); };
using bf16x32=native::simd<native::bf16,32,native::avx512_bf16>;
static_assert(!has_dot2<bf16x32,bf16x32,native::simd<float,16,native::avx512>>);
using bf16x8=native::simd<native::bf16,8,native::avx512_bf16>;
using bf16x16=native::simd<native::bf16,16,native::avx512_bf16>;
static_assert(!has_dot2<bf16x8,bf16x16,native::simd<float,4,native::avx512_bf16>>);
static_assert(!has_dot2<bf16x8,bf16x8,native::simd<float,8,native::avx512_bf16>>);
static_assert(!has_dot2<bf16x16,bf16x16,native::simd<float,8,native::avx512>>);
// Old profiles retain their predicates even in a stronger importing TU.
extern "C" bool bf16_profile_identity() { return true; }
