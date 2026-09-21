// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

template<std::size_t N> using floats = native::simd<float,N,arch>;
template<std::size_t N> using halves = native::simd<native::fp16,N,arch>;
template<class V> concept can_narrow = requires(V a) { native::cvtps_ph<arch,0>(a); };
template<class V, unsigned N> concept can_widen = requires(V a) { native::cvtph_ps<arch,N>(a); };
static_assert(std::same_as<decltype(native::cvtps_ph<arch,0>(floats<4>{})),halves<4>>);
static_assert(std::same_as<decltype(native::cvtph_ps<arch,4>(halves<4>{})),floats<4>>);
static_assert(std::same_as<decltype(native::cvtps_ph<arch,0>(floats<8>{})),halves<8>>);
static_assert(std::same_as<decltype(native::cvtph_ps<arch,8>(halves<8>{})),floats<8>>);
static_assert(!can_narrow<__m128> && !can_narrow<__m256>);
static_assert(!can_widen<__m128i,4> && !can_widen<__m128i,8>);
static_assert(!can_narrow<native::simd<std::uint32_t,4,arch>>);
static_assert(!can_widen<halves<8>,4> && !can_widen<halves<4>,8>);
