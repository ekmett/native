// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
import native;
using old = native::simd<float,4,native::neon>;
using next = native::simd<float,4,native::neon_bf16>;
static_assert(!std::same_as<old,next>);
static_assert(!old::mask::compact && !next::mask::compact);
static_assert(sizeof(next)==16 && sizeof(native::simd<float,3,native::neon_bf16>)==16);
template<class A,class B> concept addable=requires(A a,B b) { a+b; };
static_assert(!addable<old,next>);
template<class T> concept complete=requires { sizeof(T); };
static_assert(!complete<native::simd<native::bf16,8,native::neon>>);
static_assert(!complete<native::simd<native::bf16,4,native::neon_bf16>>);
static_assert(!complete<native::simd<native::fp16,8,native::neon_bf16>>);
template<class T> concept divisible=requires(T a) { a/a; };
static_assert(!divisible<native::simd<native::bf16,8,native::neon_bf16>>);
