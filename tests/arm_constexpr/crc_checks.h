// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <cstdint>

namespace crc_constant_fixture {
  constexpr auto strong = native::isa(native::arm_feature::crc);
  constexpr auto weak = native::isa{};
  constexpr std::uint32_t reverse(std::uint32_t x) {
    std::uint32_t r=0;
    for(unsigned i=0;i<32;++i) { r=(r<<1)|(x&1); x>>=1; }
    return r;
  }
  // Independent normal-polynomial long division. Reflect the register at the
  // boundaries instead of using the implementation's reflected recurrence.
  constexpr std::uint32_t reference(std::uint32_t accumulator,std::uint64_t word,
      unsigned width,std::uint32_t polynomial) {
    auto remainder=reverse(accumulator);
    for(unsigned i=0;i<width;++i) {
      auto coefficient=(remainder>>31)^static_cast<std::uint32_t>((word>>i)&1);
      remainder<<=1;
      if(coefficient) remainder^=polynomial;
    }
    return reverse(remainder);
  }
  template<class T> consteval bool update(std::uint32_t c,T value) {
    auto ieee=reference(c,value,8*sizeof(T),0x04c11db7);
    auto castagnoli=reference(c,value,8*sizeof(T),0x1edc6f41);
    return native::crc32<strong>(c,value)==ieee && native::crc32<weak>(c,value)==ieee
      && native::crc32c<strong>(c,value)==castagnoli && native::crc32c<weak>(c,value)==castagnoli;
  }
  template<class T> consteval bool basis() {
    if(!update(std::uint32_t{},T{})) return false;
    for(unsigned bit=0;bit<32;++bit)
      if(!update(std::uint32_t{1}<<bit,T{})) return false;
    for(unsigned bit=0;bit<8*sizeof(T);++bit)
      if(!update(std::uint32_t{},static_cast<T>(std::uint64_t{1}<<bit))) return false;
    return update(std::uint32_t{0x89abcdef},static_cast<T>(0xfedcba9876543210ull));
  }
  static_assert(basis<std::uint8_t>());
  static_assert(basis<std::uint16_t>());
  static_assert(basis<std::uint32_t>());
  static_assert(basis<std::uint64_t>());
  consteval bool standard_checksums() {
    std::uint32_t ieee=0xffffffff,castagnoli=0xffffffff;
    for(auto c : "123456789") if(c) {
      ieee=native::crc32<weak>(ieee,static_cast<std::uint8_t>(c));
      castagnoli=native::crc32c<weak>(castagnoli,static_cast<std::uint8_t>(c));
    }
    return ~ieee==0xcbf43926 && ~castagnoli==0xe3069283;
  }
  static_assert(standard_checksums());
  template<class T> concept exact_type = requires(std::uint32_t c,T x) {
    native::crc32<weak>(c,x); native::crc32c<weak>(c,x);
  };
  static_assert(!exact_type<std::int8_t> && !exact_type<std::int16_t>
    && !exact_type<std::int32_t> && !exact_type<std::int64_t> && !exact_type<float>);
}
