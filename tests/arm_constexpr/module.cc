// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <bit>
#include <cstdint>
#if NATIVE_ARM_CONSTEXPR_HUB
import native;
#else
import native.arm.crc;
import native.arm.sha;
#endif
#include "crc_checks.h"
constexpr auto sha1=native::isa(native::arm_feature::sha1);
consteval bool rotation_checks() {
  for(unsigned bit=0;bit<32;++bit) {
    auto word=std::uint32_t{1}<<bit;
    if(native::sha1h<sha1>(word)!=std::rotr(word,2)
      || native::sha1h<native::isa{}>(word)!=std::rotr(word,2)) return false;
  }
  return native::sha1h<sha1>(std::uint32_t{0x12345678})==0x048d159e
    && native::sha1h<native::isa{}>(std::uint32_t{0x12345678})==0x048d159e;
}
static_assert(rotation_checks());
// These modules were compiled without CRC/SHA. Literal calls still evaluate
// immediately and keep the owning module's default ISA.
static_assert(native::crc32(std::uint32_t{},std::uint8_t{1})==0x77073096);
static_assert(native::crc32c(std::uint32_t{},std::uint8_t{1})==0xf26b8303);
static_assert(native::sha1h(std::uint32_t{3})==0xc0000000);
template<class T> concept exact_sha_word = requires(T x) { native::sha1h<native::isa{}>(x); };
static_assert(!exact_sha_word<std::int32_t> && !exact_sha_word<std::uint16_t>
  && !exact_sha_word<std::uint64_t> && !exact_sha_word<float>);
int main() { return 0; }
