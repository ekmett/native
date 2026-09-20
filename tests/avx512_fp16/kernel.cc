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
using B = native::vec<native::fp16,32,native::avx512_fp16>;
static_assert(native::test::native_bridge<B>);
static_assert(sizeof(B) == 64 && alignof(B) == 64 && std::is_trivially_copyable_v<B>);
static_assert(sizeof(native::fp16) == 2 && B::mask::compact);
static_assert(std::same_as<decltype(native::vec<native::fp16,32,native::avx512_fp16>(std::array<native::fp16,32>{})),B>);
static_assert(std::same_as<decltype(native::fma(B{},B{},B{})),B>);
static_assert(noexcept(native::fma(B{},B{},B{})));
static_assert(std::same_as<decltype(B{}/B{}),B> && noexcept(B{}/B{}));
static_assert(std::same_as<decltype(sqrt(B{})),B> && noexcept(sqrt(B{})));
template<class V> concept addable = requires(V a) { a+a; };
static_assert(addable<B>);

template<std::size_t... I> auto lane_construction(std::index_sequence<I...>) {
  return native::vec<native::fp16,sizeof...(I),native::avx512_fp16>(native::fp16::from_bits(std::uint16_t(I))...);
}
static_assert(std::same_as<decltype(lane_construction(std::make_index_sequence<32>{})),B>);

extern "C" bool fp16_storage() {
  std::array<native::fp16,32> values, copy;
  std::array<std::uint16_t,32> words;
  lane_construction(std::make_index_sequence<32>{}).store(copy.data());
  for(unsigned i=0;i!=32;++i) if(copy[i].to_bits()!=i) return false;
  for (unsigned base=0; base!=65536; base+=32) {
    for (unsigned i=0;i!=32;++i) values[i]=native::fp16::from_bits(std::uint16_t(base+i));
    auto value = native::load_simd<B>(values.data());
    native::store_simd(copy.data(),value);
    value.store_bits(words.data());
    for (unsigned i=0;i!=32;++i)
      if (copy[i].to_bits()!=base+i || words[i]!=base+i) return false;
    B::from_native(value.to_native()).store(copy.data());
    for (unsigned i=0;i!=B::lanes;++i) if (copy[i].to_bits()!=base+i) return false;
    typename B::native_type native = value;
    B restored = native;
    restored.store(copy.data());
    for (unsigned i=0;i!=32;++i) if (copy[i].to_bits()!=base+i) return false;
    B::from_bits(value.bits()).store(copy.data());
    for (unsigned i=0;i!=32;++i) if (copy[i].to_bits()!=base+i) return false;
  }
  native::test::guarded_pages input_page, output_page;
  for (std::size_t n=0;n<=32;++n) {
    auto * input=reinterpret_cast<native::fp16 *>(input_page.end())-n;
    auto * output=reinterpret_cast<native::fp16 *>(output_page.end())-n;
    for (std::size_t i=0;i<n;++i) {
      std::construct_at(input+i,native::fp16::from_bits(std::uint16_t(0x7c00+i)));
      std::construct_at(output+i,native::fp16::from_bits(0xdead));
    }
    std::construct_at(output-1,native::fp16::from_bits(0xbeef));
    auto value=B::load_partial(input,n,native::fp16::from_bits(0x8001));
    value.store(copy.data());
    for (std::size_t i=0;i<32;++i)
      if (copy[i].to_bits()!=(i<n ? 0x7c00+i : 0x8001)) return false;
    value.store_partial(output,n);
    for (std::size_t i=0;i<n;++i) if (output[i].to_bits()!=0x7c00+i) return false;
    if (output[-1].to_bits()!=0xbeef) return false;
    auto generic=native::load_simd_partial<B>(input,n,native::fp16::from_bits(0x8001));
    generic.store(copy.data());
    for (std::size_t i=0;i<32;++i)
      if (copy[i].to_bits()!=(i<n ? 0x7c00+i : 0x8001)) return false;
    native::store_simd_partial(output,generic,n);
    if (output[-1].to_bits()!=0xbeef) return false;
  }
  B::load_partial(nullptr,0,native::fp16::from_bits(0x7c01)).store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0x7c01) return false;
  B{}.store_partial(nullptr,0);
  B(native::fp16::from_bits(0xfc01)).store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0xfc01) return false;
  B{}.store(copy.data());
  for (auto value:copy) if(value.to_bits()!=0) return false;
  return true;
}

extern "C" native_noinline void fp16_add(std::uint16_t const *a,std::uint16_t const *b,std::uint16_t *out) noexcept {
  (B::load_bits(a)+B::load_bits(b)).store_bits(out);
}
extern "C" native_noinline void fp16_sub(std::uint16_t const *a,std::uint16_t const *b,std::uint16_t *out) noexcept {
  (B::load_bits(a)-B::load_bits(b)).store_bits(out);
}
extern "C" native_noinline void fp16_mul(std::uint16_t const *a,std::uint16_t const *b,std::uint16_t *out) noexcept {
  (B::load_bits(a)*B::load_bits(b)).store_bits(out);
}
extern "C" native_noinline void fp16_div(std::uint16_t const *a,std::uint16_t const *b,std::uint16_t *out) noexcept {
  (B::load_bits(a)/B::load_bits(b)).store_bits(out);
}
extern "C" native_noinline void fp16_sqrt(std::uint16_t const *a,std::uint16_t *out) noexcept {
  sqrt(B::load_bits(a)).store_bits(out);
}
extern "C" native_noinline void fp16_fma(std::uint16_t const *a,std::uint16_t const *b,std::uint16_t const *c,std::uint16_t *out) noexcept {
  native::fma(B::load_bits(a),B::load_bits(b),B::load_bits(c)).store_bits(out);
}
extern "C" native_noinline void fp16_neg(std::uint16_t const *a,std::uint16_t *out) noexcept {
  (-B::load_bits(a)).store_bits(out);
}
extern "C" native_noinline void fp16_compare(std::uint16_t const *a,std::uint16_t const *b,std::uint64_t *out) noexcept {
  auto x=B::load_bits(a),y=B::load_bits(b);
  out[0]=(x==y).to_bitset();out[1]=(x!=y).to_bitset();
  out[2]=(x<y).to_bitset();out[3]=(x<=y).to_bitset();
  out[4]=(x>y).to_bitset();out[5]=(x>=y).to_bitset();
}
extern "C" native_noinline void fp16_select(std::uint64_t mask,std::uint16_t const *a,std::uint16_t const *b,std::uint16_t *out) noexcept {
  select(B::mask::from_bitset(mask),B::load_bits(a),B::load_bits(b)).store_bits(out);
}
