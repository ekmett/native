// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
import simd.avx512;
import simd.avx512_fp16;
using old = simd::vec<float,16,simd::avx512>;
using next = simd::vec<float,16,simd::avx512_fp16>;
static_assert(!std::same_as<old,next>);
static_assert(old::mask::compact && next::mask::compact);
static_assert(sizeof(next)==64 && sizeof(simd::vec<float,3,simd::avx512_fp16>)==16);
template<class A,class B> concept addable=requires(A a,B b) { a+b; };
static_assert(!addable<old,next>);
template<class T> concept complete=requires { sizeof(T); };
static_assert(!complete<simd::vec<simd::fp16,32,simd::avx512>>);
static_assert(!complete<simd::vec<simd::fp16,4,simd::avx512_fp16>>);
static_assert(!complete<simd::vec<simd::bf16,32,simd::avx512_fp16>>);
template<class T> concept divisible=requires(T a) { a/a; };
static_assert(divisible<simd::vec<simd::fp16,32,simd::avx512_fp16>>);
