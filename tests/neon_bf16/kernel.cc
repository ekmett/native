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
#include <native/attributes.h>
#include "support/guarded_pages.h"
#include "../half_storage/native_bridge.h"
import native;
using B = native::simd<native::bf16,8,native::neon_bf16>;
static_assert(native::test::native_bridge<B>);
static_assert(native::test::bf16_storage_only<B>);
static_assert(sizeof(B) == 16 && alignof(B) == 16 && std::is_trivially_copyable_v<B>);
static_assert(sizeof(native::bf16) == 2 && !B::mask::compact);
static_assert(std::same_as<decltype(native::simd<native::bf16,8,native::neon_bf16>(std::array<native::bf16,8>{})),B>);
using F = native::simd<float,4,native::neon_bf16>;
static_assert(std::same_as<decltype(native::dot2(B{},B{},F{})),F>);
static_assert(noexcept(native::dot2(B{},B{},F{})));
template<class V> concept addable = requires(V a) { a+a; };
static_assert(!addable<B>);

template<std::size_t... I> auto lane_construction(std::index_sequence<I...>) {
  return native::simd<native::bf16,sizeof...(I),native::neon_bf16>(native::bf16::from_bits(std::uint16_t(I))...);
}
static_assert(std::same_as<decltype(lane_construction(std::make_index_sequence<8>{})),B>);

extern "C" bool bf16_storage() {
  std::array<native::bf16,8> values, copy;
  std::array<std::uint16_t,8> words;
  lane_construction(std::make_index_sequence<8>{}).store(copy.data());
  for(unsigned i=0;i!=8;++i) if(copy[i].to_bits()!=i) return false;
  for (unsigned base=0; base!=65536; base+=8) {
    for (unsigned i=0;i!=8;++i) values[i]=native::bf16::from_bits(std::uint16_t(base+i));
    auto value = native::load_simd<B>(values.data());
    native::store_simd(copy.data(),value);
    value.store_bits(words.data());
    for (unsigned i=0;i!=8;++i)
      if (copy[i].to_bits()!=base+i || words[i]!=base+i) return false;
    B::from_native(value.to_native()).store(copy.data());
    for (unsigned i=0;i!=B::lanes;++i) if (copy[i].to_bits()!=base+i) return false;
    typename B::native_type native = value;
    B restored = native;
    restored.store(copy.data());
    for (unsigned i=0;i!=8;++i) if (copy[i].to_bits()!=base+i) return false;
    B::from_bits(value.bits()).store(copy.data());
    for (unsigned i=0;i!=8;++i) if (copy[i].to_bits()!=base+i) return false;
  }
  native::test::guarded_pages input_page, output_page;
  for (std::size_t n=0;n<=8;++n) {
    auto * input=reinterpret_cast<native::bf16 *>(input_page.end())-n;
    auto * output=reinterpret_cast<native::bf16 *>(output_page.end())-n;
    for (std::size_t i=0;i<n;++i) {
      std::construct_at(input+i,native::bf16::from_bits(std::uint16_t(0x7c00+i)));
      std::construct_at(output+i,native::bf16::from_bits(0xdead));
    }
    std::construct_at(output-1,native::bf16::from_bits(0xbeef));
    auto value=B::load_partial(input,n,native::bf16::from_bits(0x8001));
    value.store(copy.data());
    for (std::size_t i=0;i<8;++i)
      if (copy[i].to_bits()!=(i<n ? 0x7c00+i : 0x8001)) return false;
    value.store_partial(output,n);
    for (std::size_t i=0;i<n;++i) if (output[i].to_bits()!=0x7c00+i) return false;
    if (output[-1].to_bits()!=0xbeef) return false;
    auto generic=native::load_simd_partial<B>(input,n,native::bf16::from_bits(0x8001));
    generic.store(copy.data());
    for (std::size_t i=0;i<8;++i)
      if (copy[i].to_bits()!=(i<n ? 0x7c00+i : 0x8001)) return false;
    native::store_simd_partial(output,generic,n);
    if (output[-1].to_bits()!=0xbeef) return false;
  }
  B::load_partial(nullptr,0,native::bf16::from_bits(0x7c01)).store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0x7c01) return false;
  B{}.store_partial(nullptr,0);
  B(native::bf16::from_bits(0xfc01)).store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0xfc01) return false;
  B{}.store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0) return false;
  return true;
}

extern "C" native_noinline void bf16_dot2(std::uint16_t const *a,std::uint16_t const *b,
    float const *c,float *out) noexcept {
  native::dot2(B::load_bits(a),B::load_bits(b),F::load(c)).store(out);
}
