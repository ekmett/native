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
using B = simd::vec<simd::bf16,32,simd::avx512_bf16>;
using F = simd::vec<float,16,simd::avx512_bf16>;
static_assert(sizeof(B) == 64 && alignof(B) == 64 && std::is_trivially_copyable_v<B>);
static_assert(sizeof(simd::bf16) == 2 && B::mask::compact);
static_assert(std::same_as<decltype(simd::vec(simd::avx512_bf16{},std::array<simd::bf16,32>{})),B>);
static_assert(std::same_as<decltype(simd::dot2(B{},B{},F{})),F>);
static_assert(noexcept(simd::dot2(B{},B{},F{})));
template<class V> concept addable = requires(V a) { a+a; };
static_assert(!addable<B>);

extern "C" simd_noinline void bf16_dot2(std::uint16_t const * a, std::uint16_t const * b,
    std::uint32_t const * c, std::uint32_t * out) noexcept {
  simd::dot2(B::load_bits(a), B::load_bits(b), F::load_bits(c)).store_bits(out);
}

template<std::size_t... I> auto lane_construction(std::index_sequence<I...>) {
  return simd::vec(simd::avx512_bf16{},simd::bf16::from_bits(std::uint16_t(I))...);
}
static_assert(std::same_as<decltype(lane_construction(std::make_index_sequence<32>{})),B>);

extern "C" bool bf16_storage() {
  std::array<simd::bf16,32> values, copy;
  std::array<std::uint16_t,32> words;
  lane_construction(std::make_index_sequence<32>{}).store(copy.data());
  for(unsigned i=0;i!=32;++i) if(copy[i].to_bits()!=i) return false;
  for (unsigned base=0; base!=65536; base+=32) {
    for (unsigned i=0;i!=32;++i) values[i]=simd::bf16::from_bits(std::uint16_t(base+i));
    auto value = simd::load_simd<B>(values.data());
    simd::store_simd(copy.data(),value);
    value.store_bits(words.data());
    for (unsigned i=0;i!=32;++i)
      if (copy[i].to_bits()!=base+i || words[i]!=base+i) return false;
    B::from_native(value.to_native()).store(copy.data());
    for (unsigned i=0;i!=32;++i) if (copy[i].to_bits()!=base+i) return false;
    B::from_bits(value.bits()).store(copy.data());
    for (unsigned i=0;i!=32;++i) if (copy[i].to_bits()!=base+i) return false;
  }
  simd::test::guarded_pages input_page, output_page;
  for (std::size_t n=0;n<=32;++n) {
    auto * input=reinterpret_cast<simd::bf16 *>(input_page.end())-n;
    auto * output=reinterpret_cast<simd::bf16 *>(output_page.end())-n;
    for (std::size_t i=0;i<n;++i) {
      std::construct_at(input+i,simd::bf16::from_bits(std::uint16_t(0x7f80+i)));
      std::construct_at(output+i,simd::bf16::from_bits(0xdead));
    }
    std::construct_at(output-1,simd::bf16::from_bits(0xbeef));
    auto value=B::load_partial(input,n,simd::bf16::from_bits(0x8001));
    value.store(copy.data());
    for (std::size_t i=0;i<32;++i)
      if (copy[i].to_bits()!=(i<n ? 0x7f80+i : 0x8001)) return false;
    value.store_partial(output,n);
    for (std::size_t i=0;i<n;++i) if (output[i].to_bits()!=0x7f80+i) return false;
    if (output[-1].to_bits()!=0xbeef) return false;
    auto generic=simd::load_simd_partial<B>(input,n,simd::bf16::from_bits(0x8001));
    generic.store(copy.data());
    for (std::size_t i=0;i<32;++i)
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
