// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <simd/attributes.h>
#include "support/guarded_pages.h"
import simd.avx512_bf16;
template<std::size_t N> using bf16_vector = simd::vec<simd::bf16,N,simd::avx512_bf16>;
template<std::size_t N> using float_vector = simd::vec<float,N/2,simd::avx512_bf16>;
template<class V> concept addable = requires(V a) { a+a; };
template<std::size_t N> constexpr bool shape() {
  using B = bf16_vector<N>;
  using F = float_vector<N>;
  static_assert(sizeof(B) == 2*N && alignof(B) == 2*N && std::is_trivially_copyable_v<B>);
  static_assert(sizeof(simd::bf16) == 2 && B::mask::compact && B::lanes == N);
  static_assert(std::same_as<decltype(simd::vec(simd::avx512_bf16{},std::array<simd::bf16,N>{})),B>);
  static_assert(std::same_as<decltype(simd::dot2(B{},B{},F{})),F>);
  static_assert(noexcept(simd::dot2(B{},B{},F{})) && !addable<B>);
  return true;
}
static_assert(shape<8>() && shape<16>() && shape<32>());

extern "C" simd_noinline void bf16_dot2_8(std::uint16_t const * a, std::uint16_t const * b,
    std::uint32_t const * c, std::uint32_t * out) noexcept {
  simd::dot2(bf16_vector<8>::load_bits(a), bf16_vector<8>::load_bits(b), float_vector<8>::load_bits(c)).store_bits(out);
}
extern "C" simd_noinline void bf16_dot2_16(std::uint16_t const * a, std::uint16_t const * b,
    std::uint32_t const * c, std::uint32_t * out) noexcept {
  simd::dot2(bf16_vector<16>::load_bits(a), bf16_vector<16>::load_bits(b), float_vector<16>::load_bits(c)).store_bits(out);
}
extern "C" simd_noinline void bf16_dot2(std::uint16_t const * a, std::uint16_t const * b,
    std::uint32_t const * c, std::uint32_t * out) noexcept {
  simd::dot2(bf16_vector<32>::load_bits(a), bf16_vector<32>::load_bits(b), float_vector<32>::load_bits(c)).store_bits(out);
}

template<std::size_t... I> auto lane_construction(std::index_sequence<I...>) {
  return simd::vec(simd::avx512_bf16{},simd::bf16::from_bits(std::uint16_t(I))...);
}
template<std::size_t N> bool storage() {
  using B = bf16_vector<N>;
  static_assert(std::same_as<decltype(lane_construction(std::make_index_sequence<N>{})),B>);
  std::array<simd::bf16,N> values, copy;
  std::array<std::uint16_t,N> words;
  lane_construction(std::make_index_sequence<N>{}).store(copy.data());
  for(unsigned i=0;i!=N;++i) if(copy[i].to_bits()!=i) return false;
  for (unsigned base=0; base!=65536; base+=N) {
    for (unsigned i=0;i!=N;++i) values[i]=simd::bf16::from_bits(std::uint16_t(base+i));
    auto value = simd::load_simd<B>(values.data());
    simd::store_simd(copy.data(),value);
    value.store_bits(words.data());
    for (unsigned i=0;i!=N;++i)
      if (copy[i].to_bits()!=base+i || words[i]!=base+i) return false;
    B::from_native(value.to_native()).store(copy.data());
    for (unsigned i=0;i!=N;++i) if (copy[i].to_bits()!=base+i) return false;
    B::from_bits(value.bits()).store(copy.data());
    for (unsigned i=0;i!=N;++i) if (copy[i].to_bits()!=base+i) return false;
    B::load_bits(words.data()).storeu(copy.data());
    for (unsigned i=0;i!=N;++i) if (copy[i].to_bits()!=base+i) return false;
  }
  simd::test::guarded_pages input_page, output_page;
  for (std::size_t n=0;n<=N;++n) {
    auto * input=reinterpret_cast<simd::bf16 *>(input_page.end())-n;
    auto * output=reinterpret_cast<simd::bf16 *>(output_page.end())-n;
    for (std::size_t i=0;i<n;++i) {
      std::construct_at(input+i,simd::bf16::from_bits(std::uint16_t(0x7f80+i)));
      std::construct_at(output+i,simd::bf16::from_bits(0xdead));
    }
    std::construct_at(output-1,simd::bf16::from_bits(0xbeef));
    auto value=B::load_partial(input,n,simd::bf16::from_bits(0x8001));
    value.store(copy.data());
    for (std::size_t i=0;i<N;++i)
      if (copy[i].to_bits()!=(i<n ? 0x7f80+i : 0x8001)) return false;
    value.store_partial(output,n);
    for (std::size_t i=0;i<n;++i) if (output[i].to_bits()!=0x7f80+i) return false;
    if (output[-1].to_bits()!=0xbeef) return false;
    auto generic=simd::load_simd_partial<B>(input,n,simd::bf16::from_bits(0x8001));
    generic.store(copy.data());
    for (std::size_t i=0;i<N;++i)
      if (copy[i].to_bits()!=(i<n ? 0x7f80+i : 0x8001)) return false;
    simd::store_simd_partial(output,generic,n);
    if (output[-1].to_bits()!=0xbeef) return false;
  }
  B::load_partial(nullptr,0,simd::bf16::from_bits(0x7f81)).store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0x7f81) return false;
  B{}.store_partial(nullptr,0);
  B(simd::bf16::from_bits(0xff81)).store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0xff81) return false;
  B{}.store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0) return false;
  return true;
}

extern "C" bool bf16_storage() {
  return storage<8>() && storage<16>() && storage<32>();
}
