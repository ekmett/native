// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

template<native::isa<native::x86> A, std::size_t N> using byte_vector = native::simd<std::uint8_t,N,A>;
template<native::isa<native::x86> A, std::size_t N> using matrix_vector = native::simd<std::uint64_t,N/8,A>;
template<native::isa<native::x86> A, std::size_t N, unsigned I = 0>
concept has_operations = requires(byte_vector<A,N> x, matrix_vector<A,N> matrix) {
  { native::gf2p8mulb<A>(x,x) } noexcept -> std::same_as<byte_vector<A,N>>;
  { native::gf2p8affineqb<A,I>(x,matrix) } noexcept -> std::same_as<byte_vector<A,N>>;
  { native::gf2p8affineinvqb<A,I>(x,matrix) } noexcept -> std::same_as<byte_vector<A,N>>;
};
template<native::isa<native::x86> A, std::size_t N, unsigned I = 0>
concept has_masks = requires(byte_vector<A,N> x, matrix_vector<A,N> matrix, native::predicate<N,A> k) {
  { native::gf2p8mulb_mask<A>(x,k,x,x) } noexcept -> std::same_as<byte_vector<A,N>>;
  { native::gf2p8mulb_maskz<A>(k,x,x) } noexcept -> std::same_as<byte_vector<A,N>>;
  { native::gf2p8affineqb_mask<A,I>(x,k,x,matrix) } noexcept -> std::same_as<byte_vector<A,N>>;
  { native::gf2p8affineqb_maskz<A,I>(k,x,matrix) } noexcept -> std::same_as<byte_vector<A,N>>;
  { native::gf2p8affineinvqb_mask<A,I>(x,k,x,matrix) } noexcept -> std::same_as<byte_vector<A,N>>;
  { native::gf2p8affineinvqb_maskz<A,I>(k,x,matrix) } noexcept -> std::same_as<byte_vector<A,N>>;
};
static_assert(has_operations<arch128,16,255>);
static_assert(!has_operations<arch128,16,256>);
static_assert(has_operations<arch256,32,255>);
static_assert(!has_operations<arch256,32,256>);
static_assert(has_operations<arch512,64,255>);
static_assert(!has_operations<arch512,64,256>);
static_assert(has_masks<mask_narrow,16,255>);
static_assert(!has_masks<mask_narrow,16,256>);
static_assert(has_masks<mask_narrow,32,255>);
static_assert(!has_masks<mask_narrow,32,256>);
static_assert(has_masks<mask512,64,255>);
static_assert(!has_masks<mask512,64,256>);
static_assert(!has_operations<native::isa<native::x86>{},16>);
static_assert(!has_operations<native::avx2,16>);
static_assert(!has_operations<arch128,32>);
static_assert(!has_operations<arch256,64>);
static_assert(!has_masks<mask512,16>);
static_assert(!has_masks<mask512,32>);
static_assert(!has_masks<arch512,64>);
static_assert(!has_masks<arch512 & native::x86_feature::avx512vl,16>);
static_assert(!has_masks<native::avx512,32>);
template<class V> concept byte_operand = requires(V x) { native::gf2p8mulb<arch128>(x,x); };
static_assert(!byte_operand<__m128i>);
static_assert(!byte_operand<native::simd<std::int8_t,16,arch128>>);
static_assert(!byte_operand<byte_vector<arch256,16>>);
template<class M> concept matrix_operand = requires(byte_vector<arch128,16> x, M m) { native::gf2p8affineqb<arch128,0>(x,m); };
static_assert(!matrix_operand<byte_vector<arch128,16>>);
