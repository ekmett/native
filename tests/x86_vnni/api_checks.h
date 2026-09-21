// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

template<native::isa<native::x86> A, std::size_t N> concept has_dpbusd = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::uint8_t,N/1,A> a, native::simd<std::int8_t,N/1,A> b) {
  { native::dpbusd<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpbusd<core,16>);
static_assert(has_dpbusd<core,32>);
static_assert(has_dpbusd<evexvl,16>);
static_assert(has_dpbusd<evexvl,32>);
static_assert(has_dpbusd<evex512,64>);
static_assert(!has_dpbusd<evex512,16>);
template<native::isa<native::x86> A, std::size_t N> concept masks_dpbusd = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::uint8_t,N/1,A> a, native::simd<std::int8_t,N/1,A> b,
    native::predicate<N/4,A> k) {
  { native::mask_dpbusd<A>(acc,k,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
  { native::maskz_dpbusd<A>(k,acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(masks_dpbusd<evexvl,16> && masks_dpbusd<evexvl,32> && masks_dpbusd<evex512,64>);
static_assert(!masks_dpbusd<core,16>);
static_assert(!has_dpbusd<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpbusds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::uint8_t,N/1,A> a, native::simd<std::int8_t,N/1,A> b) {
  { native::dpbusds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpbusds<core,16>);
static_assert(has_dpbusds<core,32>);
static_assert(has_dpbusds<evexvl,16>);
static_assert(has_dpbusds<evexvl,32>);
static_assert(has_dpbusds<evex512,64>);
static_assert(!has_dpbusds<evex512,16>);
template<native::isa<native::x86> A, std::size_t N> concept masks_dpbusds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::uint8_t,N/1,A> a, native::simd<std::int8_t,N/1,A> b,
    native::predicate<N/4,A> k) {
  { native::mask_dpbusds<A>(acc,k,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
  { native::maskz_dpbusds<A>(k,acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(masks_dpbusds<evexvl,16> && masks_dpbusds<evexvl,32> && masks_dpbusds<evex512,64>);
static_assert(!masks_dpbusds<core,16>);
static_assert(!has_dpbusds<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwssd = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int16_t,N/2,A> a, native::simd<std::int16_t,N/2,A> b) {
  { native::dpwssd<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpwssd<core,16>);
static_assert(has_dpwssd<core,32>);
static_assert(has_dpwssd<evexvl,16>);
static_assert(has_dpwssd<evexvl,32>);
static_assert(has_dpwssd<evex512,64>);
static_assert(!has_dpwssd<evex512,16>);
template<native::isa<native::x86> A, std::size_t N> concept masks_dpwssd = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int16_t,N/2,A> a, native::simd<std::int16_t,N/2,A> b,
    native::predicate<N/4,A> k) {
  { native::mask_dpwssd<A>(acc,k,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
  { native::maskz_dpwssd<A>(k,acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(masks_dpwssd<evexvl,16> && masks_dpwssd<evexvl,32> && masks_dpwssd<evex512,64>);
static_assert(!masks_dpwssd<core,16>);
static_assert(!has_dpwssd<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwssds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int16_t,N/2,A> a, native::simd<std::int16_t,N/2,A> b) {
  { native::dpwssds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpwssds<core,16>);
static_assert(has_dpwssds<core,32>);
static_assert(has_dpwssds<evexvl,16>);
static_assert(has_dpwssds<evexvl,32>);
static_assert(has_dpwssds<evex512,64>);
static_assert(!has_dpwssds<evex512,16>);
template<native::isa<native::x86> A, std::size_t N> concept masks_dpwssds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int16_t,N/2,A> a, native::simd<std::int16_t,N/2,A> b,
    native::predicate<N/4,A> k) {
  { native::mask_dpwssds<A>(acc,k,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
  { native::maskz_dpwssds<A>(k,acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(masks_dpwssds<evexvl,16> && masks_dpwssds<evexvl,32> && masks_dpwssds<evex512,64>);
static_assert(!masks_dpwssds<core,16>);
static_assert(!has_dpwssds<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpbssd = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int8_t,N/1,A> a, native::simd<std::int8_t,N/1,A> b) {
  { native::dpbssd<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpbssd<int8,16>);
static_assert(has_dpbssd<int8,32>);
static_assert(!has_dpbssd<int8,64>);
static_assert(!has_dpbssd<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpbssds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int8_t,N/1,A> a, native::simd<std::int8_t,N/1,A> b) {
  { native::dpbssds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpbssds<int8,16>);
static_assert(has_dpbssds<int8,32>);
static_assert(!has_dpbssds<int8,64>);
static_assert(!has_dpbssds<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpbsud = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int8_t,N/1,A> a, native::simd<std::uint8_t,N/1,A> b) {
  { native::dpbsud<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpbsud<int8,16>);
static_assert(has_dpbsud<int8,32>);
static_assert(!has_dpbsud<int8,64>);
static_assert(!has_dpbsud<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpbsuds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int8_t,N/1,A> a, native::simd<std::uint8_t,N/1,A> b) {
  { native::dpbsuds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpbsuds<int8,16>);
static_assert(has_dpbsuds<int8,32>);
static_assert(!has_dpbsuds<int8,64>);
static_assert(!has_dpbsuds<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpbuud = requires(
    native::simd<std::uint32_t,N/4,A> acc, native::simd<std::uint8_t,N/1,A> a, native::simd<std::uint8_t,N/1,A> b) {
  { native::dpbuud<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::uint32_t,N/4,A>>;
};
static_assert(has_dpbuud<int8,16>);
static_assert(has_dpbuud<int8,32>);
static_assert(!has_dpbuud<int8,64>);
static_assert(!has_dpbuud<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpbuuds = requires(
    native::simd<std::uint32_t,N/4,A> acc, native::simd<std::uint8_t,N/1,A> a, native::simd<std::uint8_t,N/1,A> b) {
  { native::dpbuuds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::uint32_t,N/4,A>>;
};
static_assert(has_dpbuuds<int8,16>);
static_assert(has_dpbuuds<int8,32>);
static_assert(!has_dpbuuds<int8,64>);
static_assert(!has_dpbuuds<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwsud = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int16_t,N/2,A> a, native::simd<std::uint16_t,N/2,A> b) {
  { native::dpwsud<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpwsud<int16,16>);
static_assert(has_dpwsud<int16,32>);
static_assert(!has_dpwsud<int16,64>);
static_assert(!has_dpwsud<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwsuds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::int16_t,N/2,A> a, native::simd<std::uint16_t,N/2,A> b) {
  { native::dpwsuds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpwsuds<int16,16>);
static_assert(has_dpwsuds<int16,32>);
static_assert(!has_dpwsuds<int16,64>);
static_assert(!has_dpwsuds<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwusd = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::uint16_t,N/2,A> a, native::simd<std::int16_t,N/2,A> b) {
  { native::dpwusd<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpwusd<int16,16>);
static_assert(has_dpwusd<int16,32>);
static_assert(!has_dpwusd<int16,64>);
static_assert(!has_dpwusd<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwusds = requires(
    native::simd<std::int32_t,N/4,A> acc, native::simd<std::uint16_t,N/2,A> a, native::simd<std::int16_t,N/2,A> b) {
  { native::dpwusds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::int32_t,N/4,A>>;
};
static_assert(has_dpwusds<int16,16>);
static_assert(has_dpwusds<int16,32>);
static_assert(!has_dpwusds<int16,64>);
static_assert(!has_dpwusds<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwuud = requires(
    native::simd<std::uint32_t,N/4,A> acc, native::simd<std::uint16_t,N/2,A> a, native::simd<std::uint16_t,N/2,A> b) {
  { native::dpwuud<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::uint32_t,N/4,A>>;
};
static_assert(has_dpwuud<int16,16>);
static_assert(has_dpwuud<int16,32>);
static_assert(!has_dpwuud<int16,64>);
static_assert(!has_dpwuud<native::avx2,32>);
template<native::isa<native::x86> A, std::size_t N> concept has_dpwuuds = requires(
    native::simd<std::uint32_t,N/4,A> acc, native::simd<std::uint16_t,N/2,A> a, native::simd<std::uint16_t,N/2,A> b) {
  { native::dpwuuds<A>(acc,a,b) } noexcept -> std::same_as<native::simd<std::uint32_t,N/4,A>>;
};
static_assert(has_dpwuuds<int16,16>);
static_assert(has_dpwuuds<int16,32>);
static_assert(!has_dpwuuds<int16,64>);
static_assert(!has_dpwuuds<native::avx2,32>);
template<class V> concept raw_core = requires(V v) { native::dpbusd<core>(v,v,v); };
static_assert(!raw_core<__m128i> && !raw_core<__m256i>);
template<class V> concept signed_a = requires(native::simd<std::int32_t,4,core> s, V a, native::simd<std::int8_t,16,core> b) { native::dpbusd<core>(s,a,b); };
static_assert(!signed_a<native::simd<std::int8_t,16,core>>);
static_assert(!signed_a<native::simd<std::uint8_t,16,evexvl>>);
