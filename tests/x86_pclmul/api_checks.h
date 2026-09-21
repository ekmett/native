// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

template<native::isa A, std::size_t N> using vector = native::simd<std::uint64_t,N,A>;
template<native::isa A, class V, unsigned I = 0> concept has_legacy = requires(V a) {
  { native::pclmulqdq<A,I>(a,a) } noexcept -> std::same_as<V>;
};
template<native::isa A, class V, unsigned I = 0> concept has_vector = requires(V a) {
  { native::vpclmulqdq<A,I>(a,a) } noexcept -> std::same_as<V>;
};
static_assert(has_legacy<legacy,vector<legacy,2>,255>);
static_assert(!has_legacy<legacy,vector<legacy,2>,256>);
static_assert(!has_legacy<legacy,__m128i>);
static_assert(!has_legacy<legacy,native::simd<float,4,legacy>>);
static_assert(!has_legacy<legacy,vector<vex128,2>>);
static_assert(has_vector<vex128,vector<vex128,2>,255>);
static_assert(!has_vector<vex128,vector<vex128,2>,256>);
static_assert(has_vector<vex256,vector<vex256,4>,255>);
static_assert(!has_vector<vex256,vector<vex256,4>,256>);
static_assert(has_vector<evex512,vector<evex512,8>,255>);
static_assert(!has_vector<evex512,vector<evex512,8>,256>);
constexpr auto sse2_storage = native::target_features("sse2");
constexpr auto avx_storage = native::target_features("avx");
constexpr auto avx512_storage = native::target_features("avx512f");
static_assert(!has_legacy<sse2_storage,vector<sse2_storage,2>>);
static_assert(!has_vector<legacy,vector<legacy,2>>);
static_assert(!has_vector<avx_storage,vector<avx_storage,4>>);
static_assert(!has_vector<avx512_storage,vector<avx512_storage,8>>);
